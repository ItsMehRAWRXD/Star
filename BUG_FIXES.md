# Bug Fixes Documentation

This document describes the three critical bugs that were identified and fixed in the codebase.

## Bug 1: Security Vulnerability - Hardcoded Encryption Keys

**Severity**: Critical
**Files Affected**: `encryptor.cpp`, `xll_dropper.cpp`, `Star.cpp`, `dragdrop_encryptor_gui.py`

### Problem
All encryption keys were hardcoded in the source code, making the encryption easily reversible by anyone with access to the source code. This completely defeats the purpose of encryption.

### Root Cause
```cpp
// Before fix - hardcoded key
uint8_t encKey[] = { 0x39,0x39,0x08,0x0F,0x0F,0x38,0x08,0x31,0x38,0x32,0x38 };
```

### Solution
Implemented environment variable-based key management with fallback to obfuscated keys:

```cpp
// After fix - environment variable support
inline void decryptKey(uint8_t* keyBuf) {
    const char* envKey = std::getenv("ENCRYPTION_KEY");
    if (envKey && strlen(envKey) >= keyLen) {
        // Use environment variable key
        for (size_t i = 0; i < keyLen; ++i) {
            keyBuf[i] = static_cast<uint8_t>(envKey[i]);
        }
    } else {
        // Fallback to obfuscated key
        for (size_t i = 0; i < keyLen; ++i) {
            keyBuf[i] = encKey[i] ^ XOR_OBFUSCATE_KEY;
        }
    }
}
```

### Usage
Set the environment variable before running the application:
```bash
export ENCRYPTION_KEY="your-secure-key-here"
./encryptor input.txt output.enc
```

## Bug 2: Logic Error - Incorrect AES-CTR Implementation

**Severity**: High
**Files Affected**: `encryptor.cpp`, `dragdrop_encryptor_gui.py`

### Problem
The AES-CTR implementation used a zero IV (nonce), which is cryptographically insecure. In CTR mode, reusing the same nonce with the same key can lead to complete loss of security.

### Root Cause
```cpp
// Before fix - insecure zero IV
unsigned char iv[16] = {0}; // Using zero IV for simplicity; change as needed.
```

### Solution
Implemented proper nonce generation and management:

```cpp
// After fix - secure nonce generation
bool generateNonce(unsigned char* nonce, size_t nonceLen) {
    return RAND_bytes(nonce, nonceLen) == 1;
}

// Generate secure nonce for CTR mode
unsigned char nonce[16];
if (!generateNonce(nonce, 16)) {
    std::cerr << "Failed to generate secure nonce." << std::endl;
    return 1;
}

// Write nonce to output file for decryption
fout.write(reinterpret_cast<char*>(nonce), 16);
```

### Security Impact
- Each encryption now uses a unique, cryptographically secure nonce
- Nonce is prepended to encrypted data for proper decryption
- Eliminates the risk of nonce reuse attacks

## Bug 3: Performance Issue - Inefficient File Reading

**Severity**: Medium
**Files Affected**: `xll_dropper.cpp`

### Problem
The code read entire files into memory at once using `std::ios::ate` and `tellg()`, which can cause:
- Memory exhaustion with large files
- Poor performance due to unnecessary memory allocation
- Potential crashes on systems with limited memory

### Root Cause
```cpp
// Before fix - inefficient memory loading
std::ifstream fin(inPath, std::ios::binary | std::ios::ate);
std::streamsize size = fin.tellg();
fin.seekg(0, std::ios::beg);
std::vector<unsigned char> encBuf(size);
fin.read(reinterpret_cast<char*>(encBuf.data()), size);
```

### Solution
Implemented streaming file processing:

```cpp
// After fix - streaming processing
bool aesCtrCryptStream(std::ifstream& inFile, std::ofstream& outFile,
                       const unsigned char* aesKey, const unsigned char* nonce) {
    std::vector<unsigned char> inBuffer(4096);
    std::vector<unsigned char> outBuffer(4096 + EVP_MAX_BLOCK_LENGTH);

    while (inFile.read(reinterpret_cast<char*>(inBuffer.data()), inBuffer.size()) || inFile.gcount() > 0) {
        std::streamsize bytesRead = inFile.gcount();
        int outLen = 0;
        ok = EVP_DecryptUpdate(ctx, outBuffer.data(), &outLen,
                              inBuffer.data(), static_cast<int>(bytesRead)) == 1;
        if (!ok) break;
        outFile.write(reinterpret_cast<char*>(outBuffer.data()), outLen);
    }
}
```

### Performance Impact
- Constant memory usage regardless of file size
- Better performance for large files
- Reduced risk of memory-related crashes
- More efficient resource utilization

## Testing the Fixes

To verify the fixes work correctly:

1. **Test Key Management**:
   ```bash
   export ENCRYPTION_KEY="test-key-12345"
   ./encryptor sample.txt test.enc
   ./xll_dropper test.enc test.dec
   ```

2. **Test AES-CTR Security**:
   - Encrypt the same file multiple times
   - Verify that the encrypted outputs are different (due to unique nonces)
   - Verify that decryption works correctly

3. **Test Large File Handling**:
   ```bash
   # Create a large test file
   dd if=/dev/zero of=large_test.bin bs=1M count=100
   ./encryptor large_test.bin large_test.enc
   ./xll_dropper large_test.enc large_test.dec
   ```

## Security Recommendations

1. **Always use environment variables** for production keys
2. **Rotate keys regularly** in production environments
3. **Use a secure key management system** (e.g., AWS KMS, Azure Key Vault)
4. **Never commit keys to version control**
5. **Implement proper key derivation** for additional security

## Compatibility Notes

- The new nonce-based AES-CTR format is not backward compatible with files encrypted using the old zero-IV method
- Existing encrypted files will need to be re-encrypted using the new format
- The environment variable feature is backward compatible and will fall back to the old obfuscated key if not set