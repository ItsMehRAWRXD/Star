"""Base generator class for all code generators"""

from abc import ABC, abstractmethod
from typing import Dict, Any, List, Optional, Generator
import random
import itertools


class BaseGenerator(ABC):
    """Abstract base class for all code generators"""
    
    def __init__(self, config: Dict[str, Any] = None):
        """
        Initialize the generator with configuration
        
        Args:
            config: Configuration dictionary for the generator
        """
        self.config = config or {}
        self.random = random.Random(self.config.get('seed'))
        self._setup()
    
    def _setup(self):
        """Setup method for subclasses to override"""
        pass
    
    @abstractmethod
    def generate(self, **kwargs) -> str:
        """
        Generate code based on the given parameters
        
        Args:
            **kwargs: Generation parameters
            
        Returns:
            Generated code as string
        """
        pass
    
    @abstractmethod
    def generate_unlimited(self, **kwargs) -> Generator[str, None, None]:
        """
        Generate unlimited code pieces
        
        Args:
            **kwargs: Generation parameters
            
        Yields:
            Generated code pieces
        """
        pass
    
    def generate_batch(self, count: int, **kwargs) -> List[str]:
        """
        Generate a batch of code pieces
        
        Args:
            count: Number of pieces to generate
            **kwargs: Generation parameters
            
        Returns:
            List of generated code pieces
        """
        if count == -1:  # Unlimited
            return self.generate_unlimited(**kwargs)
        
        results = []
        generator = self.generate_unlimited(**kwargs)
        for _ in range(count):
            try:
                results.append(next(generator))
            except StopIteration:
                break
        return results
    
    @property
    @abstractmethod
    def supported_languages(self) -> List[str]:
        """List of supported programming languages"""
        pass
    
    @property
    @abstractmethod
    def generator_type(self) -> str:
        """Type of generator (e.g., 'template', 'pattern', 'ai')"""
        pass
    
    @property
    @abstractmethod
    def description(self) -> str:
        """Description of what this generator does"""
        pass
    
    def validate_config(self) -> bool:
        """
        Validate the generator configuration
        
        Returns:
            True if configuration is valid
        """
        return True
    
    def get_metadata(self) -> Dict[str, Any]:
        """
        Get generator metadata
        
        Returns:
            Dictionary containing generator metadata
        """
        return {
            'type': self.generator_type,
            'languages': self.supported_languages,
            'description': self.description,
            'config': self.config
        }