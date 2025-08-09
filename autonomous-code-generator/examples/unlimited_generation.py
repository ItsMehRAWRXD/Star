#!/usr/bin/env python3
"""Example: Unlimited code generation"""

from src.core.generator_manager import GeneratorManager
from src.core.generator_registry import registry
import time

# Auto-discover generators
registry.auto_discover("src.generators")

# Create manager
manager = GeneratorManager()

print("Starting unlimited code generation...")
print("Press Ctrl+C to stop\n")

try:
    # Generate unlimited simple patterns
    generator = manager.generate("simple_generator.SimpleGenerator",
                               count=-1,  # Unlimited
                               pattern_type="pattern",
                               language="generic")
    
    for i, code in enumerate(generator):
        print(f"\n=== Generated Pattern #{i+1} ===")
        print(code)
        print("-" * 40)
        
        # Small delay to make output readable
        time.sleep(0.5)
        
        # Save every 10th generation
        if (i + 1) % 10 == 0:
            print(f"\n[Progress: Generated {i+1} patterns]")
            
except KeyboardInterrupt:
    print("\n\nGeneration stopped by user.")
    
print("\nExample of unlimited ASM generation:")

try:
    # Generate unlimited ASM code
    asm_gen = manager.generate("simple_generator.ASMGenerator",
                              count=-1,
                              arch="x86")
    
    for i, code in enumerate(asm_gen):
        if i >= 5:  # Just show first 5 for demo
            break
        print(f"\n=== ASM Code #{i+1} ===")
        print(code[:200] + "..." if len(code) > 200 else code)
        
except KeyboardInterrupt:
    pass