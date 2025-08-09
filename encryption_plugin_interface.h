#pragma once

#include <vector>
#include <string>
#include <memory>
#include <cstdint>

namespace EncryptionPlugins {

// Forward declarations
struct EncryptionContext {
    std::vector<uint8_t> key;
    std::vector<uint8_t> nonce;
    std::vector<uint8_t> iv;
    size_t counter = 0;
    
    // Additional context data for plugin-specific needs
    void* pluginData = nullptr;
};

// Base interface for all encryption plugins
class IEncryptionPlugin {
public:
    virtual ~IEncryptionPlugin() = default;
    
    // Plugin metadata
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::string getVersion() const = 0;
    
    // Key requirements
    virtual size_t getKeySize() const = 0;
    virtual size_t getNonceSize() const = 0;
    virtual bool requiresIV() const = 0;
    
    // Encryption operations
    virtual bool initialize(const EncryptionContext& context) = 0;
    virtual bool encrypt(std::vector<uint8_t>& data, const EncryptionContext& context) = 0;
    virtual bool decrypt(std::vector<uint8_t>& data, const EncryptionContext& context) = 0;
    
    // Generate stub code for this encryption method
    virtual std::string generateStubCode(const std::string& varPrefix, bool useDecimalKeys = false) const = 0;
    virtual std::string generateDecryptionCode(const std::string& dataVar, const std::string& keyVar, size_t keySize) const = 0;
    
    // Optional: Performance hints
    virtual bool supportsParallelProcessing() const { return false; }
    virtual size_t getOptimalBlockSize() const { return 0; }
    
    // Optional: Security features
    virtual bool hasAntiAnalysisFeatures() const { return false; }
    virtual std::string generateAntiAnalysisCode() const { return ""; }
};

// Plugin factory function type
typedef std::unique_ptr<IEncryptionPlugin> (*CreatePluginFunc)();

// Plugin registration structure
struct PluginInfo {
    std::string name;
    std::string description;
    CreatePluginFunc createFunc;
    bool isBuiltin;
};

} // namespace EncryptionPlugins