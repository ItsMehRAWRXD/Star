# Visual Studio 2022 Universal Native Encryptor

A high-performance, native C++ file encryption tool built with Visual Studio 2022. This encryptor supports both AES-128-CTR and ChaCha20 encryption algorithms with zero external dependencies.

## Features

- **Pure Native Implementation**: No external libraries required
- **Dual Algorithm Support**: AES-128-CTR and ChaCha20 encryption
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

### Basic Usage
```cmd
# Encrypt a file with AES (default)
VS2022Encryptor.exe input_file.exe encrypted_file.bin

# Encrypt a file with ChaCha20
VS2022Encryptor.exe input_file.exe encrypted_file.bin chacha20
```

### Command Line Options
```
Usage: VS2022Encryptor.exe <inputfile> <outputfile> [algorithm]

Parameters:
  inputfile   - Path to the file you want to encrypt
  outputfile  - Path where the encrypted file will be saved
  algorithm   - Encryption algorithm: "aes" (default) or "chacha20"

Examples:
  VS2022Encryptor.exe file.exe encrypted_file.bin
  VS2022Encryptor.exe file.exe encrypted_file.bin aes
  VS2022Encryptor.exe file.exe encrypted_file.bin chacha20
```

## Encryption Details

### AES-128-CTR Mode
- **Key Size**: 128 bits (16 bytes)
- **Nonce Size**: 128 bits (16 bytes)
- **Mode**: Counter (CTR)
- **Algorithm ID**: 0x01

### ChaCha20
- **Key Size**: 256 bits (32 bytes)
- **Nonce Size**: 96 bits (12 bytes)
- **Rounds**: 20
- **Algorithm ID**: 0x02

### Output Format
The encrypted file contains:
1. **Algorithm Identifier** (1 byte): 0x01 for AES, 0x02 for ChaCha20
2. **Nonce** (16 bytes for AES, 12 bytes for ChaCha20)
3. **Encrypted Data** (variable length)

## Security Features

- **Random Key Generation**: Each encryption uses a new random key
- **Random Nonce**: Unique nonce for each encryption
- **No Key Storage**: Keys are not stored with the encrypted data
- **Cryptographic Strength**: Uses industry-standard algorithms

## Performance

- **AES-128-CTR**: Optimized for speed with native implementation
- **ChaCha20**: High-performance stream cipher
- **Memory Efficient**: Processes files in 4KB chunks
- **No External Dependencies**: Reduces attack surface

## Project Structure

```
VS2022Encryptor/
├── VS2022Encryptor.vcxproj    # Visual Studio project file
├── main.cpp                   # Main source file with encryption logic
├── encryptor.h               # Header file with function declarations
└── README.md                 # This file

vs2022_encryptor.sln          # Visual Studio solution file
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