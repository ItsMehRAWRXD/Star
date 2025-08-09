import json
from dataclasses import dataclass
from pathlib import Path
from typing import Optional

from rich import print

from autocode.executor import run_shell_command
from autocode.fs import write_files_safely
from autocode.provider.base import LanguageModelProvider
from autocode.types import FileSpec, GenerationBundle


SYSTEM_MESSAGE = (
    "You are an autonomous senior software engineer."
    " Always return JSON as specified."
)

PLAN_PROMPT = (
    "You will receive a task. Produce a short plan with numbered steps and end with 'READY'.\n"
    "Focus on clear milestones and a minimal viable first iteration."
)

GENERATE_PROMPT_TEMPLATE = (
    "Task: {task}\n\n"
    "Respond with a JSON object with keys: files (list of objects with path, content),"
    " optional run_command (string), finish (boolean), and notes (string).\n"
    "Only return JSON.\n"
)


@dataclass
class AutonomousAgent:
    llm_provider: LanguageModelProvider

    def run(
        self,
        task: str,
        output_dir: Path,
        run_command: Optional[str] = None,
        max_steps: int = 5,
        dry_run: bool = False,
        plan_first: bool = True,
    ) -> None:
        if plan_first:
            plan = self.llm_provider.complete(PLAN_PROMPT + "\n\nTask: " + task, system=SYSTEM_MESSAGE)
            print("[bold green]Plan[/bold green]:\n" + plan)

        for step_idx in range(1, max_steps + 1):
            print(f"\n[bold]Step {step_idx}/{max_steps}[/bold]")
            prompt = GENERATE_PROMPT_TEMPLATE.format(task=task)
            raw = self.llm_provider.complete(prompt, system=SYSTEM_MESSAGE)

            bundle = self._parse_bundle(raw)
            if not bundle:
                print("[red]Model output was not valid JSON. Skipping this step.[/red]")
                continue

            files = bundle.get("files", []) or []
            step_run_command = bundle.get("run_command") or run_command
            finish = bool(bundle.get("finish", False))
            notes = bundle.get("notes")

            if notes:
                print("[bold cyan]Notes[/bold cyan]:\n" + notes)

            if files:
                if dry_run:
                    print("[yellow]Dry-run: would write files:[/yellow]")
                    for f in files:
                        print(f"- {f['path']} ({len(f['content'])} bytes)")
                else:
                    write_files_safely(output_dir, files)
                    print(f"Wrote {len(files)} file(s) to {output_dir}")
            else:
                print("No files proposed in this step.")

            if step_run_command and not dry_run:
                print(f"Running command: {step_run_command}")
                code, out = run_shell_command(step_run_command, cwd=output_dir)
                print(out)
                if code == 0:
                    print("[green]Command succeeded.[/green]")
                else:
                    print(f"[red]Command failed with exit code {code}[/red]")

            if finish:
                print("[bold green]Finish flag received. Stopping.[/bold green]")
                break

    def _parse_bundle(self, raw: str) -> Optional[GenerationBundle]:
        try:
            obj = json.loads(raw)
            if not isinstance(obj, dict):
                return None
            # basic shape check
            _ = obj.get("files")
            return obj  # type: ignore[return-value]
        except Exception:
            return None