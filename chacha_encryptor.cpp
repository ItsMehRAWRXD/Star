#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <ctime>

// Pure native ChaCha20 implementation with no external dependencies

// ChaCha20 quarter round
inline void quarterRound(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d) {
    a += b; d ^= a; d = (d << 16) | (d >> 16);
    c += d; b ^= c; b = (b << 12) | (b >> 20);
    a += b; d ^= a; d = (d << 8) | (d >> 24);
    c += d; b ^= c; b = (b << 7) | (b >> 25);
}

// ChaCha20 block function
void chachaBlock(uint32_t output[16], const uint32_t input[16]) {
    uint32_t x[16];
    for (int i = 0; i < 16; i++) {
        x[i] = input[i];
    }
    
    // 20 rounds (10 double-rounds)
    for (int i = 0; i < 10; i++) {
        // Column rounds
        quarterRound(x[0], x[4], x[8], x[12]);
        quarterRound(x[1], x[5], x[9], x[13]);
        quarterRound(x[2], x[6], x[10], x[14]);
        quarterRound(x[3], x[7], x[11], x[15]);
        
        // Diagonal rounds
        quarterRound(x[0], x[5], x[10], x[15]);
        quarterRound(x[1], x[6], x[11], x[12]);
        quarterRound(x[2], x[7], x[8], x[13]);
        quarterRound(x[3], x[4], x[9], x[14]);
    }
    
    // Add original input
    for (int i = 0; i < 16; i++) {
        output[i] = x[i] + input[i];
    }
}

// Initialize ChaCha20 state
void initChachaState(uint32_t state[16], const uint8_t key[32], const uint8_t nonce[12], uint32_t counter) {
    // Constants "expand 32-byte k"
    state[0] = 0x61707865;
    state[1] = 0x3320646e;
    state[2] = 0x79622d32;
    state[3] = 0x6b206574;
    
    // Key
    for (int i = 0; i < 8; i++) {
        state[4 + i] = ((uint32_t)key[4*i]) |
                       ((uint32_t)key[4*i + 1] << 8) |
                       ((uint32_t)key[4*i + 2] << 16) |
                       ((uint32_t)key[4*i + 3] << 24);
    }
    
    // Counter
    state[12] = counter;
    
    // Nonce
    for (int i = 0; i < 3; i++) {
        state[13 + i] = ((uint32_t)nonce[4*i]) |
                        ((uint32_t)nonce[4*i + 1] << 8) |
                        ((uint32_t)nonce[4*i + 2] << 16) |
                        ((uint32_t)nonce[4*i + 3] << 24);
    }
}

// ChaCha20 encryption/decryption
void chacha20Crypt(const uint8_t* input, uint8_t* output, size_t length,
                   const uint8_t key[32], const uint8_t nonce[12], uint32_t counter) {
    uint32_t state[16];
    uint32_t keystream[16];
    uint8_t* keystream_bytes = (uint8_t*)keystream;
    
    size_t processed = 0;
    while (processed < length) {
        // Initialize state for this block
        initChachaState(state, key, nonce, counter);
        
        // Generate keystream block
        chachaBlock(keystream, state);
        
        // XOR with input
        size_t blockSize = (length - processed < 64) ? length - processed : 64;
        for (size_t i = 0; i < blockSize; i++) {
            output[processed + i] = input[processed + i] ^ keystream_bytes[i];
        }
        
        processed += blockSize;
        counter++;
    }
}

// Simple pseudo-random number generator
static uint32_t simpleSeed = 0;
inline uint32_t simpleRand() {
    simpleSeed = (simpleSeed * 1103515245 + 12345) & 0x7fffffff;
    return simpleSeed;
}

// Generate random key
inline void generateRandomKey(uint8_t* key) {
    simpleSeed = static_cast<uint32_t>(time(nullptr));
    for (int i = 0; i < 32; i++) {
        key[i] = simpleRand() & 0xFF;
    }
}

// Generate random nonce
inline void generateNonce(uint8_t* nonce) {
    for (int i = 0; i < 12; i++) {
        nonce[i] = simpleRand() & 0xFF;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "=== Universal ChaCha20 Encryptor ===" << std::endl;
        std::cout << "Usage: " << argv[0] << " <inputfile> <outputfile>" << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  " << argv[0] << " file.exe encrypted_file.bin" << std::endl;
        std::cout << "  " << argv[0] << " mirc_bot.cpp bot_encrypted.bin" << std::endl;
        std::cout << "  " << argv[0] << " document.pdf doc_encrypted.bin" << std::endl;
        std::cout << "Encrypts any file with random ChaCha20 keys." << std::endl;
        return 1;
    }

    // Generate random key and nonce
    uint8_t chachaKey[32];
    uint8_t nonce[12];
    
    generateRandomKey(chachaKey);
    generateNonce(nonce);
    
    std::cout << "Generated random key and nonce for ChaCha20 encryption." << std::endl;
    
    // Print key and nonce in hex for reference
    std::cout << "Key: ";
    for (int i = 0; i < 32; i++) {
        printf("%02x", chachaKey[i]);
    }
    std::cout << std::endl;
    
    std::cout << "Nonce: ";
    for (int i = 0; i < 12; i++) {
        printf("%02x", nonce[i]);
    }
    std::cout << std::endl;

    std::ifstream fin(argv[1], std::ios::binary);
    if (!fin) {
        std::cerr << "Failed to open input file.\n";
        return 1;
    }

    std::ofstream fout(argv[2], std::ios::binary);
    if (!fout) {
        std::cerr << "Failed to open output file.\n";
        return 1;
    }
    
    // Write nonce to output file
    fout.write(reinterpret_cast<char*>(nonce), 12);

    // Process file in chunks
    std::vector<uint8_t> buffer(4096);
    uint32_t counter = 0;
    
    while (fin.read(reinterpret_cast<char*>(buffer.data()), buffer.size()) || fin.gcount() > 0) {
        std::streamsize bytesRead = fin.gcount();
        std::vector<uint8_t> outputBuffer(bytesRead);
        
        chacha20Crypt(buffer.data(), outputBuffer.data(), bytesRead, chachaKey, nonce, counter);
        fout.write(reinterpret_cast<char*>(outputBuffer.data()), bytesRead);
        
        // Update counter based on blocks processed
        counter += (bytesRead + 63) / 64;
    }

    std::cout << "File encrypted successfully with ChaCha20." << std::endl;
    return 0;
}