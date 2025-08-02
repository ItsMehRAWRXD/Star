# AES Encryption Implementation

This project has been updated to use AES-256-CBC encryption instead of simple XOR encryption for enhanced security.

## Changes Made

### 1. Encryptor (`encryptor.cpp`)
- **Before**: Used simple XOR encryption with a repeating key
- **After**: Uses AES-256-CBC encryption with OpenSSL
- **Key Derivation**: The original XOR key is used as a seed to generate a 32-byte AES key and 16-byte IV
- **Compatibility**: Maintains the same key derivation process for backward compatibility

### 2. Star Application (`Star.cpp`)
- **Before**: Used XOR decryption with repeating key pattern
- **After**: Uses AES-256-CBC decryption with OpenSSL
- **Decryption**: Properly handles AES block cipher decryption with padding

### 3. Project Configuration
- Added OpenSSL library dependencies (`libcrypto.lib`, `libssl.lib`) to the Visual Studio project
- Created Makefile for compiling the encryptor on Linux/Unix systems

## Building

### Windows (Visual Studio)
1. Install OpenSSL development libraries
2. Open `Star.sln` in Visual Studio
3. Build the solution

### Linux/Unix
```bash
# Install OpenSSL development package
sudo apt-get install libssl-dev  # Ubuntu/Debian
# or
sudo yum install openssl-devel   # CentOS/RHEL

# Build encryptor
make
```

## Usage

### Encrypting a file:
```bash
./encryptor input_file output_file
```

### The Star application will:
1. Extract the encrypted payload from itself
2. Decrypt it using AES-256-CBC
3. Execute the decrypted payload
4. Clean up temporary files

## Security Improvements

- **AES-256-CBC**: Industry-standard encryption algorithm
- **Proper Key Derivation**: Uses the original key as a seed for AES key generation
- **Initialization Vector**: Adds randomness to prevent pattern analysis
- **Block Cipher**: Provides better security than stream cipher XOR

## Dependencies

- OpenSSL 1.1.1 or later
- C++17 compatible compiler
- Windows API (for Star application)

## Notes

- The encryption/decryption process is now more secure but slightly slower due to AES overhead
- File sizes may increase slightly due to AES padding
- The original key obfuscation mechanism is preserved for compatibility