#include "autonomous_unlimited_code_generator.hpp"
#include "language_modules.cpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <algorithm>
#include <cstring>

using namespace CodeGeneration;

class AutonomousCodeGeneratorCLI {
private:
    std::unique_ptr<AutonomousUnlimitedCodeGenerator> generator;
    bool verboseMode = false;
    bool interactiveMode = false;
    std::string outputDirectory = "./generated";

public:
    AutonomousCodeGeneratorCLI() {
        generator = std::make_unique<AutonomousUnlimitedCodeGenerator>();
        
        // Register language modules
        generator->registerLanguageModule(createLanguageModule(Language::CPlusPlus));
        generator->registerLanguageModule(createLanguageModule(Language::Python));
        generator->registerLanguageModule(createLanguageModule(Language::JavaScript));
        generator->registerLanguageModule(createLanguageModule(Language::Java));
        generator->registerLanguageModule(createLanguageModule(Language::Go));
        generator->registerLanguageModule(createLanguageModule(Language::Rust));
    }

    void showHelp() {
        std::cout << R"(
Autonomous Unlimited Code Generator
==================================

USAGE:
    autocoder [OPTIONS] [COMMAND]

COMMANDS:
    generate        Generate code from description
    interactive     Start interactive mode
    batch          Process batch file
    analyze        Analyze existing code
    convert        Convert code between languages
    scaffold       Generate full project structure
    stats          Show generation statistics

OPTIONS:
    -h, --help              Show this help message
    -v, --verbose           Enable verbose output
    -o, --output DIR        Set output directory (default: ./generated)
    -l, --language LANG     Target language (cpp, python, js, java, go, rust)
    -p, --pattern PATTERN   Code pattern (web, api, cli, ml, game, etc.)
    -c, --complexity LEVEL  Complexity level (1-5)
    --no-ai                 Disable AI assistance
    --no-tests              Skip test generation
    --no-docs               Skip documentation generation

EXAMPLES:
    # Generate a simple web API in Python
    autocoder generate -l python -p api "REST API for user management"
    
    # Interactive mode
    autocoder interactive
    
    # Generate full project with scaffolding
    autocoder scaffold -l javascript -p web "E-commerce website"
    
    # Convert C++ code to Rust
    autocoder convert --from cpp --to rust input.cpp
    
    # Batch processing
    autocoder batch requirements.txt
    
    # Autonomous mode with multiple requirements
    autocoder autonomous "web app,mobile api,cli tool"

SUPPORTED LANGUAGES:
    cpp, c++        C++
    python, py      Python
    javascript, js  JavaScript
    typescript, ts  TypeScript
    java           Java
    csharp, cs     C#
    go             Go
    rust, rs       Rust
    swift          Swift
    kotlin, kt     Kotlin

SUPPORTED PATTERNS:
    web            Web Application
    api            REST API
    graphql        GraphQL API
    cli            Command Line Interface
    desktop        Desktop Application
    mobile         Mobile Application
    ml             Machine Learning
    data           Data Analysis
    game           Game Engine
    lib            Library
    framework      Framework
    microservice   Microservice
    blockchain     Blockchain
    iot            IoT Application
    security       Security Tool

)";
    }

    Language parseLanguage(const std::string& lang) {
        std::string lower = lang;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        
        if (lower == "cpp" || lower == "c++") return Language::CPlusPlus;
        if (lower == "python" || lower == "py") return Language::Python;
        if (lower == "javascript" || lower == "js") return Language::JavaScript;
        if (lower == "typescript" || lower == "ts") return Language::TypeScript;
        if (lower == "java") return Language::Java;
        if (lower == "csharp" || lower == "cs") return Language::CSharp;
        if (lower == "go") return Language::Go;
        if (lower == "rust" || lower == "rs") return Language::Rust;
        if (lower == "swift") return Language::Swift;
        if (lower == "kotlin" || lower == "kt") return Language::Kotlin;
        if (lower == "php") return Language::PHP;
        if (lower == "ruby" || lower == "rb") return Language::Ruby;
        
        return Language::Unknown;
    }

    CodePattern parsePattern(const std::string& pattern) {
        std::string lower = pattern;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        
        if (lower == "web") return CodePattern::WebApplication;
        if (lower == "api") return CodePattern::RestAPI;
        if (lower == "graphql") return CodePattern::GraphQLAPI;
        if (lower == "cli") return CodePattern::CLI;
        if (lower == "desktop") return CodePattern::DesktopApplication;
        if (lower == "mobile") return CodePattern::MobileApplication;
        if (lower == "ml") return CodePattern::MachineLearning;
        if (lower == "data") return CodePattern::DataAnalysis;
        if (lower == "game") return CodePattern::GameEngine;
        if (lower == "lib") return CodePattern::Library;
        if (lower == "framework") return CodePattern::Framework;
        if (lower == "microservice") return CodePattern::Microservice;
        if (lower == "blockchain") return CodePattern::Blockchain;
        if (lower == "iot") return CodePattern::IoT;
        if (lower == "security") return CodePattern::Security;
        
        return CodePattern::CustomPattern;
    }

    ComplexityLevel parseComplexity(const std::string& complexity) {
        int level = std::stoi(complexity);
        switch (level) {
            case 1: return ComplexityLevel::Beginner;
            case 2: return ComplexityLevel::Intermediate;
            case 3: return ComplexityLevel::Advanced;
            case 4: return ComplexityLevel::Expert;
            case 5: return ComplexityLevel::EnterpiseGrade;
            default: return ComplexityLevel::Intermediate;
        }
    }

    void runInteractiveMode() {
        std::cout << "\n=== Autonomous Code Generator Interactive Mode ===\n\n";
        
        GenerationContext context;
        std::string input;
        
        // Get project details
        std::cout << "Project name: ";
        std::getline(std::cin, context.projectName);
        
        std::cout << "Description: ";
        std::getline(std::cin, context.description);
        
        // Select language
        std::cout << "\nAvailable languages:\n";
        std::cout << "1. C++\n2. Python\n3. JavaScript\n4. Java\n5. Go\n6. Rust\n";
        std::cout << "Select language (1-6): ";
        std::getline(std::cin, input);
        
        switch (std::stoi(input)) {
            case 1: context.targetLanguage = Language::CPlusPlus; break;
            case 2: context.targetLanguage = Language::Python; break;
            case 3: context.targetLanguage = Language::JavaScript; break;
            case 4: context.targetLanguage = Language::Java; break;
            case 5: context.targetLanguage = Language::Go; break;
            case 6: context.targetLanguage = Language::Rust; break;
            default: context.targetLanguage = Language::Python; break;
        }
        
        // Select pattern
        std::cout << "\nAvailable patterns:\n";
        std::cout << "1. Web Application\n2. REST API\n3. CLI\n4. Desktop App\n5. Machine Learning\n6. Game\n";
        std::cout << "Select pattern (1-6): ";
        std::getline(std::cin, input);
        
        switch (std::stoi(input)) {
            case 1: context.pattern = CodePattern::WebApplication; break;
            case 2: context.pattern = CodePattern::RestAPI; break;
            case 3: context.pattern = CodePattern::CLI; break;
            case 4: context.pattern = CodePattern::DesktopApplication; break;
            case 5: context.pattern = CodePattern::MachineLearning; break;
            case 6: context.pattern = CodePattern::GameEngine; break;
            default: context.pattern = CodePattern::WebApplication; break;
        }
        
        // Select complexity
        std::cout << "\nComplexity level (1-5): ";
        std::getline(std::cin, input);
        context.complexity = parseComplexity(input);
        
        // Additional requirements
        std::cout << "\nAdditional requirements (comma-separated, or press Enter to skip): ";
        std::getline(std::cin, input);
        if (!input.empty()) {
            std::stringstream ss(input);
            std::string requirement;
            while (std::getline(ss, requirement, ',')) {
                requirement.erase(0, requirement.find_first_not_of(" \t"));
                requirement.erase(requirement.find_last_not_of(" \t") + 1);
                if (!requirement.empty()) {
                    context.requirements.push_back(requirement);
                }
            }
        }
        
        // Generate code
        std::cout << "\nGenerating code...\n";
        auto result = generator->generateCode(context);
        
        // Save to files
        saveGeneratedCode(result, context);
        
        std::cout << "Generation complete! Files saved to: " << outputDirectory << "\n";
        showGenerationSummary(result, context);
    }

    int runGenerateCommand(const std::vector<std::string>& args) {
        GenerationContext context;
        context.targetLanguage = Language::Python; // Default
        context.pattern = CodePattern::WebApplication; // Default
        context.complexity = ComplexityLevel::Intermediate; // Default
        
        std::string description;
        
        for (size_t i = 0; i < args.size(); ++i) {
            const auto& arg = args[i];
            
            if (arg == "-l" || arg == "--language") {
                if (i + 1 < args.size()) {
                    context.targetLanguage = parseLanguage(args[++i]);
                }
            } else if (arg == "-p" || arg == "--pattern") {
                if (i + 1 < args.size()) {
                    context.pattern = parsePattern(args[++i]);
                }
            } else if (arg == "-c" || arg == "--complexity") {
                if (i + 1 < args.size()) {
                    context.complexity = parseComplexity(args[++i]);
                }
            } else if (arg == "--no-ai") {
                context.enableAI = false;
            } else if (arg == "--no-tests") {
                context.enableTesting = false;
            } else if (arg == "--no-docs") {
                context.enableDocumentation = false;
            } else if (arg[0] != '-') {
                description = arg;
            }
        }
        
        if (description.empty()) {
            std::cerr << "Error: Description is required\n";
            return 1;
        }
        
        context.description = description;
        context.projectName = extractProjectName(description);
        
        if (verboseMode) {
            std::cout << "Generating " << Utils::languageToString(context.targetLanguage) 
                     << " " << Utils::patternToString(context.pattern) << "...\n";
        }
        
        auto result = generator->generateCode(context);
        saveGeneratedCode(result, context);
        
        if (verboseMode) {
            showGenerationSummary(result, context);
        }
        
        std::cout << "Code generated successfully in " << outputDirectory << "\n";
        return 0;
    }

    int runBatchCommand(const std::string& batchFile) {
        std::ifstream file(batchFile);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open batch file: " << batchFile << "\n";
            return 1;
        }
        
        std::string line;
        int processed = 0, successful = 0;
        
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;
            
            // Parse line format: language:pattern:complexity:description
            std::vector<std::string> parts;
            std::stringstream ss(line);
            std::string part;
            
            while (std::getline(ss, part, ':')) {
                parts.push_back(part);
            }
            
            if (parts.size() < 4) {
                std::cerr << "Warning: Invalid line format: " << line << "\n";
                continue;
            }
            
            GenerationContext context;
            context.targetLanguage = parseLanguage(parts[0]);
            context.pattern = parsePattern(parts[1]);
            context.complexity = parseComplexity(parts[2]);
            context.description = parts[3];
            context.projectName = extractProjectName(context.description);
            
            processed++;
            
            try {
                auto result = generator->generateCode(context);
                saveGeneratedCode(result, context);
                successful++;
                
                if (verboseMode) {
                    std::cout << "Generated: " << context.projectName << "\n";
                }
            } catch (const std::exception& e) {
                std::cerr << "Error generating " << context.projectName << ": " << e.what() << "\n";
            }
        }
        
        std::cout << "Batch processing complete: " << successful << "/" << processed << " successful\n";
        return 0;
    }

    int runAutonomousMode(const std::string& requirements) {
        std::cout << "Starting autonomous mode...\n";
        
        // Parse comma-separated requirements
        std::vector<std::string> reqList;
        std::stringstream ss(requirements);
        std::string req;
        
        while (std::getline(ss, req, ',')) {
            req.erase(0, req.find_first_not_of(" \t"));
            req.erase(req.find_last_not_of(" \t") + 1);
            if (!req.empty()) {
                reqList.push_back(req);
            }
        }
        
        generator->runAutonomousMode(reqList);
        
        std::cout << "Autonomous generation complete!\n";
        return 0;
    }

    int runScaffoldCommand(const std::vector<std::string>& args) {
        GenerationContext context;
        context.targetLanguage = Language::JavaScript; // Default for web
        context.pattern = CodePattern::WebApplication; // Default
        context.complexity = ComplexityLevel::Intermediate;
        
        std::string description;
        
        // Parse arguments similar to generate command
        for (size_t i = 0; i < args.size(); ++i) {
            const auto& arg = args[i];
            
            if (arg == "-l" || arg == "--language") {
                if (i + 1 < args.size()) {
                    context.targetLanguage = parseLanguage(args[++i]);
                }
            } else if (arg == "-p" || arg == "--pattern") {
                if (i + 1 < args.size()) {
                    context.pattern = parsePattern(args[++i]);
                }
            } else if (arg[0] != '-') {
                description = arg;
            }
        }
        
        if (description.empty()) {
            std::cerr << "Error: Project description is required\n";
            return 1;
        }
        
        context.description = description;
        context.projectName = extractProjectName(description);
        
        std::cout << "Scaffolding full project: " << context.projectName << "\n";
        
        auto result = generator->generateFullProject(context, outputDirectory);
        
        std::cout << "Project scaffolding complete in " << outputDirectory << "\n";
        showGenerationSummary(result, context);
        
        return 0;
    }

    void saveGeneratedCode(const GeneratedCode& result, const GenerationContext& context) {
        // Create output directory
        std::filesystem::create_directories(outputDirectory);
        
        // Determine file extension
        std::string ext;
        switch (context.targetLanguage) {
            case Language::CPlusPlus: ext = ".cpp"; break;
            case Language::Python: ext = ".py"; break;
            case Language::JavaScript: ext = ".js"; break;
            case Language::Java: ext = ".java"; break;
            case Language::Go: ext = ".go"; break;
            case Language::Rust: ext = ".rs"; break;
            default: ext = ".txt"; break;
        }
        
        // Save main code file
        std::string mainFile = outputDirectory + "/" + context.projectName + ext;
        std::ofstream main(mainFile);
        main << result.mainCode;
        main.close();
        
        // Save additional files
        for (const auto& file : result.additionalFiles) {
            std::string filePath = outputDirectory + "/" + file.first;
            std::ofstream f(filePath);
            f << file.second;
            f.close();
        }
        
        // Save documentation
        if (!result.documentation.empty()) {
            std::ofstream docs(outputDirectory + "/README.md");
            docs << result.documentation;
            docs.close();
        }
        
        // Save test code
        if (!result.testCode.empty()) {
            std::string testExt = (context.targetLanguage == Language::CPlusPlus) ? ".cpp" : ext;
            std::ofstream tests(outputDirectory + "/test_" + context.projectName + testExt);
            tests << result.testCode;
            tests.close();
        }
        
        // Save build instructions
        if (!result.buildInstructions.empty()) {
            std::ofstream build(outputDirectory + "/BUILD.md");
            build << result.buildInstructions;
            build.close();
        }
        
        // Save metadata
        std::ofstream meta(outputDirectory + "/generation_metadata.txt");
        meta << result.generationMetadata;
        meta.close();
    }

    void showGenerationSummary(const GeneratedCode& result, const GenerationContext& context) {
        std::cout << "\n=== Generation Summary ===\n";
        std::cout << "Project: " << context.projectName << "\n";
        std::cout << "Language: " << Utils::languageToString(context.targetLanguage) << "\n";
        std::cout << "Pattern: " << Utils::patternToString(context.pattern) << "\n";
        std::cout << "Complexity: " << result.estimatedComplexity << "\n";
        std::cout << "Files generated: " << (1 + result.additionalFiles.size()) << "\n";
        
        if (!result.securityNotes.empty()) {
            std::cout << "\nSecurity Notes:\n";
            for (const auto& note : result.securityNotes) {
                std::cout << "  - " << note << "\n";
            }
        }
        
        if (!result.dependencies.empty()) {
            std::cout << "\nDependencies:\n";
            for (const auto& dep : result.dependencies) {
                std::cout << "  - " << dep << "\n";
            }
        }
        
        std::cout << "\n";
    }

    std::string extractProjectName(const std::string& description) {
        // Simple project name extraction from description
        std::string name = description;
        
        // Remove common words
        std::vector<std::string> stopWords = {"a", "an", "the", "for", "with", "using", "in", "on", "at"};
        
        // Take first meaningful word and capitalize
        std::stringstream ss(name);
        std::string word;
        std::vector<std::string> words;
        
        while (ss >> word) {
            // Remove punctuation
            word.erase(std::remove_if(word.begin(), word.end(), 
                [](char c) { return !std::isalnum(c); }), word.end());
            
            if (!word.empty() && std::find(stopWords.begin(), stopWords.end(), word) == stopWords.end()) {
                words.push_back(word);
            }
        }
        
        if (words.empty()) return "GeneratedProject";
        
        // Capitalize first letter
        std::string result = words[0];
        if (!result.empty()) {
            result[0] = std::toupper(result[0]);
        }
        
        return result;
    }

    int run(int argc, char* argv[]) {
        if (argc < 2) {
            showHelp();
            return 0;
        }
        
        std::vector<std::string> args;
        for (int i = 1; i < argc; ++i) {
            args.push_back(argv[i]);
        }
        
        // Parse global options
        auto it = args.begin();
        while (it != args.end()) {
            if (*it == "-h" || *it == "--help") {
                showHelp();
                return 0;
            } else if (*it == "-v" || *it == "--verbose") {
                verboseMode = true;
                it = args.erase(it);
            } else if (*it == "-o" || *it == "--output") {
                if (it + 1 != args.end()) {
                    outputDirectory = *(it + 1);
                    it = args.erase(it, it + 2);
                } else {
                    std::cerr << "Error: --output requires a directory\n";
                    return 1;
                }
            } else {
                ++it;
            }
        }
        
        if (args.empty()) {
            showHelp();
            return 0;
        }
        
        std::string command = args[0];
        args.erase(args.begin());
        
        if (command == "generate") {
            return runGenerateCommand(args);
        } else if (command == "interactive") {
            runInteractiveMode();
            return 0;
        } else if (command == "batch") {
            if (args.empty()) {
                std::cerr << "Error: batch command requires a file\n";
                return 1;
            }
            return runBatchCommand(args[0]);
        } else if (command == "autonomous") {
            if (args.empty()) {
                std::cerr << "Error: autonomous command requires requirements\n";
                return 1;
            }
            return runAutonomousMode(args[0]);
        } else if (command == "scaffold") {
            return runScaffoldCommand(args);
        } else if (command == "stats") {
            auto stats = generator->getStatistics();
            std::cout << "Generation Statistics:\n";
            std::cout << "Total generations: " << stats.totalGenerations << "\n";
            std::cout << "Successful: " << stats.successfulGenerations << "\n";
            std::cout << "Average complexity: " << stats.averageComplexity << "\n";
            return 0;
        } else {
            std::cerr << "Error: Unknown command: " << command << "\n";
            showHelp();
            return 1;
        }
    }
};

int main(int argc, char* argv[]) {
    try {
        AutonomousCodeGeneratorCLI cli;
        return cli.run(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << "\n";
        return 1;
    }
}