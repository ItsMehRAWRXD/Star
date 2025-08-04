# ChaCha20 Encryption Support

This project now supports both AES-128-CTR and ChaCha20 encryption algorithms. ChaCha20 is a modern, high-performance stream cipher that provides excellent security and is often faster than AES on many platforms.

## New Components

### 1. ChaCha20 Implementation (`chacha20.h`)
- Pure C++ implementation of ChaCha20 stream cipher
- No external dependencies
- Supports 32-byte keys and 12-byte nonces
- Implements ChaCha20-CTR mode for encryption/decryption

### 2. ChaCha20 Encryptor (`chacha20_encryptor.cpp`)
- Standalone ChaCha20 file encryption tool
- Generates random 32-byte keys and 12-byte nonces
- Outputs encrypted files with embedded key/nonce for decryption

### 3. Hybrid Encryptor (`hybrid_encryptor.cpp`)
- Supports both AES and ChaCha20 encryption
- Choose algorithm via command line parameter
- Automatically handles different key/nonce sizes for each algorithm

### 4. ChaCha20 Test Stub (`chacha20_test_stub.cpp`)
- Test stub that uses ChaCha20 for payload decryption
- Follows same pattern as existing AES stubs
- Includes embedded ChaCha20 key and nonce

### 5. Hybrid Stub (`hybrid_stub.cpp`)
- Universal stub that can handle both AES and ChaCha20 encrypted payloads
- Automatically detects encryption algorithm from payload format
- Supports format: `[algorithm_id][key][nonce][encrypted_data]`

## Building

```bash
# Build all ChaCha20 components
make all

# Build specific components
make chacha20_encryptor
make hybrid_encryptor
make chacha20_test_stub
make hybrid_stub
```

## Usage Examples

### ChaCha20 Encryption
```bash
# Encrypt a file with ChaCha20
./chacha20_encryptor input.txt output.bin

# The output file contains: [32-byte key][12-byte nonce][encrypted data]
```

### Hybrid Encryption
```bash
# Encrypt with AES
./hybrid_encryptor input.txt output_aes.bin aes

# Encrypt with ChaCha20
./hybrid_encryptor input.txt output_chacha.bin chacha20

# The output file contains: [1-byte algo_id][key][nonce][encrypted data]
```

### Testing
```bash
# Test ChaCha20 encryption
make test_chacha20

# Test hybrid encryption (both AES and ChaCha20)
make test_hybrid
```

## File Formats

### ChaCha20 Encryptor Output
```
[32-byte ChaCha20 key][12-byte ChaCha20 nonce][encrypted data]
```

### Hybrid Encryptor Output
```
[1-byte algorithm ID][key][nonce][encrypted data]
```

Algorithm IDs:
- `0x00`: AES-128-CTR (16-byte key, 16-byte nonce)
- `0x01`: ChaCha20 (32-byte key, 12-byte nonce)

## Security Features

### ChaCha20 Advantages
- **Performance**: Often faster than AES on many platforms
- **Security**: 256-bit security level with ChaCha20
- **Simplicity**: Simpler implementation than AES
- **No S-boxes**: Resistant to cache-timing attacks
- **Modern**: Designed for high-performance applications

### Key Features
- **Random Key Generation**: Each encryption uses a new random key
- **Random Nonce Generation**: Each encryption uses a new random nonce
- **CTR Mode**: Counter mode for stream cipher operation
- **No Padding**: Stream cipher doesn't require padding

## Integration with Existing Code

The ChaCha20 implementation is designed to work alongside the existing AES implementation:

1. **Same Interface**: Both use similar function signatures
2. **Compatible Stubs**: Hybrid stubs can handle both algorithms
3. **Same Security Level**: Both provide 128-bit+ security
4. **No Breaking Changes**: Existing AES code continues to work

## Performance Comparison

| Algorithm | Key Size | Nonce Size | Performance | Security |
|-----------|----------|------------|-------------|----------|
| AES-128-CTR | 16 bytes | 16 bytes | Good | 128-bit |
| ChaCha20 | 32 bytes | 12 bytes | Excellent | 256-bit |

## Installation

```bash
# Install ChaCha20 tools system-wide
make install

# Uninstall
make uninstall
```

## Cleanup

```bash
# Remove all built files
make clean
```

## Compatibility

- **Platforms**: Linux, macOS, Windows (with appropriate compiler)
- **Dependencies**: None (pure C++ implementation)
- **C++ Standard**: C++11 or later
- **Compiler**: GCC, Clang, MSVC

## Security Notes

1. **Key Management**: Keys are embedded in encrypted files for simplicity. In production, use proper key management.
2. **Random Generation**: Uses simple PRNG for demo purposes. Use cryptographically secure RNG in production.
3. **Nonce Reuse**: Never reuse nonces with the same key.
4. **Key Size**: ChaCha20 uses 256-bit keys for maximum security.

## Future Enhancements

- ChaCha20-Poly1305 AEAD mode
- XChaCha20 for extended nonce support
- Hardware acceleration detection
- Key derivation functions (PBKDF2, Argon2)
- Secure key storage integration