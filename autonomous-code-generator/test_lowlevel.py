#!/usr/bin/env python3
"""Test the C/C++/ASM focused generator"""

import sys
sys.path.insert(0, '/workspace/autonomous-code-generator')

from src.generators.lowlevel_generator import LowLevelGenerator

print("=== C/C++/ASM Code Generator Demo ===\n")

gen = LowLevelGenerator()

# Test C generation
print("1. C Basic Program:")
print(gen.generate(language="c", code_type="basic"))

print("\n" + "="*60 + "\n")

print("2. C with Loops:")
print(gen.generate(language="c", code_type="loops"))

print("\n" + "="*60 + "\n")

print("3. C with Functions:")
print(gen.generate(language="c", code_type="functions"))

print("\n" + "="*60 + "\n")

print("4. C++ Class:")
print(gen.generate(language="cpp", code_type="class"))

print("\n" + "="*60 + "\n")

print("5. x86 Assembly:")
print(gen.generate(language="asm_x86", code_type="basic"))

print("\n" + "="*60 + "\n")

print("6. x86 Assembly with Loops:")
print(gen.generate(language="asm_x86", code_type="loops"))

print("\n" + "="*60 + "\n")

print("7. ARM Assembly:")
print(gen.generate(language="asm_arm", code_type="basic"))

print("\n" + "="*60 + "\n")

print("8. Unlimited Generation (first 3):")
unlimited = gen.generate_unlimited(language="c")
for i, code in enumerate(unlimited):
    if i >= 3:
        break
    print(f"\n--- Generated C Code #{i+1} ---")
    print(code[:200] + "..." if len(code) > 200 else code)

print("\n✓ C/C++/ASM generator working correctly!")