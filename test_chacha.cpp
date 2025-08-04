#include <iostream>
#include <cstring>
#include <cstdint>
#include <vector>
#include <iomanip>

// ChaCha20 implementation
inline void quarterRound(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d) {
    a += b; d ^= a; d = (d << 16) | (d >> 16);
    c += d; b ^= c; b = (b << 12) | (b >> 20);
    a += b; d ^= a; d = (d << 8) | (d >> 24);
    c += d; b ^= c; b = (b << 7) | (b >> 25);
}

void chachaBlock(uint32_t output[16], const uint32_t input[16]) {
    uint32_t x[16];
    for (int i = 0; i < 16; i++) {
        x[i] = input[i];
    }
    
    for (int i = 0; i < 10; i++) {
        quarterRound(x[0], x[4], x[8], x[12]);
        quarterRound(x[1], x[5], x[9], x[13]);
        quarterRound(x[2], x[6], x[10], x[14]);
        quarterRound(x[3], x[7], x[11], x[15]);
        
        quarterRound(x[0], x[5], x[10], x[15]);
        quarterRound(x[1], x[6], x[11], x[12]);
        quarterRound(x[2], x[7], x[8], x[13]);
        quarterRound(x[3], x[4], x[9], x[14]);
    }
    
    for (int i = 0; i < 16; i++) {
        output[i] = x[i] + input[i];
    }
}

void initChachaState(uint32_t state[16], const uint8_t key[32], const uint8_t nonce[12], uint32_t counter) {
    state[0] = 0x61707865;
    state[1] = 0x3320646e;
    state[2] = 0x79622d32;
    state[3] = 0x6b206574;
    
    for (int i = 0; i < 8; i++) {
        state[4 + i] = ((uint32_t)key[4*i]) |
                       ((uint32_t)key[4*i + 1] << 8) |
                       ((uint32_t)key[4*i + 2] << 16) |
                       ((uint32_t)key[4*i + 3] << 24);
    }
    
    state[12] = counter;
    
    for (int i = 0; i < 3; i++) {
        state[13 + i] = ((uint32_t)nonce[4*i]) |
                        ((uint32_t)nonce[4*i + 1] << 8) |
                        ((uint32_t)nonce[4*i + 2] << 16) |
                        ((uint32_t)nonce[4*i + 3] << 24);
    }
}

void chacha20Crypt(const uint8_t* input, uint8_t* output, size_t length,
                   const uint8_t key[32], const uint8_t nonce[12], uint32_t counter) {
    uint32_t state[16];
    uint32_t keystream[16];
    uint8_t* keystream_bytes = (uint8_t*)keystream;
    
    size_t processed = 0;
    while (processed < length) {
        initChachaState(state, key, nonce, counter);
        chachaBlock(keystream, state);
        
        size_t blockSize = (length - processed < 64) ? length - processed : 64;
        for (size_t i = 0; i < blockSize; i++) {
            output[processed + i] = input[processed + i] ^ keystream_bytes[i];
        }
        
        processed += blockSize;
        counter++;
    }
}

void printHex(const char* label, const uint8_t* data, size_t len) {
    std::cout << label << ": ";
    for (size_t i = 0; i < len; i++) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)data[i];
    }
    std::cout << std::dec << std::endl;
}

int main() {
    std::cout << "=== ChaCha20 Test ===" << std::endl;
    
    // Test key and nonce
    uint8_t key[32] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
    };
    
    uint8_t nonce[12] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00
    };
    
    // Test plaintext
    const char* plaintext = "Hello, ChaCha20! This is a test message.";
    size_t len = strlen(plaintext);
    
    std::cout << "Plaintext: " << plaintext << std::endl;
    printHex("Key", key, 32);
    printHex("Nonce", nonce, 12);
    
    // Encrypt
    std::vector<uint8_t> ciphertext(len);
    chacha20Crypt((const uint8_t*)plaintext, ciphertext.data(), len, key, nonce, 0);
    
    std::cout << "\nEncrypted:" << std::endl;
    printHex("Ciphertext", ciphertext.data(), len);
    
    // Decrypt
    std::vector<uint8_t> decrypted(len);
    chacha20Crypt(ciphertext.data(), decrypted.data(), len, key, nonce, 0);
    
    std::cout << "\nDecrypted: ";
    for (size_t i = 0; i < len; i++) {
        std::cout << (char)decrypted[i];
    }
    std::cout << std::endl;
    
    // Verify
    if (memcmp(plaintext, decrypted.data(), len) == 0) {
        std::cout << "\n✓ Success: Decryption matches original plaintext!" << std::endl;
    } else {
        std::cout << "\n✗ Error: Decryption does not match!" << std::endl;
    }
    
    return 0;
}