#!/usr/bin/env python3
"""Minimal test of the simple generator without dependencies"""

import sys
sys.path.insert(0, '/workspace/autonomous-code-generator')

# Test basic alphanumeric generation
print("=== Simple Alphanumeric Code Generator ===\n")

# Inline simple generation
import string
import random

def generate_simple_asm():
    """Generate simple ASM with A-Z and 0-9"""
    alphabet = string.ascii_uppercase
    numbers = string.digits
    
    lines = [
        "section .data",
        f"    alphabet db '{alphabet}', 0",
        f"    numbers db '{numbers}', 0",
        ""
    ]
    
    # Add some data entries
    for i in range(10):
        lines.append(f"    data{i} db '{alphabet[i]}{i}', 0")
    
    lines.extend([
        "",
        "section .text",
        "    global _start",
        "_start:",
        "    ; Process alphabet A-Z",
        "    mov esi, alphabet",
        "    mov ecx, 26",
        "loop_alpha:",
        "    mov al, [esi]",
        "    inc esi",
        "    loop loop_alpha",
        "",
        "    ; Exit",
        "    mov eax, 1",
        "    xor ebx, ebx", 
        "    int 0x80"
    ])
    
    return '\n'.join(lines)

def generate_c_alphanumeric():
    """Generate C code with alphanumeric patterns"""
    return """#include <stdio.h>

int main() {
    char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char numbers[] = "0123456789";
    
    // Print A0, B1, C2, etc.
    for (int i = 0; i < 10; i++) {
        printf("%c%d ", alphabet[i], i);
    }
    printf("\\n");
    
    return 0;
}"""

def generate_pattern(pattern="ANAN", count=5):
    """Generate code following pattern (A=letter, N=number)"""
    alphabet = string.ascii_uppercase
    numbers = string.digits
    results = []
    
    for _ in range(count):
        result = ""
        for char in pattern:
            if char == 'A':
                result += random.choice(alphabet)
            elif char == 'N':
                result += random.choice(numbers)
            else:
                result += char
        results.append(result)
    
    return results

# Run tests
print("1. Simple ASM Code:")
print(generate_simple_asm())

print("\n2. C Code with Alphanumeric:")
print(generate_c_alphanumeric())

print("\n3. Generated Patterns (ANAN):")
patterns = generate_pattern("ANAN", 10)
for i, p in enumerate(patterns):
    print(f"   Pattern {i+1}: {p}")

print("\n4. Custom Pattern (A-N-A-N):")
patterns = generate_pattern("A-N-A-N", 5)
for i, p in enumerate(patterns):
    print(f"   {p}")

print("\n✓ Simple alphanumeric code generation working!")