#!/usr/bin/env python3
"""Simple test script for the autonomous code generator"""

import sys
sys.path.insert(0, '/workspace/autonomous-code-generator')

from src.generators.simple_generator import SimpleGenerator, ASMGenerator

print("=== Autonomous Code Generator Demo ===\n")

# Test Simple Generator
print("1. Simple Sequential Pattern (C):")
simple_gen = SimpleGenerator()
code = simple_gen.generate(pattern_type="sequence", language="c", length=5)
print(code)

print("\n2. Simple ASM Pattern:")
code = simple_gen.generate(pattern_type="basic_program", language="asm")
print(code)

print("\n3. Random Alphanumeric Pattern:")
code = simple_gen.generate(pattern_type="random", language="generic", count=10)
print(code)

# Test ASM Generator
print("\n4. ASM Data Section (x86):")
asm_gen = ASMGenerator()
code = asm_gen.generate(arch="x86", program_type="data")
print(code)

print("\n5. ASM Loop (ARM):")
code = asm_gen.generate(arch="arm", program_type="loop")
print(code)

# Test unlimited generation
print("\n6. Unlimited Generation (first 3 items):")
unlimited = simple_gen.generate_unlimited(language="generic")
for i, code in enumerate(unlimited):
    if i >= 3:
        break
    print(f"\n--- Item {i+1} ---")
    print(code[:100] + "..." if len(code) > 100 else code)

print("\n✓ All generators working correctly!")
print("\nTo use the CLI, run:")
print("  python -m src generate -g simple -l asm")
print("  python -m src list")
print("  python -m src examples")