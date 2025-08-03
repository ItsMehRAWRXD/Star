#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <cstdlib>
#include <ctime>

class NativeStubGenerator {
private:
    std::mt19937 rng;
    
    // AES-128-CTR implementation (same as native_encryptor)
    static const uint8_t sbox[256];
    static const uint8_t inv_sbox[256];
    static const uint8_t rcon[11];
    
    void subBytes(uint8_t state[16]) {
        for (int i = 0; i < 16; i++) {
            state[i] = sbox[state[i]];
        }
    }
    
    void invSubBytes(uint8_t state[16]) {
        for (int i = 0; i < 16; i++) {
            state[i] = inv_sbox[state[i]];
        }
    }
    
    void shiftRows(uint8_t state[16]) {
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
    
    void invShiftRows(uint8_t state[16]) {
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
    
    uint8_t gmul(uint8_t a, uint8_t b) {
        uint8_t p = 0;
        uint8_t hi_bit_set;
        for (int i = 0; i < 8; i++) {
            if ((b & 1) != 0) {
                p ^= a;
            }
            hi_bit_set = (uint8_t)(a & 0x80);
            a <<= 1;
            if (hi_bit_set != 0) {
                a ^= 0x1b;
            }
            b >>= 1;
        }
        return p;
    }
    
    void mixColumns(uint8_t state[16]) {
        uint8_t temp[16];
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                temp[i * 4 + j] = gmul(0x02, state[i * 4 + j]) ^
                                 gmul(0x03, state[i * 4 + (j + 1) % 4]) ^
                                 state[i * 4 + (j + 2) % 4] ^
                                 state[i * 4 + (j + 3) % 4];
            }
        }
        memcpy(state, temp, 16);
    }
    
    void invMixColumns(uint8_t state[16]) {
        uint8_t temp[16];
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                temp[i * 4 + j] = gmul(0x0e, state[i * 4 + j]) ^
                                 gmul(0x0b, state[i * 4 + (j + 1) % 4]) ^
                                 gmul(0x0d, state[i * 4 + (j + 2) % 4]) ^
                                 gmul(0x09, state[i * 4 + (j + 3) % 4]);
            }
        }
        memcpy(state, temp, 16);
    }
    
    void addRoundKey(uint8_t state[16], const uint8_t roundKey[16]) {
        for (int i = 0; i < 16; i++) {
            state[i] ^= roundKey[i];
        }
    }
    
    void keyExpansion(const uint8_t key[16], uint8_t roundKeys[11][16]) {
        memcpy(roundKeys[0], key, 16);
        
        for (int round = 1; round <= 10; round++) {
            uint8_t temp[4];
            memcpy(temp, roundKeys[round-1] + 12, 4);
            
            // RotWord
            uint8_t t = temp[0];
            temp[0] = temp[1];
            temp[1] = temp[2];
            temp[2] = temp[3];
            temp[3] = t;
            
            // SubWord
            for (int i = 0; i < 4; i++) {
                temp[i] = sbox[temp[i]];
            }
            
            // XOR with Rcon
            temp[0] ^= rcon[round];
            
            // Generate new round key
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    roundKeys[round][i * 4 + j] = roundKeys[round-1][i * 4 + j] ^ temp[j];
                }
                if (i < 3) {
                    memcpy(temp, roundKeys[round] + i * 4, 4);
                }
            }
        }
    }
    
    void aesCtrCrypt(uint8_t* data, size_t dataLen, const uint8_t key[16], const uint8_t nonce[16]) {
        uint8_t roundKeys[11][16];
        keyExpansion(key, roundKeys);
        
        uint8_t counter[16];
        memcpy(counter, nonce, 16);
        
        for (size_t i = 0; i < dataLen; i += 16) {
            uint8_t keystream[16];
            memcpy(keystream, counter, 16);
            
            // AES encryption of counter
            addRoundKey(keystream, roundKeys[0]);
            for (int round = 1; round < 10; round++) {
                subBytes(keystream);
                shiftRows(keystream);
                mixColumns(keystream);
                addRoundKey(keystream, roundKeys[round]);
            }
            subBytes(keystream);
            shiftRows(keystream);
            addRoundKey(keystream, roundKeys[10]);
            
            // XOR with data
            size_t blockSize = std::min(16UL, dataLen - i);
            for (size_t j = 0; j < blockSize; j++) {
                data[i + j] ^= keystream[j];
            }
            
            // Increment counter
            for (int j = 15; j >= 0; j--) {
                if (++counter[j] != 0) break;
            }
        }
    }
    
    std::string generateRandomString(size_t length) {
        const std::string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";
        std::string result;
        result.reserve(length);
        
        // Ensure first character is a letter or underscore
        result += charset[rng() % (charset.length() - 10)]; // Exclude numbers for first char
        
        // Generate remaining characters
        const std::string fullCharset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";
        for (size_t i = 1; i < length; ++i) {
            result += fullCharset[rng() % fullCharset.length()];
        }
        return result;
    }
    
    std::string generateRandomHex(size_t length) {
        std::string result;
        result.reserve(length);
        
        for (size_t i = 0; i < length; ++i) {
            result += "0123456789abcdef"[rng() % 16];
        }
        return result;
    }
    
    std::string generateRandomKey() {
        return generateRandomHex(32); // 16 bytes = 32 hex chars
    }
    
    std::string generateRandomNonce() {
        return generateRandomHex(32); // 16 bytes = 32 hex chars
    }
    
    std::string obfuscateString(const std::string& input) {
        std::string result = "std::string ";
        std::string varName = generateRandomString(8);
        result += varName + " = \"";
        
        for (char c : input) {
            result += "\\x" + std::string(1, "0123456789abcdef"[c >> 4]) + 
                     std::string(1, "0123456789abcdef"[c & 0xF]);
        }
        result += "\";";
        return result;
    }
    
    std::string obfuscateStringWithVar(const std::string& input, std::string& varName) {
        varName = generateRandomString(8);
        std::string result = "std::string " + varName + " = \"";
        
        for (char c : input) {
            result += "\\x" + std::string(1, "0123456789abcdef"[c >> 4]) + 
                     std::string(1, "0123456789abcdef"[c & 0xF]);
        }
        result += "\";";
        return result;
    }
    
    // Simple random number generator (same as encryptor)
    inline uint32_t simpleRand() {
        static uint32_t seed = 0x12345678;
        seed = seed * 1103515245 + 12345;
        return seed;
    }
    
    std::string generateStubTemplate(const std::string& stubType) {
        if (stubType == "basic") {
            return R"(
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <random>
#include <ctime>

// Polymorphic key and nonce (no obfuscation)
{KEY_DEFINITION}
{NONCE_DEFINITION}



// AES-128-CTR implementation (same as native_encryptor/dropper)
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

// Convert hex string to bytes
void hexToBytes(const std::string& hex, uint8_t* bytes) {
    for (size_t i = 0; i < hex.length(); i += 2) {
        bytes[i/2] = std::stoi(hex.substr(i, 2), nullptr, 16);
    }
}

int main() {
    // Embedded encrypted data
    uint8_t encryptedData[] = {EMBEDDED_DATA};
    size_t dataSize = sizeof(encryptedData);
    
    // Convert hex strings to bytes
    uint8_t key[16], nonce[16];
    hexToBytes({KEY_VAR}, key);
    hexToBytes({NONCE_VAR}, nonce);
    
    // Decrypt the data using AES-128-CTR
    aesCtrCrypt(encryptedData, encryptedData, dataSize, key, nonce);
    
    // Write decrypted data to file
    std::ofstream outFile("decrypted_output.bin", std::ios::binary);
    if (outFile.is_open()) {
        outFile.write(reinterpret_cast<char*>(encryptedData), dataSize);
        outFile.close();
        std::cout << "Data decrypted and saved to decrypted_output.bin" << std::endl;
    } else {
        std::cerr << "Failed to create output file" << std::endl;
        return 1;
    }
    
    return 0;
}
)";
        } else if (stubType == "advanced") {
            return R"(
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <ctime>

// Advanced obfuscation
{KEY_DEFINITION}
{NONCE_DEFINITION}

// Anti-debugging and obfuscation techniques
bool isDebugged() {
    // Simple timing check
    clock_t start = clock();
    for (volatile int i = 0; i < 1000000; i++) {}
    clock_t end = clock();
    return (end - start) > 100000; // Suspicious if too slow
}

// Polymorphic code mutation engine
class PolymorphicEngine {
private:
    static const uint32_t POLY_KEY = 0xDEADBEEF;
    
public:
    static uint8_t mutateByte(uint8_t input, uint32_t seed) {
        // Deterministic polymorphic transformation
        uint32_t mutation = seed ^ POLY_KEY;
        return input ^ (mutation & 0xFF);
    }
    
    static void mutateArray(uint8_t* data, size_t size, uint32_t seed) {
        for (size_t i = 0; i < size; i++) {
            data[i] = mutateByte(data[i], seed + i);
        }
    }
    
    static void demutateArray(uint8_t* data, size_t size, uint32_t seed) {
        // Reverse the mutation (same operation)
        for (size_t i = 0; i < size; i++) {
            data[i] = mutateByte(data[i], seed + i);
        }
    }
};

// AES-128-CTR implementation (same as native_encryptor/dropper)
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

static const uint8_t rcon[10] = {
    0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36
};

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
    temp = state[1]; state[1] = state[5]; state[5] = state[9]; state[9] = state[13]; state[13] = temp;
    temp = state[2]; state[2] = state[10]; state[10] = temp;
    temp = state[6]; state[6] = state[14]; state[14] = temp;
    temp = state[3]; state[3] = state[15]; state[15] = state[11]; state[11] = state[7]; state[7] = temp;
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
    for (int i = 0; i < 16; i++) state[i] = input[i];
    addRoundKey(state, roundKeys);
    for (int round = 1; round < 10; round++) {
        subBytes(state); shiftRows(state); mixColumns(state); addRoundKey(state, roundKeys + round * 16);
    }
    subBytes(state); shiftRows(state); addRoundKey(state, roundKeys + 10 * 16);
    for (int i = 0; i < 16; i++) output[i] = state[i];
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
    uint8_t keystream[16];
    for (int i = 0; i < 16; i++) counter[i] = nonce[i];
    size_t processed = 0;
    while (processed < length) {
        aesEncryptBlock(counter, keystream, roundKeys);
        size_t blockSize = (length - processed < 16) ? length - processed : 16;
        for (size_t i = 0; i < blockSize; i++) {
            output[processed + i] = input[processed + i] ^ keystream[i];
        }
        processed += blockSize;
        incrementCounter(counter);
    }
}

void hexToBytes(const std::string& hex, uint8_t* bytes) {
    for (size_t i = 0; i < hex.length(); i += 2) {
        bytes[i/2] = std::stoi(hex.substr(i, 2), nullptr, 16);
    }
}

int main() {
    if (isDebugged()) {
        std::cerr << "Debugging detected!" << std::endl;
        return 1;
    }
    
    // Embedded encrypted data
    uint8_t encryptedData[] = {EMBEDDED_DATA};
    size_t dataSize = sizeof(encryptedData);
    
    // Key and nonce
    std::string keyHex = {KEY_VAR};
    std::string nonceHex = {NONCE_VAR};
    uint8_t key[16], nonce[16];
    hexToBytes(keyHex, key);
    hexToBytes(nonceHex, nonce);
    
    // Apply polymorphic mutation to key and nonce
    uint32_t polySeed = std::time(nullptr) ^ 0xDEADBEEF;
    PolymorphicEngine::mutateArray(key, 16, polySeed);
    PolymorphicEngine::mutateArray(nonce, 16, polySeed + 1);
    
    // Demutate before use
    PolymorphicEngine::demutateArray(key, 16, polySeed);
    PolymorphicEngine::demutateArray(nonce, 16, polySeed + 1);
    
    // Decrypt
    std::vector<uint8_t> decrypted(dataSize);
    aesCtrCrypt(encryptedData, decrypted.data(), dataSize, key, nonce);
    
    // Write decrypted data to output file
    std::string filename = "output_" + std::to_string(std::time(nullptr)) + ".bin";
    std::ofstream outFile(filename, std::ios::binary);
    outFile.write(reinterpret_cast<char*>(decrypted.data()), dataSize);
    outFile.close();
    std::cout << "Decrypted file written: " << filename << std::endl;
    return 0;
}
)";
        } else if (stubType == "minimal") {
            return R"(
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdint>

// Minimal stub with embedded data
{KEY_DEFINITION}
{NONCE_DEFINITION}

int main() {
    uint8_t data[] = {EMBEDDED_DATA};
    size_t size = sizeof(data);
    
    // Simple XOR decryption
    for (size_t i = 0; i < size; i++) {
        data[i] ^= {KEY_VAR}[i % 16] ^ {NONCE_VAR}[i % 16];
    }
    
    std::ofstream out("output.bin", std::ios::binary);
    out.write(reinterpret_cast<char*>(data), size);
    return 0;
}
)";
        }
        
        return "// Invalid stub type";
    }
    
    std::string embedDataAsArray(const std::vector<uint8_t>& data) {
        std::stringstream ss;
        ss << "{";
        for (size_t i = 0; i < data.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << "0x" << std::hex << std::setw(2) << std::setfill('0') << (int)data[i];
        }
        ss << "}";
        return ss.str();
    }
    


public:
    NativeStubGenerator() : rng(std::time(nullptr)) {}
    
    void generateStub(const std::string& inputFile, const std::string& outputFile, 
                     const std::string& stubType = "basic", bool useRandomKey = true) {
        
        // Read input file
        std::ifstream inFile(inputFile, std::ios::binary);
        if (!inFile.is_open()) {
            std::cerr << "Error: Cannot open input file: " << inputFile << std::endl;
            return;
        }
        
        // Read the file data
        std::vector<uint8_t> fileData((std::istreambuf_iterator<char>(inFile)),
                                    std::istreambuf_iterator<char>());
        inFile.close();
        
        if (fileData.empty()) {
            std::cerr << "Error: Input file is empty" << std::endl;
            return;
        }
        
        // Generate nonce
        uint8_t nonce[16];
        for (int i = 0; i < 16; i++) {
            nonce[i] = simpleRand() & 0xFF;
        }
        
        // Use the same key system as encryptor/dropper
        uint8_t encKey[] = { 0x39,0x39,0x08,0x0F,0x0F,0x38,0x08,0x31,0x38,0x32,0x38 };
        constexpr size_t keyLen = sizeof(encKey);
        
        uint8_t key[keyLen];
        const char* envKey = std::getenv("ENCRYPTION_KEY");
        if (envKey && strlen(envKey) >= keyLen) {
            for (size_t i = 0; i < keyLen; ++i) {
                key[i] = static_cast<uint8_t>(envKey[i]);
            }
        } else {
            for (size_t i = 0; i < keyLen; ++i) {
                key[i] = encKey[i];
            }
        }
        
        // Prepare AES key (expand or truncate to 16 bytes)
        uint8_t aesKey[16];
        for (size_t i = 0; i < 16; ++i) {
            aesKey[i] = key[i % keyLen];
        }
        
        // Encrypt the data
        aesCtrCrypt(fileData.data(), fileData.size(), aesKey, nonce);
        
        // Convert to hex strings for stub
        std::string keyHex, nonceHex;
        for (int i = 0; i < 16; i++) {
            char hex[3];
            sprintf(hex, "%02x", aesKey[i]);
            keyHex += hex;
            sprintf(hex, "%02x", nonce[i]);
            nonceHex += hex;
        }
        

        
        // Generate stub template
        std::string stubTemplate = generateStubTemplate(stubType);
        
        // Replace placeholders
        std::string keyVar, nonceVar;
        
        // Replace placeholders in template
        size_t pos;
        
        // Replace key definition (no obfuscation for basic stub)
        pos = stubTemplate.find("{KEY_DEFINITION}");
        if (pos != std::string::npos) {
            if (stubType == "basic") {
                keyVar = "keyHex";
                std::string keyDef = "const std::string " + keyVar + " = \"" + keyHex + "\";";
                stubTemplate.replace(pos, 16, keyDef);
            } else {
                stubTemplate.replace(pos, 16, obfuscateStringWithVar(keyHex, keyVar));
            }
        }
        
        // Replace nonce definition (no obfuscation for basic stub)
        pos = stubTemplate.find("{NONCE_DEFINITION}");
        if (pos != std::string::npos) {
            if (stubType == "basic") {
                nonceVar = "nonceHex";
                std::string nonceDef = "const std::string " + nonceVar + " = \"" + nonceHex + "\";";
                stubTemplate.replace(pos, 18, nonceDef);
            } else {
                stubTemplate.replace(pos, 18, obfuscateStringWithVar(nonceHex, nonceVar));
            }
        }
        
        // Replace embedded data
        pos = stubTemplate.find("{EMBEDDED_DATA}");
        if (pos != std::string::npos) {
            stubTemplate.replace(pos, 15, embedDataAsArray(fileData));
        }
        
        // Replace key variable
        pos = stubTemplate.find("{KEY_VAR}");
        if (pos != std::string::npos) {
            stubTemplate.replace(pos, 9, keyVar);
        }
        
        // Replace nonce variable
        pos = stubTemplate.find("{NONCE_VAR}");
        if (pos != std::string::npos) {
            stubTemplate.replace(pos, 11, nonceVar);
        }
        
        // Write stub to output file
        std::ofstream outFile(outputFile);
        if (!outFile.is_open()) {
            std::cerr << "Error: Cannot create output file: " << outputFile << std::endl;
            return;
        }
        
        outFile << stubTemplate;
        outFile.close();
        
        std::cout << "✓ Stub generated successfully!" << std::endl;
        std::cout << "  Input file: " << inputFile << std::endl;
        std::cout << "  Output stub: " << outputFile << std::endl;
        std::cout << "  Stub type: " << stubType << std::endl;
        std::cout << "  Original size: " << fileData.size() << " bytes" << std::endl;
        std::cout << "  Encrypted size: " << fileData.size() << " bytes" << std::endl;
        std::cout << "  Key: " << keyHex << std::endl;
        std::cout << "  Nonce: " << nonceHex << std::endl;
    }
    
    void generateBatchStubs(const std::string& inputDir, const std::string& outputDir,
                           const std::string& stubType = "basic", bool useRandomKey = true) {
        // This would implement batch processing of multiple files
        std::cout << "Batch stub generation not yet implemented" << std::endl;
    }
    
    void showHelp() {
        std::cout << "Native Stub Generator - Unlimited stub creation for packing" << std::endl;
        std::cout << "=============================================================" << std::endl;
        std::cout << std::endl;
        std::cout << "Usage:" << std::endl;
        std::cout << "  ./native_stub_generator <input_file> <output_stub.cpp> [stub_type] [use_random_key]" << std::endl;
        std::cout << std::endl;
        std::cout << "Arguments:" << std::endl;
        std::cout << "  input_file      - File to embed in stub" << std::endl;
        std::cout << "  output_stub.cpp - Generated C++ stub file" << std::endl;
        std::cout << "  stub_type       - Type of stub (basic, advanced, minimal) [default: basic]" << std::endl;
        std::cout << "  use_random_key  - Use random key (1) or env var (0) [default: 1]" << std::endl;
        std::cout << std::endl;
        std::cout << "Stub Types:" << std::endl;
        std::cout << "  basic    - Standard stub with AES-128-CTR decryption" << std::endl;
        std::cout << "  advanced - Anti-debugging and polymorphic features" << std::endl;
        std::cout << "  minimal  - Smallest possible stub" << std::endl;
        std::cout << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  ./native_stub_generator payload.bin stub.cpp" << std::endl;
        std::cout << "  ./native_stub_generator payload.bin stub.cpp advanced" << std::endl;
        std::cout << "  ./native_stub_generator payload.bin stub.cpp basic 0" << std::endl;
        std::cout << std::endl;
    }
};

// AES S-box and constants
const uint8_t NativeStubGenerator::sbox[256] = {
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

const uint8_t NativeStubGenerator::inv_sbox[256] = {
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

const uint8_t NativeStubGenerator::rcon[11] = {
    0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36
};

int main(int argc, char* argv[]) {
    NativeStubGenerator generator;
    
    if (argc < 3 || argc > 5) {
        generator.showHelp();
        return 1;
    }
    
    std::string inputFile = argv[1];
    std::string outputFile = argv[2];
    std::string stubType = (argc > 3) ? argv[3] : "basic";
    bool useRandomKey = (argc > 4) ? (std::string(argv[4]) == "1") : true;
    
    generator.generateStub(inputFile, outputFile, stubType, useRandomKey);
    
    return 0;
}