#include "encryption_plugin_manager.h"
#include <algorithm>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace EncryptionPlugins {

PluginManager* PluginManager::instance = nullptr;

PluginManager& PluginManager::getInstance() {
    if (!instance) {
        instance = new PluginManager();
    }
    return *instance;
}

bool PluginManager::registerPlugin(const std::string& name, CreatePluginFunc createFunc, 
                                  const std::string& description, bool isBuiltin) {
    std::lock_guard<std::mutex> lock(pluginMutex);
    
    if (plugins.find(name) != plugins.end()) {
        std::cerr << "Plugin '" << name << "' already registered!" << std::endl;
        return false;
    }
    
    PluginInfo info;
    info.name = name;
    info.description = description;
    info.createFunc = createFunc;
    info.isBuiltin = isBuiltin;
    
    plugins[name] = info;
    return true;
}

bool PluginManager::unregisterPlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(pluginMutex);
    
    auto it = plugins.find(name);
    if (it == plugins.end()) {
        return false;
    }
    
    plugins.erase(it);
    return true;
}

std::unique_ptr<IEncryptionPlugin> PluginManager::createPlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(pluginMutex);
    
    auto it = plugins.find(name);
    if (it == plugins.end()) {
        return nullptr;
    }
    
    return it->second.createFunc();
}

std::vector<std::string> PluginManager::getAvailablePlugins() const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(pluginMutex));
    
    std::vector<std::string> names;
    for (const auto& pair : plugins) {
        names.push_back(pair.first);
    }
    
    return names;
}

PluginInfo PluginManager::getPluginInfo(const std::string& name) const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(pluginMutex));
    
    auto it = plugins.find(name);
    if (it != plugins.end()) {
        return it->second;
    }
    
    return PluginInfo();
}

bool PluginManager::hasPlugin(const std::string& name) const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(pluginMutex));
    return plugins.find(name) != plugins.end();
}

bool PluginManager::loadPluginFromFile(const std::string& filepath) {
    #ifdef _WIN32
    HMODULE handle = LoadLibraryA(filepath.c_str());
    if (!handle) {
        std::cerr << "Failed to load plugin: " << filepath << std::endl;
        return false;
    }
    
    typedef void (*InitPluginFunc)();
    InitPluginFunc initFunc = (InitPluginFunc)GetProcAddress(handle, "InitializePlugin");
    if (!initFunc) {
        std::cerr << "Plugin missing InitializePlugin function: " << filepath << std::endl;
        FreeLibrary(handle);
        return false;
    }
    #else
    void* handle = dlopen(filepath.c_str(), RTLD_LAZY);
    if (!handle) {
        std::cerr << "Failed to load plugin: " << filepath << " - " << dlerror() << std::endl;
        return false;
    }
    
    typedef void (*InitPluginFunc)();
    InitPluginFunc initFunc = (InitPluginFunc)dlsym(handle, "InitializePlugin");
    if (!initFunc) {
        std::cerr << "Plugin missing InitializePlugin function: " << filepath << std::endl;
        dlclose(handle);
        return false;
    }
    #endif
    
    // Call the initialization function
    initFunc();
    
    return true;
}

PluginManager::PluginList PluginManager::getPluginsByType() const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(pluginMutex));
    
    PluginList list;
    for (const auto& pair : plugins) {
        if (pair.second.isBuiltin) {
            list.builtinPlugins.push_back(pair.first);
        } else {
            list.externalPlugins.push_back(pair.first);
        }
    }
    
    // Sort the lists
    std::sort(list.builtinPlugins.begin(), list.builtinPlugins.end());
    std::sort(list.externalPlugins.begin(), list.externalPlugins.end());
    
    return list;
}

bool PluginManager::validatePlugin(IEncryptionPlugin* plugin) {
    if (!plugin) return false;
    
    // Basic validation
    if (plugin->getName().empty()) return false;
    if (plugin->getKeySize() == 0) return false;
    
    // Test encryption/decryption with dummy data
    std::vector<uint8_t> testData = {0x01, 0x02, 0x03, 0x04, 0x05};
    std::vector<uint8_t> originalData = testData;
    
    EncryptionContext context;
    context.key.resize(plugin->getKeySize(), 0xAA);
    if (plugin->getNonceSize() > 0) {
        context.nonce.resize(plugin->getNonceSize(), 0xBB);
    }
    if (plugin->requiresIV()) {
        context.iv.resize(16, 0xCC);
    }
    
    if (!plugin->initialize(context)) return false;
    if (!plugin->encrypt(testData, context)) return false;
    
    // Data should be different after encryption
    if (testData == originalData) return false;
    
    if (!plugin->decrypt(testData, context)) return false;
    
    // Data should match original after decryption
    if (testData != originalData) return false;
    
    return true;
}

void PluginManager::cleanup() {
    std::lock_guard<std::mutex> lock(pluginMutex);
    plugins.clear();
}

} // namespace EncryptionPlugins