# Self-Learning Task Agent (Benign) - C++

This is a safe, generic self-learning task agent implemented in modern C++17.
It supports:

- Strategy-based planning and execution
- Retrieval of public GitHub code search metadata (paths only)
- Optional OpenAI-assisted reasoning via HTTPS (requires env var)
- Self-observation metrics and policy-based re-branching

Safety: Do not use this for offensive or prohibited tasks. The agent includes a simple content filter to block unsafe requests.

## Build

```bash
make -C self_learning_agent
```

## Run

```bash
./self_learning_agent/bin/agent --help
./self_learning_agent/bin/agent run --task "Write a C++ function to merge intervals" --policy bandit
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