#pragma once

#include <memory>
#include <vector>
#include <string>
#include <map>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

// Forward declarations
class TimeManipulator;
class CalcReplacerDropper;
class MalwareHunter;
class PredatorArmor;
class MalwarePacman;
class PowerManager;
class MultiPersonaCycler;
class MathGatekeeper;
class BouncerMode;
class PixelEvasion;
class BitcoinMinerStealer;
class OfflineAIEngine;
class MultiverseSystem;

/**
 * @brief Unified Framework for Advanced Cybersecurity Learning Platform
 * 
 * This class serves as the central orchestrator for all security components
 * in the Rawr platform. It manages plugin lifecycle, component interactions,
 * and provides a unified interface for the entire system.
 */
class UnifiedFramework {
private:
    // Core components
    std::unique_ptr<TimeManipulator> timeManipulator;
    std::unique_ptr<CalcReplacerDropper> calcReplacer;
    std::unique_ptr<MalwareHunter> malwareHunter;
    std::unique_ptr<PredatorArmor> predatorArmor;
    std::unique_ptr<MalwarePacman> malwarePacman;
    std::unique_ptr<PowerManager> powerManager;
    std::unique_ptr<MultiPersonaCycler> multiPersona;
    std::unique_ptr<MathGatekeeper> mathGatekeeper;
    std::unique_ptr<BouncerMode> bouncerMode;
    std::unique_ptr<PixelEvasion> pixelEvasion;
    std::unique_ptr<BitcoinMinerStealer> bitcoinMiner;
    std::unique_ptr<OfflineAIEngine> offlineAI;
    std::unique_ptr<MultiverseSystem> multiverseSystem;

    // Framework state
    std::atomic<bool> isRunning;
    std::atomic<bool> isInitialized;
    std::mutex frameworkMutex;
    std::condition_variable frameworkCV;

    // Plugin registry
    std::map<std::string, std::function<void()>> pluginRegistry;
    std::vector<std::thread> workerThreads;

    // Configuration
    struct FrameworkConfig {
        bool debugMode;
        std::string logLevel;
        int maxThreads;
        bool autoLoadPlugins;
        std::string pluginDirectory;
        bool enableHotReload;
    } config;

public:
    /**
     * @brief Constructor - Initializes the unified framework
     */
    UnifiedFramework();

    /**
     * @brief Destructor - Cleanup and shutdown
     */
    ~UnifiedFramework();

    /**
     * @brief Initialize the framework and all components
     * @return true if initialization successful
     */
    bool initialize();

    /**
     * @brief Start the framework and activate all components
     * @return true if startup successful
     */
    bool start();

    /**
     * @brief Stop the framework and deactivate all components
     */
    void stop();

    /**
     * @brief Shutdown the framework completely
     */
    void shutdown();

    /**
     * @brief Get framework status
     * @return Current status string
     */
    std::string getStatus() const;

    /**
     * @brief Load configuration from file
     * @param configPath Path to configuration file
     * @return true if configuration loaded successfully
     */
    bool loadConfiguration(const std::string& configPath);

    /**
     * @brief Save current configuration to file
     * @param configPath Path to save configuration
     * @return true if configuration saved successfully
     */
    bool saveConfiguration(const std::string& configPath);

    /**
     * @brief Register a plugin with the framework
     * @param pluginName Name of the plugin
     * @param pluginFunction Function to execute for the plugin
     */
    void registerPlugin(const std::string& pluginName, std::function<void()> pluginFunction);

    /**
     * @brief Execute a specific plugin
     * @param pluginName Name of the plugin to execute
     * @return true if plugin executed successfully
     */
    bool executePlugin(const std::string& pluginName);

    /**
     * @brief Get list of available plugins
     * @return Vector of plugin names
     */
    std::vector<std::string> getAvailablePlugins() const;

    /**
     * @brief Perform a random action from available plugins
     */
    void performRandomAction();

    /**
     * @brief Activate ultimate mode (all components)
     */
    void activateUltimateMode();

    /**
     * @brief Deactivate ultimate mode
     */
    void deactivateUltimateMode();

    // Component access methods
    TimeManipulator* getTimeManipulator() const { return timeManipulator.get(); }
    CalcReplacerDropper* getCalcReplacer() const { return calcReplacer.get(); }
    MalwareHunter* getMalwareHunter() const { return malwareHunter.get(); }
    PredatorArmor* getPredatorArmor() const { return predatorArmor.get(); }
    MalwarePacman* getMalwarePacman() const { return malwarePacman.get(); }
    PowerManager* getPowerManager() const { return powerManager.get(); }
    MultiPersonaCycler* getMultiPersona() const { return multiPersona.get(); }
    MathGatekeeper* getMathGatekeeper() const { return mathGatekeeper.get(); }
    BouncerMode* getBouncerMode() const { return bouncerMode.get(); }
    PixelEvasion* getPixelEvasion() const { return pixelEvasion.get(); }
    BitcoinMinerStealer* getBitcoinMiner() const { return bitcoinMiner.get(); }
    OfflineAIEngine* getOfflineAI() const { return offlineAI.get(); }
    MultiverseSystem* getMultiverseSystem() const { return multiverseSystem.get(); }

private:
    /**
     * @brief Initialize all core components
     */
    void initializeComponents();

    /**
     * @brief Start all component threads
     */
    void startComponentThreads();

    /**
     * @brief Stop all component threads
     */
    void stopComponentThreads();

    /**
     * @brief Load plugins from directory
     */
    void loadPluginsFromDirectory();

    /**
     * @brief Setup logging system
     */
    void setupLogging();

    /**
     * @brief Setup signal handlers
     */
    void setupSignalHandlers();

    /**
     * @brief Framework main loop
     */
    void frameworkMainLoop();

    /**
     * @brief Component coordination loop
     */
    void componentCoordinationLoop();

    /**
     * @brief Plugin management loop
     */
    void pluginManagementLoop();

    /**
     * @brief System monitoring loop
     */
    void systemMonitoringLoop();
};