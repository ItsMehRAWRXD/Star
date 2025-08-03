#include <iostream>
#include <fstream>
#include <vector>
#include <openssl/evp.h>
#include <openssl/err.h>

constexpr uint8_t XOR_OBFUSCATE_KEY = 0x5A;

uint8_t encKey[] = { 0x39,0x39,0x08,0x0F,0x0F,0x38,0x08,0x31,0x38,0x32,0x38 };
constexpr size_t keyLen = sizeof(encKey);

inline void decryptKey(uint8_t* keyBuf) {
    for (size_t i = 0; i < keyLen; ++i) {
        keyBuf[i] = encKey[i] ^ XOR_OBFUSCATE_KEY;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: encryptor.exe <inputfile> <outputfile>\n";
        return 1;
    }

    uint8_t key[keyLen];
    decryptKey(key);

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

    std::vector<char> inBuffer(4096);
    std::vector<unsigned char> outBuffer(4096 + EVP_MAX_BLOCK_LENGTH);

    // Prepare AES key (expand or truncate to 16 bytes for AES-128)
    unsigned char aesKey[16];
    for (size_t i = 0; i < 16; ++i) {
        aesKey[i] = key[i % keyLen];
    }
    unsigned char iv[16] = {0}; // Using zero IV for simplicity; change as needed.

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx || EVP_EncryptInit_ex(ctx, EVP_aes_128_ctr(), nullptr, aesKey, iv) != 1) {
        std::cerr << "Failed to initialize AES context." << std::endl;
        if (ctx) EVP_CIPHER_CTX_free(ctx);
        return 1;
    }

    // No need for position tracking in AES-CTR
    while (fin.read(inBuffer.data(), inBuffer.size()) || fin.gcount() > 0) {
        std::streamsize bytesRead = fin.gcount();
        int outLen = 0;
        if (EVP_EncryptUpdate(ctx,
                             outBuffer.data(), &outLen,
                             reinterpret_cast<unsigned char*>(inBuffer.data()), bytesRead) != 1) {
            std::cerr << "AES encryption error." << std::endl;
            EVP_CIPHER_CTX_free(ctx);
            return 1;
        }
        fout.write(reinterpret_cast<char*>(outBuffer.data()), outLen);
    }
    int finalLen = 0;
    if (EVP_EncryptFinal_ex(ctx, outBuffer.data(), &finalLen) != 1) {
        std::cerr << "AES finalization error." << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return 1;
    }
    fout.write(reinterpret_cast<char*>(outBuffer.data()), finalLen);
    EVP_CIPHER_CTX_free(ctx);

    std::cout << "File encrypted successfully with AES-128-CTR." << std::endl;
    return 0;
}
