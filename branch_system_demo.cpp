#include <iostream>
#include <vector>
#include <random>
#include <thread>
#include <chrono>
#include "integrated_branch_system.cpp"

class BranchSystemDemo {
private:
    IntegratedBranchSystem& system;
    std::mt19937 rng;
    
public:
    BranchSystemDemo() : system(g_integratedSystem) {
        std::random_device rd;
        rng.seed(rd());
    }
    
    void runComprehensiveDemo() {
        std::cout << "=== BRANCH CONFIGURATION SYSTEM DEMONSTRATION ===" << std::endl;
        std::cout << "This demo shows the system configuring itself before tasks" << std::endl;
        std::cout << "and rebranding to improve effectiveness.\n" << std::endl;
        
        // Demonstrate various task types
        runEncryptionDemo();
        runStubGenerationDemo();
        runPayloadEmbeddingDemo();
        runDropperCreationDemo();
        runObfuscationDemo();
        runPayloadDeliveryDemo();
        
        // Show adaptation and optimization
        runAdaptationDemo();
        
        // Show rebranching under poor performance
        runRebranchingDemo();
        
        // Final system status
        std::cout << "\n=== FINAL SYSTEM STATUS ===" << std::endl;
        system.printSystemStatus();
        
        // Export comprehensive report
        system.exportComprehensiveReport("branch_system_report");
        std::cout << "Comprehensive report exported to branch_system_report_*.txt" << std::endl;
    }
    
    void runEncryptionDemo() {
        std::cout << "\n--- ENCRYPTION TASK DEMONSTRATION ---" << std::endl;
        
        for (int i = 0; i < 5; i++) {
            std::string inputData = "SensitiveData_" + std::to_string(i) + "_" + generateRandomString(50);
            
            IntegratedBranchSystem::TaskContext context(
                BranchConfigurationSystem::TaskType::ENCRYPTION, inputData);
            
            auto result = system.executeTask(context);
            
            std::cout << "Encryption " << (i+1) << " - Success: " << (result.success ? "YES" : "NO") 
                      << " | Effectiveness: " << result.metrics.overallEffectiveness
                      << " | Branch: " << system.branchToString(result.usedBranch) << std::endl;
            
            // Simulate varying performance
            if (i == 2) {
                // Simulate a poor performance scenario
                result.metrics.overallEffectiveness = 0.2;
                result.metrics.calculateOverall();
                g_branchConfig.recordTaskPerformance(context.taskType, result.metrics);
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    
    void runStubGenerationDemo() {
        std::cout << "\n--- STUB GENERATION TASK DEMONSTRATION ---" << std::endl;
        
        for (int i = 0; i < 5; i++) {
            std::string inputData = "PayloadStub_" + std::to_string(i) + "_" + generateRandomString(30);
            
            IntegratedBranchSystem::TaskContext context(
                BranchConfigurationSystem::TaskType::STUB_GENERATION, inputData);
            
            auto result = system.executeTask(context);
            
            std::cout << "Stub Generation " << (i+1) << " - Success: " << (result.success ? "YES" : "NO") 
                      << " | Effectiveness: " << result.metrics.overallEffectiveness
                      << " | Branch: " << system.branchToString(result.usedBranch) << std::endl;
            
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    
    void runPayloadEmbeddingDemo() {
        std::cout << "\n--- PAYLOAD EMBEDDING TASK DEMONSTRATION ---" << std::endl;
        
        for (int i = 0; i < 5; i++) {
            std::string inputData = "EmbeddedPayload_" + std::to_string(i) + "_" + generateRandomString(75);
            
            IntegratedBranchSystem::TaskContext context(
                BranchConfigurationSystem::TaskType::PAYLOAD_EMBEDDING, inputData);
            
            auto result = system.executeTask(context);
            
            std::cout << "Payload Embedding " << (i+1) << " - Success: " << (result.success ? "YES" : "NO") 
                      << " | Effectiveness: " << result.metrics.overallEffectiveness
                      << " | Branch: " << system.branchToString(result.usedBranch) << std::endl;
            
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    
    void runDropperCreationDemo() {
        std::cout << "\n--- DROPPER CREATION TASK DEMONSTRATION ---" << std::endl;
        
        for (int i = 0; i < 5; i++) {
            std::string inputData = "DropperCode_" + std::to_string(i) + "_" + generateRandomString(40);
            
            IntegratedBranchSystem::TaskContext context(
                BranchConfigurationSystem::TaskType::DROPPER_CREATION, inputData);
            
            auto result = system.executeTask(context);
            
            std::cout << "Dropper Creation " << (i+1) << " - Success: " << (result.success ? "YES" : "NO") 
                      << " | Effectiveness: " << result.metrics.overallEffectiveness
                      << " | Branch: " << system.branchToString(result.usedBranch) << std::endl;
            
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    
    void runObfuscationDemo() {
        std::cout << "\n--- OBFUSCATION TASK DEMONSTRATION ---" << std::endl;
        
        for (int i = 0; i < 5; i++) {
            std::string inputData = "ObfuscationTarget_" + std::to_string(i) + "_" + generateRandomString(60);
            
            IntegratedBranchSystem::TaskContext context(
                BranchConfigurationSystem::TaskType::OBFUSCATION, inputData);
            
            auto result = system.executeTask(context);
            
            std::cout << "Obfuscation " << (i+1) << " - Success: " << (result.success ? "YES" : "NO") 
                      << " | Effectiveness: " << result.metrics.overallEffectiveness
                      << " | Branch: " << system.branchToString(result.usedBranch) << std::endl;
            
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    
    void runPayloadDeliveryDemo() {
        std::cout << "\n--- PAYLOAD DELIVERY TASK DEMONSTRATION ---" << std::endl;
        
        for (int i = 0; i < 5; i++) {
            std::string inputData = "DeliveryPayload_" + std::to_string(i) + "_" + generateRandomString(35);
            
            IntegratedBranchSystem::TaskContext context(
                BranchConfigurationSystem::TaskType::PAYLOAD_DELIVERY, inputData);
            
            auto result = system.executeTask(context);
            
            std::cout << "Payload Delivery " << (i+1) << " - Success: " << (result.success ? "YES" : "NO") 
                      << " | Effectiveness: " << result.metrics.overallEffectiveness
                      << " | Branch: " << system.branchToString(result.usedBranch) << std::endl;
            
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    
    void runAdaptationDemo() {
        std::cout << "\n--- ADAPTATION AND LEARNING DEMONSTRATION ---" << std::endl;
        std::cout << "Running mixed tasks to show adaptation..." << std::endl;
        
        std::vector<BranchConfigurationSystem::TaskType> taskTypes = {
            BranchConfigurationSystem::TaskType::ENCRYPTION,
            BranchConfigurationSystem::TaskType::STUB_GENERATION,
            BranchConfigurationSystem::TaskType::PAYLOAD_EMBEDDING,
            BranchConfigurationSystem::TaskType::DROPPER_CREATION,
            BranchConfigurationSystem::TaskType::OBFUSCATION,
            BranchConfigurationSystem::TaskType::PAYLOAD_DELIVERY
        };
        
        for (int round = 0; round < 3; round++) {
            std::cout << "\nAdaptation Round " << (round + 1) << ":" << std::endl;
            
            for (auto taskType : taskTypes) {
                std::string inputData = "AdaptiveTest_R" + std::to_string(round) + "_" + generateRandomString(25);
                
                IntegratedBranchSystem::TaskContext context(taskType, inputData);
                auto result = system.executeTask(context);
                
                std::cout << "  " << system.taskTypeToString(taskType) << " -> " 
                          << system.branchToString(result.usedBranch) 
                          << " (Effectiveness: " << result.metrics.overallEffectiveness << ")" << std::endl;
                
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
        }
    }
    
    void runRebranchingDemo() {
        std::cout << "\n--- REBRANCHING DEMONSTRATION ---" << std::endl;
        std::cout << "Simulating poor performance to trigger rebranching..." << std::endl;
        
        // Force poor performance to trigger rebranching
        for (int i = 0; i < 8; i++) {
            std::string inputData = "RebranchTest_" + std::to_string(i) + "_" + generateRandomString(20);
            
            IntegratedBranchSystem::TaskContext context(
                BranchConfigurationSystem::TaskType::ENCRYPTION, inputData);
            
            auto result = system.executeTask(context);
            
            // Simulate declining performance
            if (i >= 3) {
                BranchConfigurationSystem::EffectivenessMetrics poorMetrics;
                poorMetrics.executionTime = 2.0;
                poorMetrics.memoryUsage = 100.0;
                poorMetrics.detectionRate = 0.8;
                poorMetrics.successRate = 0.3;
                poorMetrics.stealthScore = 0.2;
                poorMetrics.calculateOverall();
                
                g_branchConfig.recordTaskPerformance(context.taskType, poorMetrics);
                g_effectivenessAnalyzer.recordPerformance(context.taskType, result.usedBranch, poorMetrics);
                
                std::cout << "Iteration " << (i+1) << " - Simulated poor performance: " 
                          << poorMetrics.overallEffectiveness << std::endl;
            } else {
                std::cout << "Iteration " << (i+1) << " - Normal performance: " 
                          << result.metrics.overallEffectiveness << std::endl;
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        // Trigger manual rebranching
        std::cout << "\nTriggering manual rebranching for demonstration..." << std::endl;
        g_branchConfig.rebranch(BranchConfigurationSystem::TaskType::ENCRYPTION, "Manual demo rebranch");
    }
    
    void runLongTermEvolutionDemo() {
        std::cout << "\n--- LONG-TERM EVOLUTION DEMONSTRATION ---" << std::endl;
        std::cout << "Running extended simulation to show system learning..." << std::endl;
        
        std::uniform_int_distribution<> taskDist(0, 5);
        std::vector<BranchConfigurationSystem::TaskType> taskTypes = {
            BranchConfigurationSystem::TaskType::ENCRYPTION,
            BranchConfigurationSystem::TaskType::STUB_GENERATION,
            BranchConfigurationSystem::TaskType::PAYLOAD_EMBEDDING,
            BranchConfigurationSystem::TaskType::DROPPER_CREATION,
            BranchConfigurationSystem::TaskType::OBFUSCATION,
            BranchConfigurationSystem::TaskType::PAYLOAD_DELIVERY
        };
        
        // Run 100 random tasks to show evolution
        for (int i = 0; i < 100; i++) {
            auto taskType = taskTypes[taskDist(rng)];
            std::string inputData = "EvolutionTest_" + std::to_string(i) + "_" + generateRandomString(15);
            
            IntegratedBranchSystem::TaskContext context(taskType, inputData);
            auto result = system.executeTask(context);
            
            // Occasionally show status
            if (i % 25 == 24) {
                std::cout << "Progress: " << (i+1) << "/100 tasks completed" << std::endl;
                std::cout << "Current branch for " << system.taskTypeToString(taskType) 
                          << ": " << system.branchToString(result.usedBranch) << std::endl;
            }
            
            // Don't overwhelm with output
            if (i % 5 == 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
        
        std::cout << "Long-term evolution simulation complete!" << std::endl;
    }
    
    void runInteractiveModeDemo() {
        std::cout << "\n--- INTERACTIVE MODE DEMONSTRATION ---" << std::endl;
        std::cout << "Showing system configuration before tasks..." << std::endl;
        
        // Show current configuration
        system.printSystemStatus();
        
        // Ask system to configure for specific task
        std::cout << "\nConfiguring for ULTRA_PARANOID obfuscation task..." << std::endl;
        
        // Force configuration to specific branch
        g_branchConfig.configureForTask(BranchConfigurationSystem::TaskType::OBFUSCATION);
        
        // Show configuration details
        auto config = g_branchConfig.getCurrentConfiguration();
        std::cout << "Current Branch: " << g_branchConfig.branchToString(config.branch) << std::endl;
        std::cout << "Encryption Passes: " << g_branchConfig.getParameter<int>("encryption_passes", 0) << std::endl;
        std::cout << "Dummy Operations: " << g_branchConfig.getParameter<int>("dummy_operations", 0) << std::endl;
        std::cout << "Triple Encryption: " << (g_branchConfig.getParameter<bool>("triple_encryption", false) ? "Yes" : "No") << std::endl;
        
        // Execute task with this configuration
        std::string inputData = "InteractiveTestData_" + generateRandomString(30);
        IntegratedBranchSystem::TaskContext context(
            BranchConfigurationSystem::TaskType::OBFUSCATION, inputData);
        
        auto result = system.executeTask(context);
        
        std::cout << "Task Result:" << std::endl;
        std::cout << "  Success: " << (result.success ? "YES" : "NO") << std::endl;
        std::cout << "  Effectiveness: " << result.metrics.overallEffectiveness << std::endl;
        std::cout << "  Execution Time: " << result.metrics.executionTime << "s" << std::endl;
        std::cout << "  Memory Usage: " << result.metrics.memoryUsage << "KB" << std::endl;
        std::cout << "  Stealth Score: " << result.metrics.stealthScore << std::endl;
    }
    
    std::string generateRandomString(size_t length) {
        static const std::string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        std::uniform_int_distribution<> dis(0, chars.size() - 1);
        
        std::string result;
        result.reserve(length);
        
        for (size_t i = 0; i < length; i++) {
            result += chars[dis(rng)];
        }
        
        return result;
    }
};

int main() {
    try {
        BranchSystemDemo demo;
        
        std::cout << "Starting Branch Configuration System Demonstration..." << std::endl;
        std::cout << "This system demonstrates self-configuring and rebrancing capabilities.\n" << std::endl;
        
        // Run full demonstration
        demo.runComprehensiveDemo();
        
        // Run interactive mode
        demo.runInteractiveModeDemo();
        
        // Optional: Run long-term evolution (commented out for brevity)
        // demo.runLongTermEvolutionDemo();
        
        std::cout << "\n=== DEMONSTRATION COMPLETE ===" << std::endl;
        std::cout << "The branch configuration system has successfully demonstrated:" << std::endl;
        std::cout << "✓ Self-configuration before undertaking tasks" << std::endl;
        std::cout << "✓ Dynamic rebraching based on effectiveness metrics" << std::endl;
        std::cout << "✓ Multiple configuration profiles for different contexts" << std::endl;
        std::cout << "✓ Self-optimization and adaptation over time" << std::endl;
        std::cout << "✓ Comprehensive effectiveness analysis and reporting" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error during demonstration: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}