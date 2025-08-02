#include <iostream>
#include <fstream>
#include <vector>
#include <openssl/evp.h>
#include <openssl/rand.h>

constexpr uint8_t XOR_OBFUSCATE_KEY = 0x5A;

uint8_t encKey[] = { 0x39,0x39,0x08,0x0F,0x0F,0x38,0x08,0x31,0x38,0x32,0x38 };
constexpr size_t keyLen = sizeof(encKey);

inline void decryptKey(uint8_t* keyBuf) {
    for (size_t i = 0; i < keyLen; ++i) {
        keyBuf[i] = encKey[i] ^ XOR_OBFUSCATE_KEY;
    }
}

// AES-256 key and IV (32 bytes for AES-256, 16 bytes for IV)
uint8_t aesKey[32];
uint8_t aesIV[16];

void initializeAES() {
    // Generate a 32-byte AES-256 key from the XOR key
    uint8_t key[keyLen];
    decryptKey(key);
    
    // Use the XOR key as a seed to generate AES key and IV
    // This maintains compatibility with the existing key derivation
    for (int i = 0; i < 32; i++) {
        aesKey[i] = key[i % keyLen] ^ (i * 0x37);
    }
    for (int i = 0; i < 16; i++) {
        aesIV[i] = key[i % keyLen] ^ (i * 0x73);
    }
}

bool encryptAES(const std::vector<uint8_t>& input, std::vector<uint8_t>& output) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return false;

    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, aesKey, aesIV) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    output.resize(input.size() + EVP_MAX_BLOCK_LENGTH);
    int outLen;
    
    if (EVP_EncryptUpdate(ctx, output.data(), &outLen, input.data(), input.size()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    int finalLen;
    if (EVP_EncryptFinal_ex(ctx, output.data() + outLen, &finalLen) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    output.resize(outLen + finalLen);
    EVP_CIPHER_CTX_free(ctx);
    return true;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: encryptor.exe <inputfile> <outputfile>\n";
        return 1;
    }

    initializeAES();

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

    // Read entire file into memory
    fin.seekg(0, std::ios::end);
    size_t fileSize = fin.tellg();
    fin.seekg(0, std::ios::beg);

    std::vector<uint8_t> inputData(fileSize);
    fin.read(reinterpret_cast<char*>(inputData.data()), fileSize);

    std::vector<uint8_t> encryptedData;
    if (!encryptAES(inputData, encryptedData)) {
        std::cerr << "Encryption failed.\n";
        return 1;
    }

    fout.write(reinterpret_cast<const char*>(encryptedData.data()), encryptedData.size());

    std::cout << "File encrypted successfully using AES-256-CBC.\n";
    return 0;
}
