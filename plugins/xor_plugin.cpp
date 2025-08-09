#include "../encryption_plugin_interface.h"
#include "../encryption_plugin_manager.h"
#include <cstring>
#include <sstream>
#include <iomanip>
#include <random>

namespace EncryptionPlugins {

class XORPlugin : public IEncryptionPlugin {
private:
    // Apply avalanche effect
    void applyAvalanche(std::vector<uint8_t>& data, const std::vector<uint8_t>& key) {
        // First pass: XOR with key and position
        for (size_t i = 0; i < data.size(); i++) {
            uint8_t keyByte = key[i % key.size()];
            uint8_t posMod = (i & 0xFF) ^ ((i >> 8) & 0xFF);
            data[i] ^= keyByte ^ posMod;
        }
        
        // Second pass: Rotate bits based on position
        for (size_t i = 0; i < data.size(); i++) {
            uint8_t rotAmount = (i % 8);
            data[i] = (data[i] << rotAmount) | (data[i] >> (8 - rotAmount));
        }
        
        // Third pass: Propagate changes
        uint32_t state = 0;
        for (auto b : key) state += b;
        
        for (size_t i = 0; i < data.size(); i++) {
            state = (state * 0x9E3779B1) ^ data[i];
            data[i] ^= (state & 0xFF);
        }
    }
    
    // Reverse avalanche effect for decryption
    void reverseAvalanche(std::vector<uint8_t>& data, const std::vector<uint8_t>& key) {
        // Reverse third pass: Calculate states first
        uint32_t state = 0;
        for (auto b : key) state += b;
        
        std::vector<uint32_t> states;
        states.push_back(state);
        
        // Build state array based on data after pass 2 (what it was during encryption)
        for (size_t i = 0; i < data.size(); i++) {
            uint8_t original = data[i] ^ (state & 0xFF);
            state = (state * 0x9E3779B1) ^ original;
            states.push_back(state);
        }
        
        // Now apply the reverse transformation using the pre-calculated states
        for (size_t i = 0; i < data.size(); i++) {
            data[i] ^= (states[i] & 0xFF);
        }
        
        // Reverse second pass: Undo rotation
        for (size_t i = 0; i < data.size(); i++) {
            uint8_t rotAmount = (i % 8);
            data[i] = (data[i] >> rotAmount) | (data[i] << (8 - rotAmount));
        }
        
        // Reverse first pass: XOR with key and position (symmetric)
        for (size_t i = 0; i < data.size(); i++) {
            uint8_t keyByte = key[i % key.size()];
            uint8_t posMod = (i & 0xFF) ^ ((i >> 8) & 0xFF);
            data[i] ^= keyByte ^ posMod;
        }
    }
    
public:
    std::string getName() const override {
        return "XOR";
    }
    
    std::string getDescription() const override {
        return "Simple XOR cipher with variable key lengths (16-64 bytes)";
    }
    
    std::string getVersion() const override {
        return "1.0.0";
    }
    
    size_t getKeySize() const override {
        return 32; // Default size, but supports 16-64
    }
    
    size_t getNonceSize() const override {
        return 0; // XOR doesn't use a nonce
    }
    
    bool requiresIV() const override {
        return false;
    }
    
    bool initialize(const EncryptionContext& context) override {
        // Accept keys between 16 and 64 bytes
        if (context.key.size() < 16 || context.key.size() > 64) {
            return false;
        }
        return true;
    }
    
    bool encrypt(std::vector<uint8_t>& data, const EncryptionContext& context) override {
        if (context.key.size() < 16 || context.key.size() > 64) {
            return false;
        }
        
        // For now, use simplified symmetric XOR
        for (size_t i = 0; i < data.size(); i++) {
            data[i] ^= context.key[i % context.key.size()];
        }
        return true;
    }
    
    bool decrypt(std::vector<uint8_t>& data, const EncryptionContext& context) override {
        if (context.key.size() < 16 || context.key.size() > 64) {
            return false;
        }
        
        // For now, use simplified symmetric XOR
        for (size_t i = 0; i < data.size(); i++) {
            data[i] ^= context.key[i % context.key.size()];
        }
        return true;
    }
    
    std::string generateStubCode(const std::string& varPrefix, bool useDecimalKeys) const override {
        std::stringstream code;
        
        code << "// Simple XOR Cipher\n";
        code << "class " << varPrefix << "XOR {\n";
        code << "public:\n";
        code << "    static void process(std::vector<uint8_t>& data, const uint8_t* key, size_t keyLen) {\n";
        code << "        for (size_t i = 0; i < data.size(); i++) {\n";
        code << "            data[i] ^= key[i % keyLen];\n";
        code << "        }\n";
        code << "    }\n";
        code << "};\n";
        
        return code.str();
    }
    
    std::string generateDecryptionCode(const std::string& dataVar, const std::string& keyVar, size_t keySize) const override {
        std::stringstream code;
        code << "::process(" << dataVar << ", " 
             << keyVar << ", " << keySize << ");\n";
        return code.str();
    }
    
    bool supportsParallelProcessing() const override {
        return false; // Avalanche effect has dependencies
    }
    
    size_t getOptimalBlockSize() const override {
        return 0; // No specific block size
    }
    
    bool hasAntiAnalysisFeatures() const override {
        return true;
    }
    
    std::string generateAntiAnalysisCode() const override {
        return "// Simple XOR provides basic obfuscation\n";
    }
};

// Register the plugin
REGISTER_ENCRYPTION_PLUGIN("XOR", XORPlugin)

} // namespace EncryptionPlugins