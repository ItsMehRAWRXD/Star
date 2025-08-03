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

// Obfuscated key and nonce
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

// AES decryption functions (simplified for stub)
void aesDecrypt(uint8_t* data, size_t dataLen, const std::string& keyHex, const std::string& nonceHex) {
    // Convert hex strings to bytes
    uint8_t key[16], nonce[16];
    for (int i = 0; i < 16; i++) {
        key[i] = std::stoi(keyHex.substr(i*2, 2), nullptr, 16);
        nonce[i] = std::stoi(nonceHex.substr(i*2, 2), nullptr, 16);
    }
    
    // Simple XOR decryption for stub (full AES would be too large)
    for (size_t i = 0; i < dataLen; i++) {
        data[i] ^= key[i % 16] ^ nonce[i % 16];
    }
}

int main() {
    // Embedded encrypted data
    uint8_t encryptedData[] = {EMBEDDED_DATA};
    size_t dataSize = sizeof(encryptedData);
    
    // Decrypt the data
    aesDecrypt(encryptedData, dataSize, {KEY_VAR}, {NONCE_VAR});
    
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

// Polymorphic decryption
void polymorphicDecrypt(uint8_t* data, size_t dataLen, const std::string& keyHex, const std::string& nonceHex) {
    uint8_t key[16], nonce[16];
    for (int i = 0; i < 16; i++) {
        key[i] = std::stoi(keyHex.substr(i*2, 2), nullptr, 16);
        nonce[i] = std::stoi(nonceHex.substr(i*2, 2), nullptr, 16);
    }
    
    // Multiple layers of XOR with different patterns
    for (size_t i = 0; i < dataLen; i++) {
        data[i] ^= key[i % 16];
        data[i] ^= nonce[i % 16];
        data[i] ^= (i * 7 + 13) & 0xFF; // Additional obfuscation
    }
}

int main() {
    // Anti-debugging check
    if (isDebugged()) {
        std::cerr << "Debugging detected!" << std::endl;
        return 1;
    }
    
    // Embedded encrypted data
    uint8_t encryptedData[] = {EMBEDDED_DATA};
    size_t dataSize = sizeof(encryptedData);
    
    // Polymorphic decryption
    polymorphicDecrypt(encryptedData, dataSize, {KEY_VAR}, {NONCE_VAR});
    
    // Write with random filename
    std::string filename = "output_" + std::to_string(time(nullptr)) + ".bin";
    std::ofstream outFile(filename, std::ios::binary);
    if (outFile.is_open()) {
        outFile.write(reinterpret_cast<char*>(encryptedData), dataSize);
        outFile.close();
        std::cout << "Data decrypted and saved to " << filename << std::endl;
    } else {
        std::cerr << "Failed to create output file" << std::endl;
        return 1;
    }
    
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
        
        std::vector<uint8_t> fileData((std::istreambuf_iterator<char>(inFile)),
                                     std::istreambuf_iterator<char>());
        inFile.close();
        
        if (fileData.empty()) {
            std::cerr << "Error: Input file is empty" << std::endl;
            return;
        }
        
        // Generate or use provided key/nonce
        std::string key, nonce;
        if (useRandomKey) {
            key = generateRandomKey();
            nonce = generateRandomNonce();
        } else {
            // Use environment variable or default
            const char* envKey = std::getenv("ENCRYPTION_KEY");
            if (envKey) {
                key = envKey;
                // Pad or truncate to 32 hex chars
                while (key.length() < 32) key += "0";
                if (key.length() > 32) key = key.substr(0, 32);
            } else {
                key = "0123456789abcdef0123456789abcdef"; // Default key
            }
            nonce = generateRandomNonce();
        }
        
        // Encrypt the data
        std::vector<uint8_t> encryptedData = fileData;
        uint8_t keyBytes[16], nonceBytes[16];
        
        // Convert hex strings to bytes
        for (int i = 0; i < 16; i++) {
            keyBytes[i] = std::stoi(key.substr(i*2, 2), nullptr, 16);
            nonceBytes[i] = std::stoi(nonce.substr(i*2, 2), nullptr, 16);
        }
        
        // Encrypt using AES-128-CTR
        aesCtrCrypt(encryptedData.data(), encryptedData.size(), keyBytes, nonceBytes);
        
        // Generate stub template
        std::string stubTemplate = generateStubTemplate(stubType);
        
        // Replace placeholders
        std::string keyVar, nonceVar;
        
        // Replace placeholders in template
        size_t pos;
        
        // Replace key definition
        pos = stubTemplate.find("{KEY_DEFINITION}");
        if (pos != std::string::npos) {
            stubTemplate.replace(pos, 16, obfuscateStringWithVar(key, keyVar));
        }
        
        // Replace nonce definition
        pos = stubTemplate.find("{NONCE_DEFINITION}");
        if (pos != std::string::npos) {
            stubTemplate.replace(pos, 18, obfuscateStringWithVar(nonce, nonceVar));
        }
        
        // Replace embedded data
        pos = stubTemplate.find("{EMBEDDED_DATA}");
        if (pos != std::string::npos) {
            stubTemplate.replace(pos, 15, embedDataAsArray(encryptedData));
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
        std::cout << "  Encrypted size: " << encryptedData.size() << " bytes" << std::endl;
        std::cout << "  Key: " << key << std::endl;
        std::cout << "  Nonce: " << nonce << std::endl;
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