#include "autonomous_unlimited_code_generator.hpp"
#include <regex>
#include <iomanip>
#include <ctime>
#include <random>

namespace CodeGeneration {

// ========================= Template Engine Implementation =========================

void TemplateEngine::loadTemplate(const std::string& name, const std::string& templateContent) {
    templates[name] = templateContent;
}

void TemplateEngine::registerDynamicTemplate(const std::string& name, 
    std::function<std::string(const std::map<std::string, std::string>&)> generator) {
    dynamicTemplates[name] = generator;
}

std::string TemplateEngine::renderTemplate(const std::string& templateName, 
    const std::map<std::string, std::string>& variables) {
    
    // Check for dynamic template first
    auto dynamicIt = dynamicTemplates.find(templateName);
    if (dynamicIt != dynamicTemplates.end()) {
        return dynamicIt->second(variables);
    }
    
    // Check for static template
    auto staticIt = templates.find(templateName);
    if (staticIt != templates.end()) {
        return processTemplate(staticIt->second, variables);
    }
    
    return "Template not found: " + templateName;
}

std::string TemplateEngine::processTemplate(const std::string& templateContent, 
    const std::map<std::string, std::string>& variables) {
    
    std::string result = templateContent;
    
    // Replace variables in format {{variable_name}}
    for (const auto& var : variables) {
        std::string placeholder = "{{" + var.first + "}}";
        size_t pos = 0;
        while ((pos = result.find(placeholder, pos)) != std::string::npos) {
            result.replace(pos, placeholder.length(), var.second);
            pos += var.second.length();
        }
    }
    
    // Handle conditional blocks {{#if condition}}...{{/if}}
    std::regex ifRegex(R"(\{\{#if\s+(\w+)\}\}(.*?)\{\{/if\}\})");
    std::smatch match;
    while (std::regex_search(result, match, ifRegex)) {
        std::string condition = match[1].str();
        std::string content = match[2].str();
        
        auto it = variables.find(condition);
        bool conditionMet = (it != variables.end() && it->second != "false" && it->second != "0" && !it->second.empty());
        
        std::string replacement = conditionMet ? content : "";
        result.replace(match.position(), match.length(), replacement);
    }
    
    // Handle loops {{#each array}}...{{/each}}
    std::regex eachRegex(R"(\{\{#each\s+(\w+)\}\}(.*?)\{\{/each\}\})");
    while (std::regex_search(result, match, eachRegex)) {
        std::string arrayName = match[1].str();
        std::string loopContent = match[2].str();
        
        auto it = variables.find(arrayName);
        std::string replacement;
        if (it != variables.end()) {
            // Simplified: assume array is comma-separated values
            std::stringstream ss(it->second);
            std::string item;
            while (std::getline(ss, item, ',')) {
                std::string itemContent = loopContent;
                // Replace {{this}} with current item
                size_t pos = 0;
                while ((pos = itemContent.find("{{this}}", pos)) != std::string::npos) {
                    itemContent.replace(pos, 8, item);
                    pos += item.length();
                }
                replacement += itemContent;
            }
        }
        result.replace(match.position(), match.length(), replacement);
    }
    
    return result;
}

// ========================= AI Intelligence Implementation =========================

AIIntelligence::AIIntelligence() : gen(rd()) {
    // Initialize language patterns and code snippets
    
    // C++ patterns
    languagePatterns[Language::CPlusPlus] = {
        "#include <", "std::", "class ", "template<", "namespace ",
        "virtual ", "override", "const ", "auto ", "std::vector<",
        "std::string", "std::unique_ptr<", "std::shared_ptr<"
    };
    
    // Python patterns
    languagePatterns[Language::Python] = {
        "def ", "import ", "from ", "class ", "__init__",
        "self.", "return ", "if __name__", "print(", "len(",
        "range(", "str(", "int(", "list(", "dict("
    };
    
    // JavaScript patterns
    languagePatterns[Language::JavaScript] = {
        "function ", "const ", "let ", "var ", "=>",
        "console.log", "document.", "window.", "JSON.",
        "addEventListener", "querySelector", "fetch(", "async "
    };
    
    // Java patterns
    languagePatterns[Language::Java] = {
        "public class", "private ", "public ", "static ",
        "void ", "String ", "System.out", "import java.",
        "extends ", "implements ", "throws ", "try {"
    };
    
    // Common code snippets for intelligent generation
    codeSnippets["error_handling"] = {
        "try { ... } catch (exception) { ... }",
        "if (error) { throw new Error(...); }",
        "assert(condition, \"Error message\");"
    };
    
    codeSnippets["loops"] = {
        "for (auto& item : container) { ... }",
        "for (int i = 0; i < size; ++i) { ... }",
        "while (condition) { ... }"
    };
    
    codeSnippets["functions"] = {
        "auto function_name(params) -> return_type { ... }",
        "def function_name(params): ...",
        "function functionName(params) { ... }"
    };
}

std::string AIIntelligence::analyzeRequirements(const std::string& description) {
    std::string analysis = "Requirement Analysis:\n";
    
    // Detect keywords for different patterns
    std::map<std::string, CodePattern> keywords = {
        {"web", CodePattern::WebApplication},
        {"api", CodePattern::RestAPI},
        {"database", CodePattern::Database},
        {"machine learning", CodePattern::MachineLearning},
        {"ml", CodePattern::MachineLearning},
        {"game", CodePattern::GameEngine},
        {"cli", CodePattern::CLI},
        {"command line", CodePattern::CLI},
        {"mobile", CodePattern::MobileApplication},
        {"desktop", CodePattern::DesktopApplication},
        {"microservice", CodePattern::Microservice},
        {"blockchain", CodePattern::Blockchain},
        {"iot", CodePattern::IoT},
        {"security", CodePattern::Security}
    };
    
    std::vector<CodePattern> detectedPatterns;
    std::string lowerDesc = description;
    std::transform(lowerDesc.begin(), lowerDesc.end(), lowerDesc.begin(), ::tolower);
    
    for (const auto& kw : keywords) {
        if (lowerDesc.find(kw.first) != std::string::npos) {
            detectedPatterns.push_back(kw.second);
        }
    }
    
    analysis += "Detected Patterns: ";
    for (const auto& pattern : detectedPatterns) {
        analysis += Utils::patternToString(pattern) + " ";
    }
    analysis += "\n";
    
    // Analyze complexity
    int complexityScore = 0;
    if (lowerDesc.find("enterprise") != std::string::npos) complexityScore += 5;
    if (lowerDesc.find("scalable") != std::string::npos) complexityScore += 3;
    if (lowerDesc.find("distributed") != std::string::npos) complexityScore += 4;
    if (lowerDesc.find("real-time") != std::string::npos) complexityScore += 3;
    if (lowerDesc.find("high performance") != std::string::npos) complexityScore += 3;
    
    ComplexityLevel complexity = ComplexityLevel::Beginner;
    if (complexityScore >= 8) complexity = ComplexityLevel::EnterpiseGrade;
    else if (complexityScore >= 6) complexity = ComplexityLevel::Expert;
    else if (complexityScore >= 4) complexity = ComplexityLevel::Advanced;
    else if (complexityScore >= 2) complexity = ComplexityLevel::Intermediate;
    
    analysis += "Estimated Complexity: " + std::to_string(static_cast<int>(complexity)) + "\n";
    
    return analysis;
}

std::vector<std::string> AIIntelligence::suggestImprovements(const std::string& code, Language lang) {
    std::vector<std::string> suggestions;
    
    // Generic improvements
    if (code.find("// TODO") != std::string::npos) {
        suggestions.push_back("Complete TODO items");
    }
    
    if (code.find("magic number") != std::string::npos || 
        std::regex_search(code, std::regex(R"(\b\d{2,}\b)"))) {
        suggestions.push_back("Replace magic numbers with named constants");
    }
    
    // Language-specific improvements
    switch (lang) {
        case Language::CPlusPlus:
            if (code.find("new ") != std::string::npos && code.find("delete") == std::string::npos) {
                suggestions.push_back("Consider using smart pointers instead of raw pointers");
            }
            if (code.find("malloc") != std::string::npos) {
                suggestions.push_back("Use C++ new/delete or smart pointers instead of malloc/free");
            }
            break;
            
        case Language::Python:
            if (code.find("except:") != std::string::npos) {
                suggestions.push_back("Use specific exception types instead of bare except");
            }
            if (code.find("global ") != std::string::npos) {
                suggestions.push_back("Minimize use of global variables");
            }
            break;
            
        case Language::JavaScript:
            if (code.find("var ") != std::string::npos) {
                suggestions.push_back("Use 'let' or 'const' instead of 'var'");
            }
            if (code.find("==") != std::string::npos) {
                suggestions.push_back("Use strict equality (===) instead of loose equality (==)");
            }
            break;
            
        default:
            break;
    }
    
    return suggestions;
}

std::string AIIntelligence::generateIntelligentCode(const GenerationContext& context) {
    std::string code;
    
    // Generate based on pattern and language
    switch (context.pattern) {
        case CodePattern::WebApplication:
            if (context.targetLanguage == Language::JavaScript) {
                code = R"(
// Modern Web Application Structure
import express from 'express';
import cors from 'cors';
import helmet from 'helmet';

const app = express();
const PORT = process.env.PORT || 3000;

// Security middleware
app.use(helmet());
app.use(cors());
app.use(express.json({ limit: '10mb' }));

// Routes
app.get('/api/health', (req, res) => {
    res.json({ status: 'OK', timestamp: new Date().toISOString() });
});

// Error handling middleware
app.use((err, req, res, next) => {
    console.error(err.stack);
    res.status(500).json({ error: 'Something went wrong!' });
});

app.listen(PORT, () => {
    console.log(`Server running on port ${PORT}`);
});
)";
            }
            break;
            
        case CodePattern::RestAPI:
            if (context.targetLanguage == Language::Python) {
                code = R"(
from flask import Flask, request, jsonify
from flask_cors import CORS
import logging
from datetime import datetime

app = Flask(__name__)
CORS(app)

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

@app.route('/api/health', methods=['GET'])
def health_check():
    return jsonify({
        'status': 'healthy',
        'timestamp': datetime.utcnow().isoformat(),
        'version': '1.0.0'
    })

@app.errorhandler(404)
def not_found(error):
    return jsonify({'error': 'Resource not found'}), 404

@app.errorhandler(500)
def internal_error(error):
    logger.error(f"Internal error: {error}")
    return jsonify({'error': 'Internal server error'}), 500

if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0', port=5000)
)";
            }
            break;
            
        case CodePattern::CLI:
            if (context.targetLanguage == Language::CPlusPlus) {
                code = R"(
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <functional>

class CommandLineInterface {
private:
    std::map<std::string, std::function<int(const std::vector<std::string>&)>> commands;
    std::string appName;
    
public:
    CommandLineInterface(const std::string& name) : appName(name) {
        registerCommand("help", [this](const std::vector<std::string>& args) {
            return showHelp();
        });
        
        registerCommand("version", [this](const std::vector<std::string>& args) {
            std::cout << appName << " version 1.0.0\n";
            return 0;
        });
    }
    
    void registerCommand(const std::string& name, 
                        std::function<int(const std::vector<std::string>&)> handler) {
        commands[name] = handler;
    }
    
    int run(int argc, char* argv[]) {
        if (argc < 2) {
            return showHelp();
        }
        
        std::string command = argv[1];
        std::vector<std::string> args;
        
        for (int i = 2; i < argc; ++i) {
            args.push_back(argv[i]);
        }
        
        auto it = commands.find(command);
        if (it != commands.end()) {
            return it->second(args);
        } else {
            std::cerr << "Unknown command: " << command << "\n";
            return 1;
        }
    }
    
private:
    int showHelp() {
        std::cout << "Usage: " << appName << " <command> [arguments]\n";
        std::cout << "Available commands:\n";
        for (const auto& cmd : commands) {
            std::cout << "  " << cmd.first << "\n";
        }
        return 0;
    }
};

int main(int argc, char* argv[]) {
    CommandLineInterface cli("MyApp");
    
    // Register custom commands here
    cli.registerCommand("process", [](const std::vector<std::string>& args) {
        if (args.empty()) {
            std::cerr << "Usage: process <input>\n";
            return 1;
        }
        
        std::cout << "Processing: " << args[0] << "\n";
        // Add your processing logic here
        
        return 0;
    });
    
    return cli.run(argc, argv);
}
)";
            }
            break;
            
        default:
            code = "// Generated code for " + Utils::patternToString(context.pattern);
            break;
    }
    
    return code;
}

double AIIntelligence::calculateComplexity(const std::string& code, Language lang) {
    double complexity = 1.0; // Base complexity
    
    // Count control flow statements
    std::vector<std::string> controlFlowKeywords;
    switch (lang) {
        case Language::CPlusPlus:
        case Language::C:
            controlFlowKeywords = {"if", "else", "for", "while", "switch", "case", "try", "catch"};
            break;
        case Language::Python:
            controlFlowKeywords = {"if", "elif", "else", "for", "while", "try", "except", "with"};
            break;
        case Language::JavaScript:
            controlFlowKeywords = {"if", "else", "for", "while", "switch", "case", "try", "catch"};
            break;
        default:
            controlFlowKeywords = {"if", "else", "for", "while"};
            break;
    }
    
    for (const auto& keyword : controlFlowKeywords) {
        size_t pos = 0;
        while ((pos = code.find(keyword, pos)) != std::string::npos) {
            complexity += 1.0;
            pos += keyword.length();
        }
    }
    
    // Count functions/methods
    std::regex functionRegex;
    switch (lang) {
        case Language::CPlusPlus:
            functionRegex = std::regex(R"(\b\w+\s+\w+\s*\([^)]*\)\s*\{)");
            break;
        case Language::Python:
            functionRegex = std::regex(R"(def\s+\w+\s*\([^)]*\):)");
            break;
        case Language::JavaScript:
            functionRegex = std::regex(R"(function\s+\w+\s*\([^)]*\)\s*\{)");
            break;
        default:
            functionRegex = std::regex(R"(\w+\s*\([^)]*\))");
            break;
    }
    
    auto words_begin = std::sregex_iterator(code.begin(), code.end(), functionRegex);
    auto words_end = std::sregex_iterator();
    complexity += std::distance(words_begin, words_end) * 2.0;
    
    // Normalize by lines of code
    size_t lineCount = std::count(code.begin(), code.end(), '\n') + 1;
    complexity = complexity / std::max(1.0, static_cast<double>(lineCount) / 10.0);
    
    return complexity;
}

std::vector<std::string> AIIntelligence::detectSecurityIssues(const std::string& code, Language lang) {
    std::vector<std::string> issues;
    
    // Common security patterns
    if (code.find("eval(") != std::string::npos) {
        issues.push_back("Use of eval() can lead to code injection vulnerabilities");
    }
    
    if (code.find("system(") != std::string::npos || code.find("exec(") != std::string::npos) {
        issues.push_back("System command execution without input validation");
    }
    
    if (code.find("strcpy") != std::string::npos || code.find("strcat") != std::string::npos) {
        issues.push_back("Use of unsafe string functions (buffer overflow risk)");
    }
    
    // Language-specific issues
    switch (lang) {
        case Language::JavaScript:
            if (code.find("innerHTML") != std::string::npos) {
                issues.push_back("innerHTML can lead to XSS vulnerabilities");
            }
            if (code.find("document.write") != std::string::npos) {
                issues.push_back("document.write can cause XSS vulnerabilities");
            }
            break;
            
        case Language::Python:
            if (code.find("pickle.loads") != std::string::npos) {
                issues.push_back("pickle.loads with untrusted data can lead to code execution");
            }
            break;
            
        case Language::CPlusPlus:
            if (code.find("gets(") != std::string::npos) {
                issues.push_back("gets() is unsafe and deprecated");
            }
            break;
            
        default:
            break;
    }
    
    return issues;
}

// ========================= Project Scaffolding Implementation =========================

void ProjectScaffolding::generateProjectStructure(const GenerationContext& context, const std::string& outputDir) {
    std::filesystem::create_directories(outputDir);
    
    // Create standard directory structure based on language and pattern
    switch (context.targetLanguage) {
        case Language::Python:
            std::filesystem::create_directories(outputDir + "/" + context.projectName);
            std::filesystem::create_directories(outputDir + "/tests");
            std::filesystem::create_directories(outputDir + "/docs");
            break;
        case Language::JavaScript:
            std::filesystem::create_directories(outputDir + "/src");
            std::filesystem::create_directories(outputDir + "/tests");
            std::filesystem::create_directories(outputDir + "/public");
            std::filesystem::create_directories(outputDir + "/docs");
            break;
        case Language::CPlusPlus:
            std::filesystem::create_directories(outputDir + "/src");
            std::filesystem::create_directories(outputDir + "/include");
            std::filesystem::create_directories(outputDir + "/tests");
            std::filesystem::create_directories(outputDir + "/docs");
            break;
        default:
            std::filesystem::create_directories(outputDir + "/src");
            std::filesystem::create_directories(outputDir + "/tests");
            break;
    }
}

void ProjectScaffolding::generateConfigFiles(const GenerationContext& context, const std::string& outputDir) {
    // Generate basic config files based on language
    switch (context.targetLanguage) {
        case Language::Python:
            {
                std::ofstream setup(outputDir + "/setup.py");
                setup << "from setuptools import setup, find_packages\n\n";
                setup << "setup(\n";
                setup << "    name='" << context.projectName << "',\n";
                setup << "    version='1.0.0',\n";
                setup << "    description='" << context.description << "',\n";
                setup << "    packages=find_packages(),\n";
                setup << "    install_requires=[],\n";
                setup << ")\n";
                setup.close();
            }
            break;
        case Language::JavaScript:
            // package.json already generated in additional files
            break;
        case Language::CPlusPlus:
            {
                std::ofstream cmake(outputDir + "/CMakeLists.txt");
                cmake << "cmake_minimum_required(VERSION 3.16)\n";
                cmake << "project(" << context.projectName << ")\n\n";
                cmake << "set(CMAKE_CXX_STANDARD 17)\n";
                cmake << "set(CMAKE_CXX_STANDARD_REQUIRED ON)\n\n";
                cmake << "add_executable(" << context.projectName << " src/main.cpp)\n";
                cmake.close();
            }
            break;
        default:
            break;
    }
}

void ProjectScaffolding::generateBuildFiles(const GenerationContext& context, const std::string& outputDir) {
    // Generate build files based on language
    switch (context.targetLanguage) {
        case Language::CPlusPlus:
            {
                std::ofstream makefile(outputDir + "/Makefile");
                makefile << "CXX = g++\n";
                makefile << "CXXFLAGS = -std=c++17 -Wall -Wextra -O2\n";
                makefile << "TARGET = " << context.projectName << "\n";
                makefile << "SOURCES = src/main.cpp\n\n";
                makefile << "$(TARGET): $(SOURCES)\n";
                makefile << "\t$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)\n\n";
                makefile << "clean:\n";
                makefile << "\trm -f $(TARGET)\n\n";
                makefile << ".PHONY: clean\n";
                makefile.close();
            }
            break;
        default:
            break;
    }
}

void ProjectScaffolding::generateDocumentationStructure(const GenerationContext& context, const std::string& outputDir) {
    std::filesystem::create_directories(outputDir + "/docs");
    
    // Create basic documentation files
    std::ofstream readme(outputDir + "/docs/API.md");
    readme << "# " << context.projectName << " API Documentation\n\n";
    readme << "## Overview\n\n";
    readme << context.description << "\n\n";
    readme << "## Usage\n\n";
    readme << "TODO: Add usage examples\n\n";
    readme.close();
    
    std::ofstream changelog(outputDir + "/CHANGELOG.md");
    changelog << "# Changelog\n\n";
    changelog << "## [1.0.0] - " << std::to_string(std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count()) << "\n\n";
    changelog << "### Added\n";
    changelog << "- Initial release\n";
    changelog << "- " << context.description << "\n";
    changelog.close();
}

std::vector<std::string> ProjectScaffolding::getRecommendedStructure(CodePattern pattern, Language lang) {
    std::vector<std::string> structure;
    
    // Base structure
    structure.push_back("src/");
    structure.push_back("tests/");
    structure.push_back("docs/");
    
    // Pattern-specific additions
    switch (pattern) {
        case CodePattern::WebApplication:
            structure.push_back("public/");
            structure.push_back("assets/");
            if (lang == Language::JavaScript) {
                structure.push_back("components/");
                structure.push_back("pages/");
            }
            break;
        case CodePattern::RestAPI:
            structure.push_back("api/");
            structure.push_back("models/");
            structure.push_back("controllers/");
            break;
        default:
            break;
    }
    
    return structure;
}

// ========================= Main Generator Implementation =========================

AutonomousUnlimitedCodeGenerator::AutonomousUnlimitedCodeGenerator() {
    templateEngine = std::make_unique<TemplateEngine>();
    aiIntelligence = std::make_unique<AIIntelligence>();
    projectScaffolding = std::make_unique<ProjectScaffolding>();
    
    // Initialize default templates
    initializeDefaultTemplates();
}

AutonomousUnlimitedCodeGenerator::~AutonomousUnlimitedCodeGenerator() = default;

void AutonomousUnlimitedCodeGenerator::initializeDefaultTemplates() {
    // C++ class template
    templateEngine->loadTemplate("cpp_class", R"(
#pragma once

{{#if includes}}
{{#each includes}}
#include {{this}}
{{/each}}
{{/if}}

class {{className}} {
private:
    {{#each privateMembers}}
    {{this}};
    {{/each}}

public:
    {{className}}();
    ~{{className}}();
    
    {{#each publicMethods}}
    {{this}};
    {{/each}}
};
)");

    // Python class template
    templateEngine->loadTemplate("python_class", R"(
{{#if imports}}
{{#each imports}}
{{this}}
{{/each}}

{{/if}}
class {{className}}:
    """{{description}}"""
    
    def __init__(self):
        """Initialize {{className}}"""
        {{#each attributes}}
        self.{{this}} = None
        {{/each}}
    
    {{#each methods}}
    def {{this}}(self):
        """TODO: Implement {{this}}"""
        pass
    
    {{/each}}
)");

    // JavaScript module template
    templateEngine->loadTemplate("js_module", R"(
{{#if imports}}
{{#each imports}}
{{this}}
{{/each}}

{{/if}}
/**
 * {{description}}
 */
class {{className}} {
    constructor() {
        {{#each attributes}}
        this.{{this}} = null;
        {{/each}}
    }
    
    {{#each methods}}
    {{this}}() {
        // TODO: Implement {{this}}
    }
    
    {{/each}}
}

export default {{className}};
)");
}

GeneratedCode AutonomousUnlimitedCodeGenerator::generateCode(const GenerationContext& context) {
    auto startTime = std::chrono::high_resolution_clock::now();
    stats.totalGenerations++;
    
    GeneratedCode result;
    
    try {
        // Use AI to generate intelligent code
        if (context.enableAI) {
            result.mainCode = aiIntelligence->generateIntelligentCode(context);
        } else {
            // Fallback to template-based generation
            result.mainCode = generateTemplateBasedCode(context);
        }
        
        // Generate additional files based on pattern
        generateAdditionalFiles(context, result);
        
        // Generate documentation
        if (context.enableDocumentation) {
            result.documentation = generateDocumentation(context);
        }
        
        // Generate tests
        if (context.enableTesting) {
            result.testCode = generateTests(context, result.mainCode);
        }
        
        // Security analysis
        if (context.enableSecurity) {
            result.securityNotes = aiIntelligence->detectSecurityIssues(result.mainCode, context.targetLanguage);
        }
        
        // Calculate complexity
        result.estimatedComplexity = aiIntelligence->calculateComplexity(result.mainCode, context.targetLanguage);
        
        // Generate build instructions
        result.buildInstructions = generateBuildInstructions(context);
        
        // Generate metadata
        result.generationMetadata = generateMetadata(context);
        
        stats.successfulGenerations++;
        stats.languageUsage[context.targetLanguage]++;
        stats.patternUsage[context.pattern]++;
        
    } catch (const std::exception& e) {
        result.mainCode = "// Error during generation: " + std::string(e.what());
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    stats.averageGenerationTime = 
        std::chrono::milliseconds((stats.averageGenerationTime.count() * (stats.totalGenerations - 1) + duration.count()) / stats.totalGenerations);
    
    return result;
}

std::string AutonomousUnlimitedCodeGenerator::generateTemplateBasedCode(const GenerationContext& context) {
    // Template-based fallback generation logic
    std::map<std::string, std::string> variables;
    variables["className"] = context.projectName;
    variables["description"] = context.description;
    
    switch (context.targetLanguage) {
        case Language::CPlusPlus:
            return templateEngine->renderTemplate("cpp_class", variables);
        case Language::Python:
            return templateEngine->renderTemplate("python_class", variables);
        case Language::JavaScript:
            return templateEngine->renderTemplate("js_module", variables);
        default:
            return "// Code generation for " + Utils::languageToString(context.targetLanguage) + " not implemented";
    }
}

void AutonomousUnlimitedCodeGenerator::generateAdditionalFiles(const GenerationContext& context, GeneratedCode& result) {
    // Generate configuration files
    switch (context.pattern) {
        case CodePattern::WebApplication:
            if (context.targetLanguage == Language::JavaScript) {
                result.additionalFiles["package.json"] = R"({
  "name": ")" + context.projectName + R"(",
  "version": "1.0.0",
  "description": ")" + context.description + R"(",
  "main": "index.js",
  "scripts": {
    "start": "node index.js",
    "dev": "nodemon index.js",
    "test": "jest"
  },
  "dependencies": {
    "express": "^4.18.0",
    "cors": "^2.8.5",
    "helmet": "^6.0.0"
  },
  "devDependencies": {
    "nodemon": "^2.0.20",
    "jest": "^29.0.0"
  }
})";
                
                result.additionalFiles[".env"] = R"(PORT=3000
NODE_ENV=development
)";
            }
            break;
            
        case CodePattern::RestAPI:
            if (context.targetLanguage == Language::Python) {
                result.additionalFiles["requirements.txt"] = R"(Flask==2.2.2
Flask-CORS==3.0.10
gunicorn==20.1.0
pytest==7.1.3
)";
                
                result.additionalFiles["config.py"] = R"(import os

class Config:
    SECRET_KEY = os.environ.get('SECRET_KEY') or 'dev-secret-key'
    DEBUG = os.environ.get('DEBUG', 'True').lower() == 'true'
    HOST = os.environ.get('HOST', '0.0.0.0')
    PORT = int(os.environ.get('PORT', 5000))
)";
            }
            break;
            
        default:
            break;
    }
}

std::string AutonomousUnlimitedCodeGenerator::generateDocumentation(const GenerationContext& context) {
    std::string docs = "# " + context.projectName + "\n\n";
    docs += context.description + "\n\n";
    
    docs += "## Overview\n";
    docs += "This project was generated using the Autonomous Unlimited Code Generator.\n\n";
    
    docs += "**Language**: " + Utils::languageToString(context.targetLanguage) + "\n";
    docs += "**Pattern**: " + Utils::patternToString(context.pattern) + "\n";
    docs += "**Complexity**: " + std::to_string(static_cast<int>(context.complexity)) + "\n\n";
    
    docs += "## Features\n";
    for (const auto& req : context.requirements) {
        docs += "- " + req + "\n";
    }
    docs += "\n";
    
    docs += "## Dependencies\n";
    for (const auto& dep : context.dependencies) {
        docs += "- " + dep + "\n";
    }
    docs += "\n";
    
    docs += "## Getting Started\n";
    docs += "1. Install dependencies\n";
    docs += "2. Configure environment variables\n";
    docs += "3. Run the application\n\n";
    
    docs += "## Generated Features\n";
    if (context.enableAI) docs += "- AI-powered code generation\n";
    if (context.enableTesting) docs += "- Automated testing\n";
    if (context.enableSecurity) docs += "- Security analysis\n";
    if (context.enableOptimization) docs += "- Performance optimization\n";
    
    return docs;
}

std::string AutonomousUnlimitedCodeGenerator::generateTests(const GenerationContext& context, const std::string& mainCode) {
    std::string tests;
    
    switch (context.targetLanguage) {
        case Language::JavaScript:
            tests = R"(
const request = require('supertest');
const app = require('./index.js');

describe('Application Tests', () => {
    test('Health check endpoint', async () => {
        const response = await request(app)
            .get('/api/health')
            .expect(200);
        
        expect(response.body).toHaveProperty('status', 'OK');
        expect(response.body).toHaveProperty('timestamp');
    });
    
    test('Invalid endpoint returns 404', async () => {
        await request(app)
            .get('/api/nonexistent')
            .expect(404);
    });
});
)";
            break;
            
        case Language::Python:
            tests = R"(
import pytest
import json
from app import app

@pytest.fixture
def client():
    app.config['TESTING'] = True
    with app.test_client() as client:
        yield client

def test_health_check(client):
    """Test the health check endpoint"""
    response = client.get('/api/health')
    assert response.status_code == 200
    data = json.loads(response.data)
    assert 'status' in data
    assert data['status'] == 'healthy'

def test_not_found(client):
    """Test 404 error handling"""
    response = client.get('/api/nonexistent')
    assert response.status_code == 404
)";
            break;
            
        case Language::CPlusPlus:
            tests = R"(
#include <gtest/gtest.h>
#include ")" + context.projectName + R"(.hpp"

class )" + context.projectName + R"(Test : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup test fixtures
    }
    
    void TearDown() override {
        // Cleanup after tests
    }
};

TEST_F()" + context.projectName + R"(Test, BasicFunctionality) {
    // Test basic functionality
    EXPECT_TRUE(true);
}

TEST_F()" + context.projectName + R"(Test, ErrorHandling) {
    // Test error handling
    EXPECT_NO_THROW({
        // Your test code here
    });
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
)";
            break;
            
        default:
            tests = "// Tests for " + Utils::languageToString(context.targetLanguage) + " not implemented";
            break;
    }
    
    return tests;
}

std::string AutonomousUnlimitedCodeGenerator::generateBuildInstructions(const GenerationContext& context) {
    std::string instructions;
    
    switch (context.targetLanguage) {
        case Language::JavaScript:
            instructions = R"(## Build Instructions

### Prerequisites
- Node.js 16+ 
- npm or yarn

### Installation
```bash
npm install
```

### Development
```bash
npm run dev
```

### Production
```bash
npm start
```

### Testing
```bash
npm test
```
)";
            break;
            
        case Language::Python:
            instructions = R"(## Build Instructions

### Prerequisites
- Python 3.8+
- pip

### Installation
```bash
pip install -r requirements.txt
```

### Development
```bash
python app.py
```

### Production
```bash
gunicorn app:app
```

### Testing
```bash
pytest
```
)";
            break;
            
        case Language::CPlusPlus:
            instructions = R"(## Build Instructions

### Prerequisites
- C++17 compatible compiler
- CMake 3.16+

### Build
```bash
mkdir build
cd build
cmake ..
make
```

### Testing
```bash
make test
```

### Installation
```bash
make install
```
)";
            break;
            
        default:
            instructions = "Build instructions for " + Utils::languageToString(context.targetLanguage) + " not available";
            break;
    }
    
    return instructions;
}

std::string AutonomousUnlimitedCodeGenerator::generateMetadata(const GenerationContext& context) {
    std::string metadata = "Generation Metadata:\n";
    metadata += "Timestamp: " + std::to_string(std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count()) + "\n";
    metadata += "Generator Version: 1.0.0\n";
    metadata += "Language: " + Utils::languageToString(context.targetLanguage) + "\n";
    metadata += "Pattern: " + Utils::patternToString(context.pattern) + "\n";
    metadata += "Complexity Level: " + std::to_string(static_cast<int>(context.complexity)) + "\n";
    metadata += "AI Enabled: " + std::string(context.enableAI ? "true" : "false") + "\n";
    metadata += "Security Enabled: " + std::string(context.enableSecurity ? "true" : "false") + "\n";
    metadata += "Testing Enabled: " + std::string(context.enableTesting ? "true" : "false") + "\n";
    metadata += "Unique ID: " + Utils::generateUniqueIdentifier() + "\n";
    
    return metadata;
}

void AutonomousUnlimitedCodeGenerator::registerLanguageModule(std::unique_ptr<LanguageModule> module) {
    if (module) {
        Language lang = module->getLanguage();
        languageModules[lang] = std::move(module);
    }
}

GeneratedCode AutonomousUnlimitedCodeGenerator::generateFullProject(const GenerationContext& context, const std::string& outputDir) {
    // Generate the main code first
    GeneratedCode result = generateCode(context);
    
    // Use project scaffolding to create full project structure
    if (projectScaffolding) {
        projectScaffolding->generateProjectStructure(context, outputDir);
        projectScaffolding->generateConfigFiles(context, outputDir);
        projectScaffolding->generateBuildFiles(context, outputDir);
        projectScaffolding->generateDocumentationStructure(context, outputDir);
    }
    
    return result;
}

AutonomousUnlimitedCodeGenerator::GenerationStats AutonomousUnlimitedCodeGenerator::getStatistics() const {
    return stats;
}

void AutonomousUnlimitedCodeGenerator::runAutonomousMode(const std::vector<std::string>& requirements) {
    for (const auto& requirement : requirements) {
        try {
            // Analyze requirement and determine appropriate language and pattern
            std::string analysis = aiIntelligence->analyzeRequirements(requirement);
            
            // Create generation context based on analysis
            GenerationContext context;
            context.description = requirement;
            context.projectName = extractProjectNameFromDescription(requirement);
            context.targetLanguage = determineLanguageFromRequirement(requirement);
            context.pattern = determinePatternFromRequirement(requirement);
            context.complexity = ComplexityLevel::Intermediate;
            
            // Generate the code
            GeneratedCode result = generateCode(context);
            
            // Save to separate directory for each project
            std::string projectDir = "./autonomous_generated/" + context.projectName;
            std::filesystem::create_directories(projectDir);
            
            // Export the generated code
            exportGeneration(result, projectDir);
            
        } catch (const std::exception& e) {
            std::cerr << "Error processing requirement '" << requirement << "': " << e.what() << std::endl;
        }
    }
}

void AutonomousUnlimitedCodeGenerator::exportGeneration(const GeneratedCode& code, const std::string& outputDir) {
    std::filesystem::create_directories(outputDir);
    
    // Save main code file (determine extension from context)
    std::ofstream mainFile(outputDir + "/main.txt");
    mainFile << code.mainCode;
    mainFile.close();
    
    // Save additional files
    for (const auto& file : code.additionalFiles) {
        std::ofstream f(outputDir + "/" + file.first);
        f << file.second;
        f.close();
    }
    
    // Save documentation
    if (!code.documentation.empty()) {
        std::ofstream docs(outputDir + "/README.md");
        docs << code.documentation;
        docs.close();
    }
    
    // Save test code
    if (!code.testCode.empty()) {
        std::ofstream tests(outputDir + "/tests.txt");
        tests << code.testCode;
        tests.close();
    }
    
    // Save build instructions
    if (!code.buildInstructions.empty()) {
        std::ofstream build(outputDir + "/BUILD.md");
        build << code.buildInstructions;
        build.close();
    }
    
    // Save metadata
    std::ofstream meta(outputDir + "/metadata.txt");
    meta << code.generationMetadata;
    meta.close();
}

std::string AutonomousUnlimitedCodeGenerator::extractProjectNameFromDescription(const std::string& description) {
    // Simple project name extraction
    std::stringstream ss(description);
    std::string word;
    std::vector<std::string> words;
    
    while (ss >> word && words.size() < 3) {
        // Remove punctuation and convert to lowercase
        std::string cleanWord;
        for (char c : word) {
            if (std::isalnum(c)) {
                cleanWord += std::tolower(c);
            }
        }
        if (!cleanWord.empty() && cleanWord.length() > 2) {
            words.push_back(cleanWord);
        }
    }
    
    if (words.empty()) return "Project";
    
    // Capitalize first letter of first word
    std::string result = words[0];
    if (!result.empty()) {
        result[0] = std::toupper(result[0]);
    }
    
    return result;
}

Language AutonomousUnlimitedCodeGenerator::determineLanguageFromRequirement(const std::string& requirement) {
    std::string lower = requirement;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    
    // Language hints in requirements
    if (lower.find("python") != std::string::npos || lower.find("flask") != std::string::npos || 
        lower.find("django") != std::string::npos) return Language::Python;
    if (lower.find("javascript") != std::string::npos || lower.find("js") != std::string::npos || 
        lower.find("node") != std::string::npos || lower.find("react") != std::string::npos) return Language::JavaScript;
    if (lower.find("c++") != std::string::npos || lower.find("cpp") != std::string::npos) return Language::CPlusPlus;
    if (lower.find("java") != std::string::npos || lower.find("spring") != std::string::npos) return Language::Java;
    if (lower.find("go") != std::string::npos || lower.find("golang") != std::string::npos) return Language::Go;
    if (lower.find("rust") != std::string::npos) return Language::Rust;
    
    // Default based on pattern
    if (lower.find("web") != std::string::npos || lower.find("frontend") != std::string::npos) return Language::JavaScript;
    if (lower.find("api") != std::string::npos || lower.find("backend") != std::string::npos) return Language::Python;
    if (lower.find("cli") != std::string::npos || lower.find("tool") != std::string::npos) return Language::CPlusPlus;
    if (lower.find("game") != std::string::npos) return Language::Rust;
    if (lower.find("microservice") != std::string::npos) return Language::Go;
    
    return Language::Python; // Default
}

CodePattern AutonomousUnlimitedCodeGenerator::determinePatternFromRequirement(const std::string& requirement) {
    std::string lower = requirement;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    
    if (lower.find("web") != std::string::npos || lower.find("website") != std::string::npos || 
        lower.find("frontend") != std::string::npos) return CodePattern::WebApplication;
    if (lower.find("api") != std::string::npos || lower.find("rest") != std::string::npos || 
        lower.find("backend") != std::string::npos) return CodePattern::RestAPI;
    if (lower.find("cli") != std::string::npos || lower.find("command") != std::string::npos || 
        lower.find("tool") != std::string::npos) return CodePattern::CLI;
    if (lower.find("game") != std::string::npos) return CodePattern::GameEngine;
    if (lower.find("mobile") != std::string::npos || lower.find("app") != std::string::npos) return CodePattern::MobileApplication;
    if (lower.find("desktop") != std::string::npos) return CodePattern::DesktopApplication;
    if (lower.find("ml") != std::string::npos || lower.find("machine learning") != std::string::npos || 
        lower.find("ai") != std::string::npos) return CodePattern::MachineLearning;
    if (lower.find("microservice") != std::string::npos) return CodePattern::Microservice;
    if (lower.find("blockchain") != std::string::npos) return CodePattern::Blockchain;
    if (lower.find("iot") != std::string::npos) return CodePattern::IoT;
    
    return CodePattern::WebApplication; // Default
}

// ========================= Utility Functions Implementation =========================

namespace Utils {

Language detectLanguageFromExtension(const std::string& filename) {
    size_t dotPos = filename.find_last_of('.');
    if (dotPos == std::string::npos) return Language::Unknown;
    
    std::string ext = filename.substr(dotPos);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    static const std::map<std::string, Language> extMap = {
        {".cpp", Language::CPlusPlus}, {".cxx", Language::CPlusPlus}, {".cc", Language::CPlusPlus},
        {".hpp", Language::CPlusPlus}, {".h", Language::CPlusPlus}, {".hxx", Language::CPlusPlus},
        {".py", Language::Python}, {".pyw", Language::Python},
        {".js", Language::JavaScript}, {".mjs", Language::JavaScript},
        {".ts", Language::TypeScript}, {".tsx", Language::TypeScript},
        {".java", Language::Java},
        {".cs", Language::CSharp},
        {".go", Language::Go},
        {".rs", Language::Rust},
        {".swift", Language::Swift},
        {".kt", Language::Kotlin},
        {".php", Language::PHP},
        {".rb", Language::Ruby},
        {".c", Language::C},
        {".asm", Language::Assembly}, {".s", Language::Assembly},
        {".html", Language::HTML}, {".htm", Language::HTML},
        {".css", Language::CSS},
        {".sql", Language::SQL},
        {".sh", Language::Shell}, {".bash", Language::Shell},
        {".ps1", Language::PowerShell},
        {".r", Language::R},
        {".m", Language::MATLAB},
        {".hs", Language::Haskell},
        {".scala", Language::Scala},
        {".pl", Language::Perl},
        {".lua", Language::Lua},
        {".vba", Language::VBA}
    };
    
    auto it = extMap.find(ext);
    return (it != extMap.end()) ? it->second : Language::Unknown;
}

std::string languageToString(Language lang) {
    switch (lang) {
        case Language::CPlusPlus: return "C++";
        case Language::Python: return "Python";
        case Language::JavaScript: return "JavaScript";
        case Language::TypeScript: return "TypeScript";
        case Language::Java: return "Java";
        case Language::CSharp: return "C#";
        case Language::Go: return "Go";
        case Language::Rust: return "Rust";
        case Language::Swift: return "Swift";
        case Language::Kotlin: return "Kotlin";
        case Language::PHP: return "PHP";
        case Language::Ruby: return "Ruby";
        case Language::C: return "C";
        case Language::Assembly: return "Assembly";
        case Language::HTML: return "HTML";
        case Language::CSS: return "CSS";
        case Language::SQL: return "SQL";
        case Language::Shell: return "Shell";
        case Language::PowerShell: return "PowerShell";
        case Language::R: return "R";
        case Language::MATLAB: return "MATLAB";
        case Language::Haskell: return "Haskell";
        case Language::Scala: return "Scala";
        case Language::Perl: return "Perl";
        case Language::Lua: return "Lua";
        case Language::VBA: return "VBA";
        default: return "Unknown";
    }
}

std::string patternToString(CodePattern pattern) {
    switch (pattern) {
        case CodePattern::WebApplication: return "Web Application";
        case CodePattern::DesktopApplication: return "Desktop Application";
        case CodePattern::MobileApplication: return "Mobile Application";
        case CodePattern::RestAPI: return "REST API";
        case CodePattern::GraphQLAPI: return "GraphQL API";
        case CodePattern::Database: return "Database";
        case CodePattern::MachineLearning: return "Machine Learning";
        case CodePattern::DataAnalysis: return "Data Analysis";
        case CodePattern::GameEngine: return "Game Engine";
        case CodePattern::CLI: return "CLI";
        case CodePattern::Library: return "Library";
        case CodePattern::Framework: return "Framework";
        case CodePattern::Microservice: return "Microservice";
        case CodePattern::DevOps: return "DevOps";
        case CodePattern::Automation: return "Automation";
        case CodePattern::Security: return "Security";
        case CodePattern::Blockchain: return "Blockchain";
        case CodePattern::IoT: return "IoT";
        default: return "Custom Pattern";
    }
}

std::string generateRandomString(size_t length) {
    static const std::string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, chars.size() - 1);
    
    std::string result;
    result.reserve(length);
    
    for (size_t i = 0; i < length; ++i) {
        result += chars[dis(gen)];
    }
    
    return result;
}

std::string generateUniqueIdentifier() {
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    std::string randomPart = generateRandomString(8);
    return "gen_" + std::to_string(timestamp) + "_" + randomPart;
}

} // namespace Utils

} // namespace CodeGeneration