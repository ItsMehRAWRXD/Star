#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <ctime>
#include "chacha20.h"

// Simple random number generator (for key and nonce generation)
inline uint32_t simpleRand() {
    static uint32_t seed = std::time(nullptr) + std::clock();
    seed = seed * 1103515245 + 12345;
    return seed;
}

// Random ChaCha20 key generation (32 bytes)
inline void generateRandomChaChaKey(uint8_t* key) {
    for (int i = 0; i < 32; i++) {
        key[i] = simpleRand() & 0xFF;
    }
}

// Random ChaCha20 nonce generation (12 bytes)
inline void generateRandomChaChaNonce(uint8_t* nonce) {
    for (int i = 0; i < 12; i++) {
        nonce[i] = simpleRand() & 0xFF;
    }
}

// Fallback key handling (shared with other components)
constexpr uint8_t XOR_OBFUSCATE_KEY = 0x5A;
uint8_t encKey[] = { 0x39,0x39,0x08,0x0F,0x0F,0x38,0x08,0x31,0x38,0x32,0x38 };
constexpr size_t keyLen = sizeof(encKey);

inline void decryptKey(uint8_t* keyBuf) {
    const char* envKey = std::getenv("ENCRYPTION_KEY");
    if (envKey && strlen(envKey) >= keyLen) {
        for (size_t i = 0; i < keyLen; ++i) {
            keyBuf[i] = static_cast<uint8_t>(envKey[i]);
        }
    } else {
        for (size_t i = 0; i < keyLen; ++i) {
            keyBuf[i] = encKey[i] ^ XOR_OBFUSCATE_KEY;
        }
    }
}

// ChaCha20 encryption function
inline void chacha20Encrypt(const uint8_t* input, uint8_t* output, size_t length, 
                           const uint8_t* key, const uint8_t* nonce) {
    ChaCha20 chacha;
    chacha.init(key, nonce);
    
    // Copy input to output first
    memcpy(output, input, length);
    
    // Then encrypt in place
    chacha.crypt(output, length);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_file> <output_file>" << std::endl;
        return 1;
    }

    // Generate random ChaCha20 key and nonce
    uint8_t key[32];
    uint8_t nonce[12];
    generateRandomChaChaKey(key);
    generateRandomChaChaNonce(nonce);

    // Open input file
    std::ifstream fin(argv[1], std::ios::binary);
    if (!fin) {
        std::cerr << "Error: Cannot open input file " << argv[1] << std::endl;
        return 1;
    }

    // Read file content
    fin.seekg(0, std::ios::end);
    size_t fileSize = fin.tellg();
    fin.seekg(0, std::ios::beg);

    std::vector<uint8_t> inputBuffer(fileSize);
    fin.read(reinterpret_cast<char*>(inputBuffer.data()), fileSize);
    fin.close();

    // Encrypt data
    std::vector<uint8_t> outputBuffer(fileSize);
    chacha20Encrypt(inputBuffer.data(), outputBuffer.data(), fileSize, key, nonce);

    // Open output file
    std::ofstream fout(argv[2], std::ios::binary);
    if (!fout) {
        std::cerr << "Error: Cannot create output file " << argv[2] << std::endl;
        return 1;
    }

    // Write key and nonce first (for decryption)
    fout.write(reinterpret_cast<const char*>(key), 32);
    fout.write(reinterpret_cast<const char*>(nonce), 12);

    // Write encrypted data
    fout.write(reinterpret_cast<const char*>(outputBuffer.data()), fileSize);
    fout.close();

    std::cout << "ChaCha20 encryption completed successfully!" << std::endl;
    std::cout << "Key (hex): ";
    for (int i = 0; i < 32; i++) {
        printf("%02x", key[i]);
    }
    std::cout << std::endl;
    std::cout << "Nonce (hex): ";
    for (int i = 0; i < 12; i++) {
        printf("%02x", nonce[i]);
    }
    std::cout << std::endl;

    return 0;
}