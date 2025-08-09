import os
from typing import Optional

from openai import OpenAI

from autocode.provider.base import LanguageModelProvider


class OpenAIProvider(LanguageModelProvider):
    def __init__(self, model: str = "gpt-4o-mini") -> None:
        api_key = os.getenv("OPENAI_API_KEY")
        if not api_key:
            raise RuntimeError(
                "OPENAI_API_KEY environment variable is not set. "
                "Please export your key before using the OpenAI provider."
            )
        self._client = OpenAI(api_key=api_key)
        self._model = model

    def complete(self, prompt: str, *, system: Optional[str] = None) -> str:
        messages = []
        if system:
            messages.append({"role": "system", "content": system})
        messages.append({"role": "user", "content": prompt})

        response = self._client.chat.completions.create(
            model=self._model,
            messages=messages,
            temperature=0.2,
        )
        return response.choices[0].message.content or ""