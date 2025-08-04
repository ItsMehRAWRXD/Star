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
#ifdef __linux__
#include <sys/mman.h>
#endif
#ifdef _WIN32
#include <windows.h>
#endif

class StealthTripleEncryption {
private:
    // Dynamic entropy mixing
    struct DynamicEntropy {
        uint64_t timeComponent;
        uint64_t memoryComponent;
        uint64_t threadComponent;
        uint64_t counterComponent;
        
        uint64_t mix();
    };
    
    DynamicEntropy entropy;
    std::mt19937_64 rng;
    
    // Convert bytes to big decimal string (single large number)
    std::string bytesToBigDecimal(const uint8_t* bytes, size_t length);
    
    // Convert decimal string back to bytes
    std::vector<uint8_t> decimalToBytes(const std::string& decimal, size_t expectedLength);
    
    // String arithmetic helpers
    std::string addStrings(const std::string& num1, const std::string& num2);
    std::string multiplyStrings(const std::string& num1, const std::string& num2);
    
    // Generate obfuscated variable name
    std::string genVarName();
    
    // XOR implementation
    void xorData(uint8_t* data, size_t len, const uint8_t* key, size_t keyLen);
    
    // Simplified ChaCha20 round function
    void chacha20QuarterRound(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d);
    
    // Simplified AES round
    void aesSubBytes(uint8_t state[16]);
    
public:
    StealthTripleEncryption();
    
    struct TripleKey {
        uint8_t aesKey[16];
        uint8_t aesNonce[16];
        uint8_t chachaKey[32];
        uint8_t chachaNonce[12];
        uint8_t xorKey[32];
        size_t xorKeyLen;
        int encryptionOrder[3]; // 0=AES, 1=ChaCha20, 2=XOR
    };
    
    // Generate keys with maximum entropy
    TripleKey generateKeys();
    
    // Apply encryption layer
    void applyEncryptionLayer(std::vector<uint8_t>& data, int method, const TripleKey& keys);
    
    // Generate stealth stub that stores keys as decimal numbers
    std::string generateStealthStub(const std::vector<uint8_t>& payload, const TripleKey& keys);
    
    // Encrypt file with triple layer
    bool encryptFile(const std::string& inputFile, const std::string& outputFile);
};