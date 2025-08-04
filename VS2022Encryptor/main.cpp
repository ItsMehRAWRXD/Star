#include "encryptor.h"
#include "stealth_triple_encryptor.h"
#include "encryptor_stub_generator.h"
#include "xll_stub_generator.h"
#include "unlimited_stub_generator.h"
#include "pe_encryptor.h"

// AES S-box
const uint8_t sbox[256] = {
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
const uint8_t inv_sbox[256] = {
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
const uint8_t rcon[10] = {
    0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36
};

// Fallback key
uint8_t encKey[] = { 0x39,0x39,0x08,0x0F,0x0F,0x38,0x08,0x31,0x38,0x32,0x38 };

// Simple random number generator (for key and nonce generation)
uint32_t simpleRand() {
    static uint32_t seed = std::time(nullptr) + std::clock();
    seed = seed * 1103515245 + 12345;
    return seed;
}

// Random key generation
void generateRandomKey(uint8_t* key) {
    for (int i = 0; i < 16; i++) {
        key[i] = simpleRand() & 0xFF;
    }
}

void decryptKey(uint8_t* keyBuf) {
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
uint8_t gmul(uint8_t a, uint8_t b) {
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

void subBytes(uint8_t* state) {
    for (int i = 0; i < 16; i++) {
        state[i] = sbox[state[i]];
    }
}

void invSubBytes(uint8_t* state) {
    for (int i = 0; i < 16; i++) {
        state[i] = inv_sbox[state[i]];
    }
}

void shiftRows(uint8_t* state) {
    uint8_t temp;
    
    // Row 1: shift left by 1
    temp = state[1];
    state[1] = state[5];
    state[5] = state[9];
    state[9] = state[13];
    state[13] = temp;
    
    // Row 2: shift left by 2
    temp = state[2];
    state[2] = state[10];
    state[10] = temp;
    temp = state[6];
    state[6] = state[14];
    state[14] = temp;
    
    // Row 3: shift left by 3
    temp = state[3];
    state[3] = state[15];
    state[15] = state[11];
    state[11] = state[7];
    state[7] = temp;
}

void invShiftRows(uint8_t* state) {
    uint8_t temp;
    
    // Row 1: shift right by 1
    temp = state[13];
    state[13] = state[9];
    state[9] = state[5];
    state[5] = state[1];
    state[1] = temp;
    
    // Row 2: shift right by 2
    temp = state[2];
    state[2] = state[10];
    state[10] = temp;
    temp = state[6];
    state[6] = state[14];
    state[14] = temp;
    
    // Row 3: shift right by 3
    temp = state[3];
    state[3] = state[7];
    state[7] = state[11];
    state[11] = state[15];
    state[15] = temp;
}

void mixColumns(uint8_t* state) {
    for (int i = 0; i < 4; i++) {
        uint8_t s0 = state[i * 4];
        uint8_t s1 = state[i * 4 + 1];
        uint8_t s2 = state[i * 4 + 2];
        uint8_t s3 = state[i * 4 + 3];
        
        state[i * 4] = gmul(0x02, s0) ^ gmul(0x03, s1) ^ s2 ^ s3;
        state[i * 4 + 1] = s0 ^ gmul(0x02, s1) ^ gmul(0x03, s2) ^ s3;
        state[i * 4 + 2] = s0 ^ s1 ^ gmul(0x02, s2) ^ gmul(0x03, s3);
        state[i * 4 + 3] = gmul(0x03, s0) ^ s1 ^ s2 ^ gmul(0x02, s3);
    }
}

void invMixColumns(uint8_t* state) {
    for (int i = 0; i < 4; i++) {
        uint8_t s0 = state[i * 4];
        uint8_t s1 = state[i * 4 + 1];
        uint8_t s2 = state[i * 4 + 2];
        uint8_t s3 = state[i * 4 + 3];
        
        state[i * 4] = gmul(0x0e, s0) ^ gmul(0x0b, s1) ^ gmul(0x0d, s2) ^ gmul(0x09, s3);
        state[i * 4 + 1] = gmul(0x09, s0) ^ gmul(0x0e, s1) ^ gmul(0x0b, s2) ^ gmul(0x0d, s3);
        state[i * 4 + 2] = gmul(0x0d, s0) ^ gmul(0x09, s1) ^ gmul(0x0e, s2) ^ gmul(0x0b, s3);
        state[i * 4 + 3] = gmul(0x0b, s0) ^ gmul(0x0d, s1) ^ gmul(0x09, s2) ^ gmul(0x0e, s3);
    }
}

void addRoundKey(uint8_t* state, const uint8_t* roundKey) {
    for (int i = 0; i < 16; i++) {
        state[i] ^= roundKey[i];
    }
}

void keyExpansion(const uint8_t* key, uint8_t* roundKeys) {
    uint8_t temp[4];
    
    // Copy the original key
    for (int i = 0; i < 16; i++) {
        roundKeys[i] = key[i];
    }
    
    for (int i = 4; i < 44; i++) {
        for (int j = 0; j < 4; j++) {
            temp[j] = roundKeys[(i - 1) * 4 + j];
        }
        
        if (i % 4 == 0) {
            // RotWord
            uint8_t t = temp[0];
            temp[0] = temp[1];
            temp[1] = temp[2];
            temp[2] = temp[3];
            temp[3] = t;
            
            // SubWord
            temp[0] = sbox[temp[0]];
            temp[1] = sbox[temp[1]];
            temp[2] = sbox[temp[2]];
            temp[3] = sbox[temp[3]];
            
            // XOR with Rcon
            temp[0] ^= rcon[i / 4 - 1];
        }
        
        for (int j = 0; j < 4; j++) {
            roundKeys[i * 4 + j] = roundKeys[(i - 4) * 4 + j] ^ temp[j];
        }
    }
}

void aesEncryptBlock(const uint8_t* input, uint8_t* output, const uint8_t* roundKeys) {
    uint8_t state[16];
    
    // Copy input to state
    for (int i = 0; i < 16; i++) {
        state[i] = input[i];
    }
    
    // Initial round key addition
    addRoundKey(state, roundKeys);
    
    // 9 rounds
    for (int round = 1; round < 10; round++) {
        subBytes(state);
        shiftRows(state);
        mixColumns(state);
        addRoundKey(state, roundKeys + round * 16);
    }
    
    // Final round
    subBytes(state);
    shiftRows(state);
    addRoundKey(state, roundKeys + 160);
    
    // Copy state to output
    for (int i = 0; i < 16; i++) {
        output[i] = state[i];
    }
}

void incrementCounter(uint8_t* counter) {
    for (int i = 15; i >= 0; i--) {
        counter[i]++;
        if (counter[i] != 0) break;
    }
}

void aesCtrCrypt(const uint8_t* input, uint8_t* output, size_t length, 
                 const uint8_t* key, const uint8_t* nonce) {
    uint8_t roundKeys[176];
    keyExpansion(key, roundKeys);
    
    uint8_t counter[16];
    memcpy(counter, nonce, 16);
    
    uint8_t keystream[16];
    
    for (size_t i = 0; i < length; i += 16) {
        aesEncryptBlock(counter, keystream, roundKeys);
        
        size_t blockSize = (length - i < 16) ? length - i : 16;
        for (size_t j = 0; j < blockSize; j++) {
            output[i + j] = input[i + j] ^ keystream[j];
        }
        
        incrementCounter(counter);
    }
}

void generateNonce(uint8_t* nonce) {
    for (int i = 0; i < 16; i++) {
        nonce[i] = simpleRand() & 0xFF;
    }
}

void generateChaCha20Nonce(uint8_t* nonce) {
    for (int i = 0; i < 12; i++) {
        nonce[i] = simpleRand() & 0xFF;
    }
}

void quarterRound(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d) {
    a += b; d ^= a; d = (d << 16) | (d >> 16);
    c += d; b ^= c; b = (b << 12) | (b >> 20);
    a += b; d ^= a; d = (d << 8) | (d >> 24);
    c += d; b ^= c; b = (b << 7) | (b >> 25);
}

void chachaBlock(uint32_t output[16], const uint32_t input[16]) {
    uint32_t state[16];
    memcpy(state, input, 64);
    
    for (int i = 0; i < 10; i++) {
        quarterRound(state[0], state[4], state[8], state[12]);
        quarterRound(state[1], state[5], state[9], state[13]);
        quarterRound(state[2], state[6], state[10], state[14]);
        quarterRound(state[3], state[7], state[11], state[15]);
        quarterRound(state[0], state[5], state[10], state[15]);
        quarterRound(state[1], state[6], state[11], state[12]);
        quarterRound(state[2], state[7], state[8], state[13]);
        quarterRound(state[3], state[4], state[9], state[14]);
    }
    
    for (int i = 0; i < 16; i++) {
        output[i] = state[i] + input[i];
    }
}

void initChachaState(uint32_t state[16], const uint8_t key[32], const uint8_t nonce[12], uint32_t counter) {
    const uint32_t constants[4] = { 0x61707865, 0x3320646e, 0x79622d32, 0x6b206574 };
    
    state[0] = constants[0];
    state[1] = constants[1];
    state[2] = constants[2];
    state[3] = constants[3];
    
    for (int i = 0; i < 8; i++) {
        state[4 + i] = ((uint32_t)key[i * 4]) | 
                      ((uint32_t)key[i * 4 + 1] << 8) | 
                      ((uint32_t)key[i * 4 + 2] << 16) | 
                      ((uint32_t)key[i * 4 + 3] << 24);
    }
    
    state[12] = counter;
    
    for (int i = 0; i < 3; i++) {
        state[13 + i] = ((uint32_t)nonce[i * 4]) | 
                       ((uint32_t)nonce[i * 4 + 1] << 8) | 
                       ((uint32_t)nonce[i * 4 + 2] << 16) | 
                       ((uint32_t)nonce[i * 4 + 3] << 24);
    }
}

void chacha20Crypt(const uint8_t* input, uint8_t* output, size_t length,
                   const uint8_t key[32], const uint8_t nonce[12], uint32_t counter) {
    uint32_t state[16];
    uint32_t block[16];
    uint8_t keystream[64];
    
    for (size_t i = 0; i < length; i += 64) {
        initChachaState(state, key, nonce, counter);
        chachaBlock(block, state);
        
        for (int j = 0; j < 16; j++) {
            keystream[j * 4] = block[j] & 0xFF;
            keystream[j * 4 + 1] = (block[j] >> 8) & 0xFF;
            keystream[j * 4 + 2] = (block[j] >> 16) & 0xFF;
            keystream[j * 4 + 3] = (block[j] >> 24) & 0xFF;
        }
        
        size_t blockSize = (length - i < 64) ? length - i : 64;
        for (size_t j = 0; j < blockSize; j++) {
            output[i + j] = input[i + j] ^ keystream[j];
        }
        
        counter++;
    }
}

void generateChaCha20Key(uint8_t* key) {
    for (int i = 0; i < 32; i++) {
        key[i] = simpleRand() & 0xFF;
    }
}

// Basic encryption function
bool basicEncrypt(const std::string& inputFile, const std::string& outputFile, const std::string& algorithm, bool rawOutput = false) {
    bool useChaCha = (algorithm == "chacha20");

    std::ifstream fin(inputFile, std::ios::binary);
    if (!fin) {
        std::cerr << "Failed to open input file: " << inputFile << std::endl;
        return false;
    }

    std::ofstream fout(outputFile, std::ios::binary);
    if (!fout) {
        std::cerr << "Failed to open output file: " << outputFile << std::endl;
        return false;
    }

    if (useChaCha) {
        // ChaCha20 encryption
        uint8_t chachaKey[32];
        uint8_t chachaNonce[12];
        
        generateChaCha20Key(chachaKey);
        generateChaCha20Nonce(chachaNonce);
        
        std::cout << "Generated random ChaCha20 key and nonce for encryption." << std::endl;
        
        if (!rawOutput) {
            // Write algorithm identifier (1 byte: 0x02 for ChaCha20)
            uint8_t algId = 0x02;
            fout.write(reinterpret_cast<char*>(&algId), 1);
            
            // Write nonce to output file
            fout.write(reinterpret_cast<char*>(chachaNonce), 12);
        }

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
        
        if (!rawOutput) {
            // Write algorithm identifier (1 byte: 0x01 for AES)
            uint8_t algId = 0x01;
            fout.write(reinterpret_cast<char*>(&algId), 1);
            
            // Write nonce to output file
            fout.write(reinterpret_cast<char*>(nonce), 16);
        }

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
    
    std::cout << "Output saved to: " << outputFile << std::endl;
    return true;
}

void showMenu() {
    std::cout << "\n=== Visual Studio 2022 Universal Encryptor ===" << std::endl;
    std::cout << "Advanced encryption tool with multiple algorithms and stealth features\n" << std::endl;
    std::cout << "Select an option:" << std::endl;
    std::cout << "  1. Basic Encryption (AES/ChaCha20)" << std::endl;
    std::cout << "  2. Basic Encryption - Raw Binary Output" << std::endl;
    std::cout << "  3. Stealth Triple Encryption" << std::endl;
    std::cout << "  4. Generate Stealth Payload Stub" << std::endl;
    std::cout << "  5. Generate Encryptor Stub" << std::endl;
    std::cout << "  6. Generate XLL Stealth Payload Stub" << std::endl;
    std::cout << "  7. Generate Unlimited Runtime Encryptor Stub" << std::endl;
    std::cout << "  8. PE-Aware Encryption (Preserves Executable Structure)" << std::endl;
    std::cout << "  0. Exit" << std::endl;
    std::cout << "\nEnter your choice: ";
}

std::string getInputFile() {
    std::string filename;
    std::cout << "Enter input file path: ";
    std::getline(std::cin, filename);
    return filename;
}

std::string getOutputFile() {
    std::string filename;
    std::cout << "Enter output file path (e.g., encrypted_file.bin or encrypted_file.exe): ";
    std::getline(std::cin, filename);
    return filename;
}

std::string selectAlgorithm() {
    std::cout << "Select algorithm:" << std::endl;
    std::cout << "  1. AES-128-CTR (default)" << std::endl;
    std::cout << "  2. ChaCha20" << std::endl;
    std::cout << "Enter choice (1-2): ";
    
    std::string choice;
    std::getline(std::cin, choice);
    
    if (choice == "2") return "chacha20";
    return "aes";
}

int main() {
    while (true) {
        showMenu();
        
        std::string choice;
        std::getline(std::cin, choice);
        
        if (choice == "0") {
            std::cout << "Goodbye!" << std::endl;
            break;
        }
        else if (choice == "1") {
            // Basic Encryption
            std::string inputFile = getInputFile();
            std::string outputFile = getOutputFile();
            std::string algorithm = selectAlgorithm();
            
            if (!basicEncrypt(inputFile, outputFile, algorithm, false)) {
                std::cout << "Encryption failed!" << std::endl;
            } else {
                std::cout << "Encryption completed successfully!" << std::endl;
            }
        }
        else if (choice == "2") {
            // Basic Encryption - Raw Binary Output
            std::string inputFile = getInputFile();
            std::string outputFile = getOutputFile();
            std::string algorithm = selectAlgorithm();
            
            if (!basicEncrypt(inputFile, outputFile, algorithm, true)) {
                std::cout << "Encryption failed!" << std::endl;
            } else {
                std::cout << "Raw binary encryption completed successfully!" << std::endl;
            }
        }
        else if (choice == "3") {
            // Stealth Triple Encryption
            std::string inputFile = getInputFile();
            std::string outputFile = getOutputFile();
            
            StealthTripleEncryption ste;
            if (ste.encryptFile(inputFile, outputFile)) {
                std::cout << "File encrypted with triple layer encryption" << std::endl;
                std::cout << "Keys saved as decimal numbers in: " << outputFile << ".keys" << std::endl;
            } else {
                std::cout << "Stealth encryption failed!" << std::endl;
            }
        }
        else if (choice == "4") {
            // Generate Stealth Payload Stub
            std::string inputFile = getInputFile();
            std::string outputFile = getOutputFile();
            
            std::ifstream in(inputFile, std::ios::binary);
            if (!in) {
                std::cout << "Failed to open payload file: " << inputFile << std::endl;
                continue;
            }
            
            in.seekg(0, std::ios::end);
            size_t size = in.tellg();
            in.seekg(0, std::ios::beg);
            
            std::vector<uint8_t> payload(size);
            in.read(reinterpret_cast<char*>(payload.data()), size);
            in.close();
            
            StealthTripleEncryption ste;
            auto keys = ste.generateKeys();
            std::string stub = ste.generateStealthStub(payload, keys);
            
            std::ofstream out(outputFile);
            out << stub;
            out.close();
            
            std::cout << "Stealth stub generated: " << outputFile << std::endl;
            std::cout << "Keys stored as decimal numbers in code" << std::endl;
        }
        else if (choice == "5") {
            // Generate Encryptor Stub
            std::string outputFile = getOutputFile();
            
            EncryptorStubGenerator generator;
            auto keys = generator.generateKeys();
            std::string stub = generator.generateEncryptorStub(keys);
            
            std::ofstream out(outputFile);
            out << stub;
            out.close();
            
            std::cout << "Encryptor stub generated: " << outputFile << std::endl;
            std::cout << "This stub can encrypt files at runtime with triple-layer encryption" << std::endl;
            std::cout << "Usage: compiled_stub.exe <inputfile> <outputfile>" << std::endl;
        }
        else if (choice == "6") {
            // Generate XLL Stealth Payload Stub
            std::string inputFile = getInputFile();
            std::string outputFile = getOutputFile();
            
            std::ifstream in(inputFile, std::ios::binary);
            if (!in) {
                std::cout << "Failed to open payload file: " << inputFile << std::endl;
                continue;
            }
            
            in.seekg(0, std::ios::end);
            size_t size = in.tellg();
            in.seekg(0, std::ios::beg);
            
            std::vector<uint8_t> payload(size);
            in.read(reinterpret_cast<char*>(payload.data()), size);
            in.close();
            
            XLLStubGenerator generator;
            auto keys = generator.generateKeys();
            std::string stub = generator.generateXLLStub(payload, keys);
            
            std::ofstream out(outputFile);
            out << stub;
            out.close();
            
            std::cout << "XLL stealth stub generated: " << outputFile << std::endl;
            std::cout << "This stub can be compiled as an XLL add-in for Excel" << std::endl;
            std::cout << "Keys stored as decimal numbers in code" << std::endl;
        }
        else if (choice == "7") {
            // Generate Unlimited Runtime Encryptor Stub
            std::string outputFile = getOutputFile();
            
            UnlimitedStubGenerator generator;
            std::string stub = generator.generateStub();
            
            std::ofstream out(outputFile);
            out << stub;
            out.close();
            
            std::cout << "Unlimited runtime encryptor stub generated: " << outputFile << std::endl;
            std::cout << "Features:" << std::endl;
            std::cout << "  - 4-layer encryption (AES + ChaCha20 + XOR + Additional)" << std::endl;
            std::cout << "  - Extended 32-byte keys" << std::endl;
            std::cout << "  - Variable XOR key length (32-64 bytes)" << std::endl;
            std::cout << "  - Randomized encryption order" << std::endl;
            std::cout << "  - Decimal key representation" << std::endl;
            std::cout << "  - Unlimited file size support" << std::endl;
            std::cout << "Usage: g++ -o encryptor.exe " << outputFile << std::endl;
            std::cout << "Then: encryptor.exe input_file output_file" << std::endl;
            std::cout << "Examples:" << std::endl;
            std::cout << "  encryptor.exe calc.exe encrypted_calc.bin" << std::endl;
            std::cout << "  encryptor.exe notepad.exe encrypted_notepad.exe" << std::endl;
        }
        else if (choice == "8") {
            // PE-Aware Encryption
            std::string inputFile = getInputFile();
            std::string outputFile = getOutputFile();
            
            PEEncryptor peEncryptor;
            if (peEncryptor.encryptPE(inputFile, outputFile)) {
                std::cout << "PE file encrypted successfully!" << std::endl;
                std::cout << "File remains executable but code sections are encrypted" << std::endl;
                std::cout << "Keys saved separately in: " << outputFile << ".keys" << std::endl;
            } else {
                std::cout << "PE encryption failed!" << std::endl;
            }
        }
        else {
            std::cout << "Invalid choice. Please try again." << std::endl;
        }
        
        std::cout << "\nPress Enter to continue...";
        std::cin.get();
    }
    
    return 0;
}