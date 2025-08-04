#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <ctime>

// Pure native AES-128 implementation with no external dependencies

// AES S-box
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

// AES inverse S-box
static const uint8_t inv_sbox[256] = {
    0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
    0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
    0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
    0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
    0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
    0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
    0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
    0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
    0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
    0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
    0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
    0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
    0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
    0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
    0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d
};

// Round constants for key expansion
static const uint8_t rcon[10] = {
    0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36
};

// Simple random number generator (for key and nonce generation)
inline uint32_t simpleRand() {
    static uint32_t seed = std::time(nullptr) + std::clock();
    seed = seed * 1103515245 + 12345;
    return seed;
}

// Random key generation
inline void generateRandomKey(uint8_t* key) {
    for (int i = 0; i < 16; i++) {
        key[i] = simpleRand() & 0xFF;
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

inline void invSubBytes(uint8_t* state) {
    for (int i = 0; i < 16; i++) {
        state[i] = inv_sbox[state[i]];
    }
}

inline void shiftRows(uint8_t* state) {
    uint8_t temp;
    // Row 1: shift left by 1
    temp = state[1];
    state[1] = state[5];
    state[5] = state[9];
    state[9] = state[13];
    state[13] = temp;
    
    // Row 2: shift left by 2
    temp = state[2]; state[2] = state[10]; state[10] = temp;
    temp = state[6]; state[6] = state[14]; state[14] = temp;
    
    // Row 3: shift left by 3
    temp = state[3];
    state[3] = state[15];
    state[15] = state[11];
    state[11] = state[7];
    state[7] = temp;
}

inline void invShiftRows(uint8_t* state) {
    uint8_t temp;
    // Row 1: shift right by 1
    temp = state[13];
    state[13] = state[9];
    state[9] = state[5];
    state[5] = state[1];
    state[1] = temp;
    
    // Row 2: shift right by 2
    temp = state[2]; state[2] = state[10]; state[10] = temp;
    temp = state[6]; state[6] = state[14]; state[14] = temp;
    
    // Row 3: shift right by 3
    temp = state[3];
    state[3] = state[7];
    state[7] = state[11];
    state[11] = state[15];
    state[15] = temp;
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

inline void invMixColumns(uint8_t* state) {
    for (int i = 0; i < 4; i++) {
        uint8_t s0 = state[i*4], s1 = state[i*4+1], s2 = state[i*4+2], s3 = state[i*4+3];
        state[i*4] = gmul(0x0e, s0) ^ gmul(0x0b, s1) ^ gmul(0x0d, s2) ^ gmul(0x09, s3);
        state[i*4+1] = gmul(0x09, s0) ^ gmul(0x0e, s1) ^ gmul(0x0b, s2) ^ gmul(0x0d, s3);
        state[i*4+2] = gmul(0x0d, s0) ^ gmul(0x09, s1) ^ gmul(0x0e, s2) ^ gmul(0x0b, s3);
        state[i*4+3] = gmul(0x0b, s0) ^ gmul(0x0d, s1) ^ gmul(0x09, s2) ^ gmul(0x0e, s3);
    }
}

inline void addRoundKey(uint8_t* state, const uint8_t* roundKey) {
    for (int i = 0; i < 16; i++) {
        state[i] ^= roundKey[i];
    }
}

inline void keyExpansion(const uint8_t* key, uint8_t* roundKeys) {
    uint8_t temp[4];
    
    // Copy the original key
    for (int i = 0; i < 16; i++) {
        roundKeys[i] = key[i];
    }
    
    for (int i = 4; i < 44; i++) {
        for (int j = 0; j < 4; j++) {
            temp[j] = roundKeys[(i-1)*4 + j];
        }
        
        if (i % 4 == 0) {
            // RotWord
            uint8_t t = temp[0];
            temp[0] = temp[1];
            temp[1] = temp[2];
            temp[2] = temp[3];
            temp[3] = t;
            
            // SubWord
            for (int j = 0; j < 4; j++) {
                temp[j] = sbox[temp[j]];
            }
            
            // XOR with Rcon
            temp[0] ^= rcon[i/4 - 1];
        }
        
        for (int j = 0; j < 4; j++) {
            roundKeys[i*4 + j] = roundKeys[(i-4)*4 + j] ^ temp[j];
        }
    }
}

inline void aesEncryptBlock(const uint8_t* input, uint8_t* output, const uint8_t* roundKeys) {
    uint8_t state[16];
    
    // Copy input to state
    for (int i = 0; i < 16; i++) {
        state[i] = input[i];
    }
    
    // Initial round
    addRoundKey(state, roundKeys);
    
    // Main rounds
    for (int round = 1; round < 10; round++) {
        subBytes(state);
        shiftRows(state);
        mixColumns(state);
        addRoundKey(state, roundKeys + round * 16);
    }
    
    // Final round
    subBytes(state);
    shiftRows(state);
    addRoundKey(state, roundKeys + 10 * 16);
    
    // Copy state to output
    for (int i = 0; i < 16; i++) {
        output[i] = state[i];
    }
}

// CTR mode implementation
inline void incrementCounter(uint8_t* counter) {
    for (int i = 15; i >= 0; i--) {
        counter[i]++;
        if (counter[i] != 0) break;
    }
}

inline void aesCtrCrypt(const uint8_t* input, uint8_t* output, size_t length, 
                       const uint8_t* key, const uint8_t* nonce) {
    uint8_t roundKeys[176]; // 11 rounds * 16 bytes
    keyExpansion(key, roundKeys);
    
    uint8_t counter[16];
    uint8_t keystream[16];
    
    // Initialize counter with nonce
    for (int i = 0; i < 16; i++) {
        counter[i] = nonce[i];
    }
    
    size_t processed = 0;
    while (processed < length) {
        // Generate keystream block
        aesEncryptBlock(counter, keystream, roundKeys);
        
        // XOR with input
        size_t blockSize = (length - processed < 16) ? length - processed : 16;
        for (size_t i = 0; i < blockSize; i++) {
            output[processed + i] = input[processed + i] ^ keystream[i];
        }
        
        processed += blockSize;
        incrementCounter(counter);
    }
}



inline void generateNonce(uint8_t* nonce) {
    for (int i = 0; i < 16; i++) {
        nonce[i] = simpleRand() & 0xFF;
    }
}

inline void generateChaCha20Nonce(uint8_t* nonce) {
    for (int i = 0; i < 12; i++) {
        nonce[i] = simpleRand() & 0xFF;
    }
}

// ChaCha20 implementation
inline void quarterRound(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d) {
    a += b; d ^= a; d = (d << 16) | (d >> 16);
    c += d; b ^= c; b = (b << 12) | (b >> 20);
    a += b; d ^= a; d = (d << 8) | (d >> 24);
    c += d; b ^= c; b = (b << 7) | (b >> 25);
}

void chachaBlock(uint32_t output[16], const uint32_t input[16]) {
    uint32_t x[16];
    for (int i = 0; i < 16; i++) {
        x[i] = input[i];
    }
    
    for (int i = 0; i < 10; i++) {
        quarterRound(x[0], x[4], x[8], x[12]);
        quarterRound(x[1], x[5], x[9], x[13]);
        quarterRound(x[2], x[6], x[10], x[14]);
        quarterRound(x[3], x[7], x[11], x[15]);
        
        quarterRound(x[0], x[5], x[10], x[15]);
        quarterRound(x[1], x[6], x[11], x[12]);
        quarterRound(x[2], x[7], x[8], x[13]);
        quarterRound(x[3], x[4], x[9], x[14]);
    }
    
    for (int i = 0; i < 16; i++) {
        output[i] = x[i] + input[i];
    }
}

void initChachaState(uint32_t state[16], const uint8_t key[32], const uint8_t nonce[12], uint32_t counter) {
    state[0] = 0x61707865;
    state[1] = 0x3320646e;
    state[2] = 0x79622d32;
    state[3] = 0x6b206574;
    
    for (int i = 0; i < 8; i++) {
        state[4 + i] = ((uint32_t)key[4*i]) |
                       ((uint32_t)key[4*i + 1] << 8) |
                       ((uint32_t)key[4*i + 2] << 16) |
                       ((uint32_t)key[4*i + 3] << 24);
    }
    
    state[12] = counter;
    
    for (int i = 0; i < 3; i++) {
        state[13 + i] = ((uint32_t)nonce[4*i]) |
                        ((uint32_t)nonce[4*i + 1] << 8) |
                        ((uint32_t)nonce[4*i + 2] << 16) |
                        ((uint32_t)nonce[4*i + 3] << 24);
    }
}

void chacha20Crypt(const uint8_t* input, uint8_t* output, size_t length,
                   const uint8_t key[32], const uint8_t nonce[12], uint32_t counter) {
    uint32_t state[16];
    uint32_t keystream[16];
    uint8_t* keystream_bytes = (uint8_t*)keystream;
    
    size_t processed = 0;
    while (processed < length) {
        initChachaState(state, key, nonce, counter);
        chachaBlock(keystream, state);
        
        size_t blockSize = (length - processed < 64) ? length - processed : 64;
        for (size_t i = 0; i < blockSize; i++) {
            output[processed + i] = input[processed + i] ^ keystream_bytes[i];
        }
        
        processed += blockSize;
        counter++;
    }
}

inline void generateChaCha20Key(uint8_t* key) {
    for (int i = 0; i < 32; i++) {
        key[i] = simpleRand() & 0xFF;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3 || argc > 4) {
        std::cout << "=== Universal Native Encryptor ===" << std::endl;
        std::cout << "Usage: " << argv[0] << " <inputfile> <outputfile> [algorithm]" << std::endl;
        std::cout << "Algorithm: aes (default) or chacha20" << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  " << argv[0] << " file.exe encrypted_file.bin" << std::endl;
        std::cout << "  " << argv[0] << " file.exe encrypted_file.bin aes" << std::endl;
        std::cout << "  " << argv[0] << " file.exe encrypted_file.bin chacha20" << std::endl;
        std::cout << "Encrypts any file with random AES-128-CTR or ChaCha20 keys." << std::endl;
        return 1;
    }

    std::string algorithm = (argc == 4) ? argv[3] : "aes";
    bool useChaCha = (algorithm == "chacha20");

    std::ifstream fin(argv[1], std::ios::binary);
    if (!fin) {
        std::cerr << "Failed to open input file.\n";
        return 1;
    }

    std::ofstream fout(argv[2], std::ios::binary);
    if (!fout) {
        std::cerr << "Failed to open output file.\n";
        return 1;
    }

    if (useChaCha) {
        // ChaCha20 encryption
        uint8_t chachaKey[32];
        uint8_t chachaNonce[12];
        
        generateChaCha20Key(chachaKey);
        generateChaCha20Nonce(chachaNonce);
        
        std::cout << "Generated random ChaCha20 key and nonce for encryption." << std::endl;
        
        // Write algorithm identifier (1 byte: 0x02 for ChaCha20)
        uint8_t algId = 0x02;
        fout.write(reinterpret_cast<char*>(&algId), 1);
        
        // Write nonce to output file
        fout.write(reinterpret_cast<char*>(chachaNonce), 12);

        // Process file in chunks
        std::vector<uint8_t> buffer(4096);
        uint32_t counter = 0;
        
        while (fin.read(reinterpret_cast<char*>(buffer.data()), buffer.size()) || fin.gcount() > 0) {
            std::streamsize bytesRead = fin.gcount();
            std::vector<uint8_t> outputBuffer(bytesRead);
            
            chacha20Crypt(buffer.data(), outputBuffer.data(), bytesRead, chachaKey, chachaNonce, counter);
            fout.write(reinterpret_cast<char*>(outputBuffer.data()), bytesRead);
            
            counter += (bytesRead + 63) / 64;
        }

        std::cout << "File encrypted successfully with ChaCha20." << std::endl;
    } else {
        // AES encryption
        uint8_t aesKey[16];
        uint8_t nonce[16];
        
        generateRandomKey(aesKey);
        generateNonce(nonce);
        
        std::cout << "Generated random key and nonce for AES encryption." << std::endl;
        
        // Write algorithm identifier (1 byte: 0x01 for AES)
        uint8_t algId = 0x01;
        fout.write(reinterpret_cast<char*>(&algId), 1);
        
        // Write nonce to output file
        fout.write(reinterpret_cast<char*>(nonce), 16);

        // Process file in chunks
        std::vector<uint8_t> buffer(4096);
        while (fin.read(reinterpret_cast<char*>(buffer.data()), buffer.size()) || fin.gcount() > 0) {
            std::streamsize bytesRead = fin.gcount();
            std::vector<uint8_t> outputBuffer(bytesRead);
            
            aesCtrCrypt(buffer.data(), outputBuffer.data(), bytesRead, aesKey, nonce);
            fout.write(reinterpret_cast<char*>(outputBuffer.data()), bytesRead);
        }

        std::cout << "File encrypted successfully with native AES-128-CTR." << std::endl;
    }
    
    return 0;
}