#include <iostream>
#include <vector>
#include <cstring>

#include "encryption_plugin_interface.h"
#include "encryption_plugin_manager.h"

// Include plugin implementations
#include "plugins/chacha20_plugin.cpp"
#include "plugins/aes_plugin.cpp" 
#include "plugins/xor_plugin.cpp"

using namespace EncryptionPlugins;

int main() {
    std::cout << "=== Encryption Plugin System Test ===" << std::endl;
    std::cout << "Testing branch-style encryption plugins\n" << std::endl;
    
    auto& pluginManager = PluginManager::getInstance();
    
    // List available plugins
    std::cout << "Available plugins:" << std::endl;
    auto plugins = pluginManager.getAvailablePlugins();
    for (const auto& name : plugins) {
        auto plugin = pluginManager.createPlugin(name);
        if (plugin) {
            std::cout << "  - " << name << ": " << plugin->getDescription() << std::endl;
            std::cout << "    Version: " << plugin->getVersion() << std::endl;
            std::cout << "    Key size: " << plugin->getKeySize() << " bytes" << std::endl;
            std::cout << "    Nonce size: " << plugin->getNonceSize() << " bytes" << std::endl;
            std::cout << "    Requires IV: " << (plugin->requiresIV() ? "Yes" : "No") << std::endl;
            std::cout << "    Supports parallel: " << (plugin->supportsParallelProcessing() ? "Yes" : "No") << std::endl;
            std::cout << std::endl;
        }
    }
    
    // Test encryption/decryption with each plugin
    std::cout << "\nTesting encryption/decryption:" << std::endl;
    
    std::vector<uint8_t> testData = {
        0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0x57, 0x6f, 0x72, 0x6c, 0x64, 0x21
    }; // "Hello World!"
    
    for (const auto& name : plugins) {
        std::cout << "\nTesting " << name << ":" << std::endl;
        
        auto plugin = pluginManager.createPlugin(name);
        if (!plugin) continue;
        
        // Create encryption context
        EncryptionContext context;
        
        // Generate key
        context.key.resize(plugin->getKeySize());
        for (size_t i = 0; i < context.key.size(); i++) {
            context.key[i] = (i * 17 + 123) & 0xFF; // Deterministic key for testing
        }
        
        // Generate nonce if needed
        if (plugin->getNonceSize() > 0) {
            context.nonce.resize(plugin->getNonceSize());
            for (size_t i = 0; i < context.nonce.size(); i++) {
                context.nonce[i] = (i * 31 + 67) & 0xFF;
            }
        }
        
        // Generate IV if needed
        if (plugin->requiresIV()) {
            context.iv.resize(16);
            for (size_t i = 0; i < 16; i++) {
                context.iv[i] = (i * 23 + 89) & 0xFF;
            }
        }
        
        // Test data
        std::vector<uint8_t> data = testData;
        std::vector<uint8_t> originalData = data;
        
        // Initialize plugin
        if (!plugin->initialize(context)) {
            std::cout << "  Failed to initialize!" << std::endl;
            continue;
        }
        
        // Encrypt
        if (!plugin->encrypt(data, context)) {
            std::cout << "  Encryption failed!" << std::endl;
            continue;
        }
        
        std::cout << "  Original: ";
        for (auto b : originalData) std::cout << std::hex << (int)b << " ";
        std::cout << std::endl;
        
        std::cout << "  Encrypted: ";
        for (size_t i = 0; i < std::min<size_t>(12, data.size()); i++) {
            std::cout << std::hex << (int)data[i] << " ";
        }
        if (data.size() > 12) std::cout << "...";
        std::cout << std::endl;
        
        // Decrypt
        if (!plugin->decrypt(data, context)) {
            std::cout << "  Decryption failed!" << std::endl;
            continue;
        }
        
        std::cout << "  Decrypted: ";
        for (auto b : data) std::cout << std::hex << (int)b << " ";
        std::cout << std::endl;
        
        // Verify
        if (data == originalData) {
            std::cout << "  ✓ Success: Decrypted data matches original!" << std::endl;
        } else {
            std::cout << "  ✗ Error: Decrypted data does not match original!" << std::endl;
        }
        
        // Test stub code generation
        std::cout << "  Generating stub code..." << std::endl;
        std::string stubCode = plugin->generateStubCode("test", false);
        if (!stubCode.empty()) {
            std::cout << "  ✓ Stub code generated (" << stubCode.size() << " bytes)" << std::endl;
        }
    }
    
    // Validate all plugins
    std::cout << "\nValidating all plugins:" << std::endl;
    for (const auto& name : plugins) {
        auto plugin = pluginManager.createPlugin(name);
        if (plugin) {
            bool valid = pluginManager.validatePlugin(plugin.get());
            std::cout << "  " << name << ": " << (valid ? "✓ Valid" : "✗ Invalid") << std::endl;
        }
    }
    
    std::cout << "\n=== Test Complete ===" << std::endl;
    
    return 0;
}