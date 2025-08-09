"""Setup script for the autonomous code generator"""

from setuptools import setup, find_packages

with open("README.md", "r", encoding="utf-8") as fh:
    long_description = fh.read()

setup(
    name="autonomous-code-generator",
    version="1.0.0",
    author="Autonomous Code Generator",
    description="An unlimited, autonomous code generation system",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/autonomous/code-generator",
    packages=find_packages(),
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
    ],
    python_requires=">=3.7",
    install_requires=[
        "click>=8.1.0",
        "jinja2>=3.1.0",
        "pyyaml>=6.0",
        "rich>=13.0.0",
    ],
    entry_points={
        "console_scripts": [
            "codegen=src.cli:main",
        ],
    },
)