#include "../encryption_plugin_interface.h"
#include "../encryption_plugin_manager.h"
#include <cstring>
#include <sstream>
#include <iomanip>

namespace EncryptionPlugins {

class ChaCha20Plugin : public IEncryptionPlugin {
private:
    // ChaCha20 constants
    static constexpr uint32_t CHACHA_CONSTANTS[4] = {
        0x61707865, 0x3320646e, 0x79622d32, 0x6b206574
    };
    
    // Quarter round function
    void quarterRound(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d) {
        a += b; d ^= a; d = (d << 16) | (d >> 16);
        c += d; b ^= c; b = (b << 12) | (b >> 20);
        a += b; d ^= a; d = (d << 8) | (d >> 24);
        c += d; b ^= c; b = (b << 7) | (b >> 25);
    }
    
    // ChaCha20 block function
    void chacha20Block(uint32_t out[16], const uint32_t in[16]) {
        memcpy(out, in, 64);
        
        for (int i = 0; i < 10; i++) {
            // Column rounds
            quarterRound(out[0], out[4], out[8], out[12]);
            quarterRound(out[1], out[5], out[9], out[13]);
            quarterRound(out[2], out[6], out[10], out[14]);
            quarterRound(out[3], out[7], out[11], out[15]);
            
            // Diagonal rounds
            quarterRound(out[0], out[5], out[10], out[15]);
            quarterRound(out[1], out[6], out[11], out[12]);
            quarterRound(out[2], out[7], out[8], out[13]);
            quarterRound(out[3], out[4], out[9], out[14]);
        }
        
        for (int i = 0; i < 16; i++) {
            out[i] += in[i];
        }
    }
    
public:
    std::string getName() const override {
        return "ChaCha20";
    }
    
    std::string getDescription() const override {
        return "ChaCha20 stream cipher with 256-bit key and 96-bit nonce";
    }
    
    std::string getVersion() const override {
        return "1.0.0";
    }
    
    size_t getKeySize() const override {
        return 32; // 256 bits
    }
    
    size_t getNonceSize() const override {
        return 12; // 96 bits
    }
    
    bool requiresIV() const override {
        return false;
    }
    
    bool initialize(const EncryptionContext& context) override {
        if (context.key.size() != 32 || context.nonce.size() != 12) {
            return false;
        }
        return true;
    }
    
    bool encrypt(std::vector<uint8_t>& data, const EncryptionContext& context) override {
        if (context.key.size() != 32 || context.nonce.size() != 12) {
            return false;
        }
        
        uint32_t state[16];
        uint32_t keystream[16];
        
        // Initialize state with constants
        memcpy(state, CHACHA_CONSTANTS, 16);
        
        // Add key
        memcpy(&state[4], context.key.data(), 32);
        
        // Add counter (starts at 0)
        state[12] = 0;
        
        // Add nonce
        memcpy(&state[13], context.nonce.data(), 12);
        
        // Process data in 64-byte blocks
        size_t offset = 0;
        while (offset < data.size()) {
            chacha20Block(keystream, state);
            
            size_t blockSize = std::min<size_t>(64, data.size() - offset);
            uint8_t* keystreamBytes = reinterpret_cast<uint8_t*>(keystream);
            
            for (size_t i = 0; i < blockSize; i++) {
                data[offset + i] ^= keystreamBytes[i];
            }
            
            offset += blockSize;
            state[12]++; // Increment counter
        }
        
        return true;
    }
    
    bool decrypt(std::vector<uint8_t>& data, const EncryptionContext& context) override {
        // ChaCha20 is symmetric, so decryption is the same as encryption
        return encrypt(data, context);
    }
    
    std::string generateStubCode(const std::string& varPrefix, bool useDecimalKeys) const override {
        std::stringstream code;
        
        code << "// ChaCha20 implementation\n";
        code << "class " << varPrefix << "ChaCha20 {\n";
        code << "private:\n";
        code << "    static void quarterRound(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d) {\n";
        code << "        a += b; d ^= a; d = (d << 16) | (d >> 16);\n";
        code << "        c += d; b ^= c; b = (b << 12) | (b >> 20);\n";
        code << "        a += b; d ^= a; d = (d << 8) | (d >> 24);\n";
        code << "        c += d; b ^= c; b = (b << 7) | (b >> 25);\n";
        code << "    }\n";
        code << "    \n";
        code << "    static void chacha20Block(uint32_t out[16], const uint32_t in[16]) {\n";
        code << "        memcpy(out, in, 64);\n";
        code << "        for (int i = 0; i < 10; i++) {\n";
        code << "            quarterRound(out[0], out[4], out[8], out[12]);\n";
        code << "            quarterRound(out[1], out[5], out[9], out[13]);\n";
        code << "            quarterRound(out[2], out[6], out[10], out[14]);\n";
        code << "            quarterRound(out[3], out[7], out[11], out[15]);\n";
        code << "            quarterRound(out[0], out[5], out[10], out[15]);\n";
        code << "            quarterRound(out[1], out[6], out[11], out[12]);\n";
        code << "            quarterRound(out[2], out[7], out[8], out[13]);\n";
        code << "            quarterRound(out[3], out[4], out[9], out[14]);\n";
        code << "        }\n";
        code << "        for (int i = 0; i < 16; i++) out[i] += in[i];\n";
        code << "    }\n";
        code << "public:\n";
        code << "    static void process(std::vector<uint8_t>& data, const uint8_t* key, const uint8_t* nonce) {\n";
        code << "        uint32_t state[16] = {0x61707865, 0x3320646e, 0x79622d32, 0x6b206574};\n";
        code << "        memcpy(&state[4], key, 32);\n";
        code << "        state[12] = 0;\n";
        code << "        memcpy(&state[13], nonce, 12);\n";
        code << "        \n";
        code << "        size_t offset = 0;\n";
        code << "        while (offset < data.size()) {\n";
        code << "            uint32_t keystream[16];\n";
        code << "            chacha20Block(keystream, state);\n";
        code << "            size_t blockSize = std::min<size_t>(64, data.size() - offset);\n";
        code << "            uint8_t* ks = reinterpret_cast<uint8_t*>(keystream);\n";
        code << "            for (size_t i = 0; i < blockSize; i++) {\n";
        code << "                data[offset + i] ^= ks[i];\n";
        code << "            }\n";
        code << "            offset += blockSize;\n";
        code << "            state[12]++;\n";
        code << "        }\n";
        code << "    }\n";
        code << "};\n";
        
        return code.str();
    }
    
    std::string generateDecryptionCode(const std::string& dataVar, const std::string& keyVar, size_t keySize) const override {
        std::stringstream code;
        // Note: The class name prefix will be handled by the stub generator
        code << "::process(" << dataVar << ", " 
             << keyVar << ", " << keyVar << " + 32);\n";
        return code.str();
    }
    
    bool supportsParallelProcessing() const override {
        return true; // ChaCha20 can process blocks in parallel
    }
    
    size_t getOptimalBlockSize() const override {
        return 64; // ChaCha20 processes 64-byte blocks
    }
    
    bool hasAntiAnalysisFeatures() const override {
        return true;
    }
    
    std::string generateAntiAnalysisCode() const override {
        return "// ChaCha20 constant-time operations resist timing attacks\n";
    }
};

// Register the plugin
REGISTER_ENCRYPTION_PLUGIN("ChaCha20", ChaCha20Plugin)

} // namespace EncryptionPlugins