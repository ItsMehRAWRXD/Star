#pragma once
#include <vector>
#include <string>
#include <random>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <windows.h>

class SelfContainedPEEncryptor {
private:
    std::mt19937 rng;
    
    // PE structures
    struct PEInfo {
        PIMAGE_DOS_HEADER dosHeader;
        PIMAGE_NT_HEADERS ntHeaders;
        PIMAGE_SECTION_HEADER sectionHeaders;
        std::vector<uint8_t> fileData;
        size_t fileSize;
        bool is64Bit;
    };
    
    // Encryption keys
    struct PEKeys {
        uint8_t aesKey[32];
        uint8_t chachaKey[32];
        uint8_t xorKey[64];
        size_t xorKeyLen;
        int encryptionOrder[3];
    };
    
    // Helper functions
    std::string genVarName();
    std::string bytesToBigDecimal(const uint8_t* bytes, size_t length);
    std::string addStrings(const std::string& num1, const std::string& num2);
    std::string multiplyStrings(const std::string& num1, const std::string& num2);
    void xorData(uint8_t* data, size_t len, const uint8_t* key, size_t keyLen);
    void chacha20QuarterRound(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d);
    void aesSubBytes(uint8_t state[16]);
    
    // PE functions
    bool loadPE(const std::string& filename, PEInfo& peInfo);
    bool isCodeSection(const IMAGE_SECTION_HEADER& section);
    void encryptCodeSection(std::vector<uint8_t>& data, const PEKeys& keys);
    PEKeys generateKeys();
    void applyEncryptionLayer(std::vector<uint8_t>& data, int method, const PEKeys& keys);
    std::string generateSelfContainedStub(const PEInfo& peInfo, const PEKeys& keys);
    
public:
    SelfContainedPEEncryptor();
    bool encryptPE(const std::string& inputFile, const std::string& outputFile);
    std::string generateStub(const std::string& inputFile);
};