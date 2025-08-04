#include "encryptor.h"
#include "stealth_triple_encryptor.h"
#include "pe_encryptor.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdint>
#include <ctime>
#include <cstdlib>

// AES S-box
const uint8_t sbox[256] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

// Round constants for key expansion
const uint8_t rcon[10] = {
    0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36
};

// Simple random number generator
uint32_t simpleRand() {
    static uint32_t seed = std::time(nullptr) + std::clock();
    seed = seed * 1103515245 + 12345;
    return seed;
}

// Random key generation
void generateRandomKey(uint8_t* key) {
    for (int i = 0; i < 16; i++) {
        key[i] = simpleRand() & 0xFF;
    }
}

void generateChaCha20Key(uint8_t* key) {
    for (int i = 0; i < 32; i++) {
        key[i] = simpleRand() & 0xFF;
    }
}

void generateNonce(uint8_t* nonce) {
    for (int i = 0; i < 16; i++) {
        nonce[i] = simpleRand() & 0xFF;
    }
}

void generateChaCha20Nonce(uint8_t* nonce) {
    for (int i = 0; i < 12; i++) {
        nonce[i] = simpleRand() & 0xFF;
    }
}

// AES helper functions
uint8_t gmul(uint8_t a, uint8_t b) {
    uint8_t p = 0;
    for (int i = 0; i < 8; i++) {
        if (b & 1) p ^= a;
        bool hi_bit_set = a & 0x80;
        a <<= 1;
        if (hi_bit_set) a ^= 0x1b;
        b >>= 1;
    }
    return p;
}

void subBytes(uint8_t* state) {
    for (int i = 0; i < 16; i++) {
        state[i] = sbox[state[i]];
    }
}

void shiftRows(uint8_t* state) {
    uint8_t temp;
    
    // Row 1: shift left by 1
    temp = state[1];
    state[1] = state[5];
    state[5] = state[9];
    state[9] = state[13];
    state[13] = temp;
    
    // Row 2: shift left by 2
    temp = state[2];
    state[2] = state[10];
    state[10] = temp;
    temp = state[6];
    state[6] = state[14];
    state[14] = temp;
    
    // Row 3: shift left by 3 (equivalent to shift right by 1)
    temp = state[15];
    state[15] = state[11];
    state[11] = state[7];
    state[7] = state[3];
    state[3] = temp;
}

void mixColumns(uint8_t* state) {
    for (int i = 0; i < 4; i++) {
        uint8_t s0 = state[i*4], s1 = state[i*4+1], s2 = state[i*4+2], s3 = state[i*4+3];
        state[i*4] = gmul(0x02, s0) ^ gmul(0x03, s1) ^ s2 ^ s3;
        state[i*4+1] = s0 ^ gmul(0x02, s1) ^ gmul(0x03, s2) ^ s3;
        state[i*4+2] = s0 ^ s1 ^ gmul(0x02, s2) ^ gmul(0x03, s3);
        state[i*4+3] = gmul(0x03, s0) ^ s1 ^ s2 ^ gmul(0x02, s3);
    }
}

void addRoundKey(uint8_t* state, const uint8_t* roundKey) {
    for (int i = 0; i < 16; i++) {
        state[i] ^= roundKey[i];
    }
}

void keyExpansion(const uint8_t* key, uint8_t* roundKeys) {
    uint8_t temp[4];
    
    // Copy the original key
    for (int i = 0; i < 16; i++) {
        roundKeys[i] = key[i];
    }
    
    // Generate the remaining round keys
    for (int i = 4; i < 44; i++) {
        for (int j = 0; j < 4; j++) {
            temp[j] = roundKeys[(i-1)*4 + j];
        }
        
        if (i % 4 == 0) {
            // RotWord
            uint8_t t = temp[0];
            temp[0] = temp[1];
            temp[1] = temp[2];
            temp[2] = temp[3];
            temp[3] = t;
            
            // SubWord
            for (int j = 0; j < 4; j++) {
                temp[j] = sbox[temp[j]];
            }
            
            // XOR with Rcon
            temp[0] ^= rcon[i/4 - 1];
        }
        
        for (int j = 0; j < 4; j++) {
            roundKeys[i*4 + j] = roundKeys[(i-4)*4 + j] ^ temp[j];
        }
    }
}

void aesEncryptBlock(const uint8_t* input, uint8_t* output, const uint8_t* roundKeys) {
    uint8_t state[16];
    
    // Copy input to state
    for (int i = 0; i < 16; i++) {
        state[i] = input[i];
    }
    
    // Initial round key addition
    addRoundKey(state, roundKeys);
    
    // 9 rounds
    for (int round = 1; round < 10; round++) {
        subBytes(state);
        shiftRows(state);
        mixColumns(state);
        addRoundKey(state, roundKeys + round * 16);
    }
    
    // Final round (no mixColumns)
    subBytes(state);
    shiftRows(state);
    addRoundKey(state, roundKeys + 160);
    
    // Copy state to output
    for (int i = 0; i < 16; i++) {
        output[i] = state[i];
    }
}

void incrementCounter(uint8_t* counter) {
    for (int i = 15; i >= 0; i--) {
        counter[i]++;
        if (counter[i] != 0) break;
    }
}

void aesCtrCrypt(const uint8_t* input, uint8_t* output, size_t length, 
                 const uint8_t* key, const uint8_t* nonce) {
    uint8_t roundKeys[176];
    uint8_t counter[16];
    uint8_t block[16];
    
    keyExpansion(key, roundKeys);
    
    // Initialize counter with nonce
    for (int i = 0; i < 16; i++) {
        counter[i] = nonce[i];
    }
    
    for (size_t i = 0; i < length; i += 16) {
        aesEncryptBlock(counter, block, roundKeys);
        
        size_t blockSize = (length - i < 16) ? length - i : 16;
        for (size_t j = 0; j < blockSize; j++) {
            output[i + j] = input[i + j] ^ block[j];
        }
        
        incrementCounter(counter);
    }
}

void quarterRound(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d) {
    a += b; d ^= a; d = (d << 16) | (d >> 16);
    c += d; b ^= c; b = (b << 12) | (b >> 20);
    a += b; d ^= a; d = (d << 8) | (d >> 24);
    c += d; b ^= c; b = (b << 7) | (b >> 25);
}

void chachaBlock(uint32_t output[16], const uint32_t input[16]) {
    uint32_t state[16];
    
    for (int i = 0; i < 16; i++) {
        state[i] = input[i];
    }
    
    for (int i = 0; i < 10; i++) {
        quarterRound(state[0], state[4], state[8], state[12]);
        quarterRound(state[1], state[5], state[9], state[13]);
        quarterRound(state[2], state[6], state[10], state[14]);
        quarterRound(state[3], state[7], state[11], state[15]);
        quarterRound(state[0], state[5], state[10], state[15]);
        quarterRound(state[1], state[6], state[11], state[12]);
        quarterRound(state[2], state[7], state[8], state[13]);
        quarterRound(state[3], state[4], state[9], state[14]);
    }
    
    for (int i = 0; i < 16; i++) {
        output[i] = state[i] + input[i];
    }
}

void initChachaState(uint32_t state[16], const uint8_t key[32], const uint8_t nonce[12], uint32_t counter) {
    // Constants
    state[0] = 0x61707865;
    state[1] = 0x3320646e;
    state[2] = 0x79622d32;
    state[3] = 0x6b206574;
    
    // Key
    for (int i = 0; i < 8; i++) {
        state[4 + i] = ((uint32_t)key[i*4]) | 
                       ((uint32_t)key[i*4 + 1] << 8) | 
                       ((uint32_t)key[i*4 + 2] << 16) | 
                       ((uint32_t)key[i*4 + 3] << 24);
    }
    
    // Counter
    state[12] = counter;
    
    // Nonce
    for (int i = 0; i < 3; i++) {
        state[13 + i] = ((uint32_t)nonce[i*4]) | 
                        ((uint32_t)nonce[i*4 + 1] << 8) | 
                        ((uint32_t)nonce[i*4 + 2] << 16) | 
                        ((uint32_t)nonce[i*4 + 3] << 24);
    }
}

void chacha20Crypt(const uint8_t* input, uint8_t* output, size_t length,
                   const uint8_t key[32], const uint8_t nonce[12], uint32_t counter) {
    uint32_t state[16];
    uint32_t block[16];
    uint8_t keyStream[64];
    
    for (size_t i = 0; i < length; i += 64) {
        initChachaState(state, key, nonce, counter);
        chachaBlock(block, state);
        
        for (int j = 0; j < 16; j++) {
            keyStream[j*4] = block[j] & 0xFF;
            keyStream[j*4 + 1] = (block[j] >> 8) & 0xFF;
            keyStream[j*4 + 2] = (block[j] >> 16) & 0xFF;
            keyStream[j*4 + 3] = (block[j] >> 24) & 0xFF;
        }
        
        size_t blockSize = (length - i < 64) ? length - i : 64;
        for (size_t j = 0; j < blockSize; j++) {
            output[i + j] = input[i + j] ^ keyStream[j];
        }
        
        counter++;
    }
}

bool basicEncrypt(const std::string& inputFile, const std::string& outputFile, const std::string& algorithm, bool rawOutput) {
    std::ifstream fin(inputFile, std::ios::binary);
    if (!fin) {
        std::cout << "Failed to open input file: " << inputFile << std::endl;
        return false;
    }
    
    std::ofstream fout(outputFile, std::ios::binary);
    if (!fout) {
        std::cout << "Failed to create output file: " << outputFile << std::endl;
        return false;
    }
    
    bool useChaCha = (algorithm == "chacha20");
    
    if (useChaCha) {
        uint8_t key[32], nonce[12];
        generateChaCha20Key(key);
        generateChaCha20Nonce(nonce);
        
        if (!rawOutput) {
            // Write algorithm identifier
            fout.put(0x02); // ChaCha20
            
            // Write nonce
            fout.write(reinterpret_cast<const char*>(nonce), 12);
        }
        
        const size_t bufferSize = 4096;
        std::vector<uint8_t> inputBuffer(bufferSize);
        std::vector<uint8_t> outputBuffer(bufferSize);
        
        uint32_t counter = 0;
        while (fin) {
            fin.read(reinterpret_cast<char*>(inputBuffer.data()), bufferSize);
            std::streamsize bytesRead = fin.gcount();
            
            if (bytesRead > 0) {
                chacha20Crypt(inputBuffer.data(), outputBuffer.data(), bytesRead, key, nonce, counter);
                fout.write(reinterpret_cast<const char*>(outputBuffer.data()), bytesRead);
                counter += (bytesRead + 63) / 64; // Increment counter for each block
            }
        }
    } else {
        uint8_t key[16], nonce[16];
        generateRandomKey(key);
        generateNonce(nonce);
        
        if (!rawOutput) {
            // Write algorithm identifier
            fout.put(0x01); // AES
            
            // Write nonce
            fout.write(reinterpret_cast<const char*>(nonce), 16);
        }
        
        const size_t bufferSize = 4096;
        std::vector<uint8_t> inputBuffer(bufferSize);
        std::vector<uint8_t> outputBuffer(bufferSize);
        
        while (fin) {
            fin.read(reinterpret_cast<char*>(inputBuffer.data()), bufferSize);
            std::streamsize bytesRead = fin.gcount();
            
            if (bytesRead > 0) {
                aesCtrCrypt(inputBuffer.data(), outputBuffer.data(), bytesRead, key, nonce);
                fout.write(reinterpret_cast<const char*>(outputBuffer.data()), bytesRead);
            }
        }
    }
    
    fin.close();
    fout.close();
    
    return true;
}

void showMenu() {
    std::cout << "\n=== Visual Studio 2022 Universal PE Packer ===" << std::endl;
    std::cout << "Professional-grade PE packer with self-decrypting executables\n" << std::endl;
    std::cout << "Select an option:" << std::endl;
    std::cout << "  1. Basic Encryption (AES/ChaCha20)" << std::endl;
    std::cout << "  2. Basic Encryption - Raw Binary Output" << std::endl;
    std::cout << "  3. Stealth Triple Encryption" << std::endl;
    std::cout << "  4. PE-Aware Encryption (Preserves Executable Structure)" << std::endl;
    std::cout << "  0. Exit" << std::endl;
    std::cout << "\nEnter your choice: ";
}

std::string getInputFile() {
    std::string filename;
    std::cout << "Enter input file path: ";
    std::getline(std::cin, filename);
    return filename;
}

std::string getOutputFile() {
    std::string filename;
    std::cout << "Enter output file path (e.g., encrypted_file.bin or encrypted_file.exe): ";
    std::getline(std::cin, filename);
    return filename;
}

std::string selectAlgorithm() {
    std::cout << "Select algorithm:" << std::endl;
    std::cout << "  1. AES-128-CTR (default)" << std::endl;
    std::cout << "  2. ChaCha20" << std::endl;
    std::cout << "Enter choice (1-2): ";
    
    std::string choice;
    std::getline(std::cin, choice);
    
    if (choice == "2") return "chacha20";
    return "aes";
}

int main() {
    while (true) {
        showMenu();
        
        std::string choice;
        std::getline(std::cin, choice);
        
        if (choice == "0") {
            std::cout << "Goodbye!" << std::endl;
            break;
        }
        else if (choice == "1") {
            // Basic Encryption
            std::string inputFile = getInputFile();
            std::string outputFile = getOutputFile();
            std::string algorithm = selectAlgorithm();
            
            if (!basicEncrypt(inputFile, outputFile, algorithm, false)) {
                std::cout << "Encryption failed!" << std::endl;
            } else {
                std::cout << "Encryption completed successfully!" << std::endl;
            }
        }
        else if (choice == "2") {
            // Basic Encryption - Raw Binary Output
            std::string inputFile = getInputFile();
            std::string outputFile = getOutputFile();
            std::string algorithm = selectAlgorithm();
            
            if (!basicEncrypt(inputFile, outputFile, algorithm, true)) {
                std::cout << "Encryption failed!" << std::endl;
            } else {
                std::cout << "Raw binary encryption completed successfully!" << std::endl;
            }
        }
        else if (choice == "3") {
            // Stealth Triple Encryption
            std::string inputFile = getInputFile();
            std::string outputFile = getOutputFile();
            
            StealthTripleEncryption ste;
            if (ste.encryptFile(inputFile, outputFile)) {
                std::cout << "File encrypted with triple layer encryption" << std::endl;
                std::cout << "Keys saved as decimal numbers in: " << outputFile << ".keys" << std::endl;
            } else {
                std::cout << "Stealth encryption failed!" << std::endl;
            }
        }
        else if (choice == "4") {
            // PE-Aware Encryption
            std::string inputFile = getInputFile();
            std::string outputFile = getOutputFile();
            
            PEEncryptor peEncryptor;
            if (peEncryptor.encryptPE(inputFile, outputFile)) {
                std::cout << "PE file encrypted successfully!" << std::endl;
                std::cout << "File remains executable but code sections are encrypted" << std::endl;
                std::cout << "Keys embedded directly in file - self-contained!" << std::endl;
                std::cout << "File can decrypt itself without external key files" << std::endl;
            } else {
                std::cout << "PE encryption failed!" << std::endl;
            }
        }
        else {
            std::cout << "Invalid choice. Please try again." << std::endl;
        }
        
        std::cout << "\nPress Enter to continue...";
        std::cin.get();
    }
    
    return 0;
}