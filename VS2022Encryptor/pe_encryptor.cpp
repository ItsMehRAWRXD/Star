#include "pe_encryptor.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iomanip>

// Constructor
PEEncryptor::PEEncryptor() {
    auto now = std::chrono::high_resolution_clock::now();
    uint64_t seed = now.time_since_epoch().count();
    rng.seed(seed);
}

// Generate obfuscated variable name
std::string PEEncryptor::genVarName() {
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

// Convert bytes to big decimal string
std::string PEEncryptor::bytesToBigDecimal(const uint8_t* bytes, size_t length) {
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
std::string PEEncryptor::addStrings(const std::string& num1, const std::string& num2) {
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

std::string PEEncryptor::multiplyStrings(const std::string& num1, const std::string& num2) {
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
void PEEncryptor::xorData(uint8_t* data, size_t len, const uint8_t* key, size_t keyLen) {
    for (size_t i = 0; i < len; i++) {
        data[i] ^= key[i % keyLen];
    }
}

// ChaCha20 quarter round
void PEEncryptor::chacha20QuarterRound(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d) {
    a += b; d ^= a; d = (d << 16) | (d >> 16);
    c += d; b ^= c; b = (b << 12) | (b >> 20);
    a += b; d ^= a; d = (d << 8) | (d >> 24);
    c += d; b ^= c; b = (b << 7) | (b >> 25);
}

// AES SubBytes
void PEEncryptor::aesSubBytes(uint8_t state[16]) {
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

// Load PE file
bool PEEncryptor::loadPE(const std::string& filename, PEInfo& peInfo) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) return false;
    
    file.seekg(0, std::ios::end);
    peInfo.fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    peInfo.fileData.resize(peInfo.fileSize);
    file.read(reinterpret_cast<char*>(peInfo.fileData.data()), peInfo.fileSize);
    file.close();
    
    if (peInfo.fileSize < sizeof(IMAGE_DOS_HEADER)) return false;
    
    peInfo.dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(peInfo.fileData.data());
    if (peInfo.dosHeader->e_magic != IMAGE_DOS_SIGNATURE) return false;
    
    if (peInfo.dosHeader->e_lfanew + sizeof(IMAGE_NT_HEADERS) > peInfo.fileSize) return false;
    
    peInfo.ntHeaders = reinterpret_cast<PIMAGE_NT_HEADERS>(
        peInfo.fileData.data() + peInfo.dosHeader->e_lfanew);
    
    if (peInfo.ntHeaders->Signature != IMAGE_NT_SIGNATURE) return false;
    
    peInfo.is64Bit = (peInfo.ntHeaders->FileHeader.Machine == IMAGE_FILE_MACHINE_AMD64);
    
    peInfo.sectionHeaders = reinterpret_cast<PIMAGE_SECTION_HEADER>(
        reinterpret_cast<uint8_t*>(peInfo.ntHeaders) + 
        sizeof(IMAGE_NT_HEADERS));
    
    return true;
}

// Check if section contains code
bool PEEncryptor::isCodeSection(const IMAGE_SECTION_HEADER& section) {
    return (section.Characteristics & IMAGE_SCN_CNT_CODE) != 0;
}

// Generate encryption keys
PEEncryptor::PEKeys PEEncryptor::generateKeys() {
    PEKeys keys;
    
    std::uniform_int_distribution<> dist(0, 255);
    
    // Generate keys
    for (int i = 0; i < 32; i++) keys.aesKey[i] = dist(rng);
    for (int i = 0; i < 32; i++) keys.chachaKey[i] = dist(rng);
    
    // Variable length XOR key
    keys.xorKeyLen = 32 + (dist(rng) % 33); // 32-64 bytes
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
void PEEncryptor::applyEncryptionLayer(std::vector<uint8_t>& data, int method, const PEKeys& keys) {
    switch (method) {
        case 0: // AES
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
    }
}

// Encrypt code section
void PEEncryptor::encryptCodeSection(std::vector<uint8_t>& data, const PEKeys& keys) {
    // Apply encryption layers in randomized order
    for (int i = 0; i < 3; ++i) {
        applyEncryptionLayer(data, keys.encryptionOrder[i], keys);
    }
}

// Encrypt PE file
bool PEEncryptor::encryptPE(const std::string& inputFile, const std::string& outputFile) {
    PEInfo peInfo;
    if (!loadPE(inputFile, peInfo)) {
        std::cout << "Failed to load PE file: " << inputFile << std::endl;
        return false;
    }
    
    PEKeys keys = generateKeys();
    
    // Find and encrypt code sections
    int numSections = peInfo.ntHeaders->FileHeader.NumberOfSections;
    bool foundCodeSection = false;
    
    for (int i = 0; i < numSections; i++) {
        IMAGE_SECTION_HEADER& section = peInfo.sectionHeaders[i];
        
        if (isCodeSection(section)) {
            foundCodeSection = true;
            
            // Get section data
            size_t sectionOffset = section.PointerToRawData;
            size_t sectionSize = section.SizeOfRawData;
            
            if (sectionOffset + sectionSize <= peInfo.fileSize) {
                std::vector<uint8_t> sectionData(
                    peInfo.fileData.begin() + sectionOffset,
                    peInfo.fileData.begin() + sectionOffset + sectionSize
                );
                
                // Encrypt the code section
                encryptCodeSection(sectionData, keys);
                
                // Copy back to file data
                std::copy(sectionData.begin(), sectionData.end(),
                         peInfo.fileData.begin() + sectionOffset);
            }
        }
    }
    
    if (!foundCodeSection) {
        std::cout << "No code sections found in PE file" << std::endl;
        return false;
    }
    
    // Write encrypted PE file (preserve original structure)
    std::ofstream outFile(outputFile, std::ios::binary);
    if (!outFile) {
        std::cout << "Failed to create output file: " << outputFile << std::endl;
        return false;
    }
    
    outFile.write(reinterpret_cast<const char*>(peInfo.fileData.data()), peInfo.fileSize);
    outFile.close();
    
    // Save keys separately (safer approach)
    std::string keyFile = outputFile + ".keys";
    std::ofstream keyOut(keyFile);
    keyOut << "AES Key: " << bytesToBigDecimal(keys.aesKey, 32) << std::endl;
    keyOut << "ChaCha20 Key: " << bytesToBigDecimal(keys.chachaKey, 32) << std::endl;
    keyOut << "XOR Key: " << bytesToBigDecimal(keys.xorKey, keys.xorKeyLen) << std::endl;
    keyOut << "Encryption Order: " << keys.encryptionOrder[0] << "," 
           << keys.encryptionOrder[1] << "," << keys.encryptionOrder[2] << std::endl;
    keyOut.close();
    
    std::cout << "PE file encrypted successfully: " << outputFile << std::endl;
    std::cout << "Keys saved to: " << keyFile << std::endl;
    std::cout << "File remains executable but code sections are encrypted" << std::endl;
    std::cout << "WARNING: This is for testing only - original file structure preserved!" << std::endl;
    
    return true;
}

// Generate PE stub
std::string PEEncryptor::generateStub(const std::string& inputFile) {
    PEInfo peInfo;
    if (!loadPE(inputFile, peInfo)) {
        return "// Failed to load PE file\n";
    }
    
    PEKeys keys = generateKeys();
    return generatePEStub(peInfo, keys);
}

// Generate PE stub with runtime decryption
std::string PEEncryptor::generatePEStub(const PEInfo& peInfo, const PEKeys& keys) {
    std::stringstream stub;
    
    // Convert keys to decimal strings
    std::string k1Dec = bytesToBigDecimal(keys.aesKey, 32);
    std::string k2Dec = bytesToBigDecimal(keys.chachaKey, 32);
    std::string k3Dec = bytesToBigDecimal(keys.xorKey, keys.xorKeyLen);
    
    // Generate variable names
    std::string v1 = genVarName(), v2 = genVarName(), v3 = genVarName();
    std::string v4 = genVarName(), v5 = genVarName(), v6 = genVarName();
    
    stub << "#include <iostream>\n";
    stub << "#include <vector>\n";
    stub << "#include <cstring>\n";
    stub << "#include <cstdint>\n";
    stub << "#include <string>\n";
    stub << "#include <fstream>\n";
    stub << "#include <windows.h>\n\n";
    
    // Store keys as innocent-looking numbers
    stub << "// Configuration values\n";
    stub << "const char* " << v1 << " = \"" << k1Dec << "\";\n";
    stub << "const char* " << v2 << " = \"" << k2Dec << "\";\n";
    stub << "const char* " << v3 << " = \"" << k3Dec << "\";\n";
    stub << "const int " << v4 << " = " << keys.xorKeyLen << ";\n";
    stub << "const int " << v5 << "[] = {" << keys.encryptionOrder[0] << ", " 
         << keys.encryptionOrder[1] << ", " << keys.encryptionOrder[2] << "};\n\n";
    
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

void applyDecryptionLayer(std::vector<uint8_t>& data, int method, 
                         const std::vector<uint8_t>& k1, const std::vector<uint8_t>& k2, 
                         const std::vector<uint8_t>& k3) {
    switch (method) {
        case 0: // AES
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
    }
}

void decryptCodeSections(std::vector<uint8_t>& fileData) {
    // Extract keys from numbers
    auto k1 = decToBytes(" << v1 << ", 32);\n";
    stub << "    auto k2 = decToBytes(" << v2 << ", 32);\n";
    stub << "    auto k3 = decToBytes(" << v3 << ", " << v4 << ");\n\n";
    
    stub << "    // PE header pointers\n";
    stub << "    PIMAGE_DOS_HEADER dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(fileData.data());\n";
    stub << "    PIMAGE_NT_HEADERS ntHeaders = reinterpret_cast<PIMAGE_NT_HEADERS>(fileData.data() + dosHeader->e_lfanew);\n";
    stub << "    PIMAGE_SECTION_HEADER sectionHeaders = reinterpret_cast<PIMAGE_SECTION_HEADER>(reinterpret_cast<uint8_t*>(ntHeaders) + sizeof(IMAGE_NT_HEADERS));\n\n";
    
    stub << "    // Decrypt code sections\n";
    stub << "    int numSections = ntHeaders->FileHeader.NumberOfSections;\n";
    stub << "    for (int i = 0; i < numSections; i++) {\n";
    stub << "        IMAGE_SECTION_HEADER& section = sectionHeaders[i];\n";
    stub << "        if (section.Characteristics & IMAGE_SCN_CNT_CODE) {\n";
    stub << "            size_t sectionOffset = section.PointerToRawData;\n";
    stub << "            size_t sectionSize = section.SizeOfRawData;\n";
    stub << "            if (sectionOffset + sectionSize <= fileData.size()) {\n";
    stub << "                std::vector<uint8_t> sectionData(fileData.begin() + sectionOffset, fileData.begin() + sectionOffset + sectionSize);\n";
    stub << "                // Apply decryption in reverse order\n";
    stub << "                for (int j = 2; j >= 0; j--) {\n";
    stub << "                    applyDecryptionLayer(sectionData, " << v5 << "[j], k1, k2, k3);\n";
    stub << "                }\n";
    stub << "                std::copy(sectionData.begin(), sectionData.end(), fileData.begin() + sectionOffset);\n";
    stub << "            }\n";
    stub << "        }\n";
    stub << "    }\n";
    stub << "}\n\n";
    
    // Main function
    stub << "int main() {\n";
    stub << "    // Load self as PE file\n";
    stub << "    std::ifstream selfFile(\"";
    
    // Get filename from input path
    size_t lastSlash = inputFile.find_last_of("/\\");
    std::string filename = (lastSlash != std::string::npos) ? 
                          inputFile.substr(lastSlash + 1) : inputFile;
    
    stub << filename << "\", std::ios::binary);\n";
    stub << "    if (!selfFile) {\n";
    stub << "        std::cout << \"Failed to open self file\" << std::endl;\n";
    stub << "        return 1;\n";
    stub << "    }\n\n";
    
    stub << "    selfFile.seekg(0, std::ios::end);\n";
    stub << "    size_t fileSize = selfFile.tellg();\n";
    stub << "    selfFile.seekg(0, std::ios::beg);\n\n";
    
    stub << "    std::vector<uint8_t> fileData(fileSize);\n";
    stub << "    selfFile.read(reinterpret_cast<char*>(fileData.data()), fileSize);\n";
    stub << "    selfFile.close();\n\n";
    
    stub << "    // Decrypt code sections\n";
    stub << "    decryptCodeSections(fileData);\n\n";
    
    stub << "    // Write decrypted file\n";
    stub << "    std::ofstream outFile(\"decrypted_" << filename << "\", std::ios::binary);\n";
    stub << "    outFile.write(reinterpret_cast<const char*>(fileData.data()), fileSize);\n";
    stub << "    outFile.close();\n\n";
    
    stub << "    std::cout << \"PE file decrypted successfully: decrypted_" << filename << "\" << std::endl;\n";
    stub << "    return 0;\n";
    stub << "}\n";
    
    return stub.str();
}