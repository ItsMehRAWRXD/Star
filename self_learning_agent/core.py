from __future__ import annotations

import os
import re
import time
import json
import math
import base64
import random
import hashlib
import logging
from dataclasses import dataclass, field
from typing import List, Optional, Dict, Any, Tuple

import numpy as np
import yaml

try:
    import httpx
except Exception:  # pragma: no cover
    httpx = None  # type: ignore

try:
    from openai import OpenAI  # type: ignore
except Exception:  # pragma: no cover
    OpenAI = None  # type: ignore


# -------- Safety & Policy ---------

BLOCKED_KEYWORDS = [
    # security/offensive
    "malware",
    "ransomware",
    "dropper",
    "keylogger",
    "botnet",
    "exploit",
    "payload",
    "shellcode",
    "backdoor",
    "evasion",
    # violence/illegal
    "harm",
    "kill",
    "weapon",
]


def is_safe_task(text: str) -> bool:
    lower = text.lower()
    return not any(k in lower for k in BLOCKED_KEYWORDS)


# -------- Config ---------

@dataclass
class OpenAIConfig:
    enabled: bool = True
    model: str = os.getenv("OPENAI_MODEL", "gpt-4o-mini")
    api_key: Optional[str] = os.getenv("OPENAI_API_KEY")
    base_url: Optional[str] = os.getenv("OPENAI_BASE_URL")


@dataclass
class PolicyConfig:
    mode: str = "bandit"  # rules | score | bandit
    epsilon: float = 0.15
    latency_weight: float = 0.25
    success_weight: float = 0.75


@dataclass
class Thresholds:
    max_latency_s: float = 10.0
    min_success_rate: float = 0.6
    eval_interval_steps: int = 2
    cooldown_steps: int = 1


@dataclass
class AgentConfig:
    language: str = "python"
    openai: OpenAIConfig = field(default_factory=OpenAIConfig)
    policy: PolicyConfig = field(default_factory=PolicyConfig)
    thresholds: Thresholds = field(default_factory=Thresholds)

    strategies: List[str] = field(
        default_factory=lambda: [
            "RuleBasedPlanner",
            "GithubRetriever",
            "OpenAIReasoner",
        ]
    )

    @staticmethod
    def load(path: str) -> "AgentConfig":
        with open(path, "r", encoding="utf-8") as f:
            data = yaml.safe_load(f) or {}
        cfg = AgentConfig()
        # shallow update for simplicity
        for k, v in data.items():
            if hasattr(cfg, k):
                if isinstance(getattr(cfg, k), (OpenAIConfig, PolicyConfig, Thresholds)):
                    inner = getattr(cfg, k)
                    for ik, iv in v.items():
                        setattr(inner, ik, iv)
                else:
                    setattr(cfg, k, v)
        return cfg


# -------- Results & Metrics ---------

@dataclass
class RunResult:
    output: str
    success_score: float
    latency_s: float
    strategy_name: str
    meta: Dict[str, Any] = field(default_factory=dict)


@dataclass
class StrategyMetrics:
    successes: int = 0
    attempts: int = 0
    total_latency_s: float = 0.0

    def success_rate(self) -> float:
        return (self.successes / max(1, self.attempts))

    def avg_latency(self) -> float:
        return (self.total_latency_s / max(1, self.attempts))


# -------- Strategy Interface ---------

class Strategy:
    name: str = "Base"

    def supports(self, task: str, config: AgentConfig) -> bool:
        return True

    def warmup(self, task: str, config: AgentConfig) -> None:
        return None

    def run(self, task: str, config: AgentConfig) -> RunResult:
        raise NotImplementedError

    def handoff_state(self) -> Dict[str, Any]:
        return {}

    def accept_handoff(self, state: Dict[str, Any]) -> None:
        return None


# --------- Strategies ---------

class RuleBasedPlanner(Strategy):
    name = "RuleBasedPlanner"

    def run(self, task: str, config: AgentConfig) -> RunResult:
        start = time.time()
        plan_lines = [
            f"Language: {config.language}",
            "Steps:",
            "1) Restate the task succinctly",
            "2) Identify inputs/outputs",
            "3) Draft an algorithm",
            "4) Implement minimal solution",
            "5) Add tests and edge cases",
            "6) Optimize if needed",
        ]
        output = "\n".join(plan_lines)
        latency = time.time() - start
        return RunResult(output=output, success_score=0.55, latency_s=latency, strategy_name=self.name)


class GithubRetriever(Strategy):
    name = "GithubRetriever"

    def supports(self, task: str, config: AgentConfig) -> bool:
        return httpx is not None

    def run(self, task: str, config: AgentConfig) -> RunResult:
        start = time.time()
        q = re.sub(r"\s+", "+", task.strip())
        url = f"https://api.github.com/search/code?q={q}+language:{config.language}&per_page=3"
        headers = {"Accept": "application/vnd.github+json"}
        token = os.getenv("GITHUB_TOKEN")
        if token:
            headers["Authorization"] = f"Bearer {token}"
        try:
            with httpx.Client(timeout=10.0) as client:
                resp = client.get(url, headers=headers)
                resp.raise_for_status()
                items = resp.json().get("items", [])
        except Exception as e:  # pragma: no cover
            items = []
        snippets: List[str] = []
        for it in items:
            repo = it.get("repository", {}).get("full_name", "?")
            path = it.get("path", "?")
            snippet = f"- {repo}/{path}"
            snippets.append(snippet)
        output = "GitHub examples (paths only):\n" + ("\n".join(snippets) if snippets else "None found or blocked by rate limit")
        latency = time.time() - start
        score = 0.6 if snippets else 0.4
        return RunResult(output=output, success_score=score, latency_s=latency, strategy_name=self.name)


class OpenAIReasoner(Strategy):
    name = "OpenAIReasoner"

    def supports(self, task: str, config: AgentConfig) -> bool:
        return config.openai.enabled and OpenAI is not None and (config.openai.api_key is not None)

    def run(self, task: str, config: AgentConfig) -> RunResult:
        start = time.time()
        try:
            client = OpenAI(api_key=config.openai.api_key, base_url=config.openai.base_url) if config.openai.base_url else OpenAI(api_key=config.openai.api_key)
            msg = [
                {"role": "system", "content": "You are a cautious, high-quality coding assistant. Only provide benign, safe help."},
                {"role": "user", "content": f"Task: {task}\nRespond concisely with steps and, if applicable, minimal example code in {config.language}."},
            ]
            resp = client.chat.completions.create(model=config.openai.model, messages=msg, temperature=0.2, max_tokens=600)
            content = resp.choices[0].message.content  # type: ignore
            output = content or ""
            score = 0.7
        except Exception as e:  # pragma: no cover
            output = f"OpenAI error: {e}"
            score = 0.4
        latency = time.time() - start
        return RunResult(output=output, success_score=score, latency_s=latency, strategy_name=self.name)


# -------- Registry ---------

STRATEGY_REGISTRY: Dict[str, Strategy] = {
    RuleBasedPlanner.name: RuleBasedPlanner(),
    GithubRetriever.name: GithubRetriever(),
    OpenAIReasoner.name: OpenAIReasoner(),
}


# -------- Agent ---------

@dataclass
class HistoryEntry:
    strategy: str
    success_score: float
    latency_s: float
    output_digest: str


class SelfLearningAgent:
    def __init__(self, config: AgentConfig) -> None:
        self.config = config
        self.metrics: Dict[str, StrategyMetrics] = {name: StrategyMetrics() for name in STRATEGY_REGISTRY.keys()}
        self.history: List[HistoryEntry] = []
        self._cooldown_remaining = 0

    # ---- core loop ----
    def run(self, task: str, max_steps: int = 10) -> RunResult:
        if not is_safe_task(task):
            return RunResult(output="Task blocked by safety filter.", success_score=0.0, latency_s=0.0, strategy_name="Safety")

        active = self._choose_strategy(task)
        active.warmup(task, self.config)
        last_result: Optional[RunResult] = None

        for step in range(max_steps):
            t0 = time.time()
            result = active.run(task, self.config)
            self._update_metrics(result)
            self._record_history(result)
            last_result = result

            # re-branch check
            if self._should_rebranch():
                if self._cooldown_remaining > 0:
                    self._cooldown_remaining -= 1
                else:
                    next_strategy = self._choose_strategy(task)
                    if next_strategy.name != active.name:
                        state = active.handoff_state()
                        next_strategy.accept_handoff(state)
                        active = next_strategy
                        self._cooldown_remaining = self.config.thresholds.cooldown_steps

            # stop early if score good enough
            if result.success_score >= 0.8:
                break

        return last_result or RunResult(output="No output", success_score=0.0, latency_s=0.0, strategy_name="None")

    # ---- policy selection ----
    def _choose_strategy(self, task: str) -> Strategy:
        candidates = [STRATEGY_REGISTRY[n] for n in self.config.strategies if STRATEGY_REGISTRY[n].supports(task, self.config)]
        if not candidates:
            return STRATEGY_REGISTRY["RuleBasedPlanner"]
        mode = self.config.policy.mode
        if mode == "rules":
            return self._rules_policy(candidates)
        if mode == "score":
            return self._score_policy(candidates)
        return self._bandit_policy(candidates)

    def _rules_policy(self, candidates: List[Strategy]) -> Strategy:
        for s in candidates:
            if s.name == "OpenAIReasoner":
                return s
        for s in candidates:
            if s.name == "GithubRetriever":
                return s
        return candidates[0]

    def _score_policy(self, candidates: List[Strategy]) -> Strategy:
        best_s = candidates[0]
        best_v = -1e9
        for s in candidates:
            m = self.metrics[s.name]
            value = (
                self.config.policy.success_weight * m.success_rate()
                - self.config.policy.latency_weight * m.avg_latency()
            )
            if value > best_v:
                best_v = value
                best_s = s
        return best_s

    def _bandit_policy(self, candidates: List[Strategy]) -> Strategy:
        if random.random() < self.config.policy.epsilon:
            return random.choice(candidates)
        # exploit
        scored: List[Tuple[float, Strategy]] = []
        for s in candidates:
            m = self.metrics[s.name]
            score = (
                self.config.policy.success_weight * m.success_rate()
                - self.config.policy.latency_weight * m.avg_latency()
            )
            scored.append((score, s))
        scored.sort(key=lambda x: x[0], reverse=True)
        return scored[0][1]

    # ---- evaluation ----
    def _should_rebranch(self) -> bool:
        th = self.config.thresholds
        recent = self.history[-th.eval_interval_steps :]
        if not recent:
            return False
        avg_score = sum(h.success_score for h in recent) / len(recent)
        avg_latency = sum(h.latency_s for h in recent) / len(recent)
        return (avg_score < th.min_success_rate) or (avg_latency > th.max_latency_s)

    def _update_metrics(self, result: RunResult) -> None:
        m = self.metrics[result.strategy_name]
        m.attempts += 1
        m.total_latency_s += result.latency_s
        if result.success_score >= 0.6:
            m.successes += 1

    def _record_history(self, result: RunResult) -> None:
        digest = hashlib.sha256(result.output.encode("utf-8")).hexdigest()[:16]
        self.history.append(
            HistoryEntry(
                strategy=result.strategy_name,
                success_score=result.success_score,
                latency_s=result.latency_s,
                output_digest=digest,
            )
        )


# -------- Minimal tests (manual) --------
if __name__ == "__main__":  # pragma: no cover
    cfg = AgentConfig()
    agent = SelfLearningAgent(cfg)
    print(agent.run("Write a Python function for binary search").output)