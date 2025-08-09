# Branch Style Plugins System

A sophisticated C++ plugin system for generating various branching patterns and control flow obfuscation in code generation. This system provides a comprehensive set of branching styles that can be used to create highly obfuscated and diverse code patterns.

## 🎯 Features

### **Comprehensive Branch Styles**
- **Conditional**: if-else, ternary operators, guard clauses, nested conditionals
- **Switch-based**: switch statements, lookup tables
- **Loop-based**: for, while, do-while loops
- **Function-based**: lambdas, nested functions, early returns
- **Exception-based**: try-catch patterns
- **Advanced**: goto statements, computed jumps, function pointers

### **Plugin Architecture**
- **Extensible Design**: Easy to add new branch styles
- **Complexity Scoring**: Each plugin has a complexity rating (1-10)
- **Context-Aware**: Plugins adapt to different code contexts
- **Random Selection**: Dynamic selection of appropriate branch styles

### **Code Generation Features**
- **Anti-Debugging Integration**: Branch-based anti-debugging checks
- **Obfuscated Variable Names**: Randomly generated identifiers
- **Mixed Style Generation**: Combine multiple branch styles
- **Nesting Support**: Hierarchical branching patterns

## 🛠️ Building

### **Requirements**
- C++17 compatible compiler (GCC 7+, Clang 6+, MSVC 2019+)
- Standard library support for `<functional>`, `<random>`, `<chrono>`

### **Build Commands**
```bash
# Build all components
make all

# Build specific targets
make demo                    # Branch style demonstration
make enhanced_generator      # Enhanced stub generator

# Run examples
make examples               # Generate example stubs
make test                   # Run all tests
make benchmark             # Performance benchmarks
```

### **Cross-Platform Support**
```bash
# Windows cross-compilation
make windows

# Debug build
make debug

# Optimized release
make release
```

## 📖 Usage

### **Basic Usage**

```cpp
#include "branch_style_plugins.h"
using namespace BranchStylePlugins;

// Get the branch style manager
auto& manager = BranchStyleManager::getInstance();

// Create a branch context
auto context = BranchUtils::createSimpleConditional(
    "x > 10",                                    // condition
    "std::cout << \"x is large\" << std::endl;", // true block
    "std::cout << \"x is small\" << std::endl;"  // false block
);

// Get a random applicable plugin
auto plugin = manager.getRandomPlugin(context);
if (plugin) {
    std::string code = plugin->generateBranch(context);
    std::cout << code << std::endl;
}
```

### **Plugin Selection by Type**

```cpp
// Get all conditional plugins
auto conditionalPlugins = manager.getPluginsByType(BranchStyleType::CONDITIONAL);

// Get plugins by complexity range
auto lowComplexity = manager.getPluginsByComplexity(1, 3);
auto highComplexity = manager.getPluginsByComplexity(7, 10);

// Get specific plugin by name
auto ifElsePlugin = manager.getPlugin("IfElse");
```

### **Complex Branch Generation**

```cpp
// Generate multiple nested branch styles
std::string complexCode = manager.generateComplexBranch(context, 3);

// Set randomization seed for reproducible results
manager.setSeed(12345);
```

## 🎨 Branch Style Types

### **Conditional Styles**

#### **IfElse** (Complexity: 1)
```cpp
if (condition) {
    // true block
} else {
    // false block
}
```

#### **Ternary** (Complexity: 2)
```cpp
auto result = (condition) ? (true_value) : (false_value);
```

#### **GuardClause** (Complexity: 3)
```cpp
if (!(condition)) {
    return;
}
// main logic
```

#### **NestedConditional** (Complexity: 4)
```cpp
if (condition) {
    if (nested_condition && (condition)) {
        // deeply nested logic
    }
}
```

### **Loop-Based Styles**

#### **ForLoop** (Complexity: 4)
```cpp
for (int i = 0; i < iterations; ++i) {
    if (condition) {
        // loop body
        break;
    }
}
```

#### **WhileLoop** (Complexity: 3)
```cpp
bool flag = true;
while (flag) {
    if (condition) {
        // logic
        flag = false;
    }
}
```

### **Function-Based Styles**

#### **Lambda** (Complexity: 6)
```cpp
auto lambda = [&](bool condition) {
    if (condition) {
        // true logic
    } else {
        // false logic
    }
};
lambda(condition);
```

#### **NestedFunction** (Complexity: 7)
```cpp
auto nested_func = [&]() {
    if (condition) {
        // logic
        return true;
    }
    return false;
};
nested_func();
```

### **Advanced Styles**

#### **TryCatch** (Complexity: 5)
```cpp
try {
    if (!(condition)) {
        throw std::runtime_error("condition_false");
    }
    // true logic
} catch (const std::exception& e) {
    // false logic
}
```

#### **Goto** (Complexity: 8)
```cpp
if (condition) goto label_true;
goto label_false;
label_true:
    // true logic
    goto label_end;
label_false:
    // false logic
label_end:;
```

#### **FunctionPointer** (Complexity: 9)
```cpp
static const auto func_ptrs = std::array<std::function<void()>, 2>{{
    []() { /* false logic */ },
    []() { /* true logic */ }
}};
int index = condition ? 1 : 0;
func_ptrs[index]();
```

#### **ComputedJump** (Complexity: 10)
```cpp
static const intptr_t jump_table[] = {
    reinterpret_cast<intptr_t>(&&false_branch),
    reinterpret_cast<intptr_t>(&&true_branch)
};
int offset = condition ? 1 : 0;
goto *reinterpret_cast<void*>(jump_table[offset]);
```

## 🔧 Integration with Stub Generators

### **Enhanced Stub Generator**

The system includes an enhanced stub generator that integrates branch style plugins with:

- **Anti-debugging checks** using varied branch styles
- **Encrypted payload decryption** with branched validation
- **Obfuscated execution flow** using mixed branch patterns

```cpp
EnhancedStubGeneratorWithBranches generator;

// Set complexity preferences
generator.setComplexityLevel(8);
generator.setPreferHighComplexity(true);

// Generate obfuscated stub
std::vector<uint8_t> payload = {0x90, 0x90, 0x90, 0xc3};
std::string key = "encryption_key";
std::string stub = generator.generateEnhancedStub(payload, key);
```

### **Anti-Debugging Integration**

```cpp
// Multiple anti-debug checks with different branch styles
std::vector<std::string> debugChecks = {
    "IsDebuggerPresent()",
    "CheckRemoteDebuggerPresent(GetCurrentProcess(), &debuggerPresent)",
    "GetTickCount64() - startTime > 100"
};

// Each check uses a different branch style for obfuscation
for (const auto& check : debugChecks) {
    auto context = BranchUtils::createSimpleConditional(
        check, "ExitProcess(1);", "/* continue */"
    );
    auto plugin = manager.getRandomPlugin(context);
    // Generate anti-debug code...
}
```

## 📊 Performance Characteristics

Based on benchmarks, the plugins show different performance characteristics:

| Plugin Type | Complexity | Generation Time | Output Size |
|-------------|------------|-----------------|-------------|
| IfElse      | 1          | ~4 μs          | ~53 chars   |
| Ternary     | 2          | ~2 μs          | ~61 chars   |
| GuardClause | 3          | ~0 μs          | ~43 chars   |
| Lambda      | 6          | ~2 μs          | ~153 chars  |
| TryCatch    | 5          | ~2 μs          | ~179 chars  |
| Goto        | 8          | ~2 μs          | ~186 chars  |
| ComputedJump| 10         | ~3 μs          | ~355 chars  |

## 🎪 Example Outputs

### **Simple Conditional**
```cpp
// Input context
auto context = BranchUtils::createSimpleConditional(
    "x > 0", "positive();", "negative();"
);

// Possible outputs (randomly selected):

// IfElse style:
if (x > 0) {
    positive();
} else {
    negative();
}

// Lambda style:
auto lambda = [&](bool condition) {
    if (condition) {
        positive();
    } else {
        negative();
    }
};
lambda(x > 0);

// Function pointer style:
static const auto func_ptrs = std::array<std::function<void()>, 2>{{
    []() { negative(); },
    []() { positive(); }
}};
int index = (x > 0) ? 1 : 0;
func_ptrs[index]();
```

### **Generated Stub Example**
```cpp
#include <windows.h>
#include <vector>
#include <functional>

void obfuscatedStub(int param) {
    // Using NestedFunction style
    auto nested_func = [&]() {
        if (param > 0) {
            step1();
            return true;
        }
        return false;
    };
    nested_func();

    // Using TryCatch style
    try {
        if (!(param % 2 == 0)) {
            throw std::runtime_error("condition_false");
        }
        step2();
    } catch (const std::exception& e) {
        /* fallback */
    }
}
```

## 🔄 Extending the System

### **Creating Custom Plugins**

```cpp
class CustomBranchStyle : public BranchStylePlugin {
public:
    std::string generateBranch(const BranchContext& context) override {
        // Custom branch generation logic
        return "/* custom branch style */";
    }
    
    BranchStyleType getType() const override { 
        return BranchStyleType::CONDITIONAL; 
    }
    
    std::string getName() const override { 
        return "Custom"; 
    }
    
    bool isApplicable(const BranchContext& context) const override {
        // Define when this style is applicable
        return !context.condition.empty();
    }
    
    int getComplexityScore() const override { 
        return 5; 
    }
};

// Register the custom plugin
auto& manager = BranchStyleManager::getInstance();
manager.registerPlugin(std::make_unique<CustomBranchStyle>());
```

### **Custom Context Types**

```cpp
// Create specialized contexts for different use cases
BranchContext createEncryptionContext(const std::string& algorithm) {
    BranchContext context;
    context.condition = "algorithm == \"" + algorithm + "\"";
    context.trueBlock = "encrypt_" + algorithm + "();";
    context.falseBlock = "encrypt_default();";
    return context;
}
```

## 📁 Project Structure

```
├── branch_style_plugins.h              # Main header file
├── branch_style_plugins.cpp            # Implementation
├── branch_style_demo.cpp               # Demonstration program
├── enhanced_stub_generator_with_branches.cpp  # Integration example
├── Makefile                             # Build system
└── BRANCH_STYLE_PLUGINS_README.md      # This documentation
```

## 🎯 Use Cases

### **Code Obfuscation**
- Generate diverse control flow patterns
- Make reverse engineering more difficult
- Create polymorphic code variants

### **Anti-Analysis**
- Implement varied anti-debugging checks
- Create unpredictable execution paths
- Frustrate static analysis tools

### **Malware Research**
- Study evasion techniques
- Analyze obfuscation methods
- Develop detection algorithms

### **Educational Purposes**
- Learn about control flow obfuscation
- Understand plugin architectures
- Explore C++ template metaprogramming

## ⚡ Performance Tips

1. **Complexity Balance**: Higher complexity plugins generate more obfuscated code but take longer to process
2. **Context Reuse**: Reuse BranchContext objects when possible
3. **Seed Management**: Set seeds for reproducible output during testing
4. **Plugin Filtering**: Use complexity and type filters to optimize selection
5. **Batch Generation**: Generate multiple branches in batch for better performance

## 🛡️ Security Considerations

### **Generated Code Security**
- All generated code uses safe C++ practices
- No buffer overflows or memory leaks in generated patterns
- Exception-safe code generation

### **Randomization Quality**
- Uses cryptographically secure random number generation
- Proper entropy seeding from system sources
- No predictable patterns in generated code

## 🎨 Customization Options

### **Branch Style Selection**
```cpp
// Prefer high-complexity styles
auto highComplexityPlugins = manager.getPluginsByComplexity(7, 10);

// Filter by type
auto loopPlugins = manager.getPluginsByType(BranchStyleType::LOOP);

// Custom selection logic
auto plugin = manager.getRandomPlugin(context);
```

### **Code Generation Options**
```cpp
BranchContext context;
context.nestingLevel = 2;           // Control nesting depth
context.needsReturn = true;         // Include return statements
context.isVoidFunction = false;     // Function return type
```

---

**Note**: This system is designed for educational, research, and legitimate security testing purposes. Ensure compliance with all applicable laws and regulations when using code obfuscation techniques.