#pragma once

#include "encryption_plugin_interface.h"
#include <map>
#include <mutex>
#include <functional>

namespace EncryptionPlugins {

class PluginManager {
private:
    static PluginManager* instance;
    std::map<std::string, PluginInfo> plugins;
    std::mutex pluginMutex;
    
    PluginManager() = default;
    
public:
    static PluginManager& getInstance();
    
    // Plugin registration
    bool registerPlugin(const std::string& name, CreatePluginFunc createFunc, 
                       const std::string& description = "", bool isBuiltin = false);
    bool unregisterPlugin(const std::string& name);
    
    // Plugin creation
    std::unique_ptr<IEncryptionPlugin> createPlugin(const std::string& name);
    
    // Plugin discovery
    std::vector<std::string> getAvailablePlugins() const;
    PluginInfo getPluginInfo(const std::string& name) const;
    bool hasPlugin(const std::string& name) const;
    
    // Load external plugins (for future extension)
    bool loadPluginFromFile(const std::string& filepath);
    
    // Convenience method for getting all plugin names sorted by type
    struct PluginList {
        std::vector<std::string> builtinPlugins;
        std::vector<std::string> externalPlugins;
    };
    PluginList getPluginsByType() const;
    
    // Plugin validation
    bool validatePlugin(IEncryptionPlugin* plugin);
    
    // Cleanup
    void cleanup();
    
    // Prevent copying
    PluginManager(const PluginManager&) = delete;
    PluginManager& operator=(const PluginManager&) = delete;
};

// Helper macro for plugin registration
#define REGISTER_ENCRYPTION_PLUGIN(name, className) \
    namespace { \
        struct className##Registrar { \
            className##Registrar() { \
                PluginManager::getInstance().registerPlugin( \
                    name, \
                    []() -> std::unique_ptr<IEncryptionPlugin> { \
                        return std::make_unique<className>(); \
                    }, \
                    className().getDescription(), \
                    true \
                ); \
            } \
        }; \
        static className##Registrar className##Instance; \
    }

} // namespace EncryptionPlugins