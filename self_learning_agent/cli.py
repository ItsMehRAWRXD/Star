import argparse
import os
import sys
from .core import AgentConfig, SelfLearningAgent


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="self_learning_agent",
        description="Safe self-learning task agent with retrieval and assisted reasoning",
    )
    sub = parser.add_subparsers(dest="cmd", required=True)

    run = sub.add_parser("run", help="Run the agent on a task")
    run.add_argument("--task", required=True, help="Natural language task description")
    run.add_argument("--language", default="python", help="Preferred language for code tasks")
    run.add_argument("--policy", choices=["rules", "score", "bandit"], default="bandit")
    run.add_argument("--config", help="Path to YAML config to override defaults")
    run.add_argument("--max-steps", type=int, default=12)
    run.add_argument("--openai-model", default=os.getenv("OPENAI_MODEL", "gpt-4o-mini"))
    run.add_argument("--no-openai", action="store_true", help="Disable OpenAI usage")

    return parser


def main(argv=None) -> int:
    parser = build_parser()
    args = parser.parse_args(argv)

    if args.cmd == "run":
        config = AgentConfig.load(args.config) if args.config else AgentConfig()
        config.policy.mode = args.policy
        config.language = args.language
        if args.no_openai:
            config.openai.enabled = False
        config.openai.model = getattr(args, "openai_model", config.openai.model)

        agent = SelfLearningAgent(config)
        result = agent.run(args.task, max_steps=args.max_steps)
        sys.stdout.write(result.output + "\n")
        return 0

    return 0


if __name__ == "__main__":
    raise SystemExit(main())