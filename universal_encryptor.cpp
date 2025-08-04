#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <ctime>
#include <random>
#include <chrono>
#include <iomanip>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#include <shellapi.h>
#endif

class UniversalEncryptor {
private:
    std::mt19937 rng;
    
    // Enhanced RNG initialization with multiple entropy sources
    void initializeRNG() {
        std::random_device rd;
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = now.time_since_epoch();
        auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
        
        std::seed_seq seed{
            rd(), rd(), rd(), rd(),
            static_cast<unsigned int>(std::time(nullptr)),
            static_cast<unsigned int>(std::clock()),
            static_cast<unsigned int>(millis),
            static_cast<unsigned int>(millis >> 32)
        };
        
        rng.seed(seed);
    }
    
    // Reseed RNG for maximum uniqueness
    void reseedRNG() {
        initializeRNG();
    }
    
    // AES-128 S-box
    static const uint8_t sbox[256];
    static const uint8_t rcon[11];
    
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
    
    void subBytes(uint8_t state[16]) {
        for (int i = 0; i < 16; i++) {
            state[i] = sbox[state[i]];
        }
    }
    
    void shiftRows(uint8_t state[16]) {
        uint8_t temp;
        // Row 1: shift left by 1
        temp = state[1];
        state[1] = state[5];
        state[5] = state[9];
        state[9] = state[13];
        state[13] = temp;
        
        // Row 2: shift left by 2
        temp = state[2]; state[2] = state[10]; state[10] = temp;
        temp = state[6]; state[6] = state[14]; state[14] = temp;
        
        // Row 3: shift left by 3
        temp = state[3];
        state[3] = state[15];
        state[15] = state[11];
        state[11] = state[7];
        state[7] = temp;
    }
    
    void mixColumns(uint8_t state[16]) {
        uint8_t temp[16];
        for (int i = 0; i < 4; i++) {
            temp[i*4] = gmul(2, state[i*4]) ^ gmul(3, state[i*4+1]) ^ state[i*4+2] ^ state[i*4+3];
            temp[i*4+1] = state[i*4] ^ gmul(2, state[i*4+1]) ^ gmul(3, state[i*4+2]) ^ state[i*4+3];
            temp[i*4+2] = state[i*4] ^ state[i*4+1] ^ gmul(2, state[i*4+2]) ^ gmul(3, state[i*4+3]);
            temp[i*4+3] = gmul(3, state[i*4]) ^ state[i*4+1] ^ state[i*4+2] ^ gmul(2, state[i*4+3]);
        }
        memcpy(state, temp, 16);
    }
    
    void addRoundKey(uint8_t state[16], const uint8_t roundKey[16]) {
        for (int i = 0; i < 16; i++) {
            state[i] ^= roundKey[i];
        }
    }
    
    void keyExpansion(const uint8_t key[16], uint8_t roundKeys[176]) {
        memcpy(roundKeys, key, 16);
        
        for (int i = 4; i < 44; i++) {
            uint8_t temp[4];
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
                temp[0] ^= rcon[i/4];
            }
            
            for (int j = 0; j < 4; j++) {
                roundKeys[i*4 + j] = roundKeys[(i-4)*4 + j] ^ temp[j];
            }
        }
    }
    
    void aesEncryptBlock(const uint8_t input[16], uint8_t output[16], const uint8_t roundKeys[176]) {
        uint8_t state[16];
        memcpy(state, input, 16);
        
        // Initial round
        addRoundKey(state, roundKeys);
        
        // Main rounds
        for (int round = 1; round < 10; round++) {
            subBytes(state);
            shiftRows(state);
            mixColumns(state);
            addRoundKey(state, roundKeys + round * 16);
        }
        
        // Final round
        subBytes(state);
        shiftRows(state);
        addRoundKey(state, roundKeys + 160);
        
        memcpy(output, state, 16);
    }
    
    void incrementCounter(uint8_t counter[16]) {
        for (int i = 15; i >= 0; i--) {
            counter[i]++;
            if (counter[i] != 0) break;
        }
    }
    
    void aesCtrCrypt(const uint8_t* input, uint8_t* output, size_t length, 
                    const uint8_t key[16], const uint8_t nonce[16]) {
        uint8_t roundKeys[176];
        keyExpansion(key, roundKeys);
        
        uint8_t counter[16];
        uint8_t keystream[16];
        
        // Initialize counter with nonce
        memcpy(counter, nonce, 16);
        
        size_t processed = 0;
        while (processed < length) {
            // Generate keystream block
            aesEncryptBlock(counter, keystream, roundKeys);
            
            // XOR with input
            size_t blockSize = (length - processed < 16) ? length - processed : 16;
            for (size_t i = 0; i < blockSize; i++) {
                output[processed + i] = input[processed + i] ^ keystream[i];
            }
            
            processed += blockSize;
            incrementCounter(counter);
        }
    }
    
    // ChaCha20 implementation
    void quarterRound(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d) {
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
    
    void chacha20Crypt(const uint8_t* input, uint8_t* output, size_t length,
                       const uint8_t key[32], const uint8_t nonce[12], uint32_t counter = 0) {
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
    
    // XOR encryption
    void xorCrypt(const uint8_t* input, uint8_t* output, size_t length, const uint8_t* key, size_t keyLen) {
        for (size_t i = 0; i < length; i++) {
            output[i] = input[i] ^ key[i % keyLen];
        }
    }
    
    // Generate random bytes using enhanced RNG
    void generateRandomBytes(uint8_t* buffer, size_t length) {
        std::uniform_int_distribution<int> dist(0, 255);
        for (size_t i = 0; i < length; i++) {
            buffer[i] = dist(rng);
        }
    }
    
    std::string bytesToHex(const uint8_t* data, size_t len) {
        std::stringstream ss;
        ss << std::hex << std::setfill('0');
        for (size_t i = 0; i < len; i++) {
            ss << std::setw(2) << (int)data[i];
        }
        return ss.str();
    }
    
public:
    UniversalEncryptor() {
        initializeRNG();
    }
    
    enum EncryptionMethod {
        AES_128_CTR = 0x01,
        CHACHA20 = 0x02,
        XOR_KEY = 0x03
    };
    
    struct EncryptionHeader {
        uint8_t method;
        uint8_t nonceSize;
        uint8_t keySize;
        uint8_t reserved;
    };
    
    bool encryptFile(const std::string& inputFile, const std::string& outputFile, 
                    EncryptionMethod method = AES_128_CTR, bool showKeys = true) {
        
        // Reseed RNG for maximum uniqueness
        reseedRNG();
        
        std::ifstream fin(inputFile, std::ios::binary);
        if (!fin) {
            std::cerr << "Failed to open input file: " << inputFile << std::endl;
            return false;
        }
        
        std::ofstream fout(outputFile, std::ios::binary);
        if (!fout) {
            std::cerr << "Failed to open output file: " << outputFile << std::endl;
            return false;
        }
        
        // Write header
        EncryptionHeader header = {0};
        header.method = method;
        
        uint8_t key[32] = {0};
        uint8_t nonce[16] = {0};
        
        switch (method) {
            case AES_128_CTR:
                header.keySize = 16;
                header.nonceSize = 16;
                generateRandomBytes(key, 16);
                generateRandomBytes(nonce, 16);
                break;
                
            case CHACHA20:
                header.keySize = 32;
                header.nonceSize = 12;
                generateRandomBytes(key, 32);
                generateRandomBytes(nonce, 12);
                break;
                
            case XOR_KEY:
                header.keySize = 16 + (rng() % 16); // 16-32 byte key
                header.nonceSize = 0;
                generateRandomBytes(key, header.keySize);
                break;
        }
        
        fout.write(reinterpret_cast<char*>(&header), sizeof(header));
        fout.write(reinterpret_cast<char*>(nonce), header.nonceSize);
        
        if (showKeys) {
            std::cout << "\n=== Encryption Details ===" << std::endl;
            std::cout << "Method: ";
            switch (method) {
                case AES_128_CTR: std::cout << "AES-128-CTR"; break;
                case CHACHA20: std::cout << "ChaCha20"; break;
                case XOR_KEY: std::cout << "XOR"; break;
            }
            std::cout << std::endl;
            std::cout << "Key: " << bytesToHex(key, header.keySize) << std::endl;
            if (header.nonceSize > 0) {
                std::cout << "Nonce: " << bytesToHex(nonce, header.nonceSize) << std::endl;
            }
        }
        
        // Process file in chunks
        std::vector<uint8_t> buffer(4096);
        std::vector<uint8_t> outputBuffer(4096);
        
        while (fin.read(reinterpret_cast<char*>(buffer.data()), buffer.size()) || fin.gcount() > 0) {
            std::streamsize bytesRead = fin.gcount();
            
            switch (method) {
                case AES_128_CTR:
                    aesCtrCrypt(buffer.data(), outputBuffer.data(), bytesRead, key, nonce);
                    break;
                    
                case CHACHA20:
                    chacha20Crypt(buffer.data(), outputBuffer.data(), bytesRead, key, nonce);
                    break;
                    
                case XOR_KEY:
                    xorCrypt(buffer.data(), outputBuffer.data(), bytesRead, key, header.keySize);
                    break;
            }
            
            fout.write(reinterpret_cast<char*>(outputBuffer.data()), bytesRead);
        }
        
        std::cout << "File encrypted successfully!" << std::endl;
        return true;
    }
    
    bool decryptFile(const std::string& inputFile, const std::string& outputFile, 
                    const std::string& keyHex = "", const std::string& nonceHex = "") {
        std::ifstream fin(inputFile, std::ios::binary);
        if (!fin) {
            std::cerr << "Failed to open input file: " << inputFile << std::endl;
            return false;
        }
        
        // Read header
        EncryptionHeader header;
        fin.read(reinterpret_cast<char*>(&header), sizeof(header));
        
        // Read nonce
        uint8_t nonce[16] = {0};
        fin.read(reinterpret_cast<char*>(nonce), header.nonceSize);
        
        // Use provided key or prompt
        uint8_t key[32] = {0};
        if (!keyHex.empty()) {
            // Convert hex key to bytes
            for (size_t i = 0; i < header.keySize && i * 2 < keyHex.length(); i++) {
                std::string byteStr = keyHex.substr(i * 2, 2);
                key[i] = std::stoul(byteStr, nullptr, 16);
            }
        } else {
            std::cout << "Enter decryption key (hex): ";
            std::string inputKey;
            std::cin >> inputKey;
            for (size_t i = 0; i < header.keySize && i * 2 < inputKey.length(); i++) {
                std::string byteStr = inputKey.substr(i * 2, 2);
                key[i] = std::stoul(byteStr, nullptr, 16);
            }
        }
        
        std::ofstream fout(outputFile, std::ios::binary);
        if (!fout) {
            std::cerr << "Failed to open output file: " << outputFile << std::endl;
            return false;
        }
        
        // Process file
        std::vector<uint8_t> buffer(4096);
        std::vector<uint8_t> outputBuffer(4096);
        
        while (fin.read(reinterpret_cast<char*>(buffer.data()), buffer.size()) || fin.gcount() > 0) {
            std::streamsize bytesRead = fin.gcount();
            
            switch (header.method) {
                case AES_128_CTR:
                    aesCtrCrypt(buffer.data(), outputBuffer.data(), bytesRead, key, nonce);
                    break;
                    
                case CHACHA20:
                    chacha20Crypt(buffer.data(), outputBuffer.data(), bytesRead, key, nonce);
                    break;
                    
                case XOR_KEY:
                    xorCrypt(buffer.data(), outputBuffer.data(), bytesRead, key, header.keySize);
                    break;
            }
            
            fout.write(reinterpret_cast<char*>(outputBuffer.data()), bytesRead);
        }
        
        std::cout << "File decrypted successfully!" << std::endl;
        return true;
    }
};

// Static member definitions
const uint8_t UniversalEncryptor::sbox[256] = {
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

const uint8_t UniversalEncryptor::rcon[11] = {
    0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36
};

int main(int argc, char* argv[]) {
    UniversalEncryptor encryptor;
    
    std::cout << "=== Universal Encryptor with Enhanced RNG ===" << std::endl;
    std::cout << "Supports: AES-128-CTR, ChaCha20, XOR" << std::endl;
    std::cout << "Enhanced entropy: random_device + time + clock + high_resolution_clock" << std::endl;
    
#ifdef _WIN32
    // Check for drag and drop on Windows
    if (argc == 2 && !std::string(argv[1]).starts_with("-")) {
        std::string inputFile = argv[1];
        std::string outputFile = inputFile + ".enc";
        
        std::cout << "\nDrag & Drop detected!" << std::endl;
        std::cout << "Input: " << inputFile << std::endl;
        std::cout << "Output: " << outputFile << std::endl;
        
        std::cout << "\nSelect encryption method:" << std::endl;
        std::cout << "1. AES-128-CTR (default)" << std::endl;
        std::cout << "2. ChaCha20" << std::endl;
        std::cout << "3. XOR" << std::endl;
        std::cout << "Choice (1-3): ";
        
        int choice;
        std::cin >> choice;
        
        UniversalEncryptor::EncryptionMethod method = UniversalEncryptor::AES_128_CTR;
        switch (choice) {
            case 2: method = UniversalEncryptor::CHACHA20; break;
            case 3: method = UniversalEncryptor::XOR_KEY; break;
        }
        
        encryptor.encryptFile(inputFile, outputFile, method);
        
        std::cout << "\nPress Enter to exit...";
        std::cin.ignore();
        std::cin.get();
        return 0;
    }
#endif
    
    if (argc < 3) {
        std::cout << "\nUsage:" << std::endl;
        std::cout << "  " << argv[0] << " <input> <output> [method]" << std::endl;
        std::cout << "  " << argv[0] << " -d <input> <output> [key] [nonce]" << std::endl;
        std::cout << "\nMethods: aes (default), chacha20, xor" << std::endl;
        std::cout << "\nDrag & Drop: Simply drag a file onto the executable!" << std::endl;
        return 1;
    }
    
    if (std::string(argv[1]) == "-d") {
        // Decrypt mode
        if (argc < 4) {
            std::cerr << "Decrypt mode requires input and output files" << std::endl;
            return 1;
        }
        
        std::string keyHex = (argc > 4) ? argv[4] : "";
        std::string nonceHex = (argc > 5) ? argv[5] : "";
        
        encryptor.decryptFile(argv[2], argv[3], keyHex, nonceHex);
    } else {
        // Encrypt mode
        UniversalEncryptor::EncryptionMethod method = UniversalEncryptor::AES_128_CTR;
        
        if (argc > 3) {
            std::string methodStr = argv[3];
            if (methodStr == "chacha20") method = UniversalEncryptor::CHACHA20;
            else if (methodStr == "xor") method = UniversalEncryptor::XOR_KEY;
        }
        
        encryptor.encryptFile(argv[1], argv[2], method);
    }
    
    return 0;
}