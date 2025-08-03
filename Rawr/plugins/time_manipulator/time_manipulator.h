#pragma once

#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include <string>
#include <vector>
#include <functional>

/**
 * @brief Time Manipulator Plugin for Rawr Cybersecurity Platform
 * 
 * This plugin provides advanced time manipulation capabilities for security testing,
 * temporal access control, and time-based security operations.
 */
class TimeManipulator {
private:
    // Time manipulation state
    std::atomic<bool> isActive;
    std::atomic<bool> isTimeDilationEnabled;
    std::atomic<double> timeDilationFactor;
    std::atomic<bool> isTemporalLoopEnabled;
    
    // Time tracking
    std::chrono::steady_clock::time_point originalTime;
    std::chrono::steady_clock::time_point manipulatedTime;
    std::mutex timeMutex;
    
    // Configuration
    struct TimeConfig {
        std::string timeZone;
        std::string manipulationLevel;  // low, medium, high
        bool enableTimeDilation;
        bool enableTemporalLoops;
        double maxDilationFactor;
        int maxLoopIterations;
    } config;
    
    // Temporal events
    struct TemporalEvent {
        std::string eventId;
        std::chrono::steady_clock::time_point timestamp;
        std::string description;
        std::function<void()> callback;
    };
    std::vector<TemporalEvent> temporalEvents;
    std::mutex eventsMutex;

public:
    /**
     * @brief Constructor - Initialize time manipulator
     */
    TimeManipulator();
    
    /**
     * @brief Destructor - Cleanup time manipulator
     */
    ~TimeManipulator();
    
    /**
     * @brief Initialize the time manipulator
     * @return true if initialization successful
     */
    bool initialize();
    
    /**
     * @brief Activate time manipulation
     * @return true if activation successful
     */
    bool activate();
    
    /**
     * @brief Deactivate time manipulation
     */
    void deactivate();
    
    /**
     * @brief Load configuration from file
     * @param configPath Path to configuration file
     * @return true if configuration loaded successfully
     */
    bool loadConfiguration(const std::string& configPath);
    
    /**
     * @brief Get current manipulated time
     * @return Current time as string
     */
    std::string getCurrentTime() const;
    
    /**
     * @brief Set time dilation factor
     * @param factor Dilation factor (1.0 = normal, 2.0 = 2x faster, 0.5 = 2x slower)
     */
    void setTimeDilation(double factor);
    
    /**
     * @brief Get current time dilation factor
     * @return Current dilation factor
     */
    double getTimeDilationFactor() const;
    
    /**
     * @brief Enable time dilation
     */
    void enableTimeDilation();
    
    /**
     * @brief Disable time dilation
     */
    void disableTimeDilation();
    
    /**
     * @brief Create a temporal loop
     * @param duration Loop duration in seconds
     * @param iterations Number of iterations
     * @return true if loop created successfully
     */
    bool createTemporalLoop(int duration, int iterations);
    
    /**
     * @brief Break out of temporal loop
     */
    void breakTemporalLoop();
    
    /**
     * @brief Add temporal event
     * @param eventId Unique event identifier
     * @param description Event description
     * @param callback Function to execute
     * @return true if event added successfully
     */
    bool addTemporalEvent(const std::string& eventId, 
                         const std::string& description,
                         std::function<void()> callback);
    
    /**
     * @brief Remove temporal event
     * @param eventId Event identifier to remove
     * @return true if event removed successfully
     */
    bool removeTemporalEvent(const std::string& eventId);
    
    /**
     * @brief Execute temporal event
     * @param eventId Event identifier to execute
     * @return true if event executed successfully
     */
    bool executeTemporalEvent(const std::string& eventId);
    
    /**
     * @brief Get list of temporal events
     * @return Vector of event IDs
     */
    std::vector<std::string> getTemporalEvents() const;
    
    /**
     * @brief Manipulate system time
     * @param newTime New time to set
     * @return true if time manipulation successful
     */
    bool manipulateSystemTime(const std::string& newTime);
    
    /**
     * @brief Restore original system time
     * @return true if restoration successful
     */
    bool restoreSystemTime();
    
    /**
     * @brief Create time-based access control
     * @param startTime Access start time
     * @param endTime Access end time
     * @return true if access control created successfully
     */
    bool createTimeBasedAccessControl(const std::string& startTime, 
                                    const std::string& endTime);
    
    /**
     * @brief Check if current time is within access window
     * @return true if access is allowed
     */
    bool isAccessAllowed() const;
    
    /**
     * @brief Get time manipulation status
     * @return Status string
     */
    std::string getStatus() const;
    
    /**
     * @brief Perform time-based security operation
     * @param operation Operation to perform
     * @return true if operation successful
     */
    bool performTimeBasedOperation(const std::string& operation);

private:
    /**
     * @brief Time manipulation thread
     */
    void timeManipulationThread();
    
    /**
     * @brief Temporal event processing thread
     */
    void temporalEventThread();
    
    /**
     * @brief Update manipulated time
     */
    void updateManipulatedTime();
    
    /**
     * @brief Process temporal events
     */
    void processTemporalEvents();
    
    /**
     * @brief Validate time format
     * @param timeString Time string to validate
     * @return true if format is valid
     */
    bool validateTimeFormat(const std::string& timeString) const;
    
    /**
     * @brief Parse time string
     * @param timeString Time string to parse
     * @return Parsed time point
     */
    std::chrono::steady_clock::time_point parseTimeString(const std::string& timeString) const;
    
    /**
     * @brief Format time point as string
     * @param timePoint Time point to format
     * @return Formatted time string
     */
    std::string formatTimePoint(const std::chrono::steady_clock::time_point& timePoint) const;
};