"""Low-level code generator for C/C++/ASM"""

from typing import Dict, Any, List, Generator
import random
import string

from ..core.base_generator import BaseGenerator


class LowLevelGenerator(BaseGenerator):
    """Generator focused on C/C++/ASM code generation"""
    
    def _setup(self):
        """Setup character sets and code patterns"""
        self.alphabet = string.ascii_uppercase
        self.lowercase = string.ascii_lowercase
        self.numbers = string.digits
        
        # C/C++ keywords
        self.c_types = ["int", "char", "float", "double", "void", "long", "short", "unsigned"]
        self.c_keywords = ["if", "else", "for", "while", "do", "switch", "case", "break", "continue", "return"]
        self.cpp_keywords = ["class", "public", "private", "protected", "namespace", "template", "virtual"]
        
        # ASM registers
        self.x86_registers = ["eax", "ebx", "ecx", "edx", "esi", "edi", "esp", "ebp"]
        self.x86_8bit = ["al", "bl", "cl", "dl", "ah", "bh", "ch", "dh"]
        self.asm_instructions = ["mov", "add", "sub", "mul", "div", "push", "pop", "call", "ret", "jmp", "je", "jne", "cmp"]
        
    @property
    def supported_languages(self) -> List[str]:
        return ["c", "cpp", "asm_x86", "asm_x64", "asm_arm"]
    
    @property
    def generator_type(self) -> str:
        return "lowlevel"
    
    @property
    def description(self) -> str:
        return "Generates C/C++/ASM code with alphanumeric focus"
    
    def generate(self, language: str = "c", code_type: str = "basic", **kwargs) -> str:
        """Generate code for specified language"""
        if language == "c":
            return self._generate_c(code_type, **kwargs)
        elif language == "cpp":
            return self._generate_cpp(code_type, **kwargs)
        elif language.startswith("asm"):
            return self._generate_asm(language, code_type, **kwargs)
        else:
            return self._generate_c(code_type, **kwargs)
    
    def generate_unlimited(self, language: str = "c", **kwargs) -> Generator[str, None, None]:
        """Generate unlimited code variations"""
        code_types = ["basic", "loops", "functions", "arrays", "structs"]
        
        while True:
            code_type = self.random.choice(code_types)
            yield self.generate(language, code_type, **kwargs)
    
    def _generate_c(self, code_type: str, **kwargs) -> str:
        """Generate C code"""
        if code_type == "basic":
            return self._c_basic_program()
        elif code_type == "loops":
            return self._c_loops()
        elif code_type == "functions":
            return self._c_functions()
        elif code_type == "arrays":
            return self._c_arrays()
        elif code_type == "structs":
            return self._c_structs()
        else:
            return self._c_basic_program()
    
    def _c_basic_program(self) -> str:
        """Generate basic C program"""
        lines = [
            "#include <stdio.h>",
            "",
            "int main() {",
            f"    char alphabet[] = \"{self.alphabet}\";",
            f"    char numbers[] = \"{self.numbers}\";",
            "    "
        ]
        
        # Add some variable declarations
        for i in range(5):
            var_type = self.random.choice(["int", "char"])
            if var_type == "int":
                lines.append(f"    int {self.lowercase[i]}{i} = {self.random.randint(0, 99)};")
            else:
                lines.append(f"    char {self.lowercase[i]}{i} = '{self.alphabet[i]}';")
        
        lines.append("    ")
        
        # Add some operations
        for i in range(3):
            lines.append(f"    printf(\"%c%d\\n\", alphabet[{i}], {i});")
        
        lines.extend([
            "    ",
            "    return 0;",
            "}"
        ])
        
        return '\n'.join(lines)
    
    def _c_loops(self) -> str:
        """Generate C code with loops"""
        lines = [
            "#include <stdio.h>",
            "",
            "int main() {",
            "    // Loop through alphabet",
            "    for (int i = 0; i < 26; i++) {",
            "        printf(\"%c \", 'A' + i);",
            "    }",
            "    printf(\"\\n\");",
            "    ",
            "    // Loop through numbers",
            "    int j = 0;",
            "    while (j < 10) {",
            "        printf(\"%d \", j);",
            "        j++;",
            "    }",
            "    printf(\"\\n\");",
            "    ",
            "    // Nested loops",
            "    for (int a = 0; a < 5; a++) {",
            "        for (int b = 0; b < 5; b++) {",
            "            printf(\"%c%d \", 'A' + a, b);",
            "        }",
            "        printf(\"\\n\");",
            "    }",
            "    ",
            "    return 0;",
            "}"
        ]
        
        return '\n'.join(lines)
    
    def _c_functions(self) -> str:
        """Generate C code with functions"""
        lines = [
            "#include <stdio.h>",
            "",
            "// Convert number to letter",
            "char num_to_letter(int n) {",
            "    if (n >= 0 && n < 26) {",
            "        return 'A' + n;",
            "    }",
            "    return '?';",
            "}",
            "",
            "// Convert letter to number",
            "int letter_to_num(char c) {",
            "    if (c >= 'A' && c <= 'Z') {",
            "        return c - 'A';",
            "    }",
            "    return -1;",
            "}",
            "",
            "// Process alphanumeric",
            "void process_alphanumeric(char* str) {",
            "    while (*str) {",
            "        if (*str >= '0' && *str <= '9') {",
            "            printf(\"Number: %c\\n\", *str);",
            "        } else if ((*str >= 'A' && *str <= 'Z') || (*str >= 'a' && *str <= 'z')) {",
            "            printf(\"Letter: %c\\n\", *str);",
            "        }",
            "        str++;",
            "    }",
            "}",
            "",
            "int main() {",
            "    char test[] = \"ABC123XYZ789\";",
            "    process_alphanumeric(test);",
            "    ",
            "    for (int i = 0; i < 10; i++) {",
            "        printf(\"%c = %d\\n\", num_to_letter(i), i);",
            "    }",
            "    ",
            "    return 0;",
            "}"
        ]
        
        return '\n'.join(lines)
    
    def _c_arrays(self) -> str:
        """Generate C code with arrays"""
        lines = [
            "#include <stdio.h>",
            "",
            "int main() {",
            "    // Alphabet array",
            "    char alphabet[26];",
            "    for (int i = 0; i < 26; i++) {",
            "        alphabet[i] = 'A' + i;",
            "    }",
            "    ",
            "    // Number array",
            "    int numbers[10];",
            "    for (int i = 0; i < 10; i++) {",
            "        numbers[i] = i;",
            "    }",
            "    ",
            "    // 2D array",
            "    char grid[5][5];",
            "    for (int i = 0; i < 5; i++) {",
            "        for (int j = 0; j < 5; j++) {",
            "            grid[i][j] = 'A' + (i * 5 + j) % 26;",
            "        }",
            "    }",
            "    ",
            "    // Print grid",
            "    for (int i = 0; i < 5; i++) {",
            "        for (int j = 0; j < 5; j++) {",
            "            printf(\"%c \", grid[i][j]);",
            "        }",
            "        printf(\"\\n\");",
            "    }",
            "    ",
            "    return 0;",
            "}"
        ]
        
        return '\n'.join(lines)
    
    def _c_structs(self) -> str:
        """Generate C code with structs"""
        lines = [
            "#include <stdio.h>",
            "",
            "struct AlphaNum {",
            "    char letter;",
            "    int number;",
            "};",
            "",
            "struct Data {",
            "    char id[10];",
            "    int values[5];",
            "    char code;",
            "};",
            "",
            "int main() {",
            "    struct AlphaNum items[10];",
            "    ",
            "    // Initialize",
            "    for (int i = 0; i < 10; i++) {",
            "        items[i].letter = 'A' + i;",
            "        items[i].number = i;",
            "    }",
            "    ",
            "    // Process",
            "    for (int i = 0; i < 10; i++) {",
            "        printf(\"%c%d \", items[i].letter, items[i].number);",
            "    }",
            "    printf(\"\\n\");",
            "    ",
            "    struct Data d = {\"DATA01\", {1, 2, 3, 4, 5}, 'X'};",
            "    printf(\"ID: %s, Code: %c\\n\", d.id, d.code);",
            "    ",
            "    return 0;",
            "}"
        ]
        
        return '\n'.join(lines)
    
    def _generate_cpp(self, code_type: str, **kwargs) -> str:
        """Generate C++ code"""
        if code_type == "basic":
            return self._cpp_basic()
        elif code_type == "class":
            return self._cpp_class()
        else:
            return self._cpp_basic()
    
    def _cpp_basic(self) -> str:
        """Generate basic C++ program"""
        lines = [
            "#include <iostream>",
            "#include <string>",
            "",
            "using namespace std;",
            "",
            "int main() {",
            "    string alphabet = \"ABCDEFGHIJKLMNOPQRSTUVWXYZ\";",
            "    string numbers = \"0123456789\";",
            "    ",
            "    // Print combinations",
            "    for (int i = 0; i < 10; i++) {",
            "        cout << alphabet[i] << numbers[i] << \" \";",
            "    }",
            "    cout << endl;",
            "    ",
            "    // Using auto",
            "    auto process = [](char c, int n) {",
            "        return string(1, c) + to_string(n);",
            "    };",
            "    ",
            "    for (int i = 0; i < 5; i++) {",
            "        cout << process('A' + i, i) << \" \";",
            "    }",
            "    cout << endl;",
            "    ",
            "    return 0;",
            "}"
        ]
        
        return '\n'.join(lines)
    
    def _cpp_class(self) -> str:
        """Generate C++ class"""
        lines = [
            "#include <iostream>",
            "#include <vector>",
            "",
            "class AlphaNumeric {",
            "private:",
            "    std::vector<char> letters;",
            "    std::vector<int> numbers;",
            "    ",
            "public:",
            "    AlphaNumeric() {",
            "        // Initialize A-Z",
            "        for (int i = 0; i < 26; i++) {",
            "            letters.push_back('A' + i);",
            "        }",
            "        // Initialize 0-9",
            "        for (int i = 0; i < 10; i++) {",
            "            numbers.push_back(i);",
            "        }",
            "    }",
            "    ",
            "    void display() {",
            "        std::cout << \"Letters: \";",
            "        for (char c : letters) {",
            "            std::cout << c << \" \";",
            "        }",
            "        std::cout << std::endl;",
            "        ",
            "        std::cout << \"Numbers: \";",
            "        for (int n : numbers) {",
            "            std::cout << n << \" \";",
            "        }",
            "        std::cout << std::endl;",
            "    }",
            "    ",
            "    std::string getPair(int index) {",
            "        if (index < letters.size() && index < numbers.size()) {",
            "            return std::string(1, letters[index]) + std::to_string(numbers[index % 10]);",
            "        }",
            "        return \"??\";",
            "    }",
            "};",
            "",
            "int main() {",
            "    AlphaNumeric an;",
            "    an.display();",
            "    ",
            "    for (int i = 0; i < 10; i++) {",
            "        std::cout << an.getPair(i) << \" \";",
            "    }",
            "    std::cout << std::endl;",
            "    ",
            "    return 0;",
            "}"
        ]
        
        return '\n'.join(lines)
    
    def _generate_asm(self, arch: str, code_type: str, **kwargs) -> str:
        """Generate Assembly code"""
        if arch == "asm_x86":
            return self._asm_x86(code_type)
        elif arch == "asm_x64":
            return self._asm_x64(code_type)
        elif arch == "asm_arm":
            return self._asm_arm(code_type)
        else:
            return self._asm_x86(code_type)
    
    def _asm_x86(self, code_type: str) -> str:
        """Generate x86 Assembly"""
        if code_type == "basic":
            lines = [
                "section .data",
                "    alphabet db 'ABCDEFGHIJKLMNOPQRSTUVWXYZ', 0",
                "    numbers db '0123456789', 0",
                "    msg db 'A0B1C2D3E4F5', 0",
                "    len equ $ - msg",
                "",
                "section .bss",
                "    buffer resb 32",
                "",
                "section .text",
                "    global _start",
                "",
                "_start:",
                "    ; Load alphabet",
                "    mov esi, alphabet",
                "    mov edi, buffer",
                "    mov ecx, 26",
                "    ",
                "    ; Copy alphabet to buffer",
                "copy_loop:",
                "    mov al, [esi]",
                "    mov [edi], al",
                "    inc esi",
                "    inc edi",
                "    loop copy_loop",
                "    ",
                "    ; Print message",
                "    mov eax, 4          ; sys_write",
                "    mov ebx, 1          ; stdout",
                "    mov ecx, msg",
                "    mov edx, len",
                "    int 0x80",
                "    ",
                "    ; Exit",
                "    mov eax, 1          ; sys_exit",
                "    xor ebx, ebx        ; exit code 0",
                "    int 0x80"
            ]
        elif code_type == "loops":
            lines = [
                "section .data",
                "    counter dd 0",
                "    limit dd 10",
                "",
                "section .text",
                "    global _start",
                "",
                "_start:",
                "    ; Initialize",
                "    mov ecx, 0          ; counter",
                "    mov edx, 10         ; limit",
                "    ",
                "loop_start:",
                "    ; Check condition",
                "    cmp ecx, edx",
                "    jge loop_end",
                "    ",
                "    ; Process - convert number to ASCII",
                "    mov eax, ecx",
                "    add eax, 48         ; '0'",
                "    push eax",
                "    ",
                "    ; Convert index to letter",
                "    mov eax, ecx",
                "    add eax, 65         ; 'A'",
                "    push eax",
                "    ",
                "    inc ecx",
                "    jmp loop_start",
                "    ",
                "loop_end:",
                "    ; Exit",
                "    mov eax, 1",
                "    xor ebx, ebx",
                "    int 0x80"
            ]
        else:  # functions
            lines = [
                "section .text",
                "    global _start",
                "",
                "; Function: number to ASCII",
                "num_to_ascii:",
                "    push ebp",
                "    mov ebp, esp",
                "    mov eax, [ebp+8]    ; get parameter",
                "    add eax, 48         ; add '0'",
                "    pop ebp",
                "    ret",
                "",
                "; Function: letter to number",
                "letter_to_num:",
                "    push ebp",
                "    mov ebp, esp",
                "    mov eax, [ebp+8]    ; get parameter",
                "    sub eax, 65         ; subtract 'A'",
                "    pop ebp",
                "    ret",
                "",
                "_start:",
                "    ; Test functions",
                "    push 5",
                "    call num_to_ascii",
                "    add esp, 4",
                "    ",
                "    push 'F'",
                "    call letter_to_num",
                "    add esp, 4",
                "    ",
                "    ; Exit",
                "    mov eax, 1",
                "    xor ebx, ebx",
                "    int 0x80"
            ]
            
        return '\n'.join(lines)
    
    def _asm_x64(self, code_type: str) -> str:
        """Generate x64 Assembly"""
        lines = [
            "section .data",
            "    alphabet db 'ABCDEFGHIJKLMNOPQRSTUVWXYZ', 0",
            "    numbers db '0123456789', 0",
            "",
            "section .text",
            "    global _start",
            "",
            "_start:",
            "    ; x64 uses different registers",
            "    mov rsi, alphabet   ; source",
            "    mov rdi, rsi        ; destination",
            "    mov rcx, 26         ; count",
            "    ",
            "    ; Process loop",
            "process:",
            "    mov al, [rsi]       ; load byte",
            "    inc rsi",
            "    dec rcx",
            "    jnz process",
            "    ",
            "    ; Exit (x64 syscall)",
            "    mov rax, 60         ; sys_exit",
            "    xor rdi, rdi        ; exit code 0",
            "    syscall"
        ]
        
        return '\n'.join(lines)
    
    def _asm_arm(self, code_type: str) -> str:
        """Generate ARM Assembly"""
        lines = [
            ".data",
            "alphabet: .ascii \"ABCDEFGHIJKLMNOPQRSTUVWXYZ\\0\"",
            "numbers: .ascii \"0123456789\\0\"",
            "",
            ".text",
            ".global _start",
            "",
            "_start:",
            "    @ Load addresses",
            "    ldr r0, =alphabet",
            "    ldr r1, =numbers",
            "    mov r2, #0          @ counter",
            "    ",
            "loop:",
            "    cmp r2, #10         @ compare with 10",
            "    bge end             @ branch if >=",
            "    ",
            "    @ Load characters",
            "    ldrb r3, [r0, r2]   @ load alphabet[r2]",
            "    ldrb r4, [r1, r2]   @ load numbers[r2]",
            "    ",
            "    add r2, r2, #1      @ increment counter",
            "    b loop              @ branch to loop",
            "    ",
            "end:",
            "    @ Exit",
            "    mov r7, #1          @ sys_exit",
            "    swi 0               @ software interrupt"
        ]
        
        return '\n'.join(lines)