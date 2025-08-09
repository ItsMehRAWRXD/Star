# Self-Learning Task Agent (Benign)

This module provides a safe, generic self-learning task agent with:

- Strategy-based planning and execution
- Retrieval of public, permissive-licensed examples from GitHub search
- Optional OpenAI-assisted reasoning (chat/completions)
- Self-observation logs and metrics
- Policy-based re-branching when underperforming

Safety: Do not use this with offensive, malware, or prohibited tasks. The agent filters queries and blocks unsafe usage.

## Quickstart

1. Create and activate a virtualenv
2. Install requirements:

```bash
pip install -r self_learning_agent/requirements.txt
```

3. Set environment variables if using OpenAI:

```bash
export OPENAI_API_KEY=...
```

4. Run help:

```bash
python -m self_learning_agent --help
```

5. Example run:

```bash
python -m self_learning_agent run \
  --task "Write a Python function to merge overlapping intervals" \
  --language python \
  --policy bandit
```

## Config

You can pass a YAML config via `--config path.yaml` overriding:

- strategies: enabled strategies and their configs
- policy: rules/score/bandit weights
- thresholds: failure, latency, success rate targets
- exploration: epsilon schedule
- rebranch: evaluation window, cooldowns

## License

MIT (for this submodule only). Use responsibly.