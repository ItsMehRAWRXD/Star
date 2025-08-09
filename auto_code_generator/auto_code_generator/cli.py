from __future__ import annotations

import argparse
import random
import sys
import time
import uuid
from pathlib import Path
from typing import Optional
import re

from .core import (
    CodeWriter,
    RenderContext,
    SimpleTemplateEngine,
    TemplateLoader,
    discover_templates,
)


def parse_args(argv: Optional[list[str]] = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(prog="auto-code-gen", description="Auto Code Generator CLI")
    parser.add_argument("command", choices=["generate"], help="Command to run")
    parser.add_argument("--templates", dest="templates_dir", type=Path, default=None, help="Directory with templates (.tmpl). Defaults to built-in.")
    parser.add_argument("--context-json", dest="context_json", type=Path, default=None, help="Path to JSON file with variables exposed to templates.")
    parser.add_argument("--out", dest="out_dir", type=Path, required=True, help="Output directory for generated code.")
    parser.add_argument("--count", dest="count", type=int, default=1, help="Number of files to generate (ignored if --infinite).")
    parser.add_argument("--infinite", action="store_true", help="Generate indefinitely until interrupted.")
    parser.add_argument("--delay", dest="delay", type=float, default=0.0, help="Seconds to sleep between generations.")
    parser.add_argument("--shuffle", action="store_true", help="Shuffle templates before each selection.")
    parser.add_argument("--overwrite", action="store_true", help="Overwrite files if they already exist.")
    parser.add_argument("--seed", dest="seed", type=int, default=None, help="Seed for pseudo-randomness.")
    parser.add_argument("--ext", dest="ext_override", type=str, default=None, help="Force output file extension (e.g., .py). Defaults to template-driven.")
    parser.add_argument("--quiet", action="store_true", help="Suppress output messages.")
    parser.add_argument("--var", dest="vars", action="append", default=None, help="Template variable override in key=value form. Can be repeated.")
    parser.add_argument("--strip-word", dest="strip_words", action="append", default=None, help="Word to remove from rendered output. Can be repeated. Defaults to removing 'rent' and 'rot'.")
    parser.add_argument("--strip-mode", dest="strip_mode", choices=["whole", "substr"], default="whole", help="Whole word removal or substring removal.")
    return parser.parse_args(argv)


def main(argv: Optional[list[str]] = None) -> None:
    args = parse_args(argv)

    if args.seed is not None:
        random.seed(args.seed)

    out_dir: Path = args.out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    loader = TemplateLoader(args.templates_dir)
    engine = SimpleTemplateEngine()
    base_values = RenderContext.from_json(args.context_json).values if args.context_json else {}
    # Merge CLI --var overrides
    if args.vars:
        for item in args.vars:
            if "=" in item:
                k, v = item.split("=", 1)
                base_values[k.strip()] = v
    # Provide light defaults
    base_values.setdefault("target", "World")
    base_values.setdefault("event", "app_event")
    base_values.setdefault("user_id", f"user_{uuid.uuid4().hex[:8]}")
    context = RenderContext(values=base_values)
    writer = CodeWriter(out_dir=out_dir, overwrite=args.overwrite, ext_override=args.ext_override)

    # Configure stripping defaults if none provided
    strip_words = args.strip_words[:] if args.strip_words else ["rent", "rot"]
    strip_mode = args.strip_mode

    available_templates = discover_templates(loader.templates_root)
    if not available_templates:
        print("No templates found.", file=sys.stderr)
        sys.exit(1)

    def strip_content(text: str) -> str:
        if not strip_words:
            return text
        if strip_mode == "whole":
            # Whole-word, case-insensitive
            pattern = r"\b(" + "|".join(re.escape(w) for w in strip_words) + r")\b"
            return re.sub(pattern, "", text, flags=re.IGNORECASE)
        # Substring, case-insensitive for each word
        result = text
        for w in strip_words:
            result = re.sub(re.escape(w), "", result, flags=re.IGNORECASE)
        return result

    def render_one() -> Path:
        candidates = available_templates[:]
        if args.shuffle:
            random.shuffle(candidates)
        template_path = random.choice(candidates)
        template_text = loader.read_text(template_path)
        rendered = engine.render(template_text, context.values)
        rendered = strip_content(rendered)
        output_path = writer.write_unique(rendered, template_path)
        if not args.quiet:
            print(f"Generated: {output_path}")
        return output_path

    if args.command == "generate":
        if args.infinite:
            try:
                while True:
                    render_one()
                    if args.delay > 0:
                        time.sleep(args.delay)
            except KeyboardInterrupt:
                if not args.quiet:
                    print("Stopped.")
        else:
            for _ in range(max(0, args.count)):
                render_one()


if __name__ == "__main__":
    main()