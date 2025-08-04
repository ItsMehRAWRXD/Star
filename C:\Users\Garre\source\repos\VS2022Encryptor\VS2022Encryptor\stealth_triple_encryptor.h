#pragma once
#include <vector>
#include <string>
#include <random>
#include <chrono>

class StealthTripleEncryption {
private:
    std::mt19937 rng;
    
    // Key structure
    struct StealthKeys {
        uint8_t aesKey[16];
        uint8_t aesNonce[16];
        uint8_t chachaKey[32];
        uint8_t chachaNonce[12];
        uint8_t xorKey[32];
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
    
    // Core functions
    StealthKeys generateKeys();
    void applyEncryptionLayer(std::vector<uint8_t>& data, int method, const StealthKeys& keys);
    std::string generateStealthStub(const std::vector<uint8_t>& payload, const StealthKeys& keys);
    
public:
    StealthTripleEncryption();
    bool encryptFile(const std::string& inputFile, const std::string& outputFile);
    std::string generateStub(const std::string& inputFile);
};