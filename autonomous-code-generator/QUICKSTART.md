# Quick Start Guide

## What is this?

An autonomous code generator that can produce unlimited code focusing on:
- Alphabets (A-Z)
- Numbers (0-9)
- Basic programming constructs (brackets, operators, etc.)
- Assembly language (ASM) and other simple languages

## Simple Examples

### 1. Generate Assembly Code
```python
from src.generators.simple_generator import ASMGenerator

gen = ASMGenerator()
code = gen.generate(arch="x86", program_type="data")
print(code)
```

Output:
```asm
section .data
    alphabet db 'ABCDEFGHIJKLMNOPQRSTUVWXYZ', 0
    numbers db '0123456789', 0
    data0 db 'A0', 0
    data1 db 'B1', 0
    ...
```

### 2. Generate Alphanumeric Patterns
```python
from src.generators.simple_generator import SimpleGenerator

gen = SimpleGenerator()
code = gen.generate(pattern_type="pattern", pattern="ANAN")
```

Creates patterns like: `A1B2`, `X9Y0`, etc.

### 3. Unlimited Generation
```python
# Generate unlimited code
generator = gen.generate_unlimited()
for i, code in enumerate(generator):
    print(f"Code #{i}: {code}")
    # Press Ctrl+C to stop
```

## Supported Languages

- **Assembly (ASM)**: x86, x64, ARM, MIPS
- **C**: Basic programs with alphanumeric data
- **BASIC**: Simple line-numbered programs
- **Generic**: Pseudo-code patterns

## Key Features

- ✓ Focuses on A-Z and 0-9 patterns
- ✓ Simple, readable code generation
- ✓ No complex logic - just alphanumeric patterns
- ✓ Unlimited generation capability
- ✓ Multiple output formats

## Run the Demo

```bash
python3 simple_test.py
```

This shows:
1. ASM code with alphabet/number data
2. C code processing A-Z and 0-9
3. Random alphanumeric patterns
4. Custom pattern generation

## Use Cases

- Testing parsers with simple inputs
- Generating test data
- Creating alphanumeric sequences
- Learning basic ASM structure
- Pattern-based code generation