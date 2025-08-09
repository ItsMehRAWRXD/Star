#pragma once

#include <string>
#include <vector>
#include <memory>
#include <random>
#include <unordered_map>
#include <functional>
#include <algorithm>

namespace BranchStylePlugins {

// Forward declarations
class BranchStylePlugin;
class BranchStyleManager;

// Branch context information
struct BranchContext {
    std::string condition;           // The condition to evaluate
    std::string trueBlock;          // Code for true branch
    std::string falseBlock;         // Code for false branch (optional)
    std::vector<std::string> cases; // For switch-style branches
    std::string loopVariable;       // For loop-style branches
    std::string iterationCount;     // For loop bounds
    int nestingLevel = 0;           // Current nesting depth
    bool needsReturn = false;       // Whether branch should include return
    bool isVoidFunction = false;    // Whether function returns void
};

// Branch style types
enum class BranchStyleType {
    CONDITIONAL,    // if-else, ternary, guard clauses
    SWITCH,         // switch statements, lookup tables
    LOOP,           // for, while, do-while loops
    FUNCTION,       // early returns, nested functions
    EXCEPTION,      // try-catch, throw patterns
    GOTO,           // goto statements (for advanced obfuscation)
    COMPUTED        // function pointers, computed jumps
};

// Abstract base class for branch style plugins
class BranchStylePlugin {
public:
    virtual ~BranchStylePlugin() = default;
    
    // Generate branch code using this style
    virtual std::string generateBranch(const BranchContext& context) = 0;
    
    // Get the type of this branch style
    virtual BranchStyleType getType() const = 0;
    
    // Get a human-readable name for this style
    virtual std::string getName() const = 0;
    
    // Check if this style is appropriate for the given context
    virtual bool isApplicable(const BranchContext& context) const = 0;
    
    // Get complexity score (higher = more obfuscated)
    virtual int getComplexityScore() const = 0;
    
    // Generate random variable names
    virtual std::string generateVarName(const std::string& prefix = "var") const;
    
protected:
    mutable std::mt19937 rng{std::random_device{}()};
};

// Conditional branch styles
class IfElseBranchStyle : public BranchStylePlugin {
public:
    std::string generateBranch(const BranchContext& context) override;
    BranchStyleType getType() const override { return BranchStyleType::CONDITIONAL; }
    std::string getName() const override { return "IfElse"; }
    bool isApplicable(const BranchContext& context) const override;
    int getComplexityScore() const override { return 1; }
};

class TernaryBranchStyle : public BranchStylePlugin {
public:
    std::string generateBranch(const BranchContext& context) override;
    BranchStyleType getType() const override { return BranchStyleType::CONDITIONAL; }
    std::string getName() const override { return "Ternary"; }
    bool isApplicable(const BranchContext& context) const override;
    int getComplexityScore() const override { return 2; }
};

class GuardClauseBranchStyle : public BranchStylePlugin {
public:
    std::string generateBranch(const BranchContext& context) override;
    BranchStyleType getType() const override { return BranchStyleType::CONDITIONAL; }
    std::string getName() const override { return "GuardClause"; }
    bool isApplicable(const BranchContext& context) const override;
    int getComplexityScore() const override { return 3; }
};

class NestedConditionalStyle : public BranchStylePlugin {
public:
    std::string generateBranch(const BranchContext& context) override;
    BranchStyleType getType() const override { return BranchStyleType::CONDITIONAL; }
    std::string getName() const override { return "NestedConditional"; }
    bool isApplicable(const BranchContext& context) const override;
    int getComplexityScore() const override { return 4; }
};

// Switch-based branch styles
class SwitchBranchStyle : public BranchStylePlugin {
public:
    std::string generateBranch(const BranchContext& context) override;
    BranchStyleType getType() const override { return BranchStyleType::SWITCH; }
    std::string getName() const override { return "Switch"; }
    bool isApplicable(const BranchContext& context) const override;
    int getComplexityScore() const override { return 3; }
};

class LookupTableBranchStyle : public BranchStylePlugin {
public:
    std::string generateBranch(const BranchContext& context) override;
    BranchStyleType getType() const override { return BranchStyleType::SWITCH; }
    std::string getName() const override { return "LookupTable"; }
    bool isApplicable(const BranchContext& context) const override;
    int getComplexityScore() const override { return 5; }
};

// Loop-based branch styles
class ForLoopBranchStyle : public BranchStylePlugin {
public:
    std::string generateBranch(const BranchContext& context) override;
    BranchStyleType getType() const override { return BranchStyleType::LOOP; }
    std::string getName() const override { return "ForLoop"; }
    bool isApplicable(const BranchContext& context) const override;
    int getComplexityScore() const override { return 4; }
};

class WhileLoopBranchStyle : public BranchStylePlugin {
public:
    std::string generateBranch(const BranchContext& context) override;
    BranchStyleType getType() const override { return BranchStyleType::LOOP; }
    std::string getName() const override { return "WhileLoop"; }
    bool isApplicable(const BranchContext& context) const override;
    int getComplexityScore() const override { return 3; }
};

class DoWhileLoopBranchStyle : public BranchStylePlugin {
public:
    std::string generateBranch(const BranchContext& context) override;
    BranchStyleType getType() const override { return BranchStyleType::LOOP; }
    std::string getName() const override { return "DoWhileLoop"; }
    bool isApplicable(const BranchContext& context) const override;
    int getComplexityScore() const override { return 4; }
};

// Function-based branch styles
class EarlyReturnBranchStyle : public BranchStylePlugin {
public:
    std::string generateBranch(const BranchContext& context) override;
    BranchStyleType getType() const override { return BranchStyleType::FUNCTION; }
    std::string getName() const override { return "EarlyReturn"; }
    bool isApplicable(const BranchContext& context) const override;
    int getComplexityScore() const override { return 2; }
};

class LambdaBranchStyle : public BranchStylePlugin {
public:
    std::string generateBranch(const BranchContext& context) override;
    BranchStyleType getType() const override { return BranchStyleType::FUNCTION; }
    std::string getName() const override { return "Lambda"; }
    bool isApplicable(const BranchContext& context) const override;
    int getComplexityScore() const override { return 6; }
};

class NestedFunctionBranchStyle : public BranchStylePlugin {
public:
    std::string generateBranch(const BranchContext& context) override;
    BranchStyleType getType() const override { return BranchStyleType::FUNCTION; }
    std::string getName() const override { return "NestedFunction"; }
    bool isApplicable(const BranchContext& context) const override;
    int getComplexityScore() const override { return 7; }
};

// Exception-based branch styles
class TryCatchBranchStyle : public BranchStylePlugin {
public:
    std::string generateBranch(const BranchContext& context) override;
    BranchStyleType getType() const override { return BranchStyleType::EXCEPTION; }
    std::string getName() const override { return "TryCatch"; }
    bool isApplicable(const BranchContext& context) const override;
    int getComplexityScore() const override { return 5; }
};

// Goto-based branch styles (for maximum obfuscation)
class GotoBranchStyle : public BranchStylePlugin {
public:
    std::string generateBranch(const BranchContext& context) override;
    BranchStyleType getType() const override { return BranchStyleType::GOTO; }
    std::string getName() const override { return "Goto"; }
    bool isApplicable(const BranchContext& context) const override;
    int getComplexityScore() const override { return 8; }
};

// Computed branch styles (function pointers, etc.)
class FunctionPointerBranchStyle : public BranchStylePlugin {
public:
    std::string generateBranch(const BranchContext& context) override;
    BranchStyleType getType() const override { return BranchStyleType::COMPUTED; }
    std::string getName() const override { return "FunctionPointer"; }
    bool isApplicable(const BranchContext& context) const override;
    int getComplexityScore() const override { return 9; }
};

class ComputedJumpBranchStyle : public BranchStylePlugin {
public:
    std::string generateBranch(const BranchContext& context) override;
    BranchStyleType getType() const override { return BranchStyleType::COMPUTED; }
    std::string getName() const override { return "ComputedJump"; }
    bool isApplicable(const BranchContext& context) const override;
    int getComplexityScore() const override { return 10; }
};

// Branch style manager for registration and selection
class BranchStyleManager {
public:
    static BranchStyleManager& getInstance();
    
    // Register a new branch style plugin
    void registerPlugin(std::unique_ptr<BranchStylePlugin> plugin);
    
    // Get a random applicable plugin for the given context
    BranchStylePlugin* getRandomPlugin(const BranchContext& context);
    
    // Get a plugin by name
    BranchStylePlugin* getPlugin(const std::string& name);
    
    // Get all plugins of a specific type
    std::vector<BranchStylePlugin*> getPluginsByType(BranchStyleType type);
    
    // Get plugin by complexity range
    std::vector<BranchStylePlugin*> getPluginsByComplexity(int minComplexity, int maxComplexity);
    
    // Set randomization seed
    void setSeed(uint64_t seed);
    
    // Generate a random branching pattern with multiple styles
    std::string generateComplexBranch(const BranchContext& context, int numStyles = -1);
    
private:
    BranchStyleManager() = default;
    std::vector<std::unique_ptr<BranchStylePlugin>> plugins;
    std::unordered_map<std::string, BranchStylePlugin*> pluginsByName;
    mutable std::mt19937 rng{std::random_device{}()};
    
    void initializeDefaultPlugins();
};

// Utility functions for creating branch contexts
namespace BranchUtils {
    BranchContext createSimpleConditional(const std::string& condition, 
                                        const std::string& trueBlock, 
                                        const std::string& falseBlock = "");
    
    BranchContext createSwitchContext(const std::string& variable, 
                                    const std::vector<std::string>& cases);
    
    BranchContext createLoopContext(const std::string& variable, 
                                  const std::string& count, 
                                  const std::string& body);
    
    BranchContext createFunctionContext(const std::string& condition, 
                                      const std::string& returnBlock, 
                                      bool isVoid = false);
}

} // namespace BranchStylePlugins