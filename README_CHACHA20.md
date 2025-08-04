# ChaCha20 Encryption Tools

This project now includes ChaCha20 encryption support, a modern stream cipher that provides excellent security and performance.

## What is ChaCha20?

ChaCha20 is a stream cipher designed by Daniel J. Bernstein as a variant of the Salsa20 cipher. It offers:
- 256-bit security
- Better performance than AES on systems without hardware AES support
- Resistance to timing attacks
- Simple implementation

## ChaCha20 Tools Added

### 1. chacha_encryptor
Encrypts files using ChaCha20 with random keys.

**Usage:**
```bash
./chacha_encryptor <input_file> <output_file>
```

**Example:**
```bash
./chacha_encryptor payload.exe encrypted_payload.bin
```

The encryptor will:
- Generate a random 256-bit key
- Generate a random 96-bit nonce
- Encrypt the file using ChaCha20
- Write the nonce followed by the encrypted data to the output file
- Display the key and nonce for reference

### 2. chacha_stub_generator
Generates self-contained stub programs with embedded ChaCha20-encrypted payloads.

**Usage:**
```bash
./chacha_stub_generator <payload_file> <output_stub.cpp>
```

**Example:**
```bash
./chacha_stub_generator shellcode.bin chacha_stub.cpp
g++ -o chacha_stub chacha_stub.cpp
./chacha_stub
```

### 3. test_chacha
A test program that demonstrates ChaCha20 encryption/decryption.

**Usage:**
```bash
./test_chacha
```

### 4. chacha_enhanced_dropper
An enhanced dropper that uses ChaCha20 to encrypt configuration data and payloads.

## Key Features

- **Pure C++ Implementation**: No external dependencies required
- **Cross-Platform**: Works on Windows and Linux/Unix
- **Secure**: Uses cryptographically secure 256-bit keys and 96-bit nonces
- **Fast**: ChaCha20 is optimized for software implementations
- **Randomized**: Each encryption uses random keys and nonces

## Compilation

To compile the ChaCha20 tools:

```bash
# Compile the encryptor
g++ -o chacha_encryptor chacha_encryptor.cpp

# Compile the stub generator
g++ -o chacha_stub_generator chacha_stub_generator.cpp

# Compile the test program
g++ -o test_chacha test_chacha.cpp

# Compile the enhanced dropper
g++ -o chacha_enhanced_dropper chacha_enhanced_dropper.cpp
```

## Integration with Existing Tools

The ChaCha20 implementation can easily be integrated into existing stub and dropper files. Simply include the ChaCha20 functions and replace the AES encryption with ChaCha20 calls.

## Security Notes

- Always use random keys and nonces for each encryption
- Never reuse a nonce with the same key
- The implementation uses ChaCha20 with a 256-bit key and 96-bit nonce
- Counter starts at 0 for each encryption

## Technical Details

The ChaCha20 implementation includes:
- Quarter round function for the core ChaCha operation
- Block function that performs 20 rounds (10 double-rounds)
- State initialization with constants, key, nonce, and counter
- Stream cipher operation using XOR with keystream

The cipher operates on 64-byte blocks and can encrypt data of any length.