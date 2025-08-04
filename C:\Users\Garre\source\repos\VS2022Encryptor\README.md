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

## Building

### Prerequisites
- Visual Studio 2022
- Windows SDK 10.0 or later
- C++17 support

### Build Instructions

1. **Open in Visual Studio 2022**
   ```
   Open vs2022_encryptor.sln
   ```

2. **Build for Release**
   - Select Release configuration
   - Choose platform (x86 or x64)
   - Build Solution (Ctrl+Shift+B)

3. **Command Line Build**
   ```batch
   build.bat
   ```

## Usage

### Interactive Menu System

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

### Encryption Modes

#### 1. Basic Encryption
- **Algorithm**: AES-128-CTR or ChaCha20
- **Output**: Standard format with headers or raw binary
- **Use Case**: General file encryption

#### 2. Stealth Triple Encryption
- **Layers**: AES + ChaCha20 + XOR in random order
- **Keys**: Stored as decimal numbers in `.keys` file
- **Use Case**: High-security encryption with key obfuscation

#### 3. PE-Aware Encryption
- **Target**: Windows executables (.exe, .dll)
- **Method**: Encrypts code sections while preserving structure
- **Result**: Self-decrypting executable with embedded keys
- **Use Case**: Software protection and anti-analysis

## Technical Details

### PE Structure Analysis

The PE packer analyzes the Portable Executable format:

- **DOS Header**: Validates MZ signature
- **NT Headers**: Parses PE signature and file header
- **Section Headers**: Identifies code sections (.text, .code, etc.)
- **Code Sections**: Encrypted while preserving data sections

### Encryption Algorithms

#### AES-128-CTR
- **Key Size**: 128 bits
- **Mode**: Counter (CTR)
- **Implementation**: Pure C++ with S-box tables
- **Security**: Industry standard encryption

#### ChaCha20
- **Key Size**: 256 bits
- **Nonce**: 96 bits
- **Rounds**: 20
- **Implementation**: RFC 8439 compliant

#### XOR Layer
- **Key Size**: Variable (16-64 bytes)
- **Purpose**: Additional obfuscation layer
- **Randomization**: Variable key length per encryption

### Key Management

#### Decimal Representation
Keys are converted to large decimal numbers for obfuscation:
```
Original: 0x12 0x34 0x56 0x78
Decimal:  305419896
```

#### Embedded Keys (PE Mode)
- Keys stored in new `.keys` PE section
- Self-contained executables
- No external key files required
- Automatic decryption at runtime

## File Structure

```
VS2022Encryptor/
├── vs2022_encryptor.sln          # Visual Studio solution
├── VS2022Encryptor/
│   ├── VS2022Encryptor.vcxproj   # Project file
│   ├── main.cpp                  # Main application logic
│   ├── encryptor.h               # Basic encryption functions
│   ├── stealth_triple_encryptor.h/.cpp  # Triple encryption
│   ├── pe_encryptor.h/.cpp       # PE-aware encryption
│   ├── build.bat                 # Build script
│   └── README.md                 # This file
└── .gitignore                    # Git ignore rules
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

## Security Considerations

### Key Storage
- **Basic Mode**: Keys in output file headers
- **Stealth Mode**: Keys in separate `.keys` file
- **PE Mode**: Keys embedded in executable

### Anti-Analysis Features
- **Code Obfuscation**: Variable name randomization
- **Key Obfuscation**: Decimal representation
- **Entropy Mixing**: Multiple entropy sources
- **PE Structure**: Preserves legitimate executable appearance

### Limitations
- **Windows Only**: PE format is Windows-specific
- **Native Code**: Requires C++ runtime
- **Key Recovery**: Keys can be extracted from embedded sections

## Development

### Adding New Encryption Algorithms
1. Implement in `encryptor.h/.cpp`
2. Add to encryption layer selection
3. Update key generation functions
4. Test with various file types

### Extending PE Support
1. Add new section type detection
2. Implement custom section encryption
3. Update PE header parsing
4. Test with different executable formats

## License

This project is provided for educational and research purposes. Users are responsible for complying with applicable laws and regulations.

## Contributing

1. Fork the repository
2. Create a feature branch
3. Implement changes
4. Test thoroughly
5. Submit pull request

## Support

For issues and questions:
- Check the documentation
- Review the source code
- Test with different file types
- Verify system requirements

---

**Note**: This tool is designed for legitimate software protection and research purposes. Users must comply with all applicable laws and regulations regarding encryption and software protection.