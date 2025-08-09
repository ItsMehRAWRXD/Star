"""Fractal code generator for creating self-similar recursive structures"""

from typing import Dict, Any, List, Generator
import random
import math

from ..core.base_generator import BaseGenerator
from ..core.code_builder import CodeBuilder


class FractalGenerator(BaseGenerator):
    """Generator that creates fractal/recursive code structures"""
    
    def _setup(self):
        """Setup fractal patterns"""
        self.fractals = {
            'tree': self._generate_tree,
            'recursive_function': self._generate_recursive_function,
            'nested_structure': self._generate_nested_structure,
            'recursive_class': self._generate_recursive_class,
            'fractal_pattern': self._generate_fractal_pattern,
            'recursive_data': self._generate_recursive_data
        }
        
    @property
    def supported_languages(self) -> List[str]:
        return ["python", "javascript", "java"]
    
    @property
    def generator_type(self) -> str:
        return "fractal"
    
    @property
    def description(self) -> str:
        return "Generates fractal and recursive code structures"
    
    def generate(self, fractal_type: str = "tree", depth: int = 3, **kwargs) -> str:
        """Generate a fractal code structure"""
        if fractal_type not in self.fractals:
            raise ValueError(f"Unknown fractal type: {fractal_type}")
            
        return self.fractals[fractal_type](depth=depth, **kwargs)
    
    def generate_unlimited(self, min_depth: int = 1, max_depth: int = 5, **kwargs) -> Generator[str, None, None]:
        """Generate unlimited fractal variations"""
        fractal_types = list(self.fractals.keys())
        
        while True:
            fractal_type = self.random.choice(fractal_types)
            depth = self.random.randint(min_depth, max_depth)
            yield self.generate(fractal_type, depth=depth, **kwargs)
    
    def _generate_tree(self, depth: int = 3, branching: int = 2, **kwargs) -> str:
        """Generate tree-like recursive structure"""
        builder = CodeBuilder()
        
        # Tree node class
        builder.add_class("TreeNode")
        builder.add_function("__init__", ["self", "value", "children=None"], [
            "self.value = value",
            "self.children = children or []"
        ])
        builder.add_line()
        
        # Recursive tree builder
        builder.add_function("build_tree", ["depth", "value=0", "branching=2"], [
            "if depth <= 0:",
            "    return TreeNode(value)",
            "",
            "node = TreeNode(value)",
            "for i in range(branching):",
            "    child_value = value * branching + i + 1",
            "    child = build_tree(depth - 1, child_value, branching)",
            "    node.children.append(child)",
            "",
            "return node"
        ])
        builder.add_line()
        
        # Tree traversal
        builder.add_function("print_tree", ["node", "indent=0"], [
            "print(' ' * indent + f'Node({node.value})')",
            "for child in node.children:",
            "    print_tree(child, indent + 2)"
        ])
        builder.add_line()
        
        # Generate example
        builder.add_comment("Example usage")
        builder.add_line(f"tree = build_tree({depth}, branching={branching})")
        builder.add_line("print_tree(tree)")
        
        return builder.get_code()
    
    def _generate_recursive_function(self, depth: int = 3, **kwargs) -> str:
        """Generate various recursive functions"""
        builder = CodeBuilder()
        
        # Fibonacci
        builder.add_function("fibonacci", ["n"], [
            "if n <= 1:",
            "    return n",
            "return fibonacci(n - 1) + fibonacci(n - 2)"
        ])
        builder.add_line()
        
        # Factorial
        builder.add_function("factorial", ["n"], [
            "if n <= 1:",
            "    return 1",
            "return n * factorial(n - 1)"
        ])
        builder.add_line()
        
        # Ackermann function
        builder.add_function("ackermann", ["m", "n"], [
            "if m == 0:",
            "    return n + 1",
            "elif n == 0:",
            "    return ackermann(m - 1, 1)",
            "else:",
            "    return ackermann(m - 1, ackermann(m, n - 1))"
        ])
        builder.add_line()
        
        # Nested recursive function
        builder.add_function("nested_recursive", ["depth", "value=0"], [
            "def inner_recursive(d, v):",
            "    if d <= 0:",
            "        return v",
            "    return inner_recursive(d - 1, v * 2 + 1)",
            "",
            "if depth <= 0:",
            "    return value",
            "return nested_recursive(depth - 1, inner_recursive(depth, value))"
        ])
        builder.add_line()
        
        # Mutual recursion
        builder.add_function("is_even", ["n"], [
            "if n == 0:",
            "    return True",
            "return is_odd(n - 1)"
        ])
        builder.add_line()
        
        builder.add_function("is_odd", ["n"], [
            "if n == 0:",
            "    return False",
            "return is_even(n - 1)"
        ])
        
        return builder.get_code()
    
    def _generate_nested_structure(self, depth: int = 3, **kwargs) -> str:
        """Generate deeply nested data structures"""
        builder = CodeBuilder()
        
        # Generate nested function
        def generate_nested_level(level: int, max_level: int) -> List[str]:
            indent = "    " * level
            if level >= max_level:
                return [f"{indent}'data': 'level_{level}'"]
            
            lines = [f"{indent}'level_{level}': {{"]
            lines.extend(generate_nested_level(level + 1, max_level))
            lines.append(f"{indent}}},")
            return lines
        
        builder.add_line("# Deeply nested structure")
        builder.add_line("nested_data = {")
        for line in generate_nested_level(1, depth):
            builder.add_line(line.rstrip(','))
        builder.add_line("}")
        builder.add_line()
        
        # Access function
        builder.add_function("access_nested", ["data", "path"], [
            "current = data",
            "for key in path:",
            "    if isinstance(current, dict) and key in current:",
            "        current = current[key]",
            "    else:",
            "        return None",
            "return current"
        ])
        builder.add_line()
        
        # Flatten function
        builder.add_function("flatten_nested", ["data", "prefix=''"], [
            "result = {}",
            "for key, value in data.items():",
            "    full_key = f'{prefix}.{key}' if prefix else key",
            "    if isinstance(value, dict):",
            "        result.update(flatten_nested(value, full_key))",
            "    else:",
            "        result[full_key] = value",
            "return result"
        ])
        
        return builder.get_code()
    
    def _generate_recursive_class(self, depth: int = 3, **kwargs) -> str:
        """Generate recursive class hierarchies"""
        builder = CodeBuilder()
        
        # Composite pattern
        builder.add_class("Component")
        builder.add_function("__init__", ["self", "name"], [
            "self.name = name",
            "self.children = []"
        ])
        builder.add_line()
        
        builder.add_function("add", ["self", "component"], [
            "self.children.append(component)",
            "return self"
        ])
        builder.add_line()
        
        builder.add_function("remove", ["self", "component"], [
            "self.children.remove(component)",
            "return self"
        ])
        builder.add_line()
        
        builder.add_function("display", ["self", "indent=0"], [
            "print(' ' * indent + self.name)",
            "for child in self.children:",
            "    child.display(indent + 2)"
        ])
        builder.add_line()
        
        # Recursive builder
        builder.add_function("build_hierarchy", ["depth", "prefix='node'"], [
            f"root = Component(f'{{prefix}}_0')",
            "if depth <= 0:",
            "    return root",
            "",
            "def build_subtree(parent, level, max_level, index=0):",
            "    if level >= max_level:",
            "        return",
            "    for i in range(2):",  # Binary tree
            "        child_name = f'{prefix}_{level}_{index}_{i}'",
            "        child = Component(child_name)",
            "        parent.add(child)",
            "        build_subtree(child, level + 1, max_level, index * 2 + i)",
            "",
            "build_subtree(root, 1, depth)",
            "return root"
        ])
        builder.add_line()
        
        # Self-referential class
        builder.add_class("SelfReferential")
        builder.add_function("__init__", ["self", "value"], [
            "self.value = value",
            "self.reference = None"
        ])
        builder.add_line()
        
        builder.add_function("create_chain", ["self", "length"], [
            "current = self",
            "for i in range(1, length):",
            "    new_node = SelfReferential(self.value + i)",
            "    current.reference = new_node",
            "    current = new_node",
            "return self"
        ])
        
        return builder.get_code()
    
    def _generate_fractal_pattern(self, depth: int = 3, **kwargs) -> str:
        """Generate code that creates fractal patterns"""
        builder = CodeBuilder()
        
        # Koch snowflake generator
        builder.add_function("koch_snowflake", ["order", "size=300"], [
            "import turtle",
            "",
            "def koch_curve(t, order, size):",
            "    if order == 0:",
            "        t.forward(size)",
            "    else:",
            "        for angle in [60, -120, 60, 0]:",
            "            koch_curve(t, order - 1, size / 3)",
            "            t.left(angle)",
            "",
            "t = turtle.Turtle()",
            "t.speed(0)",
            "for _ in range(3):",
            "    koch_curve(t, order, size)",
            "    t.right(120)",
            "turtle.done()"
        ])
        builder.add_line()
        
        # Sierpinski triangle
        builder.add_function("sierpinski_triangle", ["order", "size=200"], [
            "def draw_triangle(points):",
            "    # Drawing code here",
            "    pass",
            "",
            "def sierpinski(order, points):",
            "    if order == 0:",
            "        draw_triangle(points)",
            "    else:",
            "        # Calculate midpoints",
            "        mid1 = ((points[0][0] + points[1][0]) / 2,",
            "                (points[0][1] + points[1][1]) / 2)",
            "        mid2 = ((points[1][0] + points[2][0]) / 2,",
            "                (points[1][1] + points[2][1]) / 2)",
            "        mid3 = ((points[0][0] + points[2][0]) / 2,",
            "                (points[0][1] + points[2][1]) / 2)",
            "        ",
            "        # Recursive calls",
            "        sierpinski(order - 1, [points[0], mid1, mid3])",
            "        sierpinski(order - 1, [mid1, points[1], mid2])",
            "        sierpinski(order - 1, [mid3, mid2, points[2]])",
            "",
            "# Initial triangle points",
            "points = [(0, 0), (size, 0), (size/2, size * 0.866)]",
            "sierpinski(order, points)"
        ])
        builder.add_line()
        
        # L-system generator
        builder.add_function("l_system", ["axiom", "rules", "iterations"], [
            "def apply_rules(string, rules):",
            "    result = ''",
            "    for char in string:",
            "        result += rules.get(char, char)",
            "    return result",
            "",
            "current = axiom",
            "for _ in range(iterations):",
            "    current = apply_rules(current, rules)",
            "return current"
        ])
        builder.add_line()
        
        builder.add_comment("Example L-system for dragon curve")
        builder.add_line("dragon_rules = {'X': 'X+YF+', 'Y': '-FX-Y'}")
        builder.add_line(f"dragon = l_system('FX', dragon_rules, {depth})")
        
        return builder.get_code()
    
    def _generate_recursive_data(self, depth: int = 3, **kwargs) -> str:
        """Generate recursive data structure generators"""
        builder = CodeBuilder()
        
        # JSON-like structure generator
        builder.add_function("generate_recursive_json", ["depth", "max_keys=3"], [
            "import random",
            "import string",
            "",
            "def random_key():",
            "    return ''.join(random.choices(string.ascii_lowercase, k=5))",
            "",
            "def generate_value(d):",
            "    if d <= 0:",
            "        # Base case: primitive values",
            "        return random.choice([",
            "            random.randint(0, 100),",
            "            random.random(),",
            "            random.choice(['true', 'false', 'null']),",
            "            '\"' + random_key() + '\"'",
            "        ])",
            "    else:",
            "        # Recursive case: nested structure",
            "        if random.random() < 0.5:",
            "            # Generate object",
            "            obj = {}",
            "            for _ in range(random.randint(1, max_keys)):",
            "                obj[random_key()] = generate_value(d - 1)",
            "            return obj",
            "        else:",
            "            # Generate array",
            "            return [generate_value(d - 1) for _ in range(random.randint(1, max_keys))]",
            "",
            "return generate_value(depth)"
        ])
        builder.add_line()
        
        # Linked list generator
        builder.add_class("Node")
        builder.add_function("__init__", ["self", "value", "next=None"], [
            "self.value = value",
            "self.next = next"
        ])
        builder.add_line()
        
        builder.add_function("generate_linked_list", ["values"], [
            "if not values:",
            "    return None",
            "return Node(values[0], generate_linked_list(values[1:]))"
        ])
        builder.add_line()
        
        # Binary tree generator
        builder.add_class("BinaryNode")
        builder.add_function("__init__", ["self", "value", "left=None", "right=None"], [
            "self.value = value",
            "self.left = left",
            "self.right = right"
        ])
        builder.add_line()
        
        builder.add_function("generate_balanced_tree", ["values"], [
            "if not values:",
            "    return None",
            "mid = len(values) // 2",
            "return BinaryNode(",
            "    values[mid],",
            "    generate_balanced_tree(values[:mid]),",
            "    generate_balanced_tree(values[mid+1:])",
            ")"
        ])
        
        return builder.get_code()