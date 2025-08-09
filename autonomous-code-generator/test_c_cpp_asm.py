#!/usr/bin/env python3
"""Standalone C/C++/ASM code generator demonstration"""

import random
import string

class CppAsmGenerator:
    """Simple C/C++/ASM generator focused on alphanumeric patterns"""
    
    def __init__(self):
        self.alphabet = string.ascii_uppercase
        self.numbers = string.digits
        
    def generate_c_basic(self):
        """Generate basic C program"""
        return """#include <stdio.h>

int main() {
    char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char numbers[] = "0123456789";
    
    // Variables with A-Z and 0-9
    int a0 = 10;
    char b1 = 'B';
    int c2 = 20;
    char d3 = 'D';
    
    // Print alphabet and numbers
    printf("Alphabet: %s\\n", alphabet);
    printf("Numbers: %s\\n", numbers);
    
    // Print combinations
    for (int i = 0; i < 10; i++) {
        printf("%c%d ", alphabet[i], i);
    }
    printf("\\n");
    
    return 0;
}"""

    def generate_c_loops(self):
        """Generate C with loops"""
        return """#include <stdio.h>

int main() {
    // Loop A-Z
    for (int i = 0; i < 26; i++) {
        printf("%c ", 'A' + i);
    }
    printf("\\n");
    
    // Loop 0-9
    for (int i = 0; i < 10; i++) {
        printf("%d ", i);
    }
    printf("\\n");
    
    // Nested loops for combinations
    for (int a = 0; a < 5; a++) {
        for (int n = 0; n < 5; n++) {
            printf("%c%d ", 'A' + a, n);
        }
        printf("\\n");
    }
    
    return 0;
}"""

    def generate_cpp(self):
        """Generate C++ code"""
        return """#include <iostream>
#include <vector>

class AlphaNumeric {
private:
    std::vector<char> letters;
    std::vector<int> numbers;
    
public:
    AlphaNumeric() {
        // Initialize A-Z
        for (int i = 0; i < 26; i++) {
            letters.push_back('A' + i);
        }
        // Initialize 0-9
        for (int i = 0; i < 10; i++) {
            numbers.push_back(i);
        }
    }
    
    void display() {
        std::cout << "Letters: ";
        for (char c : letters) {
            std::cout << c << " ";
        }
        std::cout << std::endl;
        
        std::cout << "Numbers: ";
        for (int n : numbers) {
            std::cout << n << " ";
        }
        std::cout << std::endl;
    }
};

int main() {
    AlphaNumeric an;
    an.display();
    
    // Print A0, B1, C2...
    for (int i = 0; i < 10; i++) {
        std::cout << (char)('A' + i) << i << " ";
    }
    std::cout << std::endl;
    
    return 0;
}"""

    def generate_asm_x86(self):
        """Generate x86 Assembly"""
        return """section .data
    alphabet db 'ABCDEFGHIJKLMNOPQRSTUVWXYZ', 0
    numbers db '0123456789', 0
    msg db 'A0B1C2D3E4F5G6H7I8J9', 0
    len equ $ - msg

section .bss
    buffer resb 32

section .text
    global _start

_start:
    ; Process alphabet
    mov esi, alphabet       ; source = alphabet
    mov ecx, 26            ; counter = 26 letters
    
process_alphabet:
    mov al, [esi]          ; load letter
    inc esi                ; next letter
    loop process_alphabet  ; loop 26 times
    
    ; Process numbers
    mov esi, numbers       ; source = numbers
    mov ecx, 10           ; counter = 10 digits
    
process_numbers:
    mov al, [esi]         ; load digit
    inc esi               ; next digit
    loop process_numbers  ; loop 10 times
    
    ; Print message (sys_write)
    mov eax, 4            ; syscall number for write
    mov ebx, 1            ; file descriptor (stdout)
    mov ecx, msg          ; message to write
    mov edx, len          ; message length
    int 0x80              ; call kernel
    
    ; Exit (sys_exit)
    mov eax, 1            ; syscall number for exit
    xor ebx, ebx          ; exit status 0
    int 0x80              ; call kernel"""

    def generate_asm_x64(self):
        """Generate x64 Assembly"""
        return """section .data
    alphabet db 'ABCDEFGHIJKLMNOPQRSTUVWXYZ', 0
    numbers db '0123456789', 0
    newline db 10

section .text
    global _start

_start:
    ; x64 uses different registers and syscall convention
    
    ; Print alphabet
    mov rax, 1              ; sys_write
    mov rdi, 1              ; stdout
    mov rsi, alphabet       ; buffer
    mov rdx, 26             ; count
    syscall
    
    ; Print newline
    mov rax, 1
    mov rdi, 1
    mov rsi, newline
    mov rdx, 1
    syscall
    
    ; Print numbers
    mov rax, 1
    mov rdi, 1
    mov rsi, numbers
    mov rdx, 10
    syscall
    
    ; Exit
    mov rax, 60             ; sys_exit
    xor rdi, rdi            ; exit code 0
    syscall"""

    def generate_asm_arm(self):
        """Generate ARM Assembly"""
        return """.data
alphabet: .ascii "ABCDEFGHIJKLMNOPQRSTUVWXYZ\\0"
numbers: .ascii "0123456789\\0"
msg: .ascii "A0B1C2D3E4\\n\\0"

.text
.global _start

_start:
    @ Load alphabet address
    ldr r0, =alphabet
    mov r1, #0              @ counter
    
loop_alphabet:
    cmp r1, #26             @ compare with 26
    bge done_alphabet       @ branch if >= 26
    
    ldrb r2, [r0, r1]       @ load byte at alphabet[r1]
    add r1, r1, #1          @ increment counter
    b loop_alphabet         @ branch back to loop
    
done_alphabet:
    @ Load numbers address
    ldr r0, =numbers
    mov r1, #0              @ reset counter
    
loop_numbers:
    cmp r1, #10             @ compare with 10
    bge done_numbers        @ branch if >= 10
    
    ldrb r2, [r0, r1]       @ load byte at numbers[r1]
    add r1, r1, #1          @ increment counter
    b loop_numbers          @ branch back to loop
    
done_numbers:
    @ Exit program
    mov r7, #1              @ exit syscall
    swi 0                   @ software interrupt"""

    def generate_random_c(self):
        """Generate C with random alphanumeric variables"""
        vars = []
        for i in range(5):
            letter = random.choice(self.alphabet)
            num = random.choice(self.numbers)
            vars.append(f"    int {letter.lower()}{num} = {random.randint(0, 99)};")
        
        return f"""#include <stdio.h>

int main() {{
    // Random alphanumeric variables
{chr(10).join(vars)}
    
    // Process A-Z and 0-9
    for (int i = 0; i < 10; i++) {{
        printf("%c%d ", 'A' + i, i);
    }}
    printf("\\n");
    
    return 0;
}}"""

# Run demonstrations
gen = CppAsmGenerator()

print("=== C/C++/ASM Code Generator ===\n")

print("1. Basic C Program:")
print(gen.generate_c_basic())

print("\n" + "="*60 + "\n")

print("2. C with Loops:")
print(gen.generate_c_loops())

print("\n" + "="*60 + "\n")

print("3. C++ Class:")
print(gen.generate_cpp())

print("\n" + "="*60 + "\n")

print("4. x86 Assembly:")
print(gen.generate_asm_x86())

print("\n" + "="*60 + "\n")

print("5. x64 Assembly:")
print(gen.generate_asm_x64())

print("\n" + "="*60 + "\n")

print("6. ARM Assembly:")
print(gen.generate_asm_arm())

print("\n" + "="*60 + "\n")

print("7. Random C Program:")
print(gen.generate_random_c())

print("\n✓ All C/C++/ASM code generation working!")