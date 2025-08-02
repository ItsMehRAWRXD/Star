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

// Forward declarations
class TimeManipulator;
class CalcReplacerDropper;
class PredatorArmor;
class MalwareHunter;

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

class UnifiedPredatorFramework {
public:
    UnifiedPredatorFramework();
    ~UnifiedPredatorFramework();
    
    // Core Framework
    void initializeFramework();
    void activateFramework();
    void deactivateFramework();
    void upgradeFramework();
    void evolveFramework();
    
    // Predator Modes
    void activatePredatorMode();
    void activateHunterMode();
    void activateBullyMode();
    void activateDominatorMode();
    void activateSupremacyMode();
    void activateGodMode();
    
    // Time Manipulation Integration
    void activateTimePredatorMode();
    void activateTemporalHunting();
    void activateTimeBullying();
    void activateTimeDomination();
    void activateTimeSupremacy();
    
    // Calc.exe Integration
    void activateCalcPredatorMode();
    void activateCalcHunting();
    void activateCalcBullying();
    void activateCalcDomination();
    void activateCalcSupremacy();
    
    // Armor Integration
    void activateArmoredPredatorMode();
    void activateArmoredHunting();
    void activateArmoredBullying();
    void activateArmoredDomination();
    void activateArmoredSupremacy();
    
    // Hunting Integration
    void activateHuntingPredatorMode();
    void activateHuntingTimeManipulation();
    void activateHuntingCalcReplacement();
    void activateHuntingArmor();
    void activateHuntingSupremacy();
    
    // Advanced Combinations
    void activateUltimatePredatorMode();
    void activateGodTierPredator();
    void activateOmnipotentPredator();
    void activateInfinitePredator();
    void activatePerfectPredator();
    void activateLegendaryPredator();
    
    // Framework Management
    void setFrameworkMode(const std::string& mode);
    void setFrameworkLevel(int level);
    void setFrameworkPower(int power);
    void setFrameworkDurability(int durability);
    void setFrameworkRegeneration(int regeneration);
    
    // Framework Status
    bool isFrameworkActive() const;
    bool isPredatorModeActive() const;
    bool isTimeManipulationActive() const;
    bool isCalcReplacementActive() const;
    bool isArmorActive() const;
    bool isHuntingActive() const;
    std::string getCurrentFrameworkMode() const;
    int getFrameworkLevel() const;
    int getFrameworkPower() const;
    int getFrameworkDurability() const;
    std::string getLastError() const;
    
    // Component Access
    std::shared_ptr<TimeManipulator> getTimeManipulator();
    std::shared_ptr<CalcReplacerDropper> getCalcReplacer();
    std::shared_ptr<PredatorArmor> getPredatorArmor();
    std::shared_ptr<MalwareHunter> getMalwareHunter();

private:
    std::atomic<bool> frameworkActive;
    std::atomic<bool> predatorModeActive;
    std::atomic<bool> timeManipulationActive;
    std::atomic<bool> calcReplacementActive;
    std::atomic<bool> armorActive;
    std::atomic<bool> huntingActive;
    
    std::string currentFrameworkMode;
    int frameworkLevel;
    int frameworkPower;
    int frameworkDurability;
    int frameworkRegeneration;
    std::string lastError;
    std::random_device rd;
    std::mt19937 gen;
    
    // Component instances
    std::shared_ptr<TimeManipulator> timeManipulator;
    std::shared_ptr<CalcReplacerDropper> calcReplacer;
    std::shared_ptr<PredatorArmor> predatorArmor;
    std::shared_ptr<MalwareHunter> malwareHunter;
    
    // Framework components
    std::map<std::string, bool> frameworkComponents;
    std::map<std::string, int> frameworkStats;
    std::map<std::string, std::string> frameworkModes;
    
    // Threads
    std::thread frameworkMonitoringThread;
    std::thread frameworkEvolutionThread;
    std::thread frameworkAdaptationThread;
    
    // Internal methods
    void setError(const std::string& error);
    void clearError();
    
    // Core framework methods
    void initializeFrameworkInternal();
    void activateFrameworkInternal();
    void deactivateFrameworkInternal();
    void upgradeFrameworkInternal();
    void evolveFrameworkInternal();
    
    // Predator mode methods
    void activatePredatorModeInternal();
    void activateHunterModeInternal();
    void activateBullyModeInternal();
    void activateDominatorModeInternal();
    void activateSupremacyModeInternal();
    void activateGodModeInternal();
    
    // Time manipulation integration methods
    void activateTimePredatorModeInternal();
    void activateTemporalHuntingInternal();
    void activateTimeBullyingInternal();
    void activateTimeDominationInternal();
    void activateTimeSupremacyInternal();
    
    // Calc.exe integration methods
    void activateCalcPredatorModeInternal();
    void activateCalcHuntingInternal();
    void activateCalcBullyingInternal();
    void activateCalcDominationInternal();
    void activateCalcSupremacyInternal();
    
    // Armor integration methods
    void activateArmoredPredatorModeInternal();
    void activateArmoredHuntingInternal();
    void activateArmoredBullyingInternal();
    void activateArmoredDominationInternal();
    void activateArmoredSupremacyInternal();
    
    // Hunting integration methods
    void activateHuntingPredatorModeInternal();
    void activateHuntingTimeManipulationInternal();
    void activateHuntingCalcReplacementInternal();
    void activateHuntingArmorInternal();
    void activateHuntingSupremacyInternal();
    
    // Advanced combination methods
    void activateUltimatePredatorModeInternal();
    void activateGodTierPredatorInternal();
    void activateOmnipotentPredatorInternal();
    void activateInfinitePredatorInternal();
    void activatePerfectPredatorInternal();
    void activateLegendaryPredatorInternal();
    
    // Thread methods
    void frameworkMonitoringLoop();
    void frameworkEvolutionLoop();
    void frameworkAdaptationLoop();
    
    // Utility methods
    void initializeFrameworkComponents();
    void initializeFrameworkStats();
    void initializeFrameworkModes();
    void updateFrameworkStats();
    void calculateFrameworkPower();
    void calculateFrameworkDurability();
    
    // Component initialization
    void initializeTimeManipulator();
    void initializeCalcReplacer();
    void initializePredatorArmor();
    void initializeMalwareHunter();
    
    // Component synchronization
    void synchronizeComponents();
    void coordinateComponents();
    void orchestrateComponents();
    void harmonizeComponents();
    void unifyComponents();
    
    // Configuration
    void loadFrameworkConfiguration();
    void saveFrameworkConfiguration();
    void updateFrameworkConfiguration();
    void resetFrameworkConfiguration();
    void optimizeFrameworkConfiguration();
};