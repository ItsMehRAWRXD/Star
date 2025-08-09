"""Markov chain-based code generator"""

from typing import Dict, Any, List, Generator
import random
from collections import defaultdict

from ..core.base_generator import BaseGenerator


class MarkovGenerator(BaseGenerator):
    """Generator that uses Markov chains to generate code"""
    
    def _setup(self):
        """Setup Markov chain data"""
        self.order = self.config.get('order', 2)
        self.chains = {}
        
    @property
    def supported_languages(self) -> List[str]:
        return ["python", "c", "generic"]
    
    @property
    def generator_type(self) -> str:
        return "markov"
    
    @property
    def description(self) -> str:
        return "Generates code using Markov chains"
    
    def train(self, code_samples: List[str]):
        """Train the Markov chain on code samples"""
        for sample in code_samples:
            tokens = self._tokenize(sample)
            chain = defaultdict(list)
            
            for i in range(len(tokens) - self.order):
                key = tuple(tokens[i:i + self.order])
                value = tokens[i + self.order]
                chain[key].append(value)
                
            self.chains[len(self.chains)] = dict(chain)
    
    def _tokenize(self, code: str) -> List[str]:
        """Simple tokenizer for code"""
        # Basic tokenization - can be improved
        tokens = []
        current = ""
        
        for char in code:
            if char in " \n\t()[]{}.,;:":
                if current:
                    tokens.append(current)
                    current = ""
                if char.strip():
                    tokens.append(char)
            else:
                current += char
                
        if current:
            tokens.append(current)
            
        return tokens
    
    def generate(self, length: int = 100, start_tokens: List[str] = None, **kwargs) -> str:
        """Generate code using Markov chain"""
        if not self.chains:
            # Use default training data
            self._train_default()
            
        chain = self.chains[0] if self.chains else {}
        
        if not chain:
            return "# No training data available"
            
        # Get starting tokens
        if not start_tokens:
            start_tokens = list(self.random.choice(list(chain.keys())))
            
        result = list(start_tokens)
        
        for _ in range(length - len(start_tokens)):
            key = tuple(result[-self.order:])
            if key in chain and chain[key]:
                next_token = self.random.choice(chain[key])
                result.append(next_token)
            else:
                # Random restart
                new_start = self.random.choice(list(chain.keys()))
                result.extend(new_start)
                
        return self._detokenize(result)
    
    def _detokenize(self, tokens: List[str]) -> str:
        """Convert tokens back to code"""
        result = []
        for i, token in enumerate(tokens):
            if i > 0 and token not in "()[]{}.,;:":
                result.append(" ")
            result.append(token)
        return "".join(result)
    
    def generate_unlimited(self, **kwargs) -> Generator[str, None, None]:
        """Generate unlimited code pieces"""
        while True:
            length = self.random.randint(50, 200)
            yield self.generate(length=length, **kwargs)
    
    def _train_default(self):
        """Train on default code patterns"""
        samples = [
            "for i in range(10): print(i)",
            "def func(x): return x * 2",
            "if x > 0: y = x else: y = 0",
            "class MyClass: def __init__(self): self.value = 0",
            "while True: x = input() if x == 'q': break",
            "data = [1, 2, 3, 4, 5] result = sum(data)",
            "import math x = math.sqrt(16)",
            "try: result = 10 / x except: result = 0",
        ]
        self.train(samples)


class AlphanumericMarkovGenerator(MarkovGenerator):
    """Markov generator focused on alphanumeric patterns"""
    
    @property
    def description(self) -> str:
        return "Generates alphanumeric patterns using Markov chains"
    
    def _setup(self):
        """Setup with alphanumeric focus"""
        super()._setup()
        self.order = 1  # Simpler chains for alphanumeric
        
    def _train_default(self):
        """Train on alphanumeric patterns"""
        patterns = []
        
        # Sequential patterns
        for i in range(10):
            patterns.append(f"A{i} B{i} C{i} D{i}")
            patterns.append(f"var{i} = {i * 10}")
            
        # Alphabetic patterns
        alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        for i in range(0, 26, 5):
            patterns.append(" ".join(alphabet[i:i+5]))
            
        # Numeric patterns
        for i in range(5):
            patterns.append(" ".join(str(j) for j in range(i*10, (i+1)*10)))
            
        # Mixed patterns
        for i in range(10):
            patterns.append(f"{alphabet[i]}{i} {alphabet[i+1]}{i+1}")
            
        self.train(patterns)