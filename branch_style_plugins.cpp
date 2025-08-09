#include "branch_style_plugins.h"
#include <sstream>
#include <iomanip>

namespace BranchStylePlugins {

// Base class implementation
std::string BranchStylePlugin::generateVarName(const std::string& prefix) const {
    static const std::string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::uniform_int_distribution<> dist(0, chars.length() - 1);
    std::string name = prefix;
    for (int i = 0; i < 8 + (rng() % 8); ++i) {
        name += chars[dist(rng)];
    }
    return name;
}

// IfElseBranchStyle implementation
std::string IfElseBranchStyle::generateBranch(const BranchContext& context) {
    std::ostringstream ss;
    std::string indent(context.nestingLevel * 4, ' ');
    
    ss << indent << "if (" << context.condition << ") {\n";
    ss << indent << "    " << context.trueBlock << "\n";
    ss << indent << "}";
    
    if (!context.falseBlock.empty()) {
        ss << " else {\n";
        ss << indent << "    " << context.falseBlock << "\n";
        ss << indent << "}";
    }
    
    return ss.str();
}

bool IfElseBranchStyle::isApplicable(const BranchContext& context) const {
    return !context.condition.empty() && !context.trueBlock.empty();
}

// TernaryBranchStyle implementation
std::string TernaryBranchStyle::generateBranch(const BranchContext& context) {
    if (context.falseBlock.empty()) {
        return ""; // Ternary requires both branches
    }
    
    std::string varName = generateVarName("result");
    std::ostringstream ss;
    std::string indent(context.nestingLevel * 4, ' ');
    
    ss << indent << "auto " << varName << " = (" << context.condition << ") ? "
       << "(" << context.trueBlock << ") : (" << context.falseBlock << ");";
    
    return ss.str();
}

bool TernaryBranchStyle::isApplicable(const BranchContext& context) const {
    return !context.condition.empty() && !context.trueBlock.empty() && !context.falseBlock.empty();
}

// GuardClauseBranchStyle implementation
std::string GuardClauseBranchStyle::generateBranch(const BranchContext& context) {
    std::ostringstream ss;
    std::string indent(context.nestingLevel * 4, ' ');
    
    ss << indent << "if (!(" << context.condition << ")) {\n";
    if (context.needsReturn) {
        if (context.isVoidFunction) {
            ss << indent << "    return;\n";
        } else {
            ss << indent << "    return " << context.falseBlock << ";\n";
        }
    } else {
        ss << indent << "    " << context.falseBlock << "\n";
    }
    ss << indent << "}\n";
    ss << indent << context.trueBlock;
    
    return ss.str();
}

bool GuardClauseBranchStyle::isApplicable(const BranchContext& context) const {
    return !context.condition.empty() && !context.trueBlock.empty();
}

// NestedConditionalStyle implementation
std::string NestedConditionalStyle::generateBranch(const BranchContext& context) {
    std::ostringstream ss;
    std::string indent(context.nestingLevel * 4, ' ');
    
    // Generate multiple nested conditions
    std::uniform_int_distribution<> dist(2, 4);
    int numConditions = dist(rng);
    
    for (int i = 0; i < numConditions; ++i) {
        std::string nestedVar = generateVarName("check");
        ss << indent << std::string(i * 4, ' ') << "if (";
        
        if (i == 0) {
            ss << context.condition;
        } else {
            ss << nestedVar << " && (" << context.condition << ")";
        }
        ss << ") {\n";
    }
    
    ss << indent << std::string(numConditions * 4, ' ') << context.trueBlock << "\n";
    
    for (int i = numConditions - 1; i >= 0; --i) {
        ss << indent << std::string(i * 4, ' ') << "}";
        if (i == 0 && !context.falseBlock.empty()) {
            ss << " else {\n";
            ss << indent << "    " << context.falseBlock << "\n";
            ss << indent << "}";
        }
        if (i > 0) ss << "\n";
    }
    
    return ss.str();
}

bool NestedConditionalStyle::isApplicable(const BranchContext& context) const {
    return !context.condition.empty() && !context.trueBlock.empty() && context.nestingLevel < 3;
}

// SwitchBranchStyle implementation
std::string SwitchBranchStyle::generateBranch(const BranchContext& context) {
    if (context.cases.empty()) {
        return ""; // Switch requires cases
    }
    
    std::ostringstream ss;
    std::string indent(context.nestingLevel * 4, ' ');
    std::string switchVar = generateVarName("selector");
    
    ss << indent << "int " << switchVar << " = (" << context.condition << ") ? 1 : 0;\n";
    ss << indent << "switch (" << switchVar << ") {\n";
    
    for (size_t i = 0; i < context.cases.size(); ++i) {
        ss << indent << "    case " << i << ":\n";
        ss << indent << "        " << context.cases[i] << "\n";
        ss << indent << "        break;\n";
    }
    
    if (!context.falseBlock.empty()) {
        ss << indent << "    default:\n";
        ss << indent << "        " << context.falseBlock << "\n";
        ss << indent << "        break;\n";
    }
    
    ss << indent << "}";
    
    return ss.str();
}

bool SwitchBranchStyle::isApplicable(const BranchContext& context) const {
    return !context.condition.empty() && !context.cases.empty();
}

// LookupTableBranchStyle implementation
std::string LookupTableBranchStyle::generateBranch(const BranchContext& context) {
    if (context.cases.empty()) {
        return "";
    }
    
    std::ostringstream ss;
    std::string indent(context.nestingLevel * 4, ' ');
    std::string tableName = generateVarName("lookup");
    std::string indexVar = generateVarName("idx");
    
    ss << indent << "static const auto " << tableName << " = []() {\n";
    ss << indent << "    std::array<std::function<void()>, " << context.cases.size() << "> table;\n";
    
    for (size_t i = 0; i < context.cases.size(); ++i) {
        ss << indent << "    table[" << i << "] = []() { " << context.cases[i] << " };\n";
    }
    
    ss << indent << "    return table;\n";
    ss << indent << "}();\n";
    ss << indent << "int " << indexVar << " = (" << context.condition << ") ? 0 : "
       << (context.cases.size() - 1) << ";\n";
    ss << indent << tableName << "[" << indexVar << "]();";
    
    return ss.str();
}

bool LookupTableBranchStyle::isApplicable(const BranchContext& context) const {
    return !context.condition.empty() && !context.cases.empty() && context.cases.size() <= 8;
}

// ForLoopBranchStyle implementation
std::string ForLoopBranchStyle::generateBranch(const BranchContext& context) {
    std::ostringstream ss;
    std::string indent(context.nestingLevel * 4, ' ');
    std::string loopVar = context.loopVariable.empty() ? generateVarName("i") : context.loopVariable;
    std::string iterations = context.iterationCount.empty() ? "1" : context.iterationCount;
    
    ss << indent << "for (int " << loopVar << " = 0; " << loopVar << " < " << iterations << "; ++" << loopVar << ") {\n";
    ss << indent << "    if (" << context.condition << ") {\n";
    ss << indent << "        " << context.trueBlock << "\n";
    ss << indent << "        break;\n";
    ss << indent << "    }\n";
    
    if (!context.falseBlock.empty()) {
        ss << indent << "    " << context.falseBlock << "\n";
    }
    
    ss << indent << "}";
    
    return ss.str();
}

bool ForLoopBranchStyle::isApplicable(const BranchContext& context) const {
    return !context.condition.empty() && !context.trueBlock.empty();
}

// WhileLoopBranchStyle implementation
std::string WhileLoopBranchStyle::generateBranch(const BranchContext& context) {
    std::ostringstream ss;
    std::string indent(context.nestingLevel * 4, ' ');
    std::string flagVar = generateVarName("continue_loop");
    
    ss << indent << "bool " << flagVar << " = true;\n";
    ss << indent << "while (" << flagVar << ") {\n";
    ss << indent << "    if (" << context.condition << ") {\n";
    ss << indent << "        " << context.trueBlock << "\n";
    ss << indent << "        " << flagVar << " = false;\n";
    ss << indent << "    } else {\n";
    
    if (!context.falseBlock.empty()) {
        ss << indent << "        " << context.falseBlock << "\n";
    }
    
    ss << indent << "        " << flagVar << " = false;\n";
    ss << indent << "    }\n";
    ss << indent << "}";
    
    return ss.str();
}

bool WhileLoopBranchStyle::isApplicable(const BranchContext& context) const {
    return !context.condition.empty() && !context.trueBlock.empty();
}

// DoWhileLoopBranchStyle implementation
std::string DoWhileLoopBranchStyle::generateBranch(const BranchContext& context) {
    std::ostringstream ss;
    std::string indent(context.nestingLevel * 4, ' ');
    std::string executed = generateVarName("executed");
    
    ss << indent << "bool " << executed << " = false;\n";
    ss << indent << "do {\n";
    ss << indent << "    if (!(" << executed << ") && (" << context.condition << ")) {\n";
    ss << indent << "        " << context.trueBlock << "\n";
    ss << indent << "    } else if (!(" << executed << ")) {\n";
    
    if (!context.falseBlock.empty()) {
        ss << indent << "        " << context.falseBlock << "\n";
    }
    
    ss << indent << "    }\n";
    ss << indent << "    " << executed << " = true;\n";
    ss << indent << "} while (!(" << executed << "));";
    
    return ss.str();
}

bool DoWhileLoopBranchStyle::isApplicable(const BranchContext& context) const {
    return !context.condition.empty() && !context.trueBlock.empty();
}

// EarlyReturnBranchStyle implementation
std::string EarlyReturnBranchStyle::generateBranch(const BranchContext& context) {
    std::ostringstream ss;
    std::string indent(context.nestingLevel * 4, ' ');
    
    ss << indent << "if (" << context.condition << ") {\n";
    ss << indent << "    " << context.trueBlock << "\n";
    
    if (context.needsReturn) {
        if (context.isVoidFunction) {
            ss << indent << "    return;\n";
        } else {
            ss << indent << "    return;\n";
        }
    }
    
    ss << indent << "}\n";
    
    if (!context.falseBlock.empty()) {
        ss << indent << context.falseBlock;
    }
    
    return ss.str();
}

bool EarlyReturnBranchStyle::isApplicable(const BranchContext& context) const {
    return !context.condition.empty() && !context.trueBlock.empty() && context.needsReturn;
}

// LambdaBranchStyle implementation
std::string LambdaBranchStyle::generateBranch(const BranchContext& context) {
    std::ostringstream ss;
    std::string indent(context.nestingLevel * 4, ' ');
    std::string lambdaVar = generateVarName("lambda");
    
    ss << indent << "auto " << lambdaVar << " = [&](bool condition) {\n";
    ss << indent << "    if (condition) {\n";
    ss << indent << "        " << context.trueBlock << "\n";
    ss << indent << "    }";
    
    if (!context.falseBlock.empty()) {
        ss << " else {\n";
        ss << indent << "        " << context.falseBlock << "\n";
        ss << indent << "    }";
    }
    
    ss << "\n" << indent << "};\n";
    ss << indent << lambdaVar << "(" << context.condition << ");";
    
    return ss.str();
}

bool LambdaBranchStyle::isApplicable(const BranchContext& context) const {
    return !context.condition.empty() && !context.trueBlock.empty();
}

// NestedFunctionBranchStyle implementation
std::string NestedFunctionBranchStyle::generateBranch(const BranchContext& context) {
    std::ostringstream ss;
    std::string indent(context.nestingLevel * 4, ' ');
    std::string funcName = generateVarName("nested_func");
    
    ss << indent << "auto " << funcName << " = [&]() {\n";
    ss << indent << "    if (" << context.condition << ") {\n";
    ss << indent << "        " << context.trueBlock << "\n";
    ss << indent << "        return true;\n";
    ss << indent << "    }\n";
    
    if (!context.falseBlock.empty()) {
        ss << indent << "    " << context.falseBlock << "\n";
    }
    
    ss << indent << "    return false;\n";
    ss << indent << "};\n";
    ss << indent << funcName << "();";
    
    return ss.str();
}

bool NestedFunctionBranchStyle::isApplicable(const BranchContext& context) const {
    return !context.condition.empty() && !context.trueBlock.empty();
}

// TryCatchBranchStyle implementation
std::string TryCatchBranchStyle::generateBranch(const BranchContext& context) {
    std::ostringstream ss;
    std::string indent(context.nestingLevel * 4, ' ');
    std::string exceptionVar = generateVarName("branch_exception");
    
    ss << indent << "try {\n";
    ss << indent << "    if (!(" << context.condition << ")) {\n";
    ss << indent << "        throw std::runtime_error(\"condition_false\");\n";
    ss << indent << "    }\n";
    ss << indent << "    " << context.trueBlock << "\n";
    ss << indent << "} catch (const std::exception& " << exceptionVar << ") {\n";
    
    if (!context.falseBlock.empty()) {
        ss << indent << "    " << context.falseBlock << "\n";
    }
    
    ss << indent << "}";
    
    return ss.str();
}

bool TryCatchBranchStyle::isApplicable(const BranchContext& context) const {
    return !context.condition.empty() && !context.trueBlock.empty() && context.nestingLevel < 2;
}

// GotoBranchStyle implementation
std::string GotoBranchStyle::generateBranch(const BranchContext& context) {
    std::ostringstream ss;
    std::string indent(context.nestingLevel * 4, ' ');
    std::string labelTrue = generateVarName("label_true");
    std::string labelFalse = generateVarName("label_false");
    std::string labelEnd = generateVarName("label_end");
    
    ss << indent << "if (" << context.condition << ") goto " << labelTrue << ";\n";
    ss << indent << "goto " << labelFalse << ";\n";
    ss << indent << labelTrue << ":\n";
    ss << indent << "    " << context.trueBlock << "\n";
    ss << indent << "    goto " << labelEnd << ";\n";
    ss << indent << labelFalse << ":\n";
    
    if (!context.falseBlock.empty()) {
        ss << indent << "    " << context.falseBlock << "\n";
    }
    
    ss << indent << labelEnd << ":;";
    
    return ss.str();
}

bool GotoBranchStyle::isApplicable(const BranchContext& context) const {
    return !context.condition.empty() && !context.trueBlock.empty() && context.nestingLevel < 2;
}

// FunctionPointerBranchStyle implementation
std::string FunctionPointerBranchStyle::generateBranch(const BranchContext& context) {
    std::ostringstream ss;
    std::string indent(context.nestingLevel * 4, ' ');
    std::string funcArrayName = generateVarName("func_ptrs");
    std::string indexVar = generateVarName("func_index");
    
    ss << indent << "static const auto " << funcArrayName << " = std::array<std::function<void()>, 2>{{\n";
    ss << indent << "    []() { " << (context.falseBlock.empty() ? "/* false branch */" : context.falseBlock) << " },\n";
    ss << indent << "    []() { " << context.trueBlock << " }\n";
    ss << indent << "}};\n";
    ss << indent << "int " << indexVar << " = (" << context.condition << ") ? 1 : 0;\n";
    ss << indent << funcArrayName << "[" << indexVar << "]();";
    
    return ss.str();
}

bool FunctionPointerBranchStyle::isApplicable(const BranchContext& context) const {
    return !context.condition.empty() && !context.trueBlock.empty();
}

// ComputedJumpBranchStyle implementation
std::string ComputedJumpBranchStyle::generateBranch(const BranchContext& context) {
    std::ostringstream ss;
    std::string indent(context.nestingLevel * 4, ' ');
    std::string jumpTableName = generateVarName("jump_table");
    std::string offsetVar = generateVarName("offset");
    
    ss << indent << "static const intptr_t " << jumpTableName << "[] = {\n";
    ss << indent << "    reinterpret_cast<intptr_t>(&&false_branch),\n";
    ss << indent << "    reinterpret_cast<intptr_t>(&&true_branch)\n";
    ss << indent << "};\n";
    ss << indent << "int " << offsetVar << " = (" << context.condition << ") ? 1 : 0;\n";
    ss << indent << "goto *reinterpret_cast<void*>(" << jumpTableName << "[" << offsetVar << "]);\n";
    ss << indent << "false_branch:\n";
    
    if (!context.falseBlock.empty()) {
        ss << indent << "    " << context.falseBlock << "\n";
    }
    
    ss << indent << "    goto end_branch;\n";
    ss << indent << "true_branch:\n";
    ss << indent << "    " << context.trueBlock << "\n";
    ss << indent << "end_branch:;";
    
    return ss.str();
}

bool ComputedJumpBranchStyle::isApplicable(const BranchContext& context) const {
    // Computed jumps are very compiler-specific and may not work everywhere
    return !context.condition.empty() && !context.trueBlock.empty() && context.nestingLevel == 0;
}

// BranchStyleManager implementation
BranchStyleManager& BranchStyleManager::getInstance() {
    static BranchStyleManager instance;
    static bool initialized = false;
    if (!initialized) {
        instance.initializeDefaultPlugins();
        initialized = true;
    }
    return instance;
}

void BranchStyleManager::registerPlugin(std::unique_ptr<BranchStylePlugin> plugin) {
    if (plugin) {
        pluginsByName[plugin->getName()] = plugin.get();
        plugins.push_back(std::move(plugin));
    }
}

BranchStylePlugin* BranchStyleManager::getRandomPlugin(const BranchContext& context) {
    std::vector<BranchStylePlugin*> applicable;
    
    for (const auto& plugin : plugins) {
        if (plugin->isApplicable(context)) {
            applicable.push_back(plugin.get());
        }
    }
    
    if (applicable.empty()) {
        return nullptr;
    }
    
    std::uniform_int_distribution<> dist(0, applicable.size() - 1);
    return applicable[dist(rng)];
}

BranchStylePlugin* BranchStyleManager::getPlugin(const std::string& name) {
    auto it = pluginsByName.find(name);
    return (it != pluginsByName.end()) ? it->second : nullptr;
}

std::vector<BranchStylePlugin*> BranchStyleManager::getPluginsByType(BranchStyleType type) {
    std::vector<BranchStylePlugin*> result;
    for (const auto& plugin : plugins) {
        if (plugin->getType() == type) {
            result.push_back(plugin.get());
        }
    }
    return result;
}

std::vector<BranchStylePlugin*> BranchStyleManager::getPluginsByComplexity(int minComplexity, int maxComplexity) {
    std::vector<BranchStylePlugin*> result;
    for (const auto& plugin : plugins) {
        int complexity = plugin->getComplexityScore();
        if (complexity >= minComplexity && complexity <= maxComplexity) {
            result.push_back(plugin.get());
        }
    }
    return result;
}

void BranchStyleManager::setSeed(uint64_t seed) {
    rng.seed(seed);
}

std::string BranchStyleManager::generateComplexBranch(const BranchContext& context, int numStyles) {
    if (numStyles <= 0) {
        numStyles = 1 + (rng() % 3); // 1-3 styles
    }
    
    std::ostringstream ss;
    BranchContext currentContext = context;
    
    for (int i = 0; i < numStyles; ++i) {
        auto plugin = getRandomPlugin(currentContext);
        if (plugin) {
            ss << plugin->generateBranch(currentContext);
            if (i < numStyles - 1) {
                ss << "\n";
            }
            currentContext.nestingLevel++;
        }
    }
    
    return ss.str();
}

void BranchStyleManager::initializeDefaultPlugins() {
    // Register all default plugins
    registerPlugin(std::make_unique<IfElseBranchStyle>());
    registerPlugin(std::make_unique<TernaryBranchStyle>());
    registerPlugin(std::make_unique<GuardClauseBranchStyle>());
    registerPlugin(std::make_unique<NestedConditionalStyle>());
    registerPlugin(std::make_unique<SwitchBranchStyle>());
    registerPlugin(std::make_unique<LookupTableBranchStyle>());
    registerPlugin(std::make_unique<ForLoopBranchStyle>());
    registerPlugin(std::make_unique<WhileLoopBranchStyle>());
    registerPlugin(std::make_unique<DoWhileLoopBranchStyle>());
    registerPlugin(std::make_unique<EarlyReturnBranchStyle>());
    registerPlugin(std::make_unique<LambdaBranchStyle>());
    registerPlugin(std::make_unique<NestedFunctionBranchStyle>());
    registerPlugin(std::make_unique<TryCatchBranchStyle>());
    registerPlugin(std::make_unique<GotoBranchStyle>());
    registerPlugin(std::make_unique<FunctionPointerBranchStyle>());
    registerPlugin(std::make_unique<ComputedJumpBranchStyle>());
}

// BranchUtils implementation
namespace BranchUtils {
    BranchContext createSimpleConditional(const std::string& condition, 
                                        const std::string& trueBlock, 
                                        const std::string& falseBlock) {
        BranchContext context;
        context.condition = condition;
        context.trueBlock = trueBlock;
        context.falseBlock = falseBlock;
        return context;
    }
    
    BranchContext createSwitchContext(const std::string& variable, 
                                    const std::vector<std::string>& cases) {
        BranchContext context;
        context.condition = variable + " >= 0 && " + variable + " < " + std::to_string(cases.size());
        context.cases = cases;
        return context;
    }
    
    BranchContext createLoopContext(const std::string& variable, 
                                  const std::string& count, 
                                  const std::string& body) {
        BranchContext context;
        context.loopVariable = variable;
        context.iterationCount = count;
        context.condition = variable + " < " + count;
        context.trueBlock = body;
        return context;
    }
    
    BranchContext createFunctionContext(const std::string& condition, 
                                      const std::string& returnBlock, 
                                      bool isVoid) {
        BranchContext context;
        context.condition = condition;
        context.trueBlock = returnBlock;
        context.needsReturn = true;
        context.isVoidFunction = isVoid;
        return context;
    }
}

} // namespace BranchStylePlugins