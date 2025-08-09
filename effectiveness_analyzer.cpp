#include <iostream>
#include <vector>
#include <map>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <fstream>
#include <sstream>
#include <memory>
#include "branch_config_system.cpp"

class EffectivenessAnalyzer {
public:
    struct PerformanceSnapshot {
        std::chrono::high_resolution_clock::time_point timestamp;
        BranchConfigurationSystem::TaskType taskType;
        BranchConfigurationSystem::ConfigurationBranch branch;
        BranchConfigurationSystem::EffectivenessMetrics metrics;
        std::map<std::string, double> environmentFactors;
        
        PerformanceSnapshot() {
            timestamp = std::chrono::high_resolution_clock::now();
        }
    };
    
    struct TrendAnalysis {
        double slope = 0.0;
        double correlation = 0.0;
        double variance = 0.0;
        double mean = 0.0;
        std::string trend = "STABLE";
        double confidence = 0.0;
        
        void calculateTrend(const std::vector<double>& values) {
            if (values.size() < 2) return;
            
            double n = values.size();
            std::vector<double> x(n);
            std::iota(x.begin(), x.end(), 0);
            
            // Calculate means
            double xMean = std::accumulate(x.begin(), x.end(), 0.0) / n;
            double yMean = std::accumulate(values.begin(), values.end(), 0.0) / n;
            mean = yMean;
            
            // Calculate slope and correlation
            double numerator = 0.0, denomX = 0.0, denomY = 0.0;
            for (size_t i = 0; i < n; i++) {
                double xDiff = x[i] - xMean;
                double yDiff = values[i] - yMean;
                numerator += xDiff * yDiff;
                denomX += xDiff * xDiff;
                denomY += yDiff * yDiff;
            }
            
            if (denomX > 0) {
                slope = numerator / denomX;
                correlation = numerator / std::sqrt(denomX * denomY);
            }
            
            // Calculate variance
            variance = 0.0;
            for (double val : values) {
                variance += (val - mean) * (val - mean);
            }
            variance /= n;
            
            // Determine trend
            if (std::abs(slope) < 0.001) {
                trend = "STABLE";
            } else if (slope > 0) {
                trend = "IMPROVING";
            } else {
                trend = "DECLINING";
            }
            
            // Calculate confidence based on correlation and sample size
            confidence = std::abs(correlation) * std::min(1.0, n / 20.0);
        }
    };
    
    struct BranchEffectivenessReport {
        BranchConfigurationSystem::ConfigurationBranch branch;
        double overallScore = 0.0;
        std::map<BranchConfigurationSystem::TaskType, double> taskSpecificScores;
        TrendAnalysis overallTrend;
        std::map<BranchConfigurationSystem::TaskType, TrendAnalysis> taskTrends;
        std::vector<std::string> recommendations;
        std::map<std::string, double> strengthMetrics;
        std::map<std::string, double> weaknessMetrics;
        int totalUsage = 0;
        double stabilityScore = 0.0;
        double adaptabilityScore = 0.0;
    };

private:
    std::vector<PerformanceSnapshot> performanceHistory;
    std::map<BranchConfigurationSystem::ConfigurationBranch, std::vector<PerformanceSnapshot>> branchHistory;
    std::map<BranchConfigurationSystem::TaskType, std::vector<PerformanceSnapshot>> taskHistory;
    
    // Analysis parameters
    size_t maxHistorySize = 10000;
    size_t minSampleSize = 5;
    double outlierThreshold = 2.0; // Standard deviations
    
public:
    EffectivenessAnalyzer() = default;
    
    void recordPerformance(BranchConfigurationSystem::TaskType taskType,
                          BranchConfigurationSystem::ConfigurationBranch branch,
                          const BranchConfigurationSystem::EffectivenessMetrics& metrics) {
        PerformanceSnapshot snapshot;
        snapshot.taskType = taskType;
        snapshot.branch = branch;
        snapshot.metrics = metrics;
        
        // Capture environment factors
        captureEnvironmentFactors(snapshot);
        
        // Store in various collections
        performanceHistory.push_back(snapshot);
        branchHistory[branch].push_back(snapshot);
        taskHistory[taskType].push_back(snapshot);
        
        // Maintain history size limits
        maintainHistoryLimits();
        
        // Trigger analysis if we have enough data
        if (performanceHistory.size() % 50 == 0) {
            performPeriodicAnalysis();
        }
    }
    
    void captureEnvironmentFactors(PerformanceSnapshot& snapshot) {
        // System load factor (simulated)
        snapshot.environmentFactors["system_load"] = 0.3 + (rand() % 70) / 100.0;
        
        // Memory pressure factor
        snapshot.environmentFactors["memory_pressure"] = 0.2 + (rand() % 60) / 100.0;
        
        // Time of day factor (affects some operations)
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        auto* tm = std::localtime(&time_t);
        snapshot.environmentFactors["time_factor"] = tm->tm_hour / 24.0;
        
        // Task complexity factor (based on recent task mix)
        snapshot.environmentFactors["task_complexity"] = calculateTaskComplexity();
    }
    
    double calculateTaskComplexity() {
        if (performanceHistory.size() < 10) return 0.5;
        
        // Look at last 10 operations
        auto recent = performanceHistory.end() - std::min(10UL, performanceHistory.size());
        std::map<BranchConfigurationSystem::TaskType, int> taskCounts;
        
        for (auto it = recent; it != performanceHistory.end(); ++it) {
            taskCounts[it->taskType]++;
        }
        
        // More diverse task mix = higher complexity
        double diversity = taskCounts.size() / 6.0; // 6 total task types
        return std::min(1.0, diversity * 1.5);
    }
    
    void maintainHistoryLimits() {
        if (performanceHistory.size() > maxHistorySize) {
            performanceHistory.erase(performanceHistory.begin(), 
                                   performanceHistory.begin() + (maxHistorySize / 10));
        }
        
        for (auto& [branch, history] : branchHistory) {
            if (history.size() > maxHistorySize / 8) {
                history.erase(history.begin(), history.begin() + (maxHistorySize / 80));
            }
        }
        
        for (auto& [taskType, history] : taskHistory) {
            if (history.size() > maxHistorySize / 6) {
                history.erase(history.begin(), history.begin() + (maxHistorySize / 60));
            }
        }
    }
    
    BranchEffectivenessReport analyzeBranchEffectiveness(BranchConfigurationSystem::ConfigurationBranch branch) {
        BranchEffectivenessReport report;
        report.branch = branch;
        
        if (branchHistory[branch].size() < minSampleSize) {
            report.overallScore = 0.5; // Default score for insufficient data
            report.recommendations.push_back("Insufficient data for analysis. Needs more usage.");
            return report;
        }
        
        auto& history = branchHistory[branch];
        report.totalUsage = history.size();
        
        // Extract effectiveness values for trend analysis
        std::vector<double> effectivenessValues;
        std::map<BranchConfigurationSystem::TaskType, std::vector<double>> taskEffectiveness;
        
        for (const auto& snapshot : history) {
            double adjustedEffectiveness = adjustForEnvironment(snapshot);
            effectivenessValues.push_back(adjustedEffectiveness);
            taskEffectiveness[snapshot.taskType].push_back(adjustedEffectiveness);
        }
        
        // Remove outliers
        removeOutliers(effectivenessValues);
        
        // Calculate overall metrics
        report.overallScore = std::accumulate(effectivenessValues.begin(), 
                                            effectivenessValues.end(), 0.0) / effectivenessValues.size();
        
        // Analyze overall trend
        report.overallTrend.calculateTrend(effectivenessValues);
        
        // Analyze task-specific performance
        for (const auto& [taskType, values] : taskEffectiveness) {
            if (values.size() >= minSampleSize) {
                std::vector<double> cleanValues = values;
                removeOutliers(cleanValues);
                
                report.taskSpecificScores[taskType] = 
                    std::accumulate(cleanValues.begin(), cleanValues.end(), 0.0) / cleanValues.size();
                
                report.taskTrends[taskType].calculateTrend(cleanValues);
            }
        }
        
        // Calculate stability and adaptability
        calculateStabilityAndAdaptability(report, history);
        
        // Identify strengths and weaknesses
        identifyStrengthsAndWeaknesses(report);
        
        // Generate recommendations
        generateRecommendations(report);
        
        return report;
    }
    
    double adjustForEnvironment(const PerformanceSnapshot& snapshot) {
        double effectiveness = snapshot.metrics.overallEffectiveness;
        
        // Adjust for system load (high load should reduce apparent effectiveness)
        double loadFactor = 1.0 - (snapshot.environmentFactors.at("system_load") - 0.5) * 0.2;
        
        // Adjust for memory pressure
        double memoryFactor = 1.0 - (snapshot.environmentFactors.at("memory_pressure") - 0.5) * 0.15;
        
        // Adjust for task complexity
        double complexityFactor = 1.0 + (snapshot.environmentFactors.at("task_complexity") - 0.5) * 0.1;
        
        return effectiveness * loadFactor * memoryFactor * complexityFactor;
    }
    
    void removeOutliers(std::vector<double>& values) {
        if (values.size() < 4) return;
        
        double mean = std::accumulate(values.begin(), values.end(), 0.0) / values.size();
        double variance = 0.0;
        
        for (double val : values) {
            variance += (val - mean) * (val - mean);
        }
        variance /= values.size();
        double stddev = std::sqrt(variance);
        
        values.erase(std::remove_if(values.begin(), values.end(),
            [mean, stddev, this](double val) {
                return std::abs(val - mean) > outlierThreshold * stddev;
            }), values.end());
    }
    
    void calculateStabilityAndAdaptability(BranchEffectivenessReport& report, 
                                         const std::vector<PerformanceSnapshot>& history) {
        // Stability: consistency of performance (low variance is good)
        std::vector<double> effectivenessValues;
        for (const auto& snapshot : history) {
            effectivenessValues.push_back(snapshot.metrics.overallEffectiveness);
        }
        
        if (!effectivenessValues.empty()) {
            double mean = std::accumulate(effectivenessValues.begin(), effectivenessValues.end(), 0.0) 
                         / effectivenessValues.size();
            double variance = 0.0;
            for (double val : effectivenessValues) {
                variance += (val - mean) * (val - mean);
            }
            variance /= effectivenessValues.size();
            
            // Lower variance = higher stability
            report.stabilityScore = 1.0 / (1.0 + variance * 10.0);
        }
        
        // Adaptability: how well it performs across different task types
        std::set<BranchConfigurationSystem::TaskType> uniqueTasks;
        for (const auto& snapshot : history) {
            uniqueTasks.insert(snapshot.taskType);
        }
        
        // More task types with good performance = higher adaptability
        double taskDiversity = uniqueTasks.size() / 6.0; // 6 total task types
        double avgTaskPerformance = 0.0;
        int taskCount = 0;
        
        for (const auto& [taskType, score] : report.taskSpecificScores) {
            avgTaskPerformance += score;
            taskCount++;
        }
        
        if (taskCount > 0) {
            avgTaskPerformance /= taskCount;
            report.adaptabilityScore = taskDiversity * avgTaskPerformance;
        }
    }
    
    void identifyStrengthsAndWeaknesses(BranchEffectivenessReport& report) {
        // Identify strongest task types
        double maxScore = 0.0;
        BranchConfigurationSystem::TaskType strongestTask;
        
        for (const auto& [taskType, score] : report.taskSpecificScores) {
            if (score > maxScore) {
                maxScore = score;
                strongestTask = taskType;
            }
        }
        
        if (maxScore > 0.7) {
            report.strengthMetrics["primary_strength"] = maxScore;
        }
        
        // Identify weakest task types
        double minScore = 1.0;
        BranchConfigurationSystem::TaskType weakestTask;
        
        for (const auto& [taskType, score] : report.taskSpecificScores) {
            if (score < minScore) {
                minScore = score;
                weakestTask = taskType;
            }
        }
        
        if (minScore < 0.5) {
            report.weaknessMetrics["primary_weakness"] = minScore;
        }
        
        // Performance trend strength/weakness
        if (report.overallTrend.trend == "IMPROVING" && report.overallTrend.confidence > 0.7) {
            report.strengthMetrics["positive_trend"] = report.overallTrend.slope;
        } else if (report.overallTrend.trend == "DECLINING" && report.overallTrend.confidence > 0.7) {
            report.weaknessMetrics["negative_trend"] = std::abs(report.overallTrend.slope);
        }
        
        // Stability as strength or weakness
        if (report.stabilityScore > 0.8) {
            report.strengthMetrics["high_stability"] = report.stabilityScore;
        } else if (report.stabilityScore < 0.4) {
            report.weaknessMetrics["low_stability"] = 1.0 - report.stabilityScore;
        }
    }
    
    void generateRecommendations(BranchEffectivenessReport& report) {
        // Performance-based recommendations
        if (report.overallScore < 0.5) {
            report.recommendations.push_back("CRITICAL: Overall performance below threshold. Consider parameter optimization.");
        } else if (report.overallScore < 0.7) {
            report.recommendations.push_back("MODERATE: Performance could be improved through tuning.");
        }
        
        // Trend-based recommendations
        if (report.overallTrend.trend == "DECLINING" && report.overallTrend.confidence > 0.6) {
            report.recommendations.push_back("WARNING: Declining performance trend detected. Investigate recent changes.");
        } else if (report.overallTrend.trend == "IMPROVING" && report.overallTrend.confidence > 0.6) {
            report.recommendations.push_back("POSITIVE: Performance improving. Current configuration is effective.");
        }
        
        // Stability recommendations
        if (report.stabilityScore < 0.4) {
            report.recommendations.push_back("INSTABILITY: High performance variance. Consider more conservative parameters.");
        }
        
        // Adaptability recommendations
        if (report.adaptabilityScore < 0.3) {
            report.recommendations.push_back("LIMITED SCOPE: Poor performance across task types. Specialized optimization needed.");
        }
        
        // Task-specific recommendations
        for (const auto& [taskType, trend] : report.taskTrends) {
            if (trend.trend == "DECLINING" && trend.confidence > 0.7) {
                report.recommendations.push_back("TASK DECLINE: " + taskTypeToString(taskType) + 
                                               " performance declining. Task-specific optimization required.");
            }
        }
        
        // Usage-based recommendations
        if (report.totalUsage < 10) {
            report.recommendations.push_back("INSUFFICIENT DATA: Increase usage for more reliable analysis.");
        }
    }
    
    void performPeriodicAnalysis() {
        std::cout << "\n[PERIODIC ANALYSIS] Analyzing system effectiveness..." << std::endl;
        
        // Analyze all branches
        for (auto branch : {
            BranchConfigurationSystem::ConfigurationBranch::STEALTH_FOCUSED,
            BranchConfigurationSystem::ConfigurationBranch::PERFORMANCE_OPTIMIZED,
            BranchConfigurationSystem::ConfigurationBranch::MEMORY_EFFICIENT,
            BranchConfigurationSystem::ConfigurationBranch::ANTI_DETECTION,
            BranchConfigurationSystem::ConfigurationBranch::HYBRID_ADAPTIVE,
            BranchConfigurationSystem::ConfigurationBranch::ULTRA_PARANOID,
            BranchConfigurationSystem::ConfigurationBranch::LIGHTWEIGHT,
            BranchConfigurationSystem::ConfigurationBranch::HEAVY_OBFUSCATION
        }) {
            auto report = analyzeBranchEffectiveness(branch);
            if (report.totalUsage > 0) {
                printBranchReport(report);
            }
        }
        
        // Generate system-wide insights
        generateSystemInsights();
    }
    
    void printBranchReport(const BranchEffectivenessReport& report) {
        std::cout << "\n--- " << branchToString(report.branch) << " Analysis ---" << std::endl;
        std::cout << "Overall Score: " << std::fixed << std::setprecision(3) << report.overallScore << std::endl;
        std::cout << "Usage Count: " << report.totalUsage << std::endl;
        std::cout << "Stability: " << report.stabilityScore << std::endl;
        std::cout << "Adaptability: " << report.adaptabilityScore << std::endl;
        std::cout << "Trend: " << report.overallTrend.trend 
                  << " (confidence: " << report.overallTrend.confidence << ")" << std::endl;
        
        if (!report.taskSpecificScores.empty()) {
            std::cout << "Task Performance:" << std::endl;
            for (const auto& [taskType, score] : report.taskSpecificScores) {
                std::cout << "  " << taskTypeToString(taskType) << ": " << score << std::endl;
            }
        }
        
        if (!report.recommendations.empty()) {
            std::cout << "Recommendations:" << std::endl;
            for (const auto& rec : report.recommendations) {
                std::cout << "  - " << rec << std::endl;
            }
        }
    }
    
    void generateSystemInsights() {
        if (performanceHistory.size() < 20) return;
        
        std::cout << "\n=== SYSTEM-WIDE INSIGHTS ===" << std::endl;
        
        // Overall system performance trend
        std::vector<double> recentPerformance;
        auto recent = performanceHistory.end() - std::min(50UL, performanceHistory.size());
        
        for (auto it = recent; it != performanceHistory.end(); ++it) {
            recentPerformance.push_back(it->metrics.overallEffectiveness);
        }
        
        TrendAnalysis systemTrend;
        systemTrend.calculateTrend(recentPerformance);
        
        std::cout << "System Trend: " << systemTrend.trend 
                  << " (confidence: " << systemTrend.confidence << ")" << std::endl;
        std::cout << "Average Performance: " << systemTrend.mean << std::endl;
        
        // Identify most and least effective branches
        std::map<BranchConfigurationSystem::ConfigurationBranch, double> branchAverages;
        
        for (const auto& [branch, history] : branchHistory) {
            if (history.size() >= minSampleSize) {
                double sum = 0.0;
                for (const auto& snapshot : history) {
                    sum += snapshot.metrics.overallEffectiveness;
                }
                branchAverages[branch] = sum / history.size();
            }
        }
        
        if (!branchAverages.empty()) {
            auto maxBranch = std::max_element(branchAverages.begin(), branchAverages.end(),
                [](const auto& a, const auto& b) { return a.second < b.second; });
            
            auto minBranch = std::min_element(branchAverages.begin(), branchAverages.end(),
                [](const auto& a, const auto& b) { return a.second < b.second; });
            
            std::cout << "Most Effective Branch: " << branchToString(maxBranch->first) 
                      << " (" << maxBranch->second << ")" << std::endl;
            std::cout << "Least Effective Branch: " << branchToString(minBranch->first) 
                      << " (" << minBranch->second << ")" << std::endl;
        }
        
        std::cout << "============================\n" << std::endl;
    }
    
    // Utility methods
    std::string branchToString(BranchConfigurationSystem::ConfigurationBranch branch) {
        switch(branch) {
            case BranchConfigurationSystem::ConfigurationBranch::STEALTH_FOCUSED: return "STEALTH_FOCUSED";
            case BranchConfigurationSystem::ConfigurationBranch::PERFORMANCE_OPTIMIZED: return "PERFORMANCE_OPTIMIZED";
            case BranchConfigurationSystem::ConfigurationBranch::MEMORY_EFFICIENT: return "MEMORY_EFFICIENT";
            case BranchConfigurationSystem::ConfigurationBranch::ANTI_DETECTION: return "ANTI_DETECTION";
            case BranchConfigurationSystem::ConfigurationBranch::HYBRID_ADAPTIVE: return "HYBRID_ADAPTIVE";
            case BranchConfigurationSystem::ConfigurationBranch::ULTRA_PARANOID: return "ULTRA_PARANOID";
            case BranchConfigurationSystem::ConfigurationBranch::LIGHTWEIGHT: return "LIGHTWEIGHT";
            case BranchConfigurationSystem::ConfigurationBranch::HEAVY_OBFUSCATION: return "HEAVY_OBFUSCATION";
            default: return "UNKNOWN";
        }
    }
    
    std::string taskTypeToString(BranchConfigurationSystem::TaskType taskType) {
        switch(taskType) {
            case BranchConfigurationSystem::TaskType::ENCRYPTION: return "ENCRYPTION";
            case BranchConfigurationSystem::TaskType::STUB_GENERATION: return "STUB_GENERATION";
            case BranchConfigurationSystem::TaskType::PAYLOAD_EMBEDDING: return "PAYLOAD_EMBEDDING";
            case BranchConfigurationSystem::TaskType::DROPPER_CREATION: return "DROPPER_CREATION";
            case BranchConfigurationSystem::TaskType::OBFUSCATION: return "OBFUSCATION";
            case BranchConfigurationSystem::TaskType::PAYLOAD_DELIVERY: return "PAYLOAD_DELIVERY";
            default: return "UNKNOWN";
        }
    }
    
    // Export analysis data
    void exportAnalysisReport(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) return;
        
        file << "EFFECTIVENESS ANALYSIS REPORT\n";
        file << "Generated: " << std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count() << "\n\n";
        
        for (auto branch : {
            BranchConfigurationSystem::ConfigurationBranch::STEALTH_FOCUSED,
            BranchConfigurationSystem::ConfigurationBranch::PERFORMANCE_OPTIMIZED,
            BranchConfigurationSystem::ConfigurationBranch::MEMORY_EFFICIENT,
            BranchConfigurationSystem::ConfigurationBranch::ANTI_DETECTION,
            BranchConfigurationSystem::ConfigurationBranch::HYBRID_ADAPTIVE,
            BranchConfigurationSystem::ConfigurationBranch::ULTRA_PARANOID,
            BranchConfigurationSystem::ConfigurationBranch::LIGHTWEIGHT,
            BranchConfigurationSystem::ConfigurationBranch::HEAVY_OBFUSCATION
        }) {
            if (branchHistory[branch].size() >= minSampleSize) {
                auto report = analyzeBranchEffectiveness(branch);
                
                file << "BRANCH: " << branchToString(branch) << "\n";
                file << "Overall Score: " << report.overallScore << "\n";
                file << "Stability: " << report.stabilityScore << "\n";
                file << "Adaptability: " << report.adaptabilityScore << "\n";
                file << "Usage: " << report.totalUsage << "\n";
                file << "Trend: " << report.overallTrend.trend << "\n";
                
                for (const auto& rec : report.recommendations) {
                    file << "Recommendation: " << rec << "\n";
                }
                file << "\n";
            }
        }
        
        file.close();
    }
};

// Global instance for integration
static EffectivenessAnalyzer g_effectivenessAnalyzer;