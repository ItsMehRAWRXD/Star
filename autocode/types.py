from typing import List, TypedDict


class FileSpec(TypedDict):
    path: str
    content: str


class GenerationBundle(TypedDict, total=False):
    files: List[FileSpec]
    run_command: str
    finish: bool
    notes: str