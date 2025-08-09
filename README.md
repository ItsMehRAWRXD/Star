# Autonomous Code Generator

An extensible, CLI-based autonomous code generator that plans, generates, writes files, and executes validation commands in a loop.

## Features
- Task-driven loop: plan → generate files → run command/tests → iterate
- Pluggable LLM providers (OpenAI included). Add your own providers easily
- Safe file writing within a specified output directory
- Simple JSON file-spec protocol from the model response

## Quickstart

1) Install dependencies:

```bash
python -m pip install -r requirements.txt
```

2) Configure provider (OpenAI example):

- Set environment variable: `export OPENAI_API_KEY=...`
- Optional: choose model via `--model` CLI flag or `AUTOCODE_MODEL` env var

3) Run a task (generates into `./generated` by default):

```bash
python -m autocode.cli run --task "Build a Python CLI that prints Fibonacci numbers" --outdir ./generated --max-steps 3 --provider openai --model gpt-4o-mini
```

4) Inspect results in `./generated`. If you specify `--command "pytest -q"`, the agent will attempt to make tests pass.

## CLI

```bash
python -m autocode.cli --help
python -m autocode.cli run --help
```

## JSON File Spec Output (from the model)
The generator expects the model to return a JSON object of the following shape:

```json
{
  "files": [
    {"path": "package/__init__.py", "content": "..."},
    {"path": "package/main.py", "content": "..."}
  ],
  "run_command": "pytest -q",
  "finish": false,
  "notes": "Optional reasoning or guidance"
}
```

- `files`: list of files to write relative to the output directory
- `run_command`: optional command to run after writing files
- `finish`: if true, the loop stops
- `notes`: optional text for human readability

## Development
- Add new providers under `autocode/provider/`
- Prompts live in `autocode/prompts/`

## Safety
This tool writes only within the specified output directory. Review generated code before executing. Do not use with harmful or illegal tasks.