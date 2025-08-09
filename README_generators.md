# Autonomous Code Generators

Simple, unlimited code generation utilities without any complex branching or self-improvement features.

## Overview

This project contains two autonomous code generators:

1. **autonomous_code_generator** - Basic code generator with batch processing
2. **unlimited_generator** - Advanced multi-threaded generator supporting multiple languages

## Building

```bash
make all
```

## Usage

### Basic Generator

Generate a specific number of files:
```bash
./autonomous_code_generator 100
```

Generate unlimited files (until stopped):
```bash
./autonomous_code_generator unlimited
```

### Advanced Unlimited Generator

Run the unlimited generator (press Ctrl+C to stop):
```bash
./unlimited_generator
```

## Output

- **autonomous_code_generator**: Creates files in `generated_code/` directory
- **unlimited_generator**: Creates files in `unlimited_output/` directory organized by language:
  - `cpp/` - C++ classes and functions
  - `python/` - Python scripts
  - `javascript/` - JavaScript modules
  - `bash/` - Bash scripts
  - `make/` - Makefiles
  - `cmake/` - CMake configuration files
  - `headers/` - C++ header files

## Features

### Basic Generator
- Generates C++ code with random classes, functions, and control structures
- Variable complexity and naming
- Batch processing support
- Multi-instance generation

### Unlimited Generator
- Multi-threaded generation using all available CPU cores
- Supports 8 different file types
- Continuous generation with controlled rate
- Organized output by file type
- Progress tracking every 100 files

## Clean Up

Remove all generated files and executables:
```bash
make clean
```

## Examples

Generate 50 files quickly:
```bash
./autonomous_code_generator 50
```

Start unlimited generation in background:
```bash
./unlimited_generator &
```

Monitor generation progress:
```bash
watch -n 1 'find unlimited_output -type f | wc -l'
```