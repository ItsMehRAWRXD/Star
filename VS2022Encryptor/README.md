# Visual Studio 2022 Universal Native Encryptor

A high-performance, native C++ file encryption tool built with Visual Studio 2022. This encryptor supports both basic encryption (AES-128-CTR and ChaCha20) and advanced stealth triple-layer encryption with decimal key representation and stub generation capabilities.

## Features

- **Pure Native Implementation**: No external libraries required
- **Multiple Encryption Modes**: Basic (AES-128-CTR, ChaCha20) and Advanced (Triple-Layer Stealth)
- **Stealth Capabilities**: Decimal key representation to avoid detection
- **Stub Generation**: Creates self-contained executable stubs
- **Dynamic Entropy**: Advanced entropy mixing for key generation
- **Visual Studio 2022 Ready**: Complete project files included
- **Cross-Platform Compatible**: Works on Windows x86 and x64
- **High Performance**: Optimized for large file encryption
- **Secure**: Uses cryptographically secure random key generation

## Requirements

- Visual Studio 2022 (Community, Professional, or Enterprise)
- Windows 10 or later
- C++17 support

## Building the Project

### Method 1: Using Visual Studio 2022 IDE
1. Open `vs2022_encryptor.sln` in Visual Studio 2022
2. Select your desired configuration (Debug/Release) and platform (x86/x64)
3. Build the solution (Ctrl+Shift+B)

### Method 2: Using Command Line
```cmd
# Navigate to the project directory
cd VS2022Encryptor

# Build for x64 Release
msbuild VS2022Encryptor.vcxproj /p:Configuration=Release /p:Platform=x64

# Build for x86 Release
msbuild VS2022Encryptor.vcxproj /p:Configuration=Release /p:Platform=Win32
```

## Usage

### Basic Encryption
```cmd
# Encrypt a file with AES (default)
VS2022Encryptor.exe -b input_file.exe encrypted_file.bin

# Encrypt a file with ChaCha20
VS2022Encryptor.exe -b input_file.exe encrypted_file.bin chacha20
```

### Stealth Triple Encryption
```cmd
# Encrypt a file with triple-layer stealth encryption
VS2022Encryptor.exe -e payload.exe encrypted_payload.bin

# Generate a stealth stub from a payload
VS2022Encryptor.exe -s payload.exe stealth_stub.cpp
```

### Command Line Options
```
Usage: VS2022Encryptor.exe <mode> [options]

Modes:
  -b    Basic encryption (AES-128-CTR or ChaCha20)
  -e    Stealth triple encryption
  -s    Generate stealth stub

Basic Encryption:
  VS2022Encryptor.exe -b <inputfile> <outputfile> [algorithm]
  Algorithm: "aes" (default) or "chacha20"

Stealth Encryption:
  VS2022Encryptor.exe -e <input> <output>

Stub Generation:
  VS2022Encryptor.exe -s <payload> <stub.cpp>

Examples:
  VS2022Encryptor.exe -b file.exe encrypted_file.bin
  VS2022Encryptor.exe -b file.exe encrypted_file.bin chacha20
  VS2022Encryptor.exe -e payload.exe encrypted_payload.bin
  VS2022Encryptor.exe -s payload.exe stealth_stub.cpp
```

## Encryption Details

### Basic Encryption

#### AES-128-CTR Mode
- **Key Size**: 128 bits (16 bytes)
- **Nonce Size**: 128 bits (16 bytes)
- **Mode**: Counter (CTR)
- **Algorithm ID**: 0x01

#### ChaCha20
- **Key Size**: 256 bits (32 bytes)
- **Nonce Size**: 96 bits (12 bytes)
- **Rounds**: 20
- **Algorithm ID**: 0x02

#### Basic Output Format
The encrypted file contains:
1. **Algorithm Identifier** (1 byte): 0x01 for AES, 0x02 for ChaCha20
2. **Nonce** (16 bytes for AES, 12 bytes for ChaCha20)
3. **Encrypted Data** (variable length)

### Stealth Triple Encryption

#### Triple-Layer Protection
- **Layer 1**: AES-128 (simplified)
- **Layer 2**: ChaCha20 (simplified)
- **Layer 3**: XOR with variable-length key
- **Randomized Order**: Encryption layers applied in random sequence

#### Key Features
- **Decimal Representation**: Keys stored as large decimal numbers (less detectable than hex)
- **Dynamic Entropy**: Uses time, memory, thread, and counter components
- **Variable XOR Key**: 16-32 byte random length
- **Obfuscated Variables**: Random variable names in generated stubs

#### Stealth Output Format
- **Encrypted Data**: Triple-layer encrypted payload
- **Key File**: Separate `.keys` file with decimal representations
- **Stub Generation**: Self-contained C++ source with embedded decryption

## Security Features

- **Random Key Generation**: Each encryption uses a new random key
- **Random Nonce**: Unique nonce for each encryption
- **Dynamic Entropy Mixing**: Advanced entropy collection from multiple sources
- **Triple-Layer Protection**: Multiple encryption layers with randomized order
- **Decimal Key Representation**: Less detectable than traditional hex encoding
- **No Key Storage**: Keys are not stored with the encrypted data (basic mode)
- **Cryptographic Strength**: Uses industry-standard algorithms
- **Stealth Capabilities**: Designed to avoid detection by security tools

## Performance

- **AES-128-CTR**: Optimized for speed with native implementation
- **ChaCha20**: High-performance stream cipher
- **Triple-Layer**: Efficient multi-layer encryption
- **Memory Efficient**: Processes files in 4KB chunks
- **No External Dependencies**: Reduces attack surface
- **Stub Generation**: Fast compilation-ready C++ output

## Project Structure

```
VS2022Encryptor/
├── VS2022Encryptor.vcxproj           # Visual Studio project file
├── main.cpp                          # Main source file with all encryption modes
├── encryptor.h                      # Header file for basic encryption functions
├── stealth_triple_encryptor.h       # Header file for stealth encryption
├── stealth_triple_encryptor.cpp     # Implementation of stealth encryption
├── build.bat                        # Batch file for easy building
├── .gitignore                       # Git ignore file
└── README.md                        # This file

vs2022_encryptor.sln                 # Visual Studio solution file
```

## Building for Different Platforms

### x86 (32-bit)
- Platform: Win32
- Target: 32-bit Windows applications
- Use: `/p:Platform=Win32`

### x64 (64-bit)
- Platform: x64
- Target: 64-bit Windows applications
- Use: `/p:Platform=x64`

## Troubleshooting

### Common Issues

1. **"Failed to open input file"**
   - Check if the input file exists
   - Verify file path is correct
   - Ensure you have read permissions

2. **"Failed to open output file"**
   - Check if you have write permissions to the output directory
   - Ensure the output directory exists
   - Verify disk space is available

3. **Build Errors**
   - Ensure Visual Studio 2022 is installed
   - Check that C++17 support is enabled
   - Verify Windows SDK is installed

## License

This project is provided as-is for educational and development purposes.

## Contributing

Feel free to submit issues and enhancement requests!

## Version History

- **v1.0**: Initial release with AES-128-CTR and ChaCha20 support
- Built with Visual Studio 2022 v143 toolset
- Supports Windows 10+ platforms