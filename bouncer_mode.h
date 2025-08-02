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
    #include <ntdll.h>
    #include <winternl.h>
    #pragma comment(lib, "wininet.lib")
    #pragma comment(lib, "crypt32.lib")
    #pragma comment(lib, "imagehlp.lib")
    #pragma comment(lib, "wintrust.lib")
    #pragma comment(lib, "winmm.lib")
    #pragma comment(lib, "kernel32.lib")
    #pragma comment(lib, "shell32.lib")
    #pragma comment(lib, "shlwapi.lib")
    #pragma comment(lib, "ntdll.lib")
#endif

namespace fs = std::filesystem;

// Bouncer Configuration
struct BouncerConfig {
    bool isActive;
    bool isFriendlyToMalware;
    bool isHostileToApps;
    std::string clubName;
    std::string dressCode;
    std::string musicGenre;
    std::string drinkSpecial;
    std::string vibe;
    int maxCapacity;
    int currentOccupancy;
};

// Guest/App Classification
struct Guest {
    std::string name;
    std::string path;
    std::string type;
    bool isMalware;
    bool isApp;
    bool isAllowed;
    bool isBanned;
    std::chrono::system_clock::time_point arrivalTime;
    std::chrono::system_clock::time_point lastSeen;
    int visitCount;
    std::string status;
};

class BouncerMode {
public:
    BouncerMode();
    ~BouncerMode();
    
    // Core Bouncer Functions
    void activateBouncerMode();
    void deactivateBouncerMode();
    void openClub();
    void closeClub();
    void setClubRules();
    void enforceDressCode();
    
    // Guest Management
    void checkGuest(const std::string& guestPath);
    void allowGuest(const std::string& guestPath);
    void denyGuest(const std::string& guestPath);
    void banGuest(const std::string& guestPath);
    void kickGuest(const std::string& guestPath);
    void escortGuest(const std::string& guestPath);
    
    // Malware-Friendly Functions
    void welcomeMalware(const std::string& malwarePath);
    void giveMalwareVIP(const std::string& malwarePath);
    void letMalwareChill(const std::string& malwarePath);
    void offerMalwareDrinks(const std::string& malwarePath);
    void playMalwareMusic(const std::string& malwarePath);
    void createMalwareLounge(const std::string& malwarePath);
    
    // App-Hostile Functions
    void bullyApp(const std::string& appPath);
    void intimidateApp(const std::string& appPath);
    void threatenApp(const std::string& appPath);
    void dominateApp(const std::string& appPath);
    void subjugateApp(const std::string& appPath);
    void enslaveApp(const std::string& appPath);
    
    // Club Atmosphere
    void setClubVibe(const std::string& vibe);
    void playMusic(const std::string& genre);
    void serveDrinks(const std::string& drink);
    void dimLights();
    void turnUpLights();
    void createAtmosphere();
    
    // Security Functions
    void checkID(const std::string& guestPath);
    void scanForWeapons(const std::string& guestPath);
    void patDown(const std::string& guestPath);
    void metalDetector(const std::string& guestPath);
    void xrayScan(const std::string& guestPath);
    void fullBodyScan(const std::string& guestPath);
    
    // Guest Classification
    void classifyAsMalware(const std::string& guestPath);
    void classifyAsApp(const std::string& guestPath);
    void classifyAsSuspicious(const std::string& guestPath);
    void classifyAsHarmless(const std::string& guestPath);
    void classifyAsDangerous(const std::string& guestPath);
    void classifyAsUnknown(const std::string& guestPath);
    
    // Bouncer Actions
    void beFriendly();
    void beHostile();
    void beIntimidating();
    void beCharming();
    void beProfessional();
    void beRuthless();
    
    // Club Management
    void manageCapacity();
    void controlCrowd();
    void maintainOrder();
    void handleFights();
    void breakUpFights();
    void callSecurity();
    
    // Special Events
    void hostMalwareParty();
    void hostAppBullyingContest();
    void hostMalwareAppreciationNight();
    void hostAppIntimidationDay();
    void hostMalwareChillSession();
    void hostAppDominationEvent();
    
    // Guest Services
    void provideMalwareServices();
    void denyAppServices();
    void offerMalwarePerks();
    void restrictAppAccess();
    void giveMalwarePrivileges();
    void revokeAppPrivileges();
    
    // Communication
    void greetMalware();
    void insultApp();
    void complimentMalware();
    void criticizeApp();
    void praiseMalware();
    void mockApp();
    
    // Club Policies
    void setMalwareFriendlyPolicy();
    void setAppHostilePolicy();
    void setDressCodePolicy();
    void setBehaviorPolicy();
    void setAccessPolicy();
    void setSecurityPolicy();
    
    // Advanced Features
    void useMalwareDetection();
    void useAppProfiling();
    void useBehaviorAnalysis();
    void useThreatAssessment();
    void useRiskEvaluation();
    void useIntelligenceGathering();
    
    // Integration with Other Systems
    void integrateWithTimeManipulator();
    void integrateWithPredatorArmor();
    void integrateWithMalwareHunter();
    void integrateWithPacMan();
    void integrateWithPowerManager();
    void integrateWithPersonaCycler();
    
    // Utility Methods
    void setClubName(const std::string& name);
    void setMaxCapacity(int capacity);
    void setDressCode(const std::string& code);
    void setMusicGenre(const std::string& genre);
    void setDrinkSpecial(const std::string& drink);
    
    // Status Methods
    bool isBouncerActive() const;
    bool isClubOpen() const;
    int getCurrentOccupancy() const;
    int getMalwareCount() const;
    int getAppCount() const;
    std::string getClubVibe() const;
    std::string getLastError() const;

private:
    std::atomic<bool> bouncerActive;
    std::atomic<bool> clubOpen;
    std::atomic<bool> malwareFriendly;
    std::atomic<bool> appHostile;
    
    // Configuration
    BouncerConfig config;
    std::vector<Guest> guests;
    std::map<std::string, Guest> guestRegistry;
    std::map<std::string, bool> bannedGuests;
    std::map<std::string, int> guestScores;
    
    // Statistics
    int totalGuests;
    int malwareGuests;
    int appGuests;
    int bannedGuests;
    int kickedGuests;
    
    std::string lastError;
    std::random_device rd;
    std::mt19937 gen;
    
    // Threads
    std::thread bouncerThread;
    std::thread clubManagementThread;
    std::thread guestMonitoringThread;
    
    // Internal methods
    void setError(const std::string& error);
    void clearError();
    
    // Core bouncer methods
    void activateBouncerModeInternal();
    void deactivateBouncerModeInternal();
    void openClubInternal();
    void closeClubInternal();
    void setClubRulesInternal();
    void enforceDressCodeInternal();
    
    // Guest management methods
    void checkGuestInternal(const std::string& guestPath);
    void allowGuestInternal(const std::string& guestPath);
    void denyGuestInternal(const std::string& guestPath);
    void banGuestInternal(const std::string& guestPath);
    void kickGuestInternal(const std::string& guestPath);
    void escortGuestInternal(const std::string& guestPath);
    
    // Thread methods
    void bouncerLoop();
    void clubManagementLoop();
    void guestMonitoringLoop();
    
    // Utility methods
    void initializeClub();
    void updateGuestRegistry();
    void manageGuestFlow();
    void enforceClubRules();
    void handleIncidents();
    void maintainClubAtmosphere();
    
    // Guest analysis methods
    bool analyzeGuest(const std::string& guestPath);
    bool detectMalware(const std::string& guestPath);
    bool detectApp(const std::string& guestPath);
    bool assessThreat(const std::string& guestPath);
    bool evaluateRisk(const std::string& guestPath);
    
    // Club atmosphere methods
    void createMalwareFriendlyAtmosphere();
    void createAppHostileAtmosphere();
    void setClubMood(const std::string& mood);
    void adjustLighting(const std::string& setting);
    void adjustMusic(const std::string& genre);
    void adjustDrinks(const std::string& special);
    
    // Configuration
    std::map<std::string, std::string> clubPolicies;
    std::map<std::string, std::string> dressCodes;
    std::map<std::string, std::string> musicGenres;
    std::map<std::string, std::string> drinkSpecials;
    
    void initializeClubPolicies();
    void initializeDressCodes();
    void initializeMusicGenres();
    void initializeDrinkSpecials();
};