"""Main manager for orchestrating code generation"""

from typing import Dict, Any, List, Optional, Union, Generator
import os
import json
import yaml
from pathlib import Path

from .generator_registry import registry
from .base_generator import BaseGenerator


class GeneratorManager:
    """Manager for orchestrating code generation across multiple generators"""
    
    def __init__(self, config_path: Optional[str] = None):
        """
        Initialize generator manager
        
        Args:
            config_path: Path to configuration file
        """
        self.config = {}
        self.registry = registry
        
        if config_path:
            self.load_config(config_path)
            
        # Auto-discover generators
        self._discover_generators()
        
    def _discover_generators(self):
        """Discover and register all available generators"""
        try:
            self.registry.auto_discover("src.generators")
        except Exception as e:
            print(f"Failed to auto-discover generators: {e}")
            
    def load_config(self, config_path: str):
        """Load configuration from file"""
        path = Path(config_path)
        
        if not path.exists():
            raise FileNotFoundError(f"Config file not found: {config_path}")
            
        if path.suffix == '.json':
            with open(path) as f:
                self.config = json.load(f)
        elif path.suffix in ['.yaml', '.yml']:
            with open(path) as f:
                self.config = yaml.safe_load(f)
        else:
            raise ValueError(f"Unsupported config format: {path.suffix}")
            
    def generate(self, 
                generator_name: str,
                output_path: Optional[str] = None,
                count: int = 1,
                **kwargs) -> Union[str, List[str], Generator[str, None, None]]:
        """
        Generate code using specified generator
        
        Args:
            generator_name: Name of generator to use
            output_path: Optional path to save generated code
            count: Number of pieces to generate (-1 for unlimited)
            **kwargs: Additional arguments for the generator
            
        Returns:
            Generated code or generator for unlimited generation
        """
        # Get generator
        generator_config = self.config.get('generators', {}).get(generator_name, {})
        generator = self.registry.get_generator(generator_name, generator_config)
        
        if not generator:
            available = self.registry.list_generators()
            raise ValueError(f"Generator '{generator_name}' not found. Available: {available}")
            
        # Generate code
        if count == 1:
            code = generator.generate(**kwargs)
            result = code
        elif count == -1:  # Unlimited
            result = generator.generate_unlimited(**kwargs)
        else:
            result = generator.generate_batch(count, **kwargs)
            
        # Save if output path provided
        if output_path and count != -1:
            self._save_output(output_path, result)
            
        return result
        
    def _save_output(self, output_path: str, content: Union[str, List[str]]):
        """Save generated content to file(s)"""
        path = Path(output_path)
        path.parent.mkdir(parents=True, exist_ok=True)
        
        if isinstance(content, str):
            with open(path, 'w') as f:
                f.write(content)
        else:
            # For multiple pieces, create numbered files
            base_name = path.stem
            extension = path.suffix
            
            for i, piece in enumerate(content):
                file_path = path.parent / f"{base_name}_{i:04d}{extension}"
                with open(file_path, 'w') as f:
                    f.write(piece)
                    
    def generate_project(self, 
                        template: str,
                        output_dir: str,
                        **kwargs) -> Dict[str, str]:
        """
        Generate an entire project structure
        
        Args:
            template: Project template name
            output_dir: Output directory for the project
            **kwargs: Additional template parameters
            
        Returns:
            Dictionary of generated files
        """
        # Get project template generator
        generator_name = f"project.{template}"
        generator = self.registry.get_generator(generator_name)
        
        if not generator:
            raise ValueError(f"Project template '{template}' not found")
            
        # Generate project files
        files = generator.generate(**kwargs)
        
        # Create project structure
        output_path = Path(output_dir)
        output_path.mkdir(parents=True, exist_ok=True)
        
        results = {}
        for file_path, content in files.items():
            full_path = output_path / file_path
            full_path.parent.mkdir(parents=True, exist_ok=True)
            
            with open(full_path, 'w') as f:
                f.write(content)
                
            results[str(full_path)] = content
            
        return results
        
    def list_generators(self, generator_type: Optional[str] = None) -> List[Dict[str, Any]]:
        """
        List available generators
        
        Args:
            generator_type: Optional filter by generator type
            
        Returns:
            List of generator metadata
        """
        if generator_type:
            names = self.registry.get_generators_by_type(generator_type)
        else:
            names = self.registry.list_generators()
            
        results = []
        for name in names:
            generator = self.registry.get_generator(name)
            if generator:
                results.append({
                    'name': name,
                    'type': generator.generator_type,
                    'languages': generator.supported_languages,
                    'description': generator.description
                })
                
        return results
        
    def get_generator_info(self, generator_name: str) -> Dict[str, Any]:
        """Get detailed information about a generator"""
        generator = self.registry.get_generator(generator_name)
        
        if not generator:
            raise ValueError(f"Generator '{generator_name}' not found")
            
        return generator.get_metadata()
        
    def validate_generator(self, generator_name: str) -> bool:
        """Validate a generator's configuration"""
        generator = self.registry.get_generator(generator_name)
        
        if not generator:
            return False
            
        return generator.validate_config()