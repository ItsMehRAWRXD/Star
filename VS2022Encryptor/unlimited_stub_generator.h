#pragma once
#include <vector>
#include <string>
#include <random>
#include <chrono>
#include <sstream>
#include <iomanip>

class UnlimitedStubGenerator {
private:
    std::mt19937 rng;
    
    // Key structure for unlimited encryption
    struct UnlimitedKeys {
        uint8_t aesKey[32];        // Extended AES key
        uint8_t aesNonce[16];
        uint8_t chachaKey[32];
        uint8_t chachaNonce[12];
        uint8_t xorKey[64];        // Extended XOR key
        size_t xorKeyLen;
        int encryptionOrder[4];    // 4-layer encryption
        uint8_t additionalLayer[32]; // Extra encryption layer
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
    UnlimitedKeys generateKeys();
    void applyEncryptionLayer(std::vector<uint8_t>& data, int method, const UnlimitedKeys& keys);
    std::string generateUnlimitedStub(const UnlimitedKeys& keys);
    
public:
    UnlimitedStubGenerator();
    std::string generateStub();
};