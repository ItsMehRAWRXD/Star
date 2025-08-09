import json
from typing import Optional

from autocode.provider.base import LanguageModelProvider


class LocalMockProvider(LanguageModelProvider):
    def __init__(self, model: str = "mock") -> None:
        self._model = model

    def complete(self, prompt: str, *, system: Optional[str] = None) -> str:
        # Extremely simple behavior: if prompt requests JSON, return a tiny file bundle
        if "Respond with a JSON object" in prompt or "Only return JSON" in prompt:
            bundle = {
                "files": [
                    {"path": "README.md", "content": "# Generated Project\n\nHello world from LocalMockProvider.\n"},
                    {"path": "main.py", "content": "print('hello from generated project')\n"},
                ],
                "run_command": "python3 main.py",
                "finish": True,
                "notes": "Local mock generation completed.",
            }
            return json.dumps(bundle)
        # Planning path
        return "1) Create minimal files.\n2) Print greeting.\nREADY"