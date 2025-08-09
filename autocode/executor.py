import subprocess
from pathlib import Path
from typing import Tuple


def run_shell_command(command: str, cwd: Path) -> Tuple[int, str]:
    try:
        completed = subprocess.run(
            command,
            shell=True,
            cwd=str(cwd),
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
            check=False,
        )
        return completed.returncode, completed.stdout
    except Exception as exc:
        return 1, f"Exception while running command: {exc}"