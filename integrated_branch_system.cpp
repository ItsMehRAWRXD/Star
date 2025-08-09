#include <iostream>
#include <string>
#include <memory>
#include <chrono>
#include <map>
#include <functional>
#include "branch_config_system.cpp"
#include "effectiveness_analyzer.cpp"

class IntegratedBranchSystem {
public:
    struct TaskContext {
        BranchConfigurationSystem::TaskType taskType;
        std::string inputData;
        std::map<std::string, std::string> additionalParameters;
        std::chrono::high_resolution_clock::time_point startTime;
        size_t inputSize = 0;
        
        TaskContext(BranchConfigurationSystem::TaskType type, const std::string& input) 
            : taskType(type), inputData(input), startTime(std::chrono::high_resolution_clock::now()) {
            inputSize = input.size();
        }
    };
    
    struct TaskResult {
        std::string outputData;
        bool success = false;
        std::string errorMessage;
        BranchConfigurationSystem::EffectivenessMetrics metrics;
        BranchConfigurationSystem::ConfigurationBranch usedBranch;
        std::chrono::duration<double> executionTime;
        size_t outputSize = 0;
        
        TaskResult() {
            metrics.executionTime = 0.0;
            metrics.memoryUsage = 0.0;
            metrics.detectionRate = 0.0;
            metrics.successRate = 0.0;
            metrics.stealthScore = 0.0;
        }
    };

private:
    BranchConfigurationSystem& branchConfig;
    EffectivenessAnalyzer& analyzer;
    
    // Performance tracking
    std::map<BranchConfigurationSystem::TaskType, std::vector<double>> recentPerformance;
    std::chrono::high_resolution_clock::time_point lastOptimization;
    int taskCount = 0;
    
public:
    IntegratedBranchSystem() : branchConfig(g_branchConfig), analyzer(g_effectivenessAnalyzer) {
        lastOptimization = std::chrono::high_resolution_clock::now();
    }
    
    // Main task execution interface
    TaskResult executeTask(TaskContext& context) {
        TaskResult result;
        
        // Configure system for the task
        branchConfig.configureForTask(context.taskType);
        auto currentBranch = branchConfig.getCurrentConfiguration().branch;
        result.usedBranch = currentBranch;
        
        std::cout << "[INTEGRATED SYSTEM] Executing " << taskTypeToString(context.taskType) 
                  << " with branch: " << branchToString(currentBranch) << std::endl;
        
        // Execute task based on type
        try {
            switch(context.taskType) {
                case BranchConfigurationSystem::TaskType::ENCRYPTION:
                    result = executeEncryption(context);
                    break;
                case BranchConfigurationSystem::TaskType::STUB_GENERATION:
                    result = executeStubGeneration(context);
                    break;
                case BranchConfigurationSystem::TaskType::PAYLOAD_EMBEDDING:
                    result = executePayloadEmbedding(context);
                    break;
                case BranchConfigurationSystem::TaskType::DROPPER_CREATION:
                    result = executeDropperCreation(context);
                    break;
                case BranchConfigurationSystem::TaskType::OBFUSCATION:
                    result = executeObfuscation(context);
                    break;
                case BranchConfigurationSystem::TaskType::PAYLOAD_DELIVERY:
                    result = executePayloadDelivery(context);
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
        
        // Calculate execution time
        auto endTime = std::chrono::high_resolution_clock::now();
        result.executionTime = endTime - context.startTime;
        result.metrics.executionTime = result.executionTime.count();
        
        // Calculate effectiveness metrics
        calculateEffectivenessMetrics(context, result);
        
        // Record performance for analysis
        recordTaskPerformance(context, result);
        
        // Check if rebranching is needed
        evaluateRebranchingNeed(context, result);
        
        taskCount++;
        return result;
    }
    
    // Encryption task implementation
    TaskResult executeEncryption(TaskContext& context) {
        TaskResult result;
        
        std::string encryptionMode = branchConfig.getParameter<std::string>("encryption_mode", "AES256_CBC");
        int obfuscationLayers = branchConfig.getParameter<int>("obfuscation_layers", 5);
        bool usePolymorphism = branchConfig.getParameter<bool>("use_polymorphism", false);
        
        std::cout << "[ENCRYPTION] Mode: " << encryptionMode 
                  << " | Layers: " << obfuscationLayers 
                  << " | Polymorphic: " << (usePolymorphism ? "Yes" : "No") << std::endl;
        
        // Simulate encryption process with branch-specific parameters
        std::string encrypted = context.inputData;
        
        // Apply obfuscation layers
        for (int i = 0; i < obfuscationLayers; i++) {
            encrypted = applyObfuscationLayer(encrypted, i);
        }
        
        // Apply encryption algorithm
        encrypted = applyEncryption(encrypted, encryptionMode);
        
        // Apply polymorphism if enabled
        if (usePolymorphism) {
            encrypted = applyPolymorphicTransformation(encrypted);
        }
        
        result.outputData = encrypted;
        result.outputSize = encrypted.size();
        result.success = true;
        
        // Calculate stealth score based on configuration
        result.metrics.stealthScore = calculateStealthScore(encryptionMode, obfuscationLayers, usePolymorphism);
        
        return result;
    }
    
    // Stub generation task implementation
    TaskResult executeStubGeneration(TaskContext& context) {
        TaskResult result;
        
        std::string stubTemplate = branchConfig.getParameter<std::string>("stub_template", "basic");
        int mutationRounds = branchConfig.getParameter<int>("mutation_rounds", 10);
        bool runtimePolymorphism = branchConfig.getParameter<bool>("runtime_polymorphism", false);
        bool apiObfuscation = branchConfig.getParameter<bool>("api_call_obfuscation", false);
        
        std::cout << "[STUB GENERATION] Template: " << stubTemplate 
                  << " | Mutations: " << mutationRounds 
                  << " | Runtime Poly: " << (runtimePolymorphism ? "Yes" : "No") << std::endl;
        
        // Generate stub based on configuration
        std::string stub = generateStubFromTemplate(stubTemplate);
        
        // Apply mutations
        for (int i = 0; i < mutationRounds; i++) {
            stub = applyMutation(stub, i);
        }
        
        // Add runtime polymorphism
        if (runtimePolymorphism) {
            stub = addRuntimePolymorphism(stub);
        }
        
        // Obfuscate API calls
        if (apiObfuscation) {
            stub = obfuscateApiCalls(stub);
        }
        
        result.outputData = stub;
        result.outputSize = stub.size();
        result.success = true;
        
        // Calculate detection avoidance score
        result.metrics.detectionRate = calculateDetectionRate(stubTemplate, mutationRounds, 
                                                            runtimePolymorphism, apiObfuscation);
        
        return result;
    }
    
    // Payload embedding task implementation  
    TaskResult executePayloadEmbedding(TaskContext& context) {
        TaskResult result;
        
        int codeMutationDepth = branchConfig.getParameter<int>("code_mutation_depth", 5);
        bool instructionSubstitution = branchConfig.getParameter<bool>("instruction_substitution", false);
        bool opaquePredicates = branchConfig.getParameter<bool>("opaque_predicates", false);
        bool deadCodeInsertion = branchConfig.getParameter<bool>("dead_code_insertion", false);
        
        std::cout << "[PAYLOAD EMBEDDING] Mutation Depth: " << codeMutationDepth 
                  << " | Inst. Sub: " << (instructionSubstitution ? "Yes" : "No") 
                  << " | Opaque Pred: " << (opaquePredicates ? "Yes" : "No") << std::endl;
        
        std::string embedded = context.inputData;
        
        // Apply code mutations
        for (int i = 0; i < codeMutationDepth; i++) {
            embedded = applyCodeMutation(embedded, i);
        }
        
        // Apply instruction substitution
        if (instructionSubstitution) {
            embedded = substituteInstructions(embedded);
        }
        
        // Insert opaque predicates
        if (opaquePredicates) {
            embedded = insertOpaquePredicates(embedded);
        }
        
        // Insert dead code
        if (deadCodeInsertion) {
            embedded = insertDeadCode(embedded);
        }
        
        result.outputData = embedded;
        result.outputSize = embedded.size();
        result.success = true;
        
        // Calculate obfuscation strength
        result.metrics.stealthScore = calculateObfuscationStrength(codeMutationDepth, 
                                                                  instructionSubstitution,
                                                                  opaquePredicates, deadCodeInsertion);
        
        return result;
    }
    
    // Dropper creation task implementation
    TaskResult executeDropperCreation(TaskContext& context) {
        TaskResult result;
        
        int adaptationCycles = branchConfig.getParameter<int>("adaptation_cycles", 3);
        bool dynamicAdjustment = branchConfig.getParameter<bool>("dynamic_adjustment", true);
        bool contextAwareness = branchConfig.getParameter<bool>("context_awareness", true);
        
        std::cout << "[DROPPER CREATION] Adaptation Cycles: " << adaptationCycles 
                  << " | Dynamic: " << (dynamicAdjustment ? "Yes" : "No") 
                  << " | Context Aware: " << (contextAwareness ? "Yes" : "No") << std::endl;
        
        std::string dropper = createBasicDropper(context.inputData);
        
        // Apply adaptation cycles
        for (int i = 0; i < adaptationCycles; i++) {
            dropper = adaptDropper(dropper, i);
        }
        
        // Add dynamic adjustment capabilities
        if (dynamicAdjustment) {
            dropper = addDynamicAdjustment(dropper);
        }
        
        // Add context awareness
        if (contextAwareness) {
            dropper = addContextAwareness(dropper);
        }
        
        result.outputData = dropper;
        result.outputSize = dropper.size();
        result.success = true;
        
        // Calculate adaptability score
        result.metrics.successRate = calculateAdaptabilityScore(adaptationCycles, 
                                                               dynamicAdjustment, contextAwareness);
        
        return result;
    }
    
    // Obfuscation task implementation
    TaskResult executeObfuscation(TaskContext& context) {
        TaskResult result;
        
        int encryptionPasses = branchConfig.getParameter<int>("encryption_passes", 3);
        int dummyOperations = branchConfig.getParameter<int>("dummy_operations", 50);
        bool tripleEncryption = branchConfig.getParameter<bool>("triple_encryption", false);
        bool fakeOperations = branchConfig.getParameter<bool>("fake_operations", false);
        bool antiDebug = branchConfig.getParameter<bool>("anti_debug", false);
        
        std::cout << "[OBFUSCATION] Encryption Passes: " << encryptionPasses 
                  << " | Dummy Ops: " << dummyOperations 
                  << " | Triple Enc: " << (tripleEncryption ? "Yes" : "No") << std::endl;
        
        std::string obfuscated = context.inputData;
        
        // Apply multiple encryption passes
        for (int i = 0; i < encryptionPasses; i++) {
            obfuscated = applyEncryptionPass(obfuscated, i);
        }
        
        // Apply triple encryption if enabled
        if (tripleEncryption) {
            obfuscated = applyTripleEncryption(obfuscated);
        }
        
        // Insert dummy operations
        for (int i = 0; i < dummyOperations; i++) {
            obfuscated = insertDummyOperation(obfuscated, i);
        }
        
        // Add fake operations
        if (fakeOperations) {
            obfuscated = addFakeOperations(obfuscated);
        }
        
        // Add anti-debug protection
        if (antiDebug) {
            obfuscated = addAntiDebugProtection(obfuscated);
        }
        
        result.outputData = obfuscated;
        result.outputSize = obfuscated.size();
        result.success = true;
        
        // Calculate paranoia level effectiveness
        result.metrics.stealthScore = calculateParanoiaEffectiveness(encryptionPasses, dummyOperations,
                                                                    tripleEncryption, fakeOperations, antiDebug);
        
        return result;
    }
    
    // Payload delivery task implementation
    TaskResult executePayloadDelivery(TaskContext& context) {
        TaskResult result;
        
        bool inlineOptimization = branchConfig.getParameter<bool>("inline_optimization", true);
        bool compressOutput = branchConfig.getParameter<bool>("compress_output", false);
        bool minimalImports = branchConfig.getParameter<bool>("minimal_imports", false);
        
        std::cout << "[PAYLOAD DELIVERY] Inline Opt: " << (inlineOptimization ? "Yes" : "No") 
                  << " | Compress: " << (compressOutput ? "Yes" : "No") 
                  << " | Minimal Imports: " << (minimalImports ? "Yes" : "No") << std::endl;
        
        std::string payload = context.inputData;
        
        // Apply inline optimization
        if (inlineOptimization) {
            payload = applyInlineOptimization(payload);
        }
        
        // Compress output
        if (compressOutput) {
            payload = compressPayload(payload);
        }
        
        // Minimize imports
        if (minimalImports) {
            payload = minimizeImports(payload);
        }
        
        result.outputData = payload;
        result.outputSize = payload.size();
        result.success = true;
        
        // Calculate delivery efficiency
        result.metrics.successRate = calculateDeliveryEfficiency(inlineOptimization, 
                                                                compressOutput, minimalImports);
        
        return result;
    }
    
    // Effectiveness calculation
    void calculateEffectivenessMetrics(const TaskContext& context, TaskResult& result) {
        // Memory usage estimation (simplified)
        result.metrics.memoryUsage = (context.inputSize + result.outputSize) / 1024.0; // KB
        
        // Success rate (based on whether task completed)
        result.metrics.successRate = result.success ? 1.0 : 0.0;
        
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
                  << " | Time: " << result.metrics.executionTime << "s" 
                  << " | Memory: " << result.metrics.memoryUsage << "KB" << std::endl;
    }
    
    // Performance recording
    void recordTaskPerformance(const TaskContext& context, const TaskResult& result) {
        // Record in branch configuration system
        branchConfig.recordTaskPerformance(context.taskType, result.metrics);
        
        // Record in effectiveness analyzer
        analyzer.recordPerformance(context.taskType, result.usedBranch, result.metrics);
        
        // Track recent performance for rebranching decisions
        recentPerformance[context.taskType].push_back(result.metrics.overallEffectiveness);
        
        // Limit recent performance history
        if (recentPerformance[context.taskType].size() > 10) {
            recentPerformance[context.taskType].erase(recentPerformance[context.taskType].begin());
        }
    }
    
    // Rebranching evaluation
    void evaluateRebranchingNeed(const TaskContext& context, const TaskResult& result) {
        // Check if performance is declining
        bool shouldRebranch = false;
        std::string reason;
        
        if (result.metrics.overallEffectiveness < 0.3) {
            shouldRebranch = true;
            reason = "Critical performance drop";
        } else if (recentPerformance[context.taskType].size() >= 5) {
            // Check trend in recent performance
            auto& recent = recentPerformance[context.taskType];
            double avgRecent = 0.0;
            for (size_t i = recent.size() - 3; i < recent.size(); i++) {
                avgRecent += recent[i];
            }
            avgRecent /= 3.0;
            
            double avgOlder = 0.0;
            for (size_t i = 0; i < 3 && i < recent.size() - 3; i++) {
                avgOlder += recent[i];
            }
            avgOlder /= std::min(3.0, (double)(recent.size() - 3));
            
            if (avgRecent < avgOlder * 0.8) {
                shouldRebranch = true;
                reason = "Declining performance trend";
            }
        }
        
        // Force rebranching periodically for exploration
        auto timeSinceLastOptimization = std::chrono::high_resolution_clock::now() - lastOptimization;
        if (std::chrono::duration_cast<std::chrono::minutes>(timeSinceLastOptimization).count() > 60) {
            if (taskCount % 200 == 0) {
                shouldRebranch = true;
                reason = "Periodic optimization";
            }
        }
        
        if (shouldRebranch) {
            std::cout << "[REBRANCHING] Triggering rebranch for " << taskTypeToString(context.taskType) 
                      << " - Reason: " << reason << std::endl;
            branchConfig.rebranch(context.taskType, reason);
            lastOptimization = std::chrono::high_resolution_clock::now();
        }
    }
    
    // Utility methods for task implementations (simplified/simulated)
    std::string applyObfuscationLayer(const std::string& input, int layer) {
        return input + "_obf" + std::to_string(layer);
    }
    
    std::string applyEncryption(const std::string& input, const std::string& mode) {
        return mode + "(" + input + ")";
    }
    
    std::string applyPolymorphicTransformation(const std::string& input) {
        return "POLY{" + input + "}";
    }
    
    std::string generateStubFromTemplate(const std::string& template_name) {
        return "STUB_" + template_name + "_GENERATED";
    }
    
    std::string applyMutation(const std::string& input, int round) {
        return input + "_mut" + std::to_string(round);
    }
    
    std::string addRuntimePolymorphism(const std::string& input) {
        return "RUNTIME_POLY{" + input + "}";
    }
    
    std::string obfuscateApiCalls(const std::string& input) {
        return "API_OBF{" + input + "}";
    }
    
    std::string applyCodeMutation(const std::string& input, int depth) {
        return input + "_code_mut" + std::to_string(depth);
    }
    
    std::string substituteInstructions(const std::string& input) {
        return "INST_SUB{" + input + "}";
    }
    
    std::string insertOpaquePredicates(const std::string& input) {
        return "OPAQUE{" + input + "}";
    }
    
    std::string insertDeadCode(const std::string& input) {
        return "DEAD_CODE{" + input + "}";
    }
    
    std::string createBasicDropper(const std::string& input) {
        return "DROPPER{" + input + "}";
    }
    
    std::string adaptDropper(const std::string& input, int cycle) {
        return input + "_adapt" + std::to_string(cycle);
    }
    
    std::string addDynamicAdjustment(const std::string& input) {
        return "DYNAMIC{" + input + "}";
    }
    
    std::string addContextAwareness(const std::string& input) {
        return "CONTEXT{" + input + "}";
    }
    
    std::string applyEncryptionPass(const std::string& input, int pass) {
        return input + "_enc_pass" + std::to_string(pass);
    }
    
    std::string applyTripleEncryption(const std::string& input) {
        return "TRIPLE_ENC{" + input + "}";
    }
    
    std::string insertDummyOperation(const std::string& input, int op) {
        return input + "_dummy" + std::to_string(op);
    }
    
    std::string addFakeOperations(const std::string& input) {
        return "FAKE_OPS{" + input + "}";
    }
    
    std::string addAntiDebugProtection(const std::string& input) {
        return "ANTI_DEBUG{" + input + "}";
    }
    
    std::string applyInlineOptimization(const std::string& input) {
        return "INLINE_OPT{" + input + "}";
    }
    
    std::string compressPayload(const std::string& input) {
        return "COMPRESSED{" + input + "}";
    }
    
    std::string minimizeImports(const std::string& input) {
        return "MIN_IMPORTS{" + input + "}";
    }
    
    // Metric calculation helpers
    double calculateStealthScore(const std::string& mode, int layers, bool poly) {
        double base = 0.5;
        if (mode.find("AES256") != std::string::npos) base += 0.2;
        if (mode.find("GCM") != std::string::npos) base += 0.1;
        base += std::min(0.3, layers * 0.03);
        if (poly) base += 0.1;
        return std::min(1.0, base);
    }
    
    double calculateDetectionRate(const std::string& template_name, int mutations, bool runtime, bool api) {
        double base = 0.3;
        if (template_name == "ultra_stealth") base -= 0.1;
        if (template_name == "advanced_evasion") base -= 0.15;
        base -= std::min(0.2, mutations * 0.01);
        if (runtime) base -= 0.05;
        if (api) base -= 0.05;
        return std::max(0.0, base);
    }
    
    double calculateObfuscationStrength(int depth, bool inst, bool opaque, bool dead) {
        double base = 0.4;
        base += std::min(0.3, depth * 0.05);
        if (inst) base += 0.1;
        if (opaque) base += 0.1;
        if (dead) base += 0.1;
        return std::min(1.0, base);
    }
    
    double calculateAdaptabilityScore(int cycles, bool dynamic, bool context) {
        double base = 0.6;
        base += std::min(0.2, cycles * 0.05);
        if (dynamic) base += 0.1;
        if (context) base += 0.1;
        return std::min(1.0, base);
    }
    
    double calculateParanoiaEffectiveness(int passes, int dummies, bool triple, bool fake, bool debug) {
        double base = 0.5;
        base += std::min(0.2, passes * 0.05);
        base += std::min(0.1, dummies * 0.001);
        if (triple) base += 0.1;
        if (fake) base += 0.05;
        if (debug) base += 0.05;
        return std::min(1.0, base);
    }
    
    double calculateDeliveryEfficiency(bool inline_opt, bool compress, bool minimal) {
        double base = 0.7;
        if (inline_opt) base += 0.1;
        if (compress) base += 0.1;
        if (minimal) base += 0.1;
        return std::min(1.0, base);
    }
    
    // Utility string conversion methods
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
    
    // Status and analysis methods
    void printSystemStatus() {
        branchConfig.printStatus();
        
        std::cout << "\n=== INTEGRATED SYSTEM STATUS ===" << std::endl;
        std::cout << "Total Tasks Executed: " << taskCount << std::endl;
        std::cout << "Last Optimization: " << std::chrono::duration_cast<std::chrono::minutes>(
            std::chrono::high_resolution_clock::now() - lastOptimization).count() << " minutes ago" << std::endl;
        
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
        std::cout << "================================\n" << std::endl;
    }
    
    void exportComprehensiveReport(const std::string& filename) {
        analyzer.exportAnalysisReport(filename + "_analysis.txt");
        
        std::ofstream file(filename + "_integrated.txt");
        if (file.is_open()) {
            file << "INTEGRATED BRANCH SYSTEM REPORT\n";
            file << "Generated: " << std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now().time_since_epoch()).count() << "\n\n";
            
            file << "Total Tasks Executed: " << taskCount << "\n";
            
            file << "\nRecent Performance by Task Type:\n";
            for (const auto& [taskType, performance] : recentPerformance) {
                if (!performance.empty()) {
                    double avg = 0.0;
                    for (double p : performance) avg += p;
                    avg /= performance.size();
                    
                    file << taskTypeToString(taskType) << ": " << avg 
                         << " (" << performance.size() << " samples)\n";
                }
            }
            
            file.close();
        }
    }
};

// Global instance for easy access
static IntegratedBranchSystem g_integratedSystem;