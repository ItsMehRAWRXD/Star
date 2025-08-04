# Visual Studio 2022 Universal PE Packer

A professional-grade PE (Portable Executable) packer with advanced encryption capabilities and self-decrypting executables. This tool provides multiple encryption modes for protecting executables while maintaining their functionality.

## Features

### 🔐 Multiple Encryption Modes

1. **Basic Encryption (AES/ChaCha20)**
   - AES-128-CTR encryption
   - ChaCha20 stream cipher
   - Standard output with headers
   - Raw binary output option

2. **Stealth Triple Encryption**
   - Triple-layer encryption (AES + ChaCha20 + XOR)
   - Randomized encryption order
   - Decimal key representation for obfuscation
   - Dynamic entropy mixing

3. **PE-Aware Encryption**
   - Preserves executable structure
   - Encrypts only code sections (.text, etc.)
   - Maintains PE headers and data sections
   - Self-decrypting executables
   - Embedded keys for self-contained operation

### 🛡️ Security Features

- **Advanced Encryption**: AES-128-CTR and ChaCha20 implementations
- **Key Obfuscation**: Keys stored as large decimal numbers
- **Dynamic Entropy**: Time-based, memory-based, and thread-based entropy mixing
- **PE Structure Preservation**: Maintains executable integrity
- **Self-Contained**: No external key files required for PE encryption

### 🚀 Performance

- **Native C++**: No external dependencies
- **Efficient Algorithms**: Optimized encryption implementations
- **Stream Processing**: Handles large files efficiently
- **Memory Safe**: Proper memory management and bounds checking

## Quick Start

### Building

1. **Open in Visual Studio 2022**
   ```
   Open vs2022_encryptor.sln
   ```

2. **Command Line Build**
   ```batch
   build.bat
   ```

3. **Manual Build**
   - Select Release configuration
   - Choose platform (x86 or x64)
   - Build Solution (Ctrl+Shift+B)

### Usage

Run the executable and select from the interactive menu:

```
=== Visual Studio 2022 Universal PE Packer ===
Professional-grade PE packer with self-decrypting executables

Select an option:
  1. Basic Encryption (AES/ChaCha20)
  2. Basic Encryption - Raw Binary Output
  3. Stealth Triple Encryption
  4. PE-Aware Encryption (Preserves Executable Structure)
  0. Exit

Enter your choice:
```

## Examples

### Basic File Encryption
```
Input:  document.txt
Output: document.txt.encrypted
Method: AES-128-CTR
```

### PE File Protection
```
Input:  calculator.exe
Output: calculator_protected.exe
Method: PE-Aware Triple Encryption
Result: Self-decrypting executable
```

### Raw Binary Output
```
Input:  data.bin
Output: data_encrypted.bin
Method: ChaCha20 (raw)
Format: Pure ciphertext, no headers
```

## File Structure

```
vs2022-universal-pe-packer/
├── vs2022_encryptor.sln          # Visual Studio solution
├── build.bat                     # Build script
├── README.md                     # This file
├── .gitignore                    # Git ignore rules
└── VS2022Encryptor/
    ├── VS2022Encryptor.vcxproj   # Project file
    ├── main.cpp                  # Main application logic
    ├── encryptor.h               # Basic encryption functions
    ├── stealth_triple_encryptor.h/.cpp  # Triple encryption
    ├── pe_encryptor.h/.cpp       # PE-aware encryption
    └── test_example.txt          # Test file
```

## Technical Details

### PE Structure Analysis
- **DOS Header**: Validates MZ signature
- **NT Headers**: Parses PE signature and file header
- **Section Headers**: Identifies code sections (.text, .code, etc.)
- **Code Sections**: Encrypted while preserving data sections

### Encryption Algorithms
- **AES-128-CTR**: Industry standard encryption
- **ChaCha20**: RFC 8439 compliant stream cipher
- **XOR Layer**: Variable key length obfuscation

### Key Management
- **Decimal Representation**: Keys converted to large decimal numbers
- **Embedded Keys**: Stored in new `.keys` PE section
- **Self-Contained**: No external key files required

## Security Considerations

### Anti-Analysis Features
- **Code Obfuscation**: Variable name randomization
- **Key Obfuscation**: Decimal representation
- **Entropy Mixing**: Multiple entropy sources
- **PE Structure**: Preserves legitimate executable appearance

### Limitations
- **Windows Only**: PE format is Windows-specific
- **Native Code**: Requires C++ runtime
- **Key Recovery**: Keys can be extracted from embedded sections

## License

This project is provided for educational and research purposes. Users are responsible for complying with applicable laws and regulations.

---

**Note**: This tool is designed for legitimate software protection and research purposes. Users must comply with all applicable laws and regulations regarding encryption and software protection.