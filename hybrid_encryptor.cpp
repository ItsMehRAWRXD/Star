#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <ctime>
#include <string>
#include "chacha20.h"

// AES S-box (from original encryptor)
static const uint8_t sbox[256] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

// Round constants for key expansion
static const uint8_t rcon[10] = {
    0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36
};

// Simple random number generator
inline uint32_t simpleRand() {
    static uint32_t seed = std::time(nullptr) + std::clock();
    seed = seed * 1103515245 + 12345;
    return seed;
}

// Random key generation for AES (16 bytes)
inline void generateRandomAESKey(uint8_t* key) {
    for (int i = 0; i < 16; i++) {
        key[i] = simpleRand() & 0xFF;
    }
}

// Random key generation for ChaCha20 (32 bytes)
inline void generateRandomChaChaKey(uint8_t* key) {
    for (int i = 0; i < 32; i++) {
        key[i] = simpleRand() & 0xFF;
    }
}

// Random nonce generation for AES (16 bytes)
inline void generateRandomAESNonce(uint8_t* nonce) {
    for (int i = 0; i < 16; i++) {
        nonce[i] = simpleRand() & 0xFF;
    }
}

// Random nonce generation for ChaCha20 (12 bytes)
inline void generateRandomChaChaNonce(uint8_t* nonce) {
    for (int i = 0; i < 12; i++) {
        nonce[i] = simpleRand() & 0xFF;
    }
}

// AES helper functions
inline uint8_t gmul(uint8_t a, uint8_t b) {
    uint8_t p = 0;
    for (int i = 0; i < 8; i++) {
        if (b & 1) p ^= a;
        bool hi_bit_set = a & 0x80;
        a <<= 1;
        if (hi_bit_set) a ^= 0x1b;
        b >>= 1;
    }
    return p;
}

inline void subBytes(uint8_t* state) {
    for (int i = 0; i < 16; i++) {
        state[i] = sbox[state[i]];
    }
}

inline void shiftRows(uint8_t* state) {
    uint8_t temp;
    temp = state[1];
    state[1] = state[5];
    state[5] = state[9];
    state[9] = state[13];
    state[13] = temp;
    
    temp = state[2]; state[2] = state[10]; state[10] = temp;
    temp = state[6]; state[6] = state[14]; state[14] = temp;
    
    temp = state[3];
    state[3] = state[15];
    state[15] = state[11];
    state[11] = state[7];
    state[7] = temp;
}

inline void mixColumns(uint8_t* state) {
    for (int i = 0; i < 4; i++) {
        uint8_t s0 = state[i*4], s1 = state[i*4+1], s2 = state[i*4+2], s3 = state[i*4+3];
        state[i*4] = gmul(0x02, s0) ^ gmul(0x03, s1) ^ s2 ^ s3;
        state[i*4+1] = s0 ^ gmul(0x02, s1) ^ gmul(0x03, s2) ^ s3;
        state[i*4+2] = s0 ^ s1 ^ gmul(0x02, s2) ^ gmul(0x03, s3);
        state[i*4+3] = gmul(0x03, s0) ^ s1 ^ s2 ^ gmul(0x02, s3);
    }
}

inline void addRoundKey(uint8_t* state, const uint8_t* roundKey) {
    for (int i = 0; i < 16; i++) {
        state[i] ^= roundKey[i];
    }
}

inline void keyExpansion(const uint8_t* key, uint8_t* roundKeys) {
    uint8_t temp[4];
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            roundKeys[i*4 + j] = key[i*4 + j];
        }
    }
    
    for (int i = 4; i < 44; i++) {
        for (int j = 0; j < 4; j++) {
            temp[j] = roundKeys[(i-1)*4 + j];
        }
        if (i % 4 == 0) {
            uint8_t t = temp[0];
            temp[0] = temp[1];
            temp[1] = temp[2];
            temp[2] = temp[3];
            temp[3] = t;
            for (int j = 0; j < 4; j++) {
                temp[j] = sbox[temp[j]];
            }
            temp[0] ^= rcon[i/4 - 1];
        }
        for (int j = 0; j < 4; j++) {
            roundKeys[i*4 + j] = roundKeys[(i-4)*4 + j] ^ temp[j];
        }
    }
}

inline void aesEncryptBlock(const uint8_t* input, uint8_t* output, const uint8_t* roundKeys) {
    uint8_t state[16];
    for (int i = 0; i < 16; i++) {
        state[i] = input[i];
    }
    
    addRoundKey(state, roundKeys);
    
    for (int round = 1; round < 10; round++) {
        subBytes(state);
        shiftRows(state);
        mixColumns(state);
        addRoundKey(state, roundKeys + round*16);
    }
    
    subBytes(state);
    shiftRows(state);
    addRoundKey(state, roundKeys + 160);
    
    for (int i = 0; i < 16; i++) {
        output[i] = state[i];
    }
}

inline void incrementCounter(uint8_t* counter) {
    for (int i = 15; i >= 0; i--) {
        counter[i]++;
        if (counter[i] != 0) break;
    }
}

inline void aesCtrCrypt(const uint8_t* input, uint8_t* output, size_t length, 
                       const uint8_t* key, const uint8_t* nonce) {
    uint8_t roundKeys[176];
    keyExpansion(key, roundKeys);
    
    uint8_t counter[16];
    memcpy(counter, nonce, 16);
    
    uint8_t keystream[16];
    for (size_t i = 0; i < length; i += 16) {
        aesEncryptBlock(counter, keystream, roundKeys);
        size_t remaining = length - i;
        size_t toProcess = (remaining < 16) ? remaining : 16;
        for (size_t j = 0; j < toProcess; j++) {
            output[i + j] = input[i + j] ^ keystream[j];
        }
        incrementCounter(counter);
    }
}

// Encryption function that chooses between AES and ChaCha20
void hybridEncrypt(const uint8_t* input, uint8_t* output, size_t length, 
                   const uint8_t* key, const uint8_t* nonce, bool useChaCha20) {
    if (useChaCha20) {
        chacha20CtrCrypt(input, output, length, key, nonce);
    } else {
        aesCtrCrypt(input, output, length, key, nonce);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <input_file> <output_file> <algorithm>" << std::endl;
        std::cerr << "Algorithm: 'aes' or 'chacha20'" << std::endl;
        return 1;
    }

    std::string algorithm = argv[3];
    bool useChaCha20 = (algorithm == "chacha20");
    
    if (algorithm != "aes" && algorithm != "chacha20") {
        std::cerr << "Error: Algorithm must be 'aes' or 'chacha20'" << std::endl;
        return 1;
    }

    // Generate appropriate key and nonce
    uint8_t key[32];  // Use 32 bytes for both (AES uses first 16)
    uint8_t nonce[16]; // Use 16 bytes for both (ChaCha20 uses first 12)
    
    if (useChaCha20) {
        generateRandomChaChaKey(key);
        generateRandomChaChaNonce(nonce);
    } else {
        generateRandomAESKey(key);
        generateRandomAESNonce(nonce);
    }

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
    hybridEncrypt(inputBuffer.data(), outputBuffer.data(), fileSize, key, nonce, useChaCha20);

    // Open output file
    std::ofstream fout(argv[2], std::ios::binary);
    if (!fout) {
        std::cerr << "Error: Cannot create output file " << argv[2] << std::endl;
        return 1;
    }

    // Write algorithm identifier (1 byte)
    uint8_t algoId = useChaCha20 ? 0x01 : 0x00;
    fout.write(reinterpret_cast<const char*>(&algoId), 1);

    // Write key and nonce
    if (useChaCha20) {
        fout.write(reinterpret_cast<const char*>(key), 32);
        fout.write(reinterpret_cast<const char*>(nonce), 12);
    } else {
        fout.write(reinterpret_cast<const char*>(key), 16);
        fout.write(reinterpret_cast<const char*>(nonce), 16);
    }

    // Write encrypted data
    fout.write(reinterpret_cast<const char*>(outputBuffer.data()), fileSize);
    fout.close();

    std::cout << (useChaCha20 ? "ChaCha20" : "AES") << " encryption completed successfully!" << std::endl;
    std::cout << "Key (hex): ";
    int keySize = useChaCha20 ? 32 : 16;
    for (int i = 0; i < keySize; i++) {
        printf("%02x", key[i]);
    }
    std::cout << std::endl;
    std::cout << "Nonce (hex): ";
    int nonceSize = useChaCha20 ? 12 : 16;
    for (int i = 0; i < nonceSize; i++) {
        printf("%02x", nonce[i]);
    }
    std::cout << std::endl;

    return 0;
}