from pathlib import Path
from typing import Iterable

from autocode.types import FileSpec


def write_files_safely(root: Path, files: Iterable[FileSpec]) -> None:
    root = root.resolve()
    for spec in files:
        rel = Path(spec["path"]).as_posix().lstrip("/")
        target = (root / rel).resolve()
        if not str(target).startswith(str(root)):
            raise ValueError(f"Refusing to write outside root: {target}")
        target.parent.mkdir(parents=True, exist_ok=True)
        target.write_text(spec["content"], encoding="utf-8")