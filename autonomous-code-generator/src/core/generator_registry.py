"""Registry for managing code generators"""

from typing import Dict, List, Type, Optional
import importlib
import pkgutil
from pathlib import Path

from .base_generator import BaseGenerator


class GeneratorRegistry:
    """Registry for managing and discovering code generators"""
    
    def __init__(self):
        self._generators: Dict[str, Type[BaseGenerator]] = {}
        self._instances: Dict[str, BaseGenerator] = {}
        
    def register(self, name: str, generator_class: Type[BaseGenerator]):
        """
        Register a generator class
        
        Args:
            name: Name to register the generator under
            generator_class: Generator class to register
        """
        if not issubclass(generator_class, BaseGenerator):
            raise ValueError(f"{generator_class} must be a subclass of BaseGenerator")
        
        self._generators[name] = generator_class
        
    def unregister(self, name: str):
        """Unregister a generator"""
        self._generators.pop(name, None)
        self._instances.pop(name, None)
        
    def get_generator(self, name: str, config: Dict = None) -> Optional[BaseGenerator]:
        """
        Get a generator instance by name
        
        Args:
            name: Name of the generator
            config: Configuration for the generator
            
        Returns:
            Generator instance or None if not found
        """
        if name not in self._generators:
            return None
            
        # Create new instance with config
        if config:
            return self._generators[name](config)
            
        # Return cached instance
        if name not in self._instances:
            self._instances[name] = self._generators[name]()
            
        return self._instances[name]
    
    def list_generators(self) -> List[str]:
        """List all registered generator names"""
        return list(self._generators.keys())
    
    def get_generators_by_type(self, generator_type: str) -> List[str]:
        """Get generators by type"""
        results = []
        for name, cls in self._generators.items():
            instance = self.get_generator(name)
            if instance and instance.generator_type == generator_type:
                results.append(name)
        return results
    
    def get_generators_by_language(self, language: str) -> List[str]:
        """Get generators supporting a specific language"""
        results = []
        for name, cls in self._generators.items():
            instance = self.get_generator(name)
            if instance and language in instance.supported_languages:
                results.append(name)
        return results
    
    def auto_discover(self, package_path: str):
        """
        Auto-discover and register generators from a package
        
        Args:
            package_path: Path to package containing generators
        """
        # Import the package
        package = importlib.import_module(package_path)
        
        # Walk through all modules in the package
        for importer, modname, ispkg in pkgutil.walk_packages(
            package.__path__, 
            prefix=package.__name__ + "."
        ):
            if ispkg:
                continue
                
            # Import the module
            try:
                module = importlib.import_module(modname)
                
                # Look for BaseGenerator subclasses
                for attr_name in dir(module):
                    attr = getattr(module, attr_name)
                    if (isinstance(attr, type) and 
                        issubclass(attr, BaseGenerator) and 
                        attr is not BaseGenerator):
                        
                        # Register with a sensible name
                        generator_name = f"{modname.split('.')[-1]}.{attr_name}"
                        self.register(generator_name, attr)
                        
            except Exception as e:
                print(f"Failed to import {modname}: {e}")
    
    def get_all_metadata(self) -> Dict[str, Dict]:
        """Get metadata for all registered generators"""
        metadata = {}
        for name in self._generators:
            instance = self.get_generator(name)
            if instance:
                metadata[name] = instance.get_metadata()
        return metadata


# Global registry instance
registry = GeneratorRegistry()