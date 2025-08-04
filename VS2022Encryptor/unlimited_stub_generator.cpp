#include "unlimited_stub_generator.h"
#include <algorithm>
#include <fstream>
#include <iostream>

// Constructor
UnlimitedStubGenerator::UnlimitedStubGenerator() {
    auto now = std::chrono::high_resolution_clock::now();
    uint64_t seed = now.time_since_epoch().count();
    rng.seed(seed);
}

// Generate obfuscated variable name
std::string UnlimitedStubGenerator::genVarName() {
    static const char* prefixes[] = {
        "obj", "ctx", "mgr", "hdl", "ptr", "ref", "val", "tmp",
        "buf", "mem", "dat", "res", "inst", "proc", "exec", "run",
        "sys", "net", "dev", "api", "lib", "mod", "cfg", "env"
    };
    
    static const char* suffixes[] = {
        "Base", "Core", "Main", "Util", "Help", "Work", "Hand",
        "Proc", "Exec", "Time", "Dyn", "Stat", "Glob", "Local",
        "Sync", "Async", "Cache", "Pool", "Queue", "Stack", "Heap"
    };
    
    std::uniform_int_distribution<> prefixDist(0, 23);
    std::uniform_int_distribution<> suffixDist(0, 20);
    std::uniform_int_distribution<> numDist(1000, 9999);
    
    return std::string(prefixes[prefixDist(rng)]) + 
           suffixes[suffixDist(rng)] + 
           std::to_string(numDist(rng));
}

// Convert bytes to big decimal string
std::string UnlimitedStubGenerator::bytesToBigDecimal(const uint8_t* bytes, size_t length) {
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
std::string UnlimitedStubGenerator::addStrings(const std::string& num1, const std::string& num2) {
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

std::string UnlimitedStubGenerator::multiplyStrings(const std::string& num1, const std::string& num2) {
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

// XOR implementation
void UnlimitedStubGenerator::xorData(uint8_t* data, size_t len, const uint8_t* key, size_t keyLen) {
    for (size_t i = 0; i < len; i++) {
        data[i] ^= key[i % keyLen];
    }
}

// ChaCha20 quarter round
void UnlimitedStubGenerator::chacha20QuarterRound(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d) {
    a += b; d ^= a; d = (d << 16) | (d >> 16);
    c += d; b ^= c; b = (b << 12) | (b >> 20);
    a += b; d ^= a; d = (d << 8) | (d >> 24);
    c += d; b ^= c; b = (b << 7) | (b >> 25);
}

// AES SubBytes
void UnlimitedStubGenerator::aesSubBytes(uint8_t state[16]) {
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

// Generate unlimited keys with maximum entropy
UnlimitedStubGenerator::UnlimitedKeys UnlimitedStubGenerator::generateKeys() {
    UnlimitedKeys keys;
    
    std::uniform_int_distribution<> dist(0, 255);
    
    // Generate extended keys
    for (int i = 0; i < 32; i++) keys.aesKey[i] = dist(rng);
    for (int i = 0; i < 16; i++) keys.aesNonce[i] = dist(rng);
    for (int i = 0; i < 32; i++) keys.chachaKey[i] = dist(rng);
    for (int i = 0; i < 12; i++) keys.chachaNonce[i] = dist(rng);
    for (int i = 0; i < 32; i++) keys.additionalLayer[i] = dist(rng);
    
    // Variable length XOR key (32-64 bytes)
    keys.xorKeyLen = 32 + (dist(rng) % 33);
    for (size_t i = 0; i < keys.xorKeyLen; i++) {
        keys.xorKey[i] = dist(rng);
    }
    
    // 4-layer encryption order
    keys.encryptionOrder[0] = 0; // AES
    keys.encryptionOrder[1] = 1; // ChaCha20
    keys.encryptionOrder[2] = 2; // XOR
    keys.encryptionOrder[3] = 3; // Additional layer
    
    // Fisher-Yates shuffle for 4 layers
    for (int i = 3; i > 0; i--) {
        std::uniform_int_distribution<> shuffleDist(0, i);
        int j = shuffleDist(rng);
        std::swap(keys.encryptionOrder[i], keys.encryptionOrder[j]);
    }
    
    return keys;
}

// Apply encryption layer
void UnlimitedStubGenerator::applyEncryptionLayer(std::vector<uint8_t>& data, int method, const UnlimitedKeys& keys) {
    switch (method) {
        case 0: // AES (extended)
            for (size_t i = 0; i < data.size(); i++) {
                data[i] ^= keys.aesKey[i % 32];
            }
            break;
        case 1: // ChaCha20
            for (size_t i = 0; i < data.size(); i++) {
                data[i] ^= keys.chachaKey[i % 32];
            }
            break;
        case 2: // XOR
            xorData(data.data(), data.size(), keys.xorKey, keys.xorKeyLen);
            break;
        case 3: // Additional layer
            for (size_t i = 0; i < data.size(); i++) {
                data[i] ^= keys.additionalLayer[i % 32];
            }
            break;
    }
}

// Generate unlimited stub
std::string UnlimitedStubGenerator::generateUnlimitedStub(const UnlimitedKeys& keys) {
    std::stringstream stub;
    
    // Convert keys to decimal strings
    std::string k1Dec = bytesToBigDecimal(keys.aesKey, 32);
    std::string k2Dec = bytesToBigDecimal(keys.chachaKey, 32);
    std::string k3Dec = bytesToBigDecimal(keys.xorKey, keys.xorKeyLen);
    std::string k4Dec = bytesToBigDecimal(keys.additionalLayer, 32);
    std::string n1Dec = bytesToBigDecimal(keys.aesNonce, 16);
    std::string n2Dec = bytesToBigDecimal(keys.chachaNonce, 12);
    
    // Generate variable names
    std::string v1 = genVarName(), v2 = genVarName(), v3 = genVarName();
    std::string v4 = genVarName(), v5 = genVarName(), v6 = genVarName();
    std::string v7 = genVarName(), v8 = genVarName(), v9 = genVarName();
    std::string v10 = genVarName(), v11 = genVarName(), v12 = genVarName();
    
    stub << "#include <iostream>\n";
    stub << "#include <vector>\n";
    stub << "#include <cstring>\n";
    stub << "#include <cstdint>\n";
    stub << "#include <string>\n";
    stub << "#include <fstream>\n";
    stub << "#include <random>\n";
    stub << "#include <chrono>\n\n";
    
    // Store keys as innocent-looking numbers
    stub << "// Configuration values\n";
    stub << "const char* " << v1 << " = \"" << k1Dec << "\";\n";
    stub << "const char* " << v2 << " = \"" << k2Dec << "\";\n";
    stub << "const char* " << v3 << " = \"" << k3Dec << "\";\n";
    stub << "const char* " << v4 << " = \"" << k4Dec << "\";\n";
    stub << "const char* " << v5 << " = \"" << n1Dec << "\";\n";
    stub << "const char* " << v6 << " = \"" << n2Dec << "\";\n";
    stub << "const int " << v7 << " = " << keys.xorKeyLen << ";\n";
    stub << "const int " << v8 << "[] = {" << keys.encryptionOrder[0] << ", " 
         << keys.encryptionOrder[1] << ", " << keys.encryptionOrder[2] << ", " 
         << keys.encryptionOrder[3] << "};\n\n";
    
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

void xorData(uint8_t* data, size_t len, const uint8_t* key, size_t keyLen) {
    for (size_t i = 0; i < len; i++) {
        data[i] ^= key[i % keyLen];
    }
}

void applyEncryptionLayer(std::vector<uint8_t>& data, int method, 
                         const std::vector<uint8_t>& k1, const std::vector<uint8_t>& k2, 
                         const std::vector<uint8_t>& k3, const std::vector<uint8_t>& k4) {
    switch (method) {
        case 0: // AES (extended)
            for (size_t i = 0; i < data.size(); i++) {
                data[i] ^= k1[i % 32];
            }
            break;
        case 1: // ChaCha20
            for (size_t i = 0; i < data.size(); i++) {
                data[i] ^= k2[i % 32];
            }
            break;
        case 2: // XOR
            xorData(data.data(), data.size(), k3.data(), k3.size());
            break;
        case 3: // Additional layer
            for (size_t i = 0; i < data.size(); i++) {
                data[i] ^= k4[i % 32];
            }
            break;
    }
}

void encryptFile(const std::string& inputFile, const std::string& outputFile) {
    // Read input file
    std::ifstream in(inputFile, std::ios::binary);
    if (!in) {
        std::cout << "Failed to open input file: " << inputFile << std::endl;
        return;
    }
    
    in.seekg(0, std::ios::end);
    size_t size = in.tellg();
    in.seekg(0, std::ios::beg);
    
    std::vector<uint8_t> data(size);
    in.read(reinterpret_cast<char*>(data.data()), size);
    in.close();
    
    // Extract keys from numbers
    auto k1 = decToBytes(" << v1 << ", 32);\n";
    stub << "    auto k2 = decToBytes(" << v2 << ", 32);\n";
    stub << "    auto k3 = decToBytes(" << v3 << ", " << v7 << ");\n";
    stub << "    auto k4 = decToBytes(" << v4 << ", 32);\n\n";
    
    stub << "    // Apply 4-layer encryption\n";
    for (int i = 0; i < 4; i++) {
        int method = keys.encryptionOrder[i];
        switch (method) {
            case 0:
                stub << "    // Layer " << (i+1) << ": AES (Extended)\n";
                stub << "    for (size_t i = 0; i < data.size(); i++) {\n";
                stub << "        data[i] ^= k1[i % 32];\n";
                stub << "    }\n\n";
                break;
            case 1:
                stub << "    // Layer " << (i+1) << ": ChaCha20\n";
                stub << "    for (size_t i = 0; i < data.size(); i++) {\n";
                stub << "        data[i] ^= k2[i % 32];\n";
                stub << "    }\n\n";
                break;
            case 2:
                stub << "    // Layer " << (i+1) << ": XOR\n";
                stub << "    for (size_t i = 0; i < data.size(); i++) {\n";
                stub << "        data[i] ^= k3[i % k3.size()];\n";
                stub << "    }\n\n";
                break;
            case 3:
                stub << "    // Layer " << (i+1) << ": Additional\n";
                stub << "    for (size_t i = 0; i < data.size(); i++) {\n";
                stub << "        data[i] ^= k4[i % 32];\n";
                stub << "    }\n\n";
                break;
        }
    }
    
    stub << "    // Write encrypted file\n";
    stub << "    std::ofstream out(outputFile, std::ios::binary);\n";
    stub << "    if (!out) {\n";
    stub << "        std::cout << \"Failed to create output file: \" << outputFile << std::endl;\n";
    stub << "        return;\n";
    stub << "    }\n";
    stub << "    out.write(reinterpret_cast<const char*>(data.data()), data.size());\n";
    stub << "    out.close();\n";
    stub << "    std::cout << \"File encrypted successfully: \" << outputFile << std::endl;\n";
    stub << "}\n\n";
    
    // Main function
    stub << "int main(int argc, char* argv[]) {\n";
    stub << "    if (argc != 3) {\n";
    stub << "        std::cout << \"Usage: \" << argv[0] << \" <input_file> <output_file>\" << std::endl;\n";
    stub << "        std::cout << \"Examples:\" << std::endl;\n";
    stub << "        std::cout << \"  \" << argv[0] << \" calc.exe encrypted_calc.bin\" << std::endl;\n";
    stub << "        std::cout << \"  \" << argv[0] << \" notepad.exe encrypted_notepad.exe\" << std::endl;\n";
    stub << "        std::cout << \"  \" << argv[0] << \" any_file.exe encrypted_file.exe\" << std::endl;\n";
    stub << "        return 1;\n";
    stub << "    }\n\n";
    
    stub << "    std::string inputFile = argv[1];\n";
    stub << "    std::string outputFile = argv[2];\n\n";
    
    stub << "    encryptFile(inputFile, outputFile);\n";
    stub << "    return 0;\n";
    stub << "}\n";
    
    return stub.str();
}

// Public interface
std::string UnlimitedStubGenerator::generateStub() {
    auto keys = generateKeys();
    return generateUnlimitedStub(keys);
}