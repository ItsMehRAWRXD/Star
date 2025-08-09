# Auto Code Generator

Autonomous unlimited code generator CLI that continuously creates code files from simple templates across multiple languages. Zero external dependencies.

## Features
- Continuous or finite generation
- Seeded randomness for reproducibility
- Minimal templating: `{{ var }}` with filters: `snake`, `camel`, `kebab`, and `uuid`
- Supports multiple languages via file extension inference from `.tmpl` name (e.g., `foo.py.tmpl` -> `.py`)
- Safe write with unique file names and optional overwrite

## Run
You can run directly with Python (no install needed):
```bash
python3 -m auto_code_generator.cli generate --out ./out --count 5 --shuffle
```

## Templates
Templates are `.tmpl` files placed in a directory structure, for example:
```
auto_code_generator/
  templates/
    python/
      function.py.tmpl
      class.py.tmpl
    javascript/
      util.js.tmpl
```

Example variables you may use in templates: `function_name`, `class_name`, `params`, `return_value`, `docstring`, `init_params`, `method_name`, `method_params`. Unknown variables resolve to empty string.