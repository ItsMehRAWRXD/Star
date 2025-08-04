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
        std::regex keyPattern(R"(const\s+std::string\s+(\w+)\s*=\s*"([0-9a-fA-F]+)"\s*;)");
        
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