import json
import os
from pathlib import Path
from typing import Optional

import typer
from rich import print

from autocode.agent import AutonomousAgent
from autocode.provider.openai_provider import OpenAIProvider
from autocode.provider.local_mock import LocalMockProvider

app = typer.Typer(add_completion=False, no_args_is_help=True)


def _resolve_provider(name: str, model: Optional[str]):
    name_lower = (name or os.getenv("AUTOCODE_PROVIDER", "openai")).lower()
    model_name = model or os.getenv("AUTOCODE_MODEL") or "gpt-4o-mini"

    if name_lower == "openai":
        return OpenAIProvider(model=model_name)
    if name_lower in ("local", "mock", "local-mock"):
        return LocalMockProvider()

    raise typer.BadParameter(f"Unknown provider: {name}")


@app.command()
def run(
    task: str = typer.Option(..., "--task", help="Task description for the generator"),
    outdir: Path = typer.Option(
        Path("generated"), "--outdir", help="Output directory for generated files"
    ),
    provider: str = typer.Option("openai", "--provider", help="LLM provider name"),
    model: Optional[str] = typer.Option(None, "--model", help="Model name for provider"),
    command: Optional[str] = typer.Option(
        None, "--command", help="Command to run after each generation step (e.g., tests)"
    ),
    max_steps: int = typer.Option(5, "--max-steps", min=1, help="Maximum loop iterations"),
    dry_run: bool = typer.Option(
        False, "--dry-run", help="Plan and generate but do not write or execute"
    ),
    plan_first: bool = typer.Option(True, "--plan-first/--no-plan-first", help="Whether to ask for a plan before generation"),
):
    outdir.mkdir(parents=True, exist_ok=True)

    llm = _resolve_provider(provider, model)
    agent = AutonomousAgent(llm_provider=llm)

    print(f"[bold]Task[/bold]: {task}")
    print(f"[bold]Output dir[/bold]: {outdir}")
    if command:
        print(f"[bold]Command[/bold]: {command}")

    agent.run(
        task=task,
        output_dir=outdir,
        run_command=command,
        max_steps=max_steps,
        dry_run=dry_run,
        plan_first=plan_first,
    )


if __name__ == "__main__":
    app()