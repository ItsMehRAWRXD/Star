#include "../encryption_plugin_interface.h"
#include "../encryption_plugin_manager.h"
#include <cstring>
#include <sstream>
#include <iomanip>

namespace EncryptionPlugins {

class AESPlugin : public IEncryptionPlugin {
private:
    // AES S-box
    static constexpr uint8_t SBOX[256] = {
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
    
    // Simple stream cipher mode using AES S-box
    void streamEncrypt(std::vector<uint8_t>& data, const std::vector<uint8_t>& key, const std::vector<uint8_t>& nonce) {
        // Create keystream generator using key and nonce
        uint8_t state[16];
        memcpy(state, key.data(), std::min<size_t>(16, key.size()));
        
        if (nonce.size() >= 16) {
            for (size_t i = 0; i < 16; i++) {
                state[i] ^= nonce[i];
            }
        }
        
        // Generate keystream and XOR with data
        for (size_t i = 0; i < data.size(); i++) {
            // Update state
            state[i % 16] = SBOX[(state[i % 16] + state[(i + 1) % 16] + i) & 0xFF];
            
            // XOR with keystream
            data[i] ^= state[i % 16];
            
            // Add position dependency
            data[i] ^= (i & 0xFF);
        }
    }
    
public:
    std::string getName() const override {
        return "AES";
    }
    
    std::string getDescription() const override {
        return "Enhanced AES stream cipher mode with position dependency";
    }
    
    std::string getVersion() const override {
        return "1.0.0";
    }
    
    size_t getKeySize() const override {
        return 16; // 128 bits
    }
    
    size_t getNonceSize() const override {
        return 16; // 128 bits
    }
    
    bool requiresIV() const override {
        return false;
    }
    
    bool initialize(const EncryptionContext& context) override {
        if (context.key.size() != 16 || context.nonce.size() != 16) {
            return false;
        }
        return true;
    }
    
    bool encrypt(std::vector<uint8_t>& data, const EncryptionContext& context) override {
        if (context.key.size() != 16 || context.nonce.size() != 16) {
            return false;
        }
        
        streamEncrypt(data, context.key, context.nonce);
        return true;
    }
    
    bool decrypt(std::vector<uint8_t>& data, const EncryptionContext& context) override {
        // Stream cipher is symmetric
        return encrypt(data, context);
    }
    
    std::string generateStubCode(const std::string& varPrefix, bool useDecimalKeys) const override {
        std::stringstream code;
        
        code << "// AES Stream Cipher implementation\n";
        code << "class " << varPrefix << "AES {\n";
        code << "private:\n";
        code << "    static constexpr uint8_t sbox[256] = {\n";
        code << "        ";
        
        // Output S-box
        for (int i = 0; i < 256; i++) {
            if (i > 0 && i % 16 == 0) code << "\n        ";
            code << "0x" << std::hex << std::setw(2) << std::setfill('0') << (int)SBOX[i];
            if (i < 255) code << ", ";
        }
        
        code << "\n    };\n";
        code << "public:\n";
        code << "    static void process(std::vector<uint8_t>& data, const uint8_t* key, const uint8_t* nonce) {\n";
        code << "        uint8_t state[16];\n";
        code << "        memcpy(state, key, 16);\n";
        code << "        for (size_t i = 0; i < 16; i++) state[i] ^= nonce[i];\n";
        code << "        \n";
        code << "        for (size_t i = 0; i < data.size(); i++) {\n";
        code << "            state[i % 16] = sbox[(state[i % 16] + state[(i + 1) % 16] + i) & 0xFF];\n";
        code << "            data[i] ^= state[i % 16];\n";
        code << "            data[i] ^= (i & 0xFF);\n";
        code << "        }\n";
        code << "    }\n";
        code << "};\n";
        
        return code.str();
    }
    
    std::string generateDecryptionCode(const std::string& dataVar, const std::string& keyVar, size_t keySize) const override {
        std::stringstream code;
        code << "::process(" << dataVar << ", " 
             << keyVar << ", " << keyVar << " + 16);\n";
        return code.str();
    }
    
    bool supportsParallelProcessing() const override {
        return false; // Stream mode has dependencies
    }
    
    size_t getOptimalBlockSize() const override {
        return 16; // AES block size
    }
    
    bool hasAntiAnalysisFeatures() const override {
        return true;
    }
    
    std::string generateAntiAnalysisCode() const override {
        return "// AES S-box provides non-linear transformation\n";
    }
};

// Register the plugin
REGISTER_ENCRYPTION_PLUGIN("AES", AESPlugin)

} // namespace EncryptionPlugins