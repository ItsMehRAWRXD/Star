from __future__ import annotations

import argparse
import random
import sys
import time
from pathlib import Path
from typing import Optional

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
    return parser.parse_args(argv)


def main(argv: Optional[list[str]] = None) -> None:
    args = parse_args(argv)

    if args.seed is not None:
        random.seed(args.seed)

    out_dir: Path = args.out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    loader = TemplateLoader(args.templates_dir)
    engine = SimpleTemplateEngine()
    context = RenderContext.from_json(args.context_json) if args.context_json else RenderContext.empty()
    writer = CodeWriter(out_dir=out_dir, overwrite=args.overwrite, ext_override=args.ext_override)

    available_templates = discover_templates(loader.templates_root)
    if not available_templates:
        print("No templates found.", file=sys.stderr)
        sys.exit(1)

    def render_one() -> Path:
        candidates = available_templates[:]
        if args.shuffle:
            random.shuffle(candidates)
        template_path = random.choice(candidates)
        template_text = loader.read_text(template_path)
        rendered = engine.render(template_text, context.values)
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