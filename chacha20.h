#ifndef CHACHA20_H
#define CHACHA20_H

#include <cstdint>
#include <cstring>

// ChaCha20 implementation
class ChaCha20 {
private:
    uint32_t state[16];
    
    static inline uint32_t rotl32(uint32_t x, int n) {
        return (x << n) | (x >> (32 - n));
    }
    
    static inline void quarterRound(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d) {
        a += b; d ^= a; d = rotl32(d, 16);
        c += d; b ^= c; b = rotl32(b, 12);
        a += b; d ^= a; d = rotl32(d, 8);
        c += d; b ^= c; b = rotl32(b, 7);
    }
    
    void chacha20Block(uint32_t* output) {
        uint32_t working[16];
        memcpy(working, state, sizeof(working));
        
        for (int i = 0; i < 10; i++) {
            quarterRound(working[0], working[4], working[8], working[12]);
            quarterRound(working[1], working[5], working[9], working[13]);
            quarterRound(working[2], working[6], working[10], working[14]);
            quarterRound(working[3], working[7], working[11], working[15]);
            quarterRound(working[0], working[5], working[10], working[15]);
            quarterRound(working[1], working[6], working[11], working[12]);
            quarterRound(working[2], working[7], working[8], working[13]);
            quarterRound(working[3], working[4], working[9], working[14]);
        }
        
        for (int i = 0; i < 16; i++) {
            output[i] = working[i] + state[i];
        }
    }

public:
    // Initialize with key (32 bytes) and nonce (12 bytes)
    void init(const uint8_t* key, const uint8_t* nonce) {
        // ChaCha20 constants
        state[0] = 0x61707865; // "expa"
        state[1] = 0x3320646e; // "nd 3"
        state[2] = 0x79622d32; // "2-by"
        state[3] = 0x6b206574; // "te k"
        
        // Key (8 words)
        for (int i = 0; i < 8; i++) {
            state[4 + i] = ((uint32_t)key[4*i+3] << 24) | 
                          ((uint32_t)key[4*i+2] << 16) | 
                          ((uint32_t)key[4*i+1] << 8) | 
                          key[4*i];
        }
        
        // Counter (starts at 0)
        state[12] = 0;
        state[13] = 0;
        
        // Nonce (3 words)
        for (int i = 0; i < 3; i++) {
            state[13 + i] = ((uint32_t)nonce[4*i+3] << 24) | 
                           ((uint32_t)nonce[4*i+2] << 16) | 
                           ((uint32_t)nonce[4*i+1] << 8) | 
                           nonce[4*i];
        }
    }
    
    // Encrypt/decrypt data in place
    void crypt(uint8_t* data, size_t length) {
        uint32_t block[16];
        uint8_t keystream[64];
        
        for (size_t i = 0; i < length; i += 64) {
            chacha20Block(block);
            
            // Convert block to bytes
            for (int j = 0; j < 16; j++) {
                keystream[j*4] = block[j] & 0xFF;
                keystream[j*4+1] = (block[j] >> 8) & 0xFF;
                keystream[j*4+2] = (block[j] >> 16) & 0xFF;
                keystream[j*4+3] = (block[j] >> 24) & 0xFF;
            }
            
            // XOR with data
            size_t remaining = length - i;
            size_t toProcess = (remaining < 64) ? remaining : 64;
            for (size_t j = 0; j < toProcess; j++) {
                data[i + j] ^= keystream[j];
            }
            
            // Increment counter
            state[12]++;
            if (state[12] == 0) {
                state[13]++;
            }
        }
    }
    
    // Set counter value
    void setCounter(uint64_t counter) {
        state[12] = counter & 0xFFFFFFFF;
        state[13] = (counter >> 32) & 0xFFFFFFFF;
    }
};

// Helper functions for ChaCha20-CTR mode
inline void chacha20CtrCrypt(const uint8_t* input, uint8_t* output, size_t length,
                           const uint8_t* key, const uint8_t* nonce, uint64_t counter = 0) {
    ChaCha20 chacha;
    chacha.init(key, nonce);
    chacha.setCounter(counter);
    
    // Copy input to output first
    memcpy(output, input, length);
    
    // Then encrypt in place
    chacha.crypt(output, length);
}

// Generate random ChaCha20 key (32 bytes)
inline void generateChaChaKey(uint8_t* key) {
    static uint32_t seed = std::time(nullptr) + std::clock();
    for (int i = 0; i < 32; i++) {
        seed = seed * 1103515245 + 12345;
        key[i] = seed & 0xFF;
    }
}

// Generate random ChaCha20 nonce (12 bytes)
inline void generateChaChaNonce(uint8_t* nonce) {
    static uint32_t seed = std::time(nullptr) + std::clock() + 12345;
    for (int i = 0; i < 12; i++) {
        seed = seed * 1103515245 + 12345;
        nonce[i] = seed & 0xFF;
    }
}

#endif // CHACHA20_H