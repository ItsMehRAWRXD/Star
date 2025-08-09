"""Template-based code generator using Jinja2"""

from typing import Dict, Any, List, Generator
import os
from pathlib import Path
from jinja2 import Environment, FileSystemLoader, Template

from ..core.base_generator import BaseGenerator


class TemplateGenerator(BaseGenerator):
    """Generator that uses Jinja2 templates for code generation"""
    
    def _setup(self):
        """Setup Jinja2 environment"""
        template_dir = self.config.get('template_dir', 'src/templates')
        self.env = Environment(
            loader=FileSystemLoader(template_dir),
            trim_blocks=True,
            lstrip_blocks=True
        )
        
    @property
    def supported_languages(self) -> List[str]:
        return ["python", "javascript", "java", "cpp", "go", "rust", "any"]
    
    @property
    def generator_type(self) -> str:
        return "template"
    
    @property
    def description(self) -> str:
        return "Generates code using Jinja2 templates"
    
    def generate(self, template_name: str, context: Dict[str, Any] = None, **kwargs) -> str:
        """Generate code from a template"""
        context = context or {}
        context.update(kwargs)
        
        # Load template
        if template_name.endswith('.j2') or template_name.endswith('.jinja2'):
            template = self.env.get_template(template_name)
        else:
            # Try common extensions
            for ext in ['.py.j2', '.js.j2', '.java.j2', '.j2']:
                try:
                    template = self.env.get_template(f"{template_name}{ext}")
                    break
                except:
                    continue
            else:
                # Create template from string
                template = Template(template_name)
                
        # Render template
        return template.render(**context)
    
    def generate_unlimited(self, template_name: str, context_generator: Generator = None, **kwargs) -> Generator[str, None, None]:
        """Generate unlimited code pieces from template"""
        base_context = kwargs
        
        if context_generator is None:
            # Default context generator - incrementing IDs
            def default_context():
                i = 0
                while True:
                    yield {'id': i, 'name': f'item_{i}'}
                    i += 1
            context_generator = default_context()
            
        while True:
            context = next(context_generator)
            context.update(base_context)
            yield self.generate(template_name, context)


class ClassTemplateGenerator(TemplateGenerator):
    """Specialized template generator for classes"""
    
    @property
    def description(self) -> str:
        return "Generates class definitions from templates"
        
    def generate(self, class_name: str, attributes: List[Dict] = None, 
                methods: List[Dict] = None, base_class: str = None, **kwargs) -> str:
        """Generate a class from template"""
        template = """
class {{ class_name }}{% if base_class %}({{ base_class }}){% endif %}:
    \"\"\"{{ docstring or 'Generated class' }}\"\"\"
    
    {% if attributes %}
    def __init__(self{% for attr in attributes %}, {{ attr.name }}{% endfor %}):
        {% for attr in attributes %}
        self.{{ attr.name }} = {{ attr.name }}
        {% endfor %}
    {% endif %}
    
    {% for method in methods %}
    def {{ method.name }}(self{% if method.params %}, {{ method.params|join(', ') }}{% endif %}):
        \"\"\"{{ method.docstring or 'Method ' + method.name }}\"\"\"
        {{ method.body|indent(8) }}
    {% endfor %}
"""
        
        context = {
            'class_name': class_name,
            'attributes': attributes or [],
            'methods': methods or [],
            'base_class': base_class
        }
        context.update(kwargs)
        
        tmpl = Template(template)
        return tmpl.render(**context)


class FunctionTemplateGenerator(TemplateGenerator):
    """Specialized template generator for functions"""
    
    @property
    def description(self) -> str:
        return "Generates function definitions from templates"
        
    def generate(self, function_name: str, params: List[str] = None,
                return_type: str = None, body: str = None, **kwargs) -> str:
        """Generate a function from template"""
        template = """
def {{ function_name }}({{ params|join(', ') if params else '' }}){% if return_type %} -> {{ return_type }}{% endif %}:
    \"\"\"{{ docstring or 'Generated function' }}\"\"\"
    {% if body %}
    {{ body|indent(4) }}
    {% else %}
    pass
    {% endif %}
"""
        
        context = {
            'function_name': function_name,
            'params': params or [],
            'return_type': return_type,
            'body': body
        }
        context.update(kwargs)
        
        tmpl = Template(template)
        return tmpl.render(**context)