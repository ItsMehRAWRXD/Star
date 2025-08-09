# C/C++/ASM Code Generator

An autonomous code generator specifically designed for generating C, C++, and Assembly language code with a focus on alphanumeric patterns (A-Z, 0-9).

## What It Generates

### C Code
- Basic programs with alphabet/number arrays
- Loop structures processing A-Z and 0-9
- Functions for alphanumeric conversion
- Arrays and data structures
- Structs with alphanumeric data

### C++ Code
- Classes handling alphabets and numbers
- STL containers with alphanumeric data
- Modern C++ features (auto, lambdas)
- Object-oriented alphanumeric processing

### Assembly Code (x86/x64/ARM)
- Data sections with alphabet/number strings
- Loops processing A-Z and 0-9
- System calls for I/O
- Register operations on alphanumeric data
- Cross-architecture support

## Example Output

### C Program
```c
#include <stdio.h>

int main() {
    char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char numbers[] = "0123456789";
    
    for (int i = 0; i < 10; i++) {
        printf("%c%d ", alphabet[i], i);
    }
    printf("\n");
    
    return 0;
}
```

### x86 Assembly
```asm
section .data
    alphabet db 'ABCDEFGHIJKLMNOPQRSTUVWXYZ', 0
    numbers db '0123456789', 0
    msg db 'A0B1C2D3E4F5G6H7I8J9', 0
    len equ $ - msg

section .text
    global _start

_start:
    mov esi, alphabet
    mov ecx, 26
    ; Process alphabet...
```

### C++ Class
```cpp
class AlphaNumeric {
private:
    std::vector<char> letters;
    std::vector<int> numbers;
public:
    AlphaNumeric() {
        for (int i = 0; i < 26; i++) {
            letters.push_back('A' + i);
        }
    }
};
```

## Key Features

- **Pure Code Generation**: No complex logic, just C/C++/ASM code
- **Alphanumeric Focus**: A-Z, 0-9, and basic programming constructs
- **Multiple Architectures**: x86, x64, ARM assembly support
- **Unlimited Generation**: Can generate infinite variations
- **Real Compilable Code**: All generated code is syntactically correct

## Use Cases

- Learning C/C++/ASM syntax
- Testing compilers/assemblers
- Generating test data
- Code pattern examples
- Educational demonstrations

## Quick Test

```bash
python3 test_c_cpp_asm.py
```

This will generate examples of:
1. Basic C programs
2. C with loops and functions
3. C++ classes
4. x86/x64/ARM Assembly
5. Random alphanumeric programs