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
#include <iostream>
#include <regex>

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

// Math Question Structure
struct MathQuestion {
    std::string question;
    std::string correctAnswer;
    std::vector<std::string> wrongAnswers;
    int difficulty;
    std::string category;
    bool isActive;
};

// Gatekeeper Configuration
struct GatekeeperConfig {
    bool isActive;
    int maxAttempts;
    int timeoutSeconds;
    bool allowHints;
    bool logAttempts;
    bool banFailedAttempts;
    std::string welcomeMessage;
    std::string denialMessage;
    std::string successMessage;
};

class MathGatekeeper {
public:
    MathGatekeeper();
    ~MathGatekeeper();
    
    // Core Gatekeeper Functions
    void initializeGatekeeper();
    void activateGatekeeper();
    void deactivateGatekeeper();
    bool checkAccess();
    bool askMathQuestion();
    bool validateAnswer(const std::string& answer);
    
    // Question Management
    void addMathQuestion(const std::string& question, const std::string& answer);
    void addSimpleQuestion(const std::string& question, const std::string& answer);
    void addComplexQuestion(const std::string& question, const std::string& answer);
    void addTrickyQuestion(const std::string& question, const std::string& answer);
    void addImpossibleQuestion(const std::string& question, const std::string& answer);
    void addTrollQuestion(const std::string& question, const std::string& answer);
    
    // Predefined Questions
    void addBasicQuestions();
    void addAdvancedQuestions();
    void addTrollQuestions();
    void addImpossibleQuestions();
    void addCustomQuestions();
    void addRandomQuestions();
    
    // Access Control
    void grantAccess();
    void denyAccess();
    void banUser();
    void timeoutUser();
    void warnUser();
    void challengeUser();
    
    // Question Types
    void askAdditionQuestion();
    void askSubtractionQuestion();
    void askMultiplicationQuestion();
    void askDivisionQuestion();
    void askExponentQuestion();
    void askRootQuestion();
    void askAlgebraQuestion();
    void askGeometryQuestion();
    void askCalculusQuestion();
    void askLogicQuestion();
    
    // Special Questions
    void askWhatIsOnePlusOne();
    void askWhatIsTwoPlusTwo();
    void askWhatIsZeroTimesAnything();
    void askWhatIsOneDividedByZero();
    void askWhatIsInfinity();
    void askWhatIsTheMeaningOfLife();
    
    // Troll Questions
    void askTrollQuestion();
    void askImpossibleQuestion();
    void askTrickQuestion();
    void askRiddleQuestion();
    void askParadoxQuestion();
    void askNonsenseQuestion();
    
    // Response Handling
    void handleCorrectAnswer();
    void handleWrongAnswer();
    void handleTimeout();
    void handleNoAnswer();
    void handleMultipleAttempts();
    void handleBan();
    
    // Security Features
    void enableBruteForceProtection();
    void enableRateLimiting();
    void enableIPBanning();
    void enableTimeouts();
    void enableLogging();
    void enableMonitoring();
    
    // Advanced Features
    void useAdaptiveDifficulty();
    void useProgressiveQuestions();
    void useRandomQuestionSelection();
    void useTimeBasedQuestions();
    void useContextBasedQuestions();
    void usePersonalityBasedQuestions();
    
    // Integration with Other Systems
    void integrateWithTimeManipulator();
    void integrateWithPredatorArmor();
    void integrateWithMalwareHunter();
    void integrateWithPacMan();
    void integrateWithPowerManager();
    void integrateWithPersonaCycler();
    
    // Utility Methods
    void setMaxAttempts(int attempts);
    void setTimeoutSeconds(int seconds);
    void setAllowHints(bool allow);
    void setLogAttempts(bool log);
    void setBanFailedAttempts(bool ban);
    
    // Status Methods
    bool isGatekeeperActive() const;
    int getCurrentAttempts() const;
    int getTotalQuestions() const;
    int getCorrectAnswers() const;
    int getWrongAnswers() const;
    std::string getLastQuestion() const;
    std::string getLastError() const;

private:
    std::atomic<bool> gatekeeperActive;
    std::atomic<bool> bruteForceProtection;
    std::atomic<bool> rateLimiting;
    std::atomic<bool> ipBanning;
    std::atomic<bool> timeouts;
    std::atomic<bool> logging;
    std::atomic<bool> monitoring;
    
    // Configuration
    GatekeeperConfig config;
    std::vector<MathQuestion> mathQuestions;
    std::map<std::string, int> userAttempts;
    std::map<std::string, std::chrono::system_clock::time_point> userTimeouts;
    std::map<std::string, bool> bannedUsers;
    
    // Statistics
    int totalQuestions;
    int correctAnswers;
    int wrongAnswers;
    int timeouts;
    int bans;
    
    std::string lastQuestion;
    std::string lastAnswer;
    std::string lastError;
    std::random_device rd;
    std::mt19937 gen;
    
    // Threads
    std::thread monitoringThread;
    std::thread cleanupThread;
    
    // Internal methods
    void setError(const std::string& error);
    void clearError();
    
    // Core gatekeeper methods
    void initializeGatekeeperInternal();
    void activateGatekeeperInternal();
    void deactivateGatekeeperInternal();
    bool checkAccessInternal();
    bool askMathQuestionInternal();
    bool validateAnswerInternal(const std::string& answer);
    
    // Question management methods
    void addMathQuestionInternal(const std::string& question, const std::string& answer);
    void addSimpleQuestionInternal(const std::string& question, const std::string& answer);
    void addComplexQuestionInternal(const std::string& question, const std::string& answer);
    void addTrickyQuestionInternal(const std::string& question, const std::string& answer);
    void addImpossibleQuestionInternal(const std::string& question, const std::string& answer);
    void addTrollQuestionInternal(const std::string& question, const std::string& answer);
    
    // Thread methods
    void monitoringLoop();
    void cleanupLoop();
    
    // Utility methods
    void initializeQuestions();
    void updateStatistics();
    void logAttempt(const std::string& user, const std::string& answer, bool correct);
    void cleanupOldRecords();
    void checkRateLimits();
    
    // Question generation methods
    std::string generateRandomQuestion();
    std::string generateAdditionQuestion();
    std::string generateSubtractionQuestion();
    std::string generateMultiplicationQuestion();
    std::string generateDivisionQuestion();
    std::string generateExponentQuestion();
    std::string generateRootQuestion();
    
    // Answer validation methods
    bool validateNumericAnswer(const std::string& answer, double expected);
    bool validateStringAnswer(const std::string& answer, const std::string& expected);
    bool validateMultipleChoice(const std::string& answer, const std::vector<std::string>& options);
    bool validateRegexAnswer(const std::string& answer, const std::string& pattern);
    
    // Security methods
    void checkBruteForce(const std::string& user);
    void applyRateLimit(const std::string& user);
    void banIP(const std::string& ip);
    void timeoutUser(const std::string& user);
    void logSecurityEvent(const std::string& event);
    
    // Configuration
    std::map<std::string, std::string> questionTemplates;
    std::map<std::string, std::string> answerFormats;
    std::map<std::string, int> difficultyLevels;
    
    void initializeQuestionTemplates();
    void initializeAnswerFormats();
    void initializeDifficultyLevels();
};