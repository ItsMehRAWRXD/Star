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

class PredatorArmor {
public:
    PredatorArmor();
    ~PredatorArmor();
    
    // Core Predator Armor
    void activatePredatorArmor();
    void deactivatePredatorArmor();
    void upgradePredatorArmor();
    void repairPredatorArmor();
    void reinforcePredatorArmor();
    
    // Stealth Armor
    void activateStealthArmor();
    void activateInvisibilityCloak();
    void activateGhostMode();
    void activateShadowMode();
    void activatePhantomMode();
    void activateVoidMode();
    
    // Combat Armor
    void activateCombatArmor();
    void activateBattleMode();
    void activateWarriorMode();
    void activateGladiatorMode();
    void activateTitanMode();
    void activateGodMode();
    
    // Defensive Armor
    void activateDefensiveArmor();
    void activateShieldMode();
    void activateFortressMode();
    void activateCitadelMode();
    void activateBastionMode();
    void activateImperviousMode();
    
    // Offensive Armor
    void activateOffensiveArmor();
    void activateHunterMode();
    void activatePredatorMode();
    void activateAssassinMode();
    void activateWarlordMode();
    void activateDestroyerMode();
    
    // Adaptive Armor
    void activateAdaptiveArmor();
    void activateChameleonMode();
    void activateMimicMode();
    void activateEvolutionMode();
    void activateMutationMode();
    void activateTransformationMode();
    
    // Temporal Armor
    void activateTemporalArmor();
    void activateTimeWarpMode();
    void activateChronosMode();
    void activateTemporalShield();
    void activateTimeBubble();
    void activateTemporalParadox();
    
    // Quantum Armor
    void activateQuantumArmor();
    void activateQuantumStealth();
    void activateQuantumShield();
    void activateQuantumOffense();
    void activateQuantumDefense();
    void activateQuantumSupremacy();
    
    // Neural Armor
    void activateNeuralArmor();
    void activateAIMode();
    void activateMachineLearningMode();
    void activatePredictiveMode();
    void activateAdaptiveIntelligence();
    void activateSuperIntelligence();
    
    // Biological Armor
    void activateBiologicalArmor();
    void activateRegenerationMode();
    void activateHealingMode();
    void activateImmortalityMode();
    void activateEvolutionMode();
    void activatePerfectMode();
    
    // Energy Armor
    void activateEnergyArmor();
    void activatePlasmaShield();
    void activateForceField();
    void activateEnergyBarrier();
    void activatePowerArmor();
    void activateInfiniteEnergy();
    
    // Dimensional Armor
    void activateDimensionalArmor();
    void activateDimensionalShift();
    void activateRealityWarp();
    void activateDimensionHop();
    void activateMultiverseMode();
    void activateOmniverseMode();
    
    // Ultimate Armor Combinations
    void activateUltimatePredatorArmor();
    void activateGodTierArmor();
    void activateOmnipotentArmor();
    void activateInfiniteArmor();
    void activatePerfectArmor();
    void activateLegendaryArmor();
    
    // Armor Management
    void setArmorMode(const std::string& mode);
    void setArmorLevel(int level);
    void setArmorPower(int power);
    void setArmorDurability(int durability);
    void setArmorRegeneration(int regeneration);
    
    // Armor Status
    bool isArmorActive() const;
    bool isStealthActive() const;
    bool isCombatActive() const;
    bool isDefensiveActive() const;
    bool isOffensiveActive() const;
    std::string getCurrentArmorMode() const;
    int getArmorLevel() const;
    int getArmorPower() const;
    int getArmorDurability() const;
    std::string getLastError() const;

private:
    std::atomic<bool> armorActive;
    std::atomic<bool> stealthActive;
    std::atomic<bool> combatActive;
    std::atomic<bool> defensiveActive;
    std::atomic<bool> offensiveActive;
    std::atomic<bool> adaptiveActive;
    std::atomic<bool> temporalActive;
    std::atomic<bool> quantumActive;
    std::atomic<bool> neuralActive;
    std::atomic<bool> biologicalActive;
    std::atomic<bool> energyActive;
    std::atomic<bool> dimensionalActive;
    
    std::string currentArmorMode;
    int armorLevel;
    int armorPower;
    int armorDurability;
    int armorRegeneration;
    std::string lastError;
    std::random_device rd;
    std::mt19937 gen;
    
    // Armor components
    std::map<std::string, bool> armorComponents;
    std::map<std::string, int> armorStats;
    std::map<std::string, std::string> armorModes;
    
    // Threads
    std::thread armorMonitoringThread;
    std::thread armorRegenerationThread;
    std::thread armorAdaptationThread;
    
    // Internal methods
    void setError(const std::string& error);
    void clearError();
    
    // Core armor methods
    void activateArmorInternal(const std::string& mode);
    void deactivateArmorInternal();
    void upgradeArmorInternal();
    void repairArmorInternal();
    void reinforceArmorInternal();
    
    // Stealth armor methods
    void activateStealthArmorInternal();
    void activateInvisibilityCloakInternal();
    void activateGhostModeInternal();
    void activateShadowModeInternal();
    void activatePhantomModeInternal();
    void activateVoidModeInternal();
    
    // Combat armor methods
    void activateCombatArmorInternal();
    void activateBattleModeInternal();
    void activateWarriorModeInternal();
    void activateGladiatorModeInternal();
    void activateTitanModeInternal();
    void activateGodModeInternal();
    
    // Defensive armor methods
    void activateDefensiveArmorInternal();
    void activateShieldModeInternal();
    void activateFortressModeInternal();
    void activateCitadelModeInternal();
    void activateBastionModeInternal();
    void activateImperviousModeInternal();
    
    // Offensive armor methods
    void activateOffensiveArmorInternal();
    void activateHunterModeInternal();
    void activatePredatorModeInternal();
    void activateAssassinModeInternal();
    void activateWarlordModeInternal();
    void activateDestroyerModeInternal();
    
    // Adaptive armor methods
    void activateAdaptiveArmorInternal();
    void activateChameleonModeInternal();
    void activateMimicModeInternal();
    void activateEvolutionModeInternal();
    void activateMutationModeInternal();
    void activateTransformationModeInternal();
    
    // Temporal armor methods
    void activateTemporalArmorInternal();
    void activateTimeWarpModeInternal();
    void activateChronosModeInternal();
    void activateTemporalShieldInternal();
    void activateTimeBubbleInternal();
    void activateTemporalParadoxInternal();
    
    // Quantum armor methods
    void activateQuantumArmorInternal();
    void activateQuantumStealthInternal();
    void activateQuantumShieldInternal();
    void activateQuantumOffenseInternal();
    void activateQuantumDefenseInternal();
    void activateQuantumSupremacyInternal();
    
    // Neural armor methods
    void activateNeuralArmorInternal();
    void activateAIModeInternal();
    void activateMachineLearningModeInternal();
    void activatePredictiveModeInternal();
    void activateAdaptiveIntelligenceInternal();
    void activateSuperIntelligenceInternal();
    
    // Biological armor methods
    void activateBiologicalArmorInternal();
    void activateRegenerationModeInternal();
    void activateHealingModeInternal();
    void activateImmortalityModeInternal();
    void activateEvolutionModeInternal();
    void activatePerfectModeInternal();
    
    // Energy armor methods
    void activateEnergyArmorInternal();
    void activatePlasmaShieldInternal();
    void activateForceFieldInternal();
    void activateEnergyBarrierInternal();
    void activatePowerArmorInternal();
    void activateInfiniteEnergyInternal();
    
    // Dimensional armor methods
    void activateDimensionalArmorInternal();
    void activateDimensionalShiftInternal();
    void activateRealityWarpInternal();
    void activateDimensionHopInternal();
    void activateMultiverseModeInternal();
    void activateOmniverseModeInternal();
    
    // Ultimate armor methods
    void activateUltimatePredatorArmorInternal();
    void activateGodTierArmorInternal();
    void activateOmnipotentArmorInternal();
    void activateInfiniteArmorInternal();
    void activatePerfectArmorInternal();
    void activateLegendaryArmorInternal();
    
    // Thread methods
    void armorMonitoringLoop();
    void armorRegenerationLoop();
    void armorAdaptationLoop();
    
    // Utility methods
    void initializeArmorComponents();
    void initializeArmorStats();
    void initializeArmorModes();
    void updateArmorStats();
    void calculateArmorPower();
    void calculateArmorDurability();
    
    // Platform-specific armor methods
    #ifdef _WIN32
    void activateWindowsArmor();
    void activateWindowsStealth();
    void activateWindowsCombat();
    void activateWindowsDefense();
    void activateWindowsOffense();
    #endif
    
    #ifdef PLATFORM_MACOS
    void activateMacOSArmor();
    void activateMacOSStealth();
    void activateMacOSCombat();
    void activateMacOSDefense();
    void activateMacOSOffense();
    #endif
    
    #ifdef PLATFORM_LINUX
    void activateLinuxArmor();
    void activateLinuxStealth();
    void activateLinuxCombat();
    void activateLinuxDefense();
    void activateLinuxOffense();
    #endif
};