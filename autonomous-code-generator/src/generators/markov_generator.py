"""Markov chain-based code generator"""

from typing import Dict, Any, List, Generator, Tuple
import random
from collections import defaultdict, Counter
import re

from ..core.base_generator import BaseGenerator


class MarkovGenerator(BaseGenerator):
    """Generator that uses Markov chains to create code"""
    
    def _setup(self):
        """Setup Markov chain parameters"""
        self.order = self.config.get('order', 2)  # n-gram size
        self.models = {}  # Store trained models per language
        
    @property
    def supported_languages(self) -> List[str]:
        return ["python", "javascript", "java", "any"]
    
    @property
    def generator_type(self) -> str:
        return "markov"
    
    @property
    def description(self) -> str:
        return "Generates code using Markov chains trained on code patterns"
    
    def train(self, code_samples: List[str], language: str = "python"):
        """Train the Markov model on code samples"""
        # Tokenize all samples
        all_tokens = []
        for sample in code_samples:
            tokens = self._tokenize(sample, language)
            all_tokens.extend(tokens)
        
        # Build n-gram model
        model = defaultdict(Counter)
        
        for i in range(len(all_tokens) - self.order):
            # Get n-gram key
            key = tuple(all_tokens[i:i + self.order])
            # Get next token
            next_token = all_tokens[i + self.order]
            # Update model
            model[key][next_token] += 1
            
        self.models[language] = model
        
    def _tokenize(self, code: str, language: str) -> List[str]:
        """Tokenize code based on language"""
        if language == "python":
            # Python tokenization
            pattern = r'(\w+|[^\w\s]|\n|\s+)'
        elif language == "javascript":
            # JavaScript tokenization
            pattern = r'(\w+|[^\w\s]|\n|\s+)'
        else:
            # Generic tokenization
            pattern = r'(\w+|[^\w\s]|\n|\s+)'
            
        tokens = [t for t in re.findall(pattern, code) if t.strip() or t == '\n']
        return tokens
    
    def generate(self, length: int = 100, language: str = "python", 
                seed: str = None, **kwargs) -> str:
        """Generate code using Markov chain"""
        if language not in self.models:
            # Use pre-trained model or train on examples
            self._train_default_model(language)
            
        model = self.models[language]
        
        # Initialize with seed or random start
        if seed:
            tokens = self._tokenize(seed, language)
            current = tuple(tokens[-self.order:])
        else:
            current = self.random.choice(list(model.keys()))
            tokens = list(current)
            
        # Generate tokens
        for _ in range(length - self.order):
            if current in model:
                # Get possible next tokens with weights
                candidates = model[current]
                if candidates:
                    # Weighted random choice
                    next_token = self._weighted_choice(candidates)
                    tokens.append(next_token)
                    # Update current n-gram
                    current = tuple(tokens[-self.order:])
                else:
                    # Dead end, pick new random start
                    current = self.random.choice(list(model.keys()))
                    tokens.extend(current)
            else:
                # Unknown n-gram, pick random continuation
                if model:
                    current = self.random.choice(list(model.keys()))
                    tokens.extend(current)
                else:
                    break
                    
        # Join tokens back into code
        return self._join_tokens(tokens, language)
    
    def _weighted_choice(self, counter: Counter) -> str:
        """Make weighted random choice from counter"""
        total = sum(counter.values())
        r = self.random.uniform(0, total)
        
        cumsum = 0
        for token, count in counter.items():
            cumsum += count
            if cumsum >= r:
                return token
                
        return list(counter.keys())[0]
    
    def _join_tokens(self, tokens: List[str], language: str) -> str:
        """Join tokens back into code"""
        code = []
        for i, token in enumerate(tokens):
            if i == 0:
                code.append(token)
            elif token == '\n':
                code.append(token)
            elif token in '.,;:!?':
                code.append(token)
            elif token in '()[]{}':
                # No space before brackets
                if token in '([{' and i > 0 and code[-1] != ' ':
                    code.append(' ')
                code.append(token)
            elif i > 0 and tokens[i-1] in ')]}':
                # Space after closing brackets
                code.append(' ')
                code.append(token)
            elif token.strip():
                # Regular token
                if i > 0 and tokens[i-1] != '\n' and not tokens[i-1] in '([{':
                    code.append(' ')
                code.append(token)
            else:
                code.append(token)
                
        return ''.join(code)
    
    def generate_unlimited(self, base_length: int = 100, **kwargs) -> Generator[str, None, None]:
        """Generate unlimited code variations"""
        while True:
            # Vary length
            length = self.random.randint(int(base_length * 0.5), int(base_length * 1.5))
            # Generate with random seeds
            yield self.generate(length=length, **kwargs)
    
    def _train_default_model(self, language: str):
        """Train default model for a language"""
        if language == "python":
            samples = [
                """def fibonacci(n):
    if n <= 1:
        return n
    return fibonacci(n-1) + fibonacci(n-2)""",
                
                """class Stack:
    def __init__(self):
        self.items = []
    
    def push(self, item):
        self.items.append(item)
    
    def pop(self):
        return self.items.pop()""",
        
                """for i in range(10):
    if i % 2 == 0:
        print(f"Even: {i}")
    else:
        print(f"Odd: {i}")""",
        
                """try:
    result = process_data(input_data)
    save_result(result)
except Exception as e:
    logger.error(f"Error: {e}")
    raise""",
                    
                """def quicksort(arr):
    if len(arr) <= 1:
        return arr
    pivot = arr[len(arr) // 2]
    left = [x for x in arr if x < pivot]
    middle = [x for x in arr if x == pivot]
    right = [x for x in arr if x > pivot]
    return quicksort(left) + middle + quicksort(right)"""
            ]
            
        elif language == "javascript":
            samples = [
                """function factorial(n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}""",
                
                """class Queue {
    constructor() {
        this.items = [];
    }
    
    enqueue(item) {
        this.items.push(item);
    }
    
    dequeue() {
        return this.items.shift();
    }
}""",
                
                """const numbers = [1, 2, 3, 4, 5];
const doubled = numbers.map(n => n * 2);
const evens = numbers.filter(n => n % 2 === 0);
const sum = numbers.reduce((a, b) => a + b, 0);""",
                
                """async function fetchData(url) {
    try {
        const response = await fetch(url);
        const data = await response.json();
        return data;
    } catch (error) {
        console.error('Error:', error);
        throw error;
    }
}"""
            ]
        else:
            # Generic code samples
            samples = [
                "function process(data) { return data; }",
                "class Entity { constructor(name) { this.name = name; } }",
                "for (int i = 0; i < 10; i++) { print(i); }",
                "if (condition) { doSomething(); } else { doSomethingElse(); }"
            ]
            
        self.train(samples, language)


class CodePatternMarkovGenerator(MarkovGenerator):
    """Specialized Markov generator for specific code patterns"""
    
    def __init__(self, config: Dict[str, Any] = None):
        super().__init__(config)
        self.pattern_models = {}
        
    def train_pattern(self, pattern_name: str, examples: List[str]):
        """Train on specific pattern examples"""
        all_tokens = []
        for example in examples:
            tokens = self._tokenize(example, "python")
            all_tokens.extend(tokens)
            
        model = defaultdict(Counter)
        for i in range(len(all_tokens) - self.order):
            key = tuple(all_tokens[i:i + self.order])
            next_token = all_tokens[i + self.order]
            model[key][next_token] += 1
            
        self.pattern_models[pattern_name] = model
        
    def generate_pattern(self, pattern_name: str, length: int = 50) -> str:
        """Generate code for a specific pattern"""
        if pattern_name not in self.pattern_models:
            # Train default patterns
            self._train_default_patterns()
            
        if pattern_name not in self.pattern_models:
            raise ValueError(f"Unknown pattern: {pattern_name}")
            
        model = self.pattern_models[pattern_name]
        
        # Start with random n-gram
        current = self.random.choice(list(model.keys()))
        tokens = list(current)
        
        # Generate
        for _ in range(length - self.order):
            if current in model:
                candidates = model[current]
                if candidates:
                    next_token = self._weighted_choice(candidates)
                    tokens.append(next_token)
                    current = tuple(tokens[-self.order:])
                else:
                    break
            else:
                break
                
        return self._join_tokens(tokens, "python")
    
    def _train_default_patterns(self):
        """Train default patterns"""
        patterns = {
            'loops': [
                "for i in range(n):",
                "for item in items:",
                "for key, value in dict.items():",
                "while condition:",
                "while True:",
                "for i, item in enumerate(items):"
            ],
            'conditions': [
                "if x > 0:",
                "if condition:",
                "if x and y:",
                "if x or y:",
                "if not x:",
                "elif condition:",
                "else:"
            ],
            'functions': [
                "def function(param):",
                "def process(data):",
                "def calculate(x, y):",
                "def __init__(self):",
                "def __str__(self):",
                "return result"
            ],
            'classes': [
                "class MyClass:",
                "class Base(object):",
                "class Derived(Base):",
                "def __init__(self, value):",
                "self.attribute = value"
            ]
        }
        
        for pattern_name, examples in patterns.items():
            self.train_pattern(pattern_name, examples)