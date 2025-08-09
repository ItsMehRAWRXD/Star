"""Function-focused code generator"""

from typing import Dict, Any, List, Generator
import random

from ..core.base_generator import BaseGenerator
from ..core.code_builder import CodeBuilder


class FunctionGenerator(BaseGenerator):
    """Generator that creates various types of functions"""
    
    def _setup(self):
        """Setup function types and patterns"""
        self.function_types = {
            'math': self._generate_math_functions,
            'string': self._generate_string_functions,
            'array': self._generate_array_functions,
            'utility': self._generate_utility_functions,
            'alphanumeric': self._generate_alphanumeric_functions
        }
        
    @property
    def supported_languages(self) -> List[str]:
        return ["python", "javascript", "c", "java"]
    
    @property
    def generator_type(self) -> str:
        return "function"
    
    @property
    def description(self) -> str:
        return "Generates various types of functions"
    
    def generate(self, function_type: str = "utility", language: str = "python", **kwargs) -> str:
        """Generate functions of a specific type"""
        if function_type not in self.function_types:
            function_type = "utility"
            
        return self.function_types[function_type](language, **kwargs)
    
    def generate_unlimited(self, **kwargs) -> Generator[str, None, None]:
        """Generate unlimited function variations"""
        types = list(self.function_types.keys())
        
        while True:
            func_type = self.random.choice(types)
            yield self.generate(func_type, **kwargs)
    
    def _generate_math_functions(self, language: str, **kwargs) -> str:
        """Generate mathematical functions"""
        builder = CodeBuilder()
        
        if language == "python":
            # Basic math operations
            builder.add_function("add", ["a", "b"], ["return a + b"])
            builder.add_line()
            
            builder.add_function("multiply", ["a", "b"], ["return a * b"])
            builder.add_line()
            
            # Power function
            builder.add_function("power", ["base", "exp"], [
                "result = 1",
                "for _ in range(exp):",
                "    result *= base",
                "return result"
            ])
            builder.add_line()
            
            # Factorial
            builder.add_function("factorial", ["n"], [
                "if n <= 1:",
                "    return 1",
                "return n * factorial(n - 1)"
            ])
            
        elif language == "c":
            builder.add_line("#include <stdio.h>")
            builder.add_line()
            
            builder.add_line("int add(int a, int b) {")
            builder.add_line("    return a + b;")
            builder.add_line("}")
            builder.add_line()
            
            builder.add_line("int multiply(int a, int b) {")
            builder.add_line("    return a * b;")
            builder.add_line("}")
            
        return builder.get_code()
    
    def _generate_string_functions(self, language: str, **kwargs) -> str:
        """Generate string manipulation functions"""
        builder = CodeBuilder()
        
        if language == "python":
            # Reverse string
            builder.add_function("reverse_string", ["s"], [
                "return s[::-1]"
            ])
            builder.add_line()
            
            # Count characters
            builder.add_function("count_chars", ["s", "char"], [
                "count = 0",
                "for c in s:",
                "    if c == char:",
                "        count += 1",
                "return count"
            ])
            builder.add_line()
            
            # Extract alphanumeric
            builder.add_function("extract_alphanumeric", ["s"], [
                "result = ''",
                "for char in s:",
                "    if char.isalnum():",
                "        result += char",
                "return result"
            ])
            
        return builder.get_code()
    
    def _generate_array_functions(self, language: str, **kwargs) -> str:
        """Generate array/list manipulation functions"""
        builder = CodeBuilder()
        
        if language == "python":
            # Find max
            builder.add_function("find_max", ["arr"], [
                "if not arr:",
                "    return None",
                "max_val = arr[0]",
                "for val in arr:",
                "    if val > max_val:",
                "        max_val = val",
                "return max_val"
            ])
            builder.add_line()
            
            # Sum array
            builder.add_function("sum_array", ["arr"], [
                "total = 0",
                "for val in arr:",
                "    total += val",
                "return total"
            ])
            
        elif language == "c":
            builder.add_line("int find_max(int arr[], int size) {")
            builder.add_line("    int max = arr[0];")
            builder.add_line("    for (int i = 1; i < size; i++) {")
            builder.add_line("        if (arr[i] > max) {")
            builder.add_line("            max = arr[i];")
            builder.add_line("        }")
            builder.add_line("    }")
            builder.add_line("    return max;")
            builder.add_line("}")
            
        return builder.get_code()
    
    def _generate_utility_functions(self, language: str, **kwargs) -> str:
        """Generate utility functions"""
        builder = CodeBuilder()
        
        if language == "python":
            # Is even
            builder.add_function("is_even", ["n"], [
                "return n % 2 == 0"
            ])
            builder.add_line()
            
            # Is prime
            builder.add_function("is_prime", ["n"], [
                "if n < 2:",
                "    return False",
                "for i in range(2, int(n**0.5) + 1):",
                "    if n % i == 0:",
                "        return False",
                "return True"
            ])
            builder.add_line()
            
            # Swap values
            builder.add_function("swap", ["a", "b"], [
                "return b, a"
            ])
            
        return builder.get_code()
    
    def _generate_alphanumeric_functions(self, language: str, **kwargs) -> str:
        """Generate functions that work with alphanumeric data"""
        builder = CodeBuilder()
        
        if language == "python":
            # Generate alphabet
            builder.add_function("get_alphabet", [], [
                "return 'ABCDEFGHIJKLMNOPQRSTUVWXYZ'"
            ])
            builder.add_line()
            
            # Generate numbers
            builder.add_function("get_numbers", [], [
                "return '0123456789'"
            ])
            builder.add_line()
            
            # Convert number to letter
            builder.add_function("num_to_letter", ["n"], [
                "alphabet = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ'",
                "if 0 <= n < 26:",
                "    return alphabet[n]",
                "return '?'"
            ])
            builder.add_line()
            
            # Create alphanumeric ID
            builder.add_function("create_id", ["prefix", "number"], [
                "return f'{prefix}{number:04d}'"
            ])
            builder.add_line()
            
            # Check if alphanumeric
            builder.add_function("is_alphanumeric", ["s"], [
                "for char in s:",
                "    if not char.isalnum():",
                "        return False",
                "return True"
            ])
            
        elif language == "c":
            builder.add_line("#include <stdio.h>")
            builder.add_line("#include <ctype.h>")
            builder.add_line()
            
            builder.add_line("char num_to_letter(int n) {")
            builder.add_line("    if (n >= 0 && n < 26) {")
            builder.add_line("        return 'A' + n;")
            builder.add_line("    }")
            builder.add_line("    return '?';")
            builder.add_line("}")
            builder.add_line()
            
            builder.add_line("int is_alphanumeric(char c) {")
            builder.add_line("    return isalnum(c);")
            builder.add_line("}")
            
        return builder.get_code()


class SimpleFunctionGenerator(FunctionGenerator):
    """Generator focused on simple alphanumeric functions"""
    
    @property
    def description(self) -> str:
        return "Generates simple functions for alphanumeric processing"
    
    def generate(self, count: int = 5, **kwargs) -> str:
        """Generate multiple simple functions"""
        builder = CodeBuilder()
        
        for i in range(count):
            # Simple function that processes A-Z and 0-9
            builder.add_function(f"process_{i}", ["input"], [
                f"# Process function {i}",
                "alphabet = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ'",
                "numbers = '0123456789'",
                f"return alphabet[{i} % 26] + numbers[{i} % 10]"
            ])
            builder.add_line()
            
        return builder.get_code()