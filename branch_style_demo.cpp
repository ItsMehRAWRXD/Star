#include "branch_style_plugins.h"
#include <iostream>
#include <vector>
#include <string>
#include <chrono>

using namespace BranchStylePlugins;

void demonstrateConditionalStyles() {
    std::cout << "\n=== CONDITIONAL BRANCH STYLES ===\n\n";
    
    auto& manager = BranchStyleManager::getInstance();
    
    // Example 1: Simple if-else
    auto context1 = BranchUtils::createSimpleConditional(
        "x > 10", 
        "std::cout << \"x is greater than 10\" << std::endl;",
        "std::cout << \"x is not greater than 10\" << std::endl;"
    );
    
    std::cout << "1. IfElse Style:\n";
    auto ifElsePlugin = manager.getPlugin("IfElse");
    if (ifElsePlugin) {
        std::cout << ifElsePlugin->generateBranch(context1) << "\n\n";
    }
    
    // Example 2: Ternary operator
    std::cout << "2. Ternary Style:\n";
    auto ternaryPlugin = manager.getPlugin("Ternary");
    if (ternaryPlugin) {
        std::cout << ternaryPlugin->generateBranch(context1) << "\n\n";
    }
    
    // Example 3: Guard clause
    auto context3 = BranchUtils::createFunctionContext(
        "isValid",
        "processData();",
        false
    );
    
    std::cout << "3. Guard Clause Style:\n";
    auto guardPlugin = manager.getPlugin("GuardClause");
    if (guardPlugin) {
        std::cout << guardPlugin->generateBranch(context3) << "\n\n";
    }
    
    // Example 4: Nested conditional
    std::cout << "4. Nested Conditional Style:\n";
    auto nestedPlugin = manager.getPlugin("NestedConditional");
    if (nestedPlugin) {
        std::cout << nestedPlugin->generateBranch(context1) << "\n\n";
    }
}

void demonstrateSwitchStyles() {
    std::cout << "\n=== SWITCH BRANCH STYLES ===\n\n";
    
    auto& manager = BranchStyleManager::getInstance();
    
    // Example: Switch with multiple cases
    std::vector<std::string> cases = {
        "std::cout << \"Case 0: Initialize\" << std::endl;",
        "std::cout << \"Case 1: Process\" << std::endl;",
        "std::cout << \"Case 2: Finalize\" << std::endl;"
    };
    
    auto switchContext = BranchUtils::createSwitchContext("operation", cases);
    switchContext.falseBlock = "std::cout << \"Unknown operation\" << std::endl;";
    
    std::cout << "1. Switch Style:\n";
    auto switchPlugin = manager.getPlugin("Switch");
    if (switchPlugin) {
        std::cout << switchPlugin->generateBranch(switchContext) << "\n\n";
    }
    
    std::cout << "2. Lookup Table Style:\n";
    auto lookupPlugin = manager.getPlugin("LookupTable");
    if (lookupPlugin) {
        std::cout << lookupPlugin->generateBranch(switchContext) << "\n\n";
    }
}

void demonstrateLoopStyles() {
    std::cout << "\n=== LOOP BRANCH STYLES ===\n\n";
    
    auto& manager = BranchStyleManager::getInstance();
    
    auto loopContext = BranchUtils::createLoopContext(
        "iterator",
        "10",
        "processItem(iterator);"
    );
    loopContext.condition = "iterator % 2 == 0";
    loopContext.falseBlock = "skipItem(iterator);";
    
    std::cout << "1. For Loop Style:\n";
    auto forPlugin = manager.getPlugin("ForLoop");
    if (forPlugin) {
        std::cout << forPlugin->generateBranch(loopContext) << "\n\n";
    }
    
    std::cout << "2. While Loop Style:\n";
    auto whilePlugin = manager.getPlugin("WhileLoop");
    if (whilePlugin) {
        std::cout << whilePlugin->generateBranch(loopContext) << "\n\n";
    }
    
    std::cout << "3. Do-While Loop Style:\n";
    auto doWhilePlugin = manager.getPlugin("DoWhileLoop");
    if (doWhilePlugin) {
        std::cout << doWhilePlugin->generateBranch(loopContext) << "\n\n";
    }
}

void demonstrateFunctionStyles() {
    std::cout << "\n=== FUNCTION BRANCH STYLES ===\n\n";
    
    auto& manager = BranchStyleManager::getInstance();
    
    auto funcContext = BranchUtils::createSimpleConditional(
        "isReady()",
        "executeOperation();",
        "handleError();"
    );
    
    std::cout << "1. Lambda Style:\n";
    auto lambdaPlugin = manager.getPlugin("Lambda");
    if (lambdaPlugin) {
        std::cout << lambdaPlugin->generateBranch(funcContext) << "\n\n";
    }
    
    std::cout << "2. Nested Function Style:\n";
    auto nestedFuncPlugin = manager.getPlugin("NestedFunction");
    if (nestedFuncPlugin) {
        std::cout << nestedFuncPlugin->generateBranch(funcContext) << "\n\n";
    }
    
    auto earlyReturnContext = BranchUtils::createFunctionContext(
        "hasError()",
        "logError(); cleanup();",
        true
    );
    
    std::cout << "3. Early Return Style:\n";
    auto earlyReturnPlugin = manager.getPlugin("EarlyReturn");
    if (earlyReturnPlugin) {
        std::cout << earlyReturnPlugin->generateBranch(earlyReturnContext) << "\n\n";
    }
}

void demonstrateAdvancedStyles() {
    std::cout << "\n=== ADVANCED BRANCH STYLES ===\n\n";
    
    auto& manager = BranchStyleManager::getInstance();
    
    auto advancedContext = BranchUtils::createSimpleConditional(
        "checkCondition()",
        "performAction();",
        "performAlternative();"
    );
    
    std::cout << "1. Try-Catch Style:\n";
    auto tryCatchPlugin = manager.getPlugin("TryCatch");
    if (tryCatchPlugin) {
        std::cout << tryCatchPlugin->generateBranch(advancedContext) << "\n\n";
    }
    
    std::cout << "2. Goto Style:\n";
    auto gotoPlugin = manager.getPlugin("Goto");
    if (gotoPlugin) {
        std::cout << gotoPlugin->generateBranch(advancedContext) << "\n\n";
    }
    
    std::cout << "3. Function Pointer Style:\n";
    auto funcPtrPlugin = manager.getPlugin("FunctionPointer");
    if (funcPtrPlugin) {
        std::cout << funcPtrPlugin->generateBranch(advancedContext) << "\n\n";
    }
    
    std::cout << "4. Computed Jump Style (GCC-specific):\n";
    auto computedPlugin = manager.getPlugin("ComputedJump");
    if (computedPlugin) {
        std::cout << computedPlugin->generateBranch(advancedContext) << "\n\n";
    }
}

void demonstrateRandomGeneration() {
    std::cout << "\n=== RANDOM BRANCH GENERATION ===\n\n";
    
    auto& manager = BranchStyleManager::getInstance();
    
    auto context = BranchUtils::createSimpleConditional(
        "shouldExecute",
        "executeMainPath();",
        "executeAlternatePath();"
    );
    
    std::cout << "Random branches for the same logic:\n\n";
    
    for (int i = 0; i < 5; ++i) {
        std::cout << "Variant " << (i + 1) << ":\n";
        auto plugin = manager.getRandomPlugin(context);
        if (plugin) {
            std::cout << "Style: " << plugin->getName() 
                      << " (Complexity: " << plugin->getComplexityScore() << ")\n";
            std::cout << plugin->generateBranch(context) << "\n\n";
        }
    }
}

void demonstrateComplexGeneration() {
    std::cout << "\n=== COMPLEX BRANCH GENERATION ===\n\n";
    
    auto& manager = BranchStyleManager::getInstance();
    
    auto context = BranchUtils::createSimpleConditional(
        "validateInput(data)",
        "processValidData(data);",
        "handleInvalidData(data);"
    );
    
    std::cout << "Complex branching with multiple styles:\n";
    std::string complexBranch = manager.generateComplexBranch(context, 3);
    std::cout << complexBranch << "\n\n";
}

void demonstratePluginFiltering() {
    std::cout << "\n=== PLUGIN FILTERING ===\n\n";
    
    auto& manager = BranchStyleManager::getInstance();
    
    // Get plugins by type
    std::cout << "Conditional plugins:\n";
    auto conditionalPlugins = manager.getPluginsByType(BranchStyleType::CONDITIONAL);
    for (auto plugin : conditionalPlugins) {
        std::cout << "- " << plugin->getName() 
                  << " (Complexity: " << plugin->getComplexityScore() << ")\n";
    }
    
    std::cout << "\nLoop plugins:\n";
    auto loopPlugins = manager.getPluginsByType(BranchStyleType::LOOP);
    for (auto plugin : loopPlugins) {
        std::cout << "- " << plugin->getName() 
                  << " (Complexity: " << plugin->getComplexityScore() << ")\n";
    }
    
    // Get plugins by complexity
    std::cout << "\nLow complexity plugins (1-3):\n";
    auto lowComplexityPlugins = manager.getPluginsByComplexity(1, 3);
    for (auto plugin : lowComplexityPlugins) {
        std::cout << "- " << plugin->getName() 
                  << " (Complexity: " << plugin->getComplexityScore() << ")\n";
    }
    
    std::cout << "\nHigh complexity plugins (7-10):\n";
    auto highComplexityPlugins = manager.getPluginsByComplexity(7, 10);
    for (auto plugin : highComplexityPlugins) {
        std::cout << "- " << plugin->getName() 
                  << " (Complexity: " << plugin->getComplexityScore() << ")\n";
    }
}

void generateStubWithBranchStyles() {
    std::cout << "\n=== STUB GENERATION WITH BRANCH STYLES ===\n\n";
    
    auto& manager = BranchStyleManager::getInstance();
    
    std::cout << "Generated stub with various branch styles:\n\n";
    std::cout << "#include <iostream>\n";
    std::cout << "#include <functional>\n";
    std::cout << "#include <array>\n";
    std::cout << "#include <stdexcept>\n\n";
    
    std::cout << "void obfuscatedStub(int param) {\n";
    
    // Generate multiple different branch patterns
    std::vector<std::string> conditions = {
        "param > 0",
        "param % 2 == 0",
        "(param & 0xFF) == 0x42",
        "param < 100"
    };
    
    std::vector<std::string> actions = {
        "step1();",
        "step2();",
        "step3();",
        "cleanup();"
    };
    
    for (size_t i = 0; i < conditions.size(); ++i) {
        auto context = BranchUtils::createSimpleConditional(
            conditions[i],
            actions[i],
            "/* fallback */"
        );
        context.nestingLevel = 1;
        
        auto plugin = manager.getRandomPlugin(context);
        if (plugin) {
            std::cout << "    // Using " << plugin->getName() << " style\n";
            std::cout << plugin->generateBranch(context) << "\n\n";
        }
    }
    
    std::cout << "}\n\n";
    
    std::cout << "int main() {\n";
    std::cout << "    obfuscatedStub(42);\n";
    std::cout << "    return 0;\n";
    std::cout << "}\n";
}

int main() {
    std::cout << "========================================\n";
    std::cout << "    BRANCH STYLE PLUGINS DEMO\n";
    std::cout << "========================================\n";
    
    // Set a seed for reproducible demo
    auto& manager = BranchStyleManager::getInstance();
    manager.setSeed(std::chrono::steady_clock::now().time_since_epoch().count());
    
    demonstrateConditionalStyles();
    demonstrateSwitchStyles();
    demonstrateLoopStyles();
    demonstrateFunctionStyles();
    demonstrateAdvancedStyles();
    demonstrateRandomGeneration();
    demonstrateComplexGeneration();
    demonstratePluginFiltering();
    generateStubWithBranchStyles();
    
    std::cout << "\n========================================\n";
    std::cout << "           DEMO COMPLETE\n";
    std::cout << "========================================\n";
    
    return 0;
}