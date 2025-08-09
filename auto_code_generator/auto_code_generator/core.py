from __future__ import annotations

import re
import uuid
from dataclasses import dataclass
from pathlib import Path
from typing import Dict, List, Optional

DEFAULT_TEMPLATES_DIR = Path(__file__).parent / "templates"


def discover_templates(root: Path) -> List[Path]:
    """Return list of template file paths relative to the root.

    Filters for files ending in .tmpl.
    """
    root = root.resolve()
    results: List[Path] = []
    for path in root.rglob("*.tmpl"):
        results.append(path.relative_to(root))
    return results


@dataclass
class RenderContext:
    values: Dict[str, object]

    @staticmethod
    def empty() -> "RenderContext":
        return RenderContext(values={})


class TemplateLoader:
    def __init__(self, user_templates_dir: Optional[Path] = None) -> None:
        self.templates_root = (user_templates_dir or DEFAULT_TEMPLATES_DIR).resolve()

    def read_text(self, rel_path: Path) -> str:
        full_path = self.templates_root / rel_path
        return full_path.read_text(encoding="utf-8")


class SimpleTemplateEngine:
    """Very small template engine supporting {{ var }} and filters like {{ var|snake }}.

    Supported filters: snake, camel, kebab. Special value: {{ uuid }}.
    """

    TOKEN_RE = re.compile(r"\{\{\s*([^}]+?)\s*\}\}")

    def render(self, template_text: str, context: Dict[str, object]) -> str:
        def replace(match: re.Match[str]) -> str:
            expr = match.group(1)
            parts = [p.strip() for p in expr.split("|")]
            value_expr = parts[0]
            value: str
            if value_expr == "uuid":
                value = str(uuid.uuid4())
            else:
                # Only simple identifiers allowed
                value_raw = context.get(value_expr, "")
                value = str(value_raw)
            for filt in parts[1:]:
                if filt == "snake":
                    value = to_snake_case(value)
                elif filt == "camel":
                    value = to_camel_case(value)
                elif filt == "kebab":
                    value = to_kebab_case(value)
                else:
                    # unknown filter: ignore
                    continue
            return value

        return self.TOKEN_RE.sub(replace, template_text)


class CodeWriter:
    def __init__(self, out_dir: Path, overwrite: bool = False, ext_override: Optional[str] = None) -> None:
        self.out_dir = out_dir.resolve()
        self.overwrite = overwrite
        if ext_override is not None and not ext_override.startswith("."):
            ext_override = "." + ext_override
        self.ext_override = ext_override

    def write_unique(self, content: str, template_rel_path: Path) -> Path:
        ext = self.ext_override or infer_extension_from_template(template_rel_path)
        base_name = derive_output_basename(template_rel_path)
        safe_base = to_kebab_case(base_name)

        attempt = 0
        while True:
            unique_suffix = f"-{uuid.uuid4().hex[:8]}" if attempt > 0 else ""
            filename = f"{safe_base}{unique_suffix}{ext}"
            target = self.out_dir / filename
            if self.overwrite or not target.exists():
                self._write(target, content)
                return target
            attempt += 1

    def _write(self, path: Path, content: str) -> None:
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(content, encoding="utf-8")


def infer_extension_from_template(template_rel_path: Path) -> str:
    name = template_rel_path.name
    # common patterns: something.py.tmpl -> .py; util.js.tmpl -> .js
    parts = name.split(".")
    if len(parts) >= 3 and parts[-1] == "tmpl":
        return "." + parts[-2]
    return ".txt"


def derive_output_basename(template_rel_path: Path) -> str:
    """Derive output base filename by stripping the last two suffixes when the last is .tmpl."""
    name = template_rel_path.name
    suffixes = template_rel_path.suffixes
    if len(suffixes) >= 2 and suffixes[-1] == ".tmpl":
        trim_len = len(suffixes[-1]) + len(suffixes[-2])
        base = name[:-trim_len]
    else:
        base = template_rel_path.stem
    return base

_s1 = re.compile(r"[^a-zA-Z0-9]+")
_s2 = re.compile(r"(_|\s)+")


def to_kebab_case(value: str) -> str:
    value = _s1.sub(" ", value).strip().lower()
    value = _s2.sub("-", value)
    return value


def to_snake_case(value: str) -> str:
    value = _s1.sub(" ", value).strip().lower()
    value = _s2.sub("_", value)
    return value


def to_camel_case(value: str) -> str:
    parts = _s1.sub(" ", value).strip().split()
    if not parts:
        return ""
    return parts[0].lower() + "".join(p.capitalize() for p in parts[1:])