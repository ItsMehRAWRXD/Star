"""Simple code generator for basic alphanumeric patterns"""

from typing import Dict, Any, List, Generator
import random
import string

from ..core.base_generator import BaseGenerator


class SimpleGenerator(BaseGenerator):
    """Generator for simple alphanumeric code patterns"""
    
    def _setup(self):
        """Setup character sets and patterns"""
        self.alphabet = string.ascii_uppercase  # A-Z
        self.numbers = string.digits  # 0-9
        self.lowercase = string.ascii_lowercase  # a-z
        self.alphanumeric = self.alphabet + self.numbers + self.lowercase
        self.brackets = "()[]{}<>"
        self.operators = "+-*/=!&|^~%"
        self.punctuation = ".,;:_"
        
    @property
    def supported_languages(self) -> List[str]:
        return ["asm", "c", "basic", "generic"]
    
    @property
    def generator_type(self) -> str:
        return "simple"
    
    @property
    def description(self) -> str:
        return "Generates simple alphanumeric code patterns"
    
    def generate(self, pattern_type: str = "sequence", language: str = "generic", **kwargs) -> str:
        """Generate simple code patterns"""
        if pattern_type == "sequence":
            return self._generate_sequence(language, **kwargs)
        elif pattern_type == "random":
            return self._generate_random(language, **kwargs)
        elif pattern_type == "pattern":
            return self._generate_pattern(language, **kwargs)
        elif pattern_type == "basic_program":
            return self._generate_basic_program(language, **kwargs)
        else:
            return self._generate_sequence(language, **kwargs)
    
    def generate_unlimited(self, **kwargs) -> Generator[str, None, None]:
        """Generate unlimited simple patterns"""
        pattern_types = ["sequence", "random", "pattern", "basic_program"]
        i = 0
        while True:
            pattern_type = pattern_types[i % len(pattern_types)]
            yield self.generate(pattern_type, **kwargs)
            i += 1
    
    def _generate_sequence(self, language: str, length: int = 10, **kwargs) -> str:
        """Generate sequential patterns"""
        lines = []
        
        if language == "asm":
            # Simple ASM data sequences
            lines.append("section .data")
            lines.append(f"    alphabet db '{self.alphabet}'")
            lines.append(f"    numbers db '{self.numbers}'")
            lines.append(f"    seq1 db {','.join(str(i) for i in range(length))}")
            lines.append("")
            lines.append("section .text")
            lines.append("    global _start")
            lines.append("_start:")
            for i in range(min(length, 10)):
                lines.append(f"    mov al, {i}")
            lines.append("    mov eax, 1")
            lines.append("    int 0x80")
            
        elif language == "c":
            lines.append("#include <stdio.h>")
            lines.append("")
            lines.append("int main() {")
            lines.append(f"    char alphabet[] = \"{self.alphabet}\";")
            lines.append(f"    char numbers[] = \"{self.numbers}\";")
            lines.append(f"    int seq[] = {{{','.join(str(i) for i in range(length))}}};")
            lines.append("    ")
            for i in range(min(length, 5)):
                lines.append(f"    printf(\"%c%d\\n\", alphabet[{i}], numbers[{i}]);")
            lines.append("    return 0;")
            lines.append("}")
            
        elif language == "basic":
            for i in range(length):
                lines.append(f"{10 * (i + 1)} PRINT \"{self.alphabet[i % 26]}{i % 10}\"")
            lines.append(f"{10 * (length + 1)} END")
            
        else:  # generic
            lines.append(f"ALPHABET: {self.alphabet}")
            lines.append(f"NUMBERS: {self.numbers}")
            for i in range(length):
                lines.append(f"SEQ[{i}] = {self.alphabet[i % 26]}{i % 10}")
                
        return '\n'.join(lines)
    
    def _generate_random(self, language: str, count: int = 20, **kwargs) -> str:
        """Generate random alphanumeric patterns"""
        lines = []
        
        if language == "asm":
            lines.append("section .data")
            for i in range(count):
                value = self.random.choice(self.alphanumeric)
                lines.append(f"    var{i} db '{value}'")
            lines.append("")
            lines.append("section .text")
            lines.append("_start:")
            for i in range(min(count, 10)):
                reg = self.random.choice(['al', 'bl', 'cl', 'dl'])
                lines.append(f"    mov {reg}, [var{i}]")
                
        elif language == "c":
            lines.append("int main() {")
            for i in range(count):
                var_type = self.random.choice(['char', 'int'])
                if var_type == 'char':
                    value = self.random.choice(self.alphanumeric)
                    lines.append(f"    char v{i} = '{value}';")
                else:
                    value = self.random.randint(0, 99)
                    lines.append(f"    int v{i} = {value};")
            lines.append("    return 0;")
            lines.append("}")
            
        else:  # generic
            for i in range(count):
                var_name = f"{self.random.choice(self.lowercase)}{i}"
                value = ''.join(self.random.choices(self.alphanumeric, k=5))
                lines.append(f"{var_name} = {value}")
                
        return '\n'.join(lines)
    
    def _generate_pattern(self, language: str, pattern: str = None, **kwargs) -> str:
        """Generate code following specific patterns"""
        if not pattern:
            pattern = "ANAN"  # Alphabet-Number-Alphabet-Number
            
        lines = []
        
        def expand_pattern(p):
            result = ""
            for char in p:
                if char == 'A':
                    result += self.random.choice(self.alphabet)
                elif char == 'N':
                    result += self.random.choice(self.numbers)
                elif char == 'a':
                    result += self.random.choice(self.lowercase)
                else:
                    result += char
            return result
        
        if language == "asm":
            lines.append("section .data")
            for i in range(10):
                lines.append(f"    pat{i} db '{expand_pattern(pattern)}'")
                
        elif language == "c":
            lines.append("int main() {")
            for i in range(10):
                lines.append(f"    char pat{i}[] = \"{expand_pattern(pattern)}\";")
            lines.append("    return 0;")
            lines.append("}")
            
        else:
            for i in range(10):
                lines.append(f"PATTERN_{i} = {expand_pattern(pattern)}")
                
        return '\n'.join(lines)
    
    def _generate_basic_program(self, language: str, **kwargs) -> str:
        """Generate a basic program structure"""
        lines = []
        
        if language == "asm":
            lines.extend([
                "section .data",
                "    msg db 'A1B2C3D4E5F6', 0",
                "    len equ $ - msg",
                "",
                "section .text",
                "    global _start",
                "",
                "_start:",
                "    ; Print message",
                "    mov eax, 4",
                "    mov ebx, 1",
                "    mov ecx, msg",
                "    mov edx, len",
                "    int 0x80",
                "    ",
                "    ; Exit",
                "    mov eax, 1",
                "    xor ebx, ebx",
                "    int 0x80"
            ])
            
        elif language == "c":
            lines.extend([
                "#include <stdio.h>",
                "",
                "int main() {",
                "    char data[] = \"ABCDEF\";",
                "    int nums[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};",
                "    ",
                "    for (int i = 0; i < 6; i++) {",
                "        printf(\"%c%d \", data[i], nums[i]);",
                "    }",
                "    printf(\"\\n\");",
                "    ",
                "    return 0;",
                "}"
            ])
            
        elif language == "basic":
            lines.extend([
                "10 REM Simple BASIC Program",
                "20 DIM A$(26)",
                "30 DIM N(10)",
                "40 FOR I = 1 TO 26",
                "50 A$(I) = CHR$(64 + I)",
                "60 NEXT I",
                "70 FOR I = 0 TO 9",
                "80 N(I) = I",
                "90 NEXT I",
                "100 FOR I = 1 TO 10",
                "110 PRINT A$(I); N(I MOD 10)",
                "120 NEXT I",
                "130 END"
            ])
            
        else:
            lines.extend([
                "# Simple Program",
                "ALPHABET = ABCDEFGHIJKLMNOPQRSTUVWXYZ",
                "NUMBERS = 0123456789",
                "",
                "START:",
                "    LOAD ALPHABET",
                "    LOAD NUMBERS",
                "    LOOP I FROM 0 TO 9:",
                "        PRINT ALPHABET[I]",
                "        PRINT NUMBERS[I]",
                "    END LOOP",
                "END"
            ])
            
        return '\n'.join(lines)


class ASMGenerator(SimpleGenerator):
    """Specialized generator for Assembly language"""
    
    @property
    def supported_languages(self) -> List[str]:
        return ["x86", "x64", "arm", "mips"]
    
    @property
    def description(self) -> str:
        return "Generates Assembly language code"
    
    def generate(self, arch: str = "x86", program_type: str = "data", **kwargs) -> str:
        """Generate ASM code for different architectures"""
        if program_type == "data":
            return self._generate_data_section(arch, **kwargs)
        elif program_type == "loop":
            return self._generate_loop(arch, **kwargs)
        elif program_type == "function":
            return self._generate_function(arch, **kwargs)
        else:
            return self._generate_data_section(arch, **kwargs)
    
    def _generate_data_section(self, arch: str, **kwargs) -> str:
        """Generate data section with alphanumeric values"""
        lines = []
        
        if arch in ["x86", "x64"]:
            lines.append("section .data")
            # Alphabet array
            lines.append("    alphabet db 'ABCDEFGHIJKLMNOPQRSTUVWXYZ', 0")
            # Numbers array
            lines.append("    numbers db '0123456789', 0")
            # Mixed data
            for i in range(10):
                lines.append(f"    data{i} db '{self.alphabet[i]}{i}', 0")
            # Byte values
            lines.append("    bytes db " + ", ".join(str(i) for i in range(10)))
            
        elif arch == "arm":
            lines.append(".data")
            lines.append("alphabet: .ascii \"ABCDEFGHIJKLMNOPQRSTUVWXYZ\\0\"")
            lines.append("numbers: .ascii \"0123456789\\0\"")
            for i in range(10):
                lines.append(f"data{i}: .ascii \"{self.alphabet[i]}{i}\\0\"")
                
        elif arch == "mips":
            lines.append(".data")
            lines.append("alphabet: .asciiz \"ABCDEFGHIJKLMNOPQRSTUVWXYZ\"")
            lines.append("numbers: .asciiz \"0123456789\"")
            for i in range(10):
                lines.append(f"data{i}: .asciiz \"{self.alphabet[i]}{i}\"")
                
        return '\n'.join(lines)
    
    def _generate_loop(self, arch: str, **kwargs) -> str:
        """Generate loop that processes alphanumeric data"""
        lines = []
        
        if arch == "x86":
            lines.extend([
                "section .text",
                "global _start",
                "",
                "_start:",
                "    mov ecx, 10     ; Loop counter",
                "    mov esi, 0      ; Index",
                "",
                "loop_start:",
                "    ; Process alphabet[esi]",
                "    mov al, [alphabet + esi]",
                "    ; Process numbers[esi]", 
                "    mov bl, [numbers + esi]",
                "    ",
                "    inc esi",
                "    loop loop_start",
                "    ",
                "    ; Exit",
                "    mov eax, 1",
                "    xor ebx, ebx",
                "    int 0x80"
            ])
            
        elif arch == "arm":
            lines.extend([
                ".text",
                ".global _start",
                "",
                "_start:",
                "    mov r0, #0      @ Counter",
                "    mov r1, #10     @ Limit",
                "",
                "loop:",
                "    cmp r0, r1",
                "    bge end",
                "    ",
                "    @ Process data",
                "    add r0, r0, #1",
                "    b loop",
                "",
                "end:",
                "    mov r7, #1",
                "    swi 0"
            ])
            
        return '\n'.join(lines)
    
    def _generate_function(self, arch: str, **kwargs) -> str:
        """Generate function that processes alphanumeric input"""
        lines = []
        
        if arch == "x86":
            lines.extend([
                "; Function to convert number to ASCII",
                "num_to_ascii:",
                "    push ebp",
                "    mov ebp, esp",
                "    mov eax, [ebp+8]    ; Get parameter",
                "    add eax, 48         ; Add ASCII '0'",
                "    pop ebp",
                "    ret",
                "",
                "; Function to convert letter index to ASCII",
                "letter_to_ascii:",
                "    push ebp",
                "    mov ebp, esp",
                "    mov eax, [ebp+8]    ; Get parameter",
                "    add eax, 65         ; Add ASCII 'A'",
                "    pop ebp",
                "    ret"
            ])
            
        return '\n'.join(lines)