"""Pattern-based code generator for common code patterns"""

from typing import Dict, Any, List, Generator
import random
import itertools

from ..core.base_generator import BaseGenerator
from ..core.code_builder import CodeBuilder


class PatternGenerator(BaseGenerator):
    """Generator that creates code based on common programming patterns"""
    
    def _setup(self):
        """Setup available patterns"""
        self.patterns = {
            'singleton': self._generate_singleton,
            'factory': self._generate_factory,
            'observer': self._generate_observer,
            'decorator': self._generate_decorator,
            'strategy': self._generate_strategy,
            'repository': self._generate_repository,
            'mvc': self._generate_mvc,
            'rest_api': self._generate_rest_api,
            'crud': self._generate_crud,
            'iterator': self._generate_iterator
        }
        
    @property
    def supported_languages(self) -> List[str]:
        return ["python", "javascript", "java"]
    
    @property
    def generator_type(self) -> str:
        return "pattern"
    
    @property
    def description(self) -> str:
        return "Generates code based on common design patterns"
    
    def generate(self, pattern: str, language: str = "python", **kwargs) -> str:
        """Generate code for a specific pattern"""
        if pattern not in self.patterns:
            raise ValueError(f"Unknown pattern: {pattern}. Available: {list(self.patterns.keys())}")
            
        return self.patterns[pattern](language, **kwargs)
    
    def generate_unlimited(self, patterns: List[str] = None, **kwargs) -> Generator[str, None, None]:
        """Generate unlimited pattern variations"""
        if not patterns:
            patterns = list(self.patterns.keys())
            
        pattern_cycle = itertools.cycle(patterns)
        
        i = 0
        while True:
            pattern = next(pattern_cycle)
            # Generate with varying parameters
            params = {
                'class_name': f'{pattern.title()}Class{i}',
                'namespace': f'pattern{i}',
                **kwargs
            }
            yield self.generate(pattern, **params)
            i += 1
    
    def _generate_singleton(self, language: str, class_name: str = "Singleton", **kwargs) -> str:
        """Generate singleton pattern"""
        builder = CodeBuilder()
        
        if language == "python":
            builder.add_class(class_name)
            builder.add_line("_instance = None")
            builder.add_line()
            builder.add_function("__new__", ["cls"], [
                "if cls._instance is None:",
                "    cls._instance = super().__new__(cls)",
                "return cls._instance"
            ])
            builder.add_line()
            builder.add_function("__init__", ["self"], [
                "if not hasattr(self, 'initialized'):",
                "    self.initialized = True",
                "    # Initialize your singleton here"
            ])
            
        elif language == "javascript":
            builder.add_line(f"class {class_name} {{")
            builder.indent()
            builder.add_line("constructor() {")
            builder.indent()
            builder.add_line(f"if ({class_name}.instance) {{")
            builder.indent()
            builder.add_line(f"return {class_name}.instance;")
            builder.dedent()
            builder.add_line("}")
            builder.add_line(f"{class_name}.instance = this;")
            builder.add_line("// Initialize your singleton here")
            builder.dedent()
            builder.add_line("}")
            builder.dedent()
            builder.add_line("}")
            
        return builder.get_code()
    
    def _generate_factory(self, language: str, factory_name: str = "Factory", **kwargs) -> str:
        """Generate factory pattern"""
        builder = CodeBuilder()
        
        if language == "python":
            # Base product class
            builder.add_class("Product")
            builder.add_function("operation", ["self"], ["raise NotImplementedError"])
            builder.add_line()
            
            # Concrete products
            for i in range(2):
                builder.add_class(f"ConcreteProduct{i+1}", ["Product"])
                builder.add_function("operation", ["self"], [f'return "Result from ConcreteProduct{i+1}"'])
                builder.add_line()
            
            # Factory
            builder.add_class(factory_name)
            builder.add_function("create_product", ["self", "product_type: str"], [
                "if product_type == 'A':",
                "    return ConcreteProduct1()",
                "elif product_type == 'B':",
                "    return ConcreteProduct2()",
                "else:",
                "    raise ValueError(f'Unknown product type: {product_type}')"
            ])
            
        return builder.get_code()
    
    def _generate_observer(self, language: str, **kwargs) -> str:
        """Generate observer pattern"""
        builder = CodeBuilder()
        
        if language == "python":
            # Observer interface
            builder.add_class("Observer")
            builder.add_function("update", ["self", "subject"], ["raise NotImplementedError"])
            builder.add_line()
            
            # Subject
            builder.add_class("Subject")
            builder.add_function("__init__", ["self"], [
                "self._observers = []",
                "self._state = None"
            ])
            builder.add_line()
            builder.add_function("attach", ["self", "observer: Observer"], [
                "self._observers.append(observer)"
            ])
            builder.add_line()
            builder.add_function("detach", ["self", "observer: Observer"], [
                "self._observers.remove(observer)"
            ])
            builder.add_line()
            builder.add_function("notify", ["self"], [
                "for observer in self._observers:",
                "    observer.update(self)"
            ])
            builder.add_line()
            builder.add_function("set_state", ["self", "state"], [
                "self._state = state",
                "self.notify()"
            ])
            
        return builder.get_code()
    
    def _generate_decorator(self, language: str, **kwargs) -> str:
        """Generate decorator pattern"""
        builder = CodeBuilder()
        
        if language == "python":
            builder.add_line("from functools import wraps")
            builder.add_line()
            
            # Function decorator
            builder.add_function("timing_decorator", ["func"], [
                "@wraps(func)",
                "def wrapper(*args, **kwargs):",
                "    import time",
                "    start = time.time()",
                "    result = func(*args, **kwargs)",
                "    end = time.time()",
                "    print(f'{func.__name__} took {end - start:.4f} seconds')",
                "    return result",
                "return wrapper"
            ])
            builder.add_line()
            
            # Class decorator
            builder.add_function("add_repr", ["cls"], [
                "def __repr__(self):",
                "    attrs = ', '.join(f'{k}={v}' for k, v in self.__dict__.items())",
                "    return f'{cls.__name__}({attrs})'",
                "cls.__repr__ = __repr__",
                "return cls"
            ])
            
        return builder.get_code()
    
    def _generate_strategy(self, language: str, **kwargs) -> str:
        """Generate strategy pattern"""
        builder = CodeBuilder()
        
        if language == "python":
            # Strategy interface
            builder.add_class("Strategy")
            builder.add_function("execute", ["self", "data"], ["raise NotImplementedError"])
            builder.add_line()
            
            # Concrete strategies
            builder.add_class("ConcreteStrategyA", ["Strategy"])
            builder.add_function("execute", ["self", "data"], [
                "return sorted(data)"
            ])
            builder.add_line()
            
            builder.add_class("ConcreteStrategyB", ["Strategy"])
            builder.add_function("execute", ["self", "data"], [
                "return sorted(data, reverse=True)"
            ])
            builder.add_line()
            
            # Context
            builder.add_class("Context")
            builder.add_function("__init__", ["self", "strategy: Strategy"], [
                "self._strategy = strategy"
            ])
            builder.add_line()
            builder.add_function("set_strategy", ["self", "strategy: Strategy"], [
                "self._strategy = strategy"
            ])
            builder.add_line()
            builder.add_function("execute_strategy", ["self", "data"], [
                "return self._strategy.execute(data)"
            ])
            
        return builder.get_code()
    
    def _generate_repository(self, language: str, entity: str = "User", **kwargs) -> str:
        """Generate repository pattern"""
        builder = CodeBuilder()
        
        if language == "python":
            builder.add_line("from typing import List, Optional, Dict")
            builder.add_line()
            
            # Entity
            builder.add_class(entity)
            builder.add_function("__init__", ["self", "id: int", "name: str"], [
                "self.id = id",
                "self.name = name"
            ])
            builder.add_line()
            
            # Repository interface
            builder.add_class(f"{entity}Repository")
            builder.add_function("find_by_id", ["self", "id: int"], [f"raise NotImplementedError"])
            builder.add_function("find_all", ["self"], ["raise NotImplementedError"])
            builder.add_function("save", ["self", f"entity: {entity}"], ["raise NotImplementedError"])
            builder.add_function("delete", ["self", "id: int"], ["raise NotImplementedError"])
            builder.add_line()
            
            # In-memory implementation
            builder.add_class(f"InMemory{entity}Repository", [f"{entity}Repository"])
            builder.add_function("__init__", ["self"], [
                "self._storage: Dict[int, {entity}] = {}"
            ])
            builder.add_line()
            builder.add_function("find_by_id", ["self", "id: int"], [
                "return self._storage.get(id)"
            ])
            builder.add_line()
            builder.add_function("find_all", ["self"], [
                "return list(self._storage.values())"
            ])
            builder.add_line()
            builder.add_function("save", ["self", f"entity: {entity}"], [
                "self._storage[entity.id] = entity",
                "return entity"
            ])
            builder.add_line()
            builder.add_function("delete", ["self", "id: int"], [
                "if id in self._storage:",
                "    del self._storage[id]",
                "    return True",
                "return False"
            ])
            
        return builder.get_code()
    
    def _generate_mvc(self, language: str, model_name: str = "Product", **kwargs) -> str:
        """Generate MVC pattern"""
        builder = CodeBuilder()
        
        if language == "python":
            # Model
            builder.add_class(f"{model_name}Model")
            builder.add_function("__init__", ["self"], [
                "self._data = []"
            ])
            builder.add_function("add_item", ["self", "item"], [
                "self._data.append(item)"
            ])
            builder.add_function("get_items", ["self"], [
                "return self._data.copy()"
            ])
            builder.add_line()
            
            # View
            builder.add_class(f"{model_name}View")
            builder.add_function("display", ["self", "items"], [
                "print(f'=== {model_name} List ===')",
                "for i, item in enumerate(items, 1):",
                "    print(f'{i}. {item}')"
            ])
            builder.add_function("get_user_input", ["self", "prompt: str"], [
                "return input(prompt)"
            ])
            builder.add_line()
            
            # Controller
            builder.add_class(f"{model_name}Controller")
            builder.add_function("__init__", ["self", "model", "view"], [
                "self.model = model",
                "self.view = view"
            ])
            builder.add_function("add_item", ["self"], [
                "item = self.view.get_user_input('Enter item: ')",
                "self.model.add_item(item)"
            ])
            builder.add_function("show_items", ["self"], [
                "items = self.model.get_items()",
                "self.view.display(items)"
            ])
            
        return builder.get_code()
    
    def _generate_rest_api(self, language: str, resource: str = "User", **kwargs) -> str:
        """Generate REST API pattern"""
        builder = CodeBuilder()
        
        if language == "python":
            builder.add_line("from flask import Flask, jsonify, request")
            builder.add_line("from typing import Dict, List")
            builder.add_line()
            builder.add_line("app = Flask(__name__)")
            builder.add_line(f"{resource.lower()}s: Dict[int, Dict] = {{}}")
            builder.add_line("next_id = 1")
            builder.add_line()
            
            # GET all
            builder.add_line(f"@app.route('/{resource.lower()}s', methods=['GET'])")
            builder.add_function(f"get_{resource.lower()}s", [], [
                f"return jsonify(list({resource.lower()}s.values()))"
            ])
            builder.add_line()
            
            # GET one
            builder.add_line(f"@app.route('/{resource.lower()}s/<int:id>', methods=['GET'])")
            builder.add_function(f"get_{resource.lower()}", ["id"], [
                f"if id in {resource.lower()}s:",
                f"    return jsonify({resource.lower()}s[id])",
                "return {'error': 'Not found'}, 404"
            ])
            builder.add_line()
            
            # POST
            builder.add_line(f"@app.route('/{resource.lower()}s', methods=['POST'])")
            builder.add_function(f"create_{resource.lower()}", [], [
                "global next_id",
                "data = request.json",
                "data['id'] = next_id",
                f"{resource.lower()}s[next_id] = data",
                "next_id += 1",
                "return jsonify(data), 201"
            ])
            builder.add_line()
            
            # PUT
            builder.add_line(f"@app.route('/{resource.lower()}s/<int:id>', methods=['PUT'])")
            builder.add_function(f"update_{resource.lower()}", ["id"], [
                f"if id not in {resource.lower()}s:",
                "    return {'error': 'Not found'}, 404",
                "data = request.json",
                "data['id'] = id",
                f"{resource.lower()}s[id] = data",
                "return jsonify(data)"
            ])
            builder.add_line()
            
            # DELETE
            builder.add_line(f"@app.route('/{resource.lower()}s/<int:id>', methods=['DELETE'])")
            builder.add_function(f"delete_{resource.lower()}", ["id"], [
                f"if id not in {resource.lower()}s:",
                "    return {'error': 'Not found'}, 404",
                f"del {resource.lower()}s[id]",
                "return '', 204"
            ])
            
        return builder.get_code()
    
    def _generate_crud(self, language: str, entity: str = "Item", **kwargs) -> str:
        """Generate CRUD operations"""
        builder = CodeBuilder()
        
        if language == "python":
            builder.add_line("from typing import List, Optional, Dict")
            builder.add_line()
            
            builder.add_class(f"{entity}CRUD")
            builder.add_function("__init__", ["self"], [
                "self._items: Dict[int, Dict] = {}",
                "self._next_id = 1"
            ])
            builder.add_line()
            
            builder.add_function("create", ["self", "**data"], [
                "item = {'id': self._next_id, **data}",
                "self._items[self._next_id] = item",
                "self._next_id += 1",
                "return item"
            ])
            builder.add_line()
            
            builder.add_function("read", ["self", "id: int"], [
                "return self._items.get(id)"
            ])
            builder.add_line()
            
            builder.add_function("update", ["self", "id: int", "**data"], [
                "if id in self._items:",
                "    self._items[id].update(data)",
                "    return self._items[id]",
                "return None"
            ])
            builder.add_line()
            
            builder.add_function("delete", ["self", "id: int"], [
                "if id in self._items:",
                "    item = self._items[id]",
                "    del self._items[id]",
                "    return item",
                "return None"
            ])
            builder.add_line()
            
            builder.add_function("list_all", ["self"], [
                "return list(self._items.values())"
            ])
            
        return builder.get_code()
    
    def _generate_iterator(self, language: str, **kwargs) -> str:
        """Generate iterator pattern"""
        builder = CodeBuilder()
        
        if language == "python":
            builder.add_class("NumberIterator")
            builder.add_function("__init__", ["self", "start: int", "end: int"], [
                "self.start = start",
                "self.end = end",
                "self.current = start"
            ])
            builder.add_line()
            
            builder.add_function("__iter__", ["self"], [
                "return self"
            ])
            builder.add_line()
            
            builder.add_function("__next__", ["self"], [
                "if self.current < self.end:",
                "    number = self.current",
                "    self.current += 1",
                "    return number",
                "raise StopIteration"
            ])
            
        return builder.get_code()