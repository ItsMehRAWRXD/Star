#pragma once
#include <vector>
#include <string>
#include <cstdint>

// Basic encryption functions
bool basicEncrypt(const std::string& inputFile, const std::string& outputFile, const std::string& algorithm, bool rawOutput = false);

// AES-128-CTR encryption
void aesCtrCrypt(const uint8_t* input, uint8_t* output, size_t length, 
                 const uint8_t* key, const uint8_t* nonce);

// ChaCha20 encryption
void chacha20Crypt(const uint8_t* input, uint8_t* output, size_t length,
                   const uint8_t key[32], const uint8_t nonce[12], uint32_t counter);

// Key generation
void generateRandomKey(uint8_t* key);
void generateChaCha20Key(uint8_t* key);
void generateNonce(uint8_t* nonce);
void generateChaCha20Nonce(uint8_t* nonce);