from abc import ABC, abstractmethod
from typing import Optional


class LanguageModelProvider(ABC):
    @abstractmethod
    def complete(self, prompt: str, *, system: Optional[str] = None) -> str:
        """Return the model's completion text for the given prompt.

        Providers should raise a RuntimeError with a helpful error message if
        they are not configured properly (e.g., missing credentials).
        """
        raise NotImplementedError