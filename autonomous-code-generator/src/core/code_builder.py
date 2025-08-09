"""Code builder utility for constructing code programmatically"""

from typing import List, Optional, Union
import textwrap


class CodeBuilder:
    """Builder class for constructing code with proper indentation and formatting"""
    
    def __init__(self, indent_size: int = 4, indent_char: str = ' '):
        """
        Initialize code builder
        
        Args:
            indent_size: Number of characters per indent level
            indent_char: Character to use for indentation
        """
        self.lines: List[str] = []
        self.indent_size = indent_size
        self.indent_char = indent_char
        self.current_indent = 0
        
    def add_line(self, line: str = "", indent_delta: int = 0):
        """
        Add a line with current indentation
        
        Args:
            line: Line to add
            indent_delta: Change in indentation after this line
        """
        if line:
            indent = self.indent_char * (self.current_indent * self.indent_size)
            self.lines.append(f"{indent}{line}")
        else:
            self.lines.append("")
            
        self.current_indent = max(0, self.current_indent + indent_delta)
        return self
        
    def add_lines(self, lines: Union[str, List[str]], indent_delta: int = 0):
        """Add multiple lines"""
        if isinstance(lines, str):
            lines = lines.split('\n')
            
        for line in lines:
            self.add_line(line)
            
        self.current_indent = max(0, self.current_indent + indent_delta)
        return self
        
    def indent(self):
        """Increase indentation level"""
        self.current_indent += 1
        return self
        
    def dedent(self):
        """Decrease indentation level"""
        self.current_indent = max(0, self.current_indent - 1)
        return self
        
    def add_block(self, header: str, body: Union[str, List[str]], footer: str = ""):
        """
        Add a code block with header and body
        
        Args:
            header: Block header (e.g., function definition)
            body: Block body
            footer: Optional block footer
        """
        self.add_line(header)
        self.indent()
        self.add_lines(body)
        self.dedent()
        if footer:
            self.add_line(footer)
        return self
        
    def add_class(self, name: str, bases: List[str] = None, body: Union[str, List[str]] = None):
        """Add a class definition"""
        bases_str = f"({', '.join(bases)})" if bases else ""
        header = f"class {name}{bases_str}:"
        body = body or ["pass"]
        self.add_block(header, body)
        return self
        
    def add_function(self, name: str, params: List[str] = None, body: Union[str, List[str]] = None,
                    return_type: str = None, decorators: List[str] = None):
        """Add a function definition"""
        # Add decorators
        if decorators:
            for decorator in decorators:
                self.add_line(f"@{decorator}")
                
        # Build function signature
        params_str = ", ".join(params) if params else ""
        return_str = f" -> {return_type}" if return_type else ""
        header = f"def {name}({params_str}){return_str}:"
        
        body = body or ["pass"]
        self.add_block(header, body)
        return self
        
    def add_if(self, condition: str, body: Union[str, List[str]]):
        """Add if statement"""
        self.add_block(f"if {condition}:", body)
        return self
        
    def add_elif(self, condition: str, body: Union[str, List[str]]):
        """Add elif statement"""
        self.add_block(f"elif {condition}:", body)
        return self
        
    def add_else(self, body: Union[str, List[str]]):
        """Add else statement"""
        self.add_block("else:", body)
        return self
        
    def add_for(self, iterator: str, iterable: str, body: Union[str, List[str]]):
        """Add for loop"""
        self.add_block(f"for {iterator} in {iterable}:", body)
        return self
        
    def add_while(self, condition: str, body: Union[str, List[str]]):
        """Add while loop"""
        self.add_block(f"while {condition}:", body)
        return self
        
    def add_try(self, body: Union[str, List[str]]):
        """Add try block"""
        self.add_block("try:", body)
        return self
        
    def add_except(self, exception: str = "", body: Union[str, List[str]] = None):
        """Add except block"""
        except_str = f"except {exception}:" if exception else "except:"
        body = body or ["pass"]
        self.add_block(except_str, body)
        return self
        
    def add_finally(self, body: Union[str, List[str]]):
        """Add finally block"""
        self.add_block("finally:", body)
        return self
        
    def add_comment(self, comment: str):
        """Add a comment line"""
        self.add_line(f"# {comment}")
        return self
        
    def add_docstring(self, docstring: str, style: str = '"""'):
        """Add a docstring"""
        lines = docstring.strip().split('\n')
        if len(lines) == 1:
            self.add_line(f'{style}{lines[0]}{style}')
        else:
            self.add_line(style)
            for line in lines:
                self.add_line(line)
            self.add_line(style)
        return self
        
    def get_code(self) -> str:
        """Get the generated code as a string"""
        return '\n'.join(self.lines)
        
    def clear(self):
        """Clear the builder"""
        self.lines = []
        self.current_indent = 0
        return self
        
    def __str__(self) -> str:
        """String representation returns the code"""
        return self.get_code()