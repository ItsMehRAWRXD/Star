#!/usr/bin/env python3
"""Example: Generate Assembly code with alphanumeric patterns"""

from src.core.generator_manager import GeneratorManager
from src.core.generator_registry import registry

# Auto-discover generators
registry.auto_discover("src.generators")

# Create manager
manager = GeneratorManager()

# Generate simple ASM code
print("=== Simple ASM Sequence ===")
asm_code = manager.generate("simple_generator.SimpleGenerator", 
                           pattern_type="sequence", 
                           language="asm")
print(asm_code)

print("\n=== ASM with Random Patterns ===")
asm_random = manager.generate("simple_generator.SimpleGenerator",
                             pattern_type="random",
                             language="asm",
                             count=1)
print(asm_random)

print("\n=== ASM Data Section ===")
asm_data = manager.generate("simple_generator.ASMGenerator",
                           arch="x86",
                           program_type="data")
print(asm_data)

print("\n=== ASM Loop Example ===")
asm_loop = manager.generate("simple_generator.ASMGenerator",
                           arch="x86", 
                           program_type="loop")
print(asm_loop)

# Generate multiple ASM files
print("\n=== Generating 5 ASM files ===")
results = manager.generate("simple_generator.ASMGenerator",
                          count=5,
                          arch="x86")
for i, code in enumerate(results):
    filename = f"examples/output/asm_{i:03d}.asm"
    print(f"Generated: {filename}")
    # In real usage, you would save to file here