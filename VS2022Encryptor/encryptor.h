#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <ctime>

// Pure native AES-128 implementation with no external dependencies

// AES S-box
extern const uint8_t sbox[256];

// AES inverse S-box
extern const uint8_t inv_sbox[256];

// Round constants for key expansion
extern const uint8_t rcon[10];

// Simple random number generator (for key and nonce generation)
uint32_t simpleRand();

// Random key generation
void generateRandomKey(uint8_t* key);

// Fallback key handling (shared with other components)
constexpr uint8_t XOR_OBFUSCATE_KEY = 0x5A;
extern uint8_t encKey[];
constexpr size_t keyLen = sizeof(encKey);

void decryptKey(uint8_t* keyBuf);

// AES helper functions
uint8_t gmul(uint8_t a, uint8_t b);
void subBytes(uint8_t* state);
void invSubBytes(uint8_t* state);
void shiftRows(uint8_t* state);
void invShiftRows(uint8_t* state);
void mixColumns(uint8_t* state);
void invMixColumns(uint8_t* state);
void addRoundKey(uint8_t* state, const uint8_t* roundKey);
void keyExpansion(const uint8_t* key, uint8_t* roundKeys);
void aesEncryptBlock(const uint8_t* input, uint8_t* output, const uint8_t* roundKeys);
void incrementCounter(uint8_t* counter);
void aesCtrCrypt(const uint8_t* input, uint8_t* output, size_t length, 
                 const uint8_t* key, const uint8_t* nonce);
void generateNonce(uint8_t* nonce);
void generateChaCha20Nonce(uint8_t* nonce);
void quarterRound(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d);
void chachaBlock(uint32_t output[16], const uint32_t input[16]);
void initChachaState(uint32_t state[16], const uint8_t key[32], const uint8_t nonce[12], uint32_t counter);
void chacha20Crypt(const uint8_t* input, uint8_t* output, size_t length,
                   const uint8_t key[32], const uint8_t nonce[12], uint32_t counter);
void generateChaCha20Key(uint8_t* key);