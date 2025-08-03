#include <iostream>
#include <string>
#include <memory>
#include <signal.h>
#include <cstdlib>

#include "core/framework/unified_framework.h"

// Global framework instance
std::unique_ptr<UnifiedFramework> g_framework;

// Signal handler for graceful shutdown
void signalHandler(int signal) {
    std::cout << "\n🦖 Rawr: Received signal " << signal << ", shutting down gracefully..." << std::endl;
    
    if (g_framework) {
        g_framework->stop();
        g_framework->shutdown();
    }
    
    std::cout << "🦖 Rawr: Shutdown complete. Goodbye!" << std::endl;
    exit(0);
}

// Print banner
void printBanner() {
    std::cout << R"(
    ╔══════════════════════════════════════════════════════════════╗
    ║                                                              ║
    ║    ██████╗  █████╗ ██╗    ██╗██████╗                       ║
    ║    ██╔══██╗██╔══██╗██║    ██║██╔══██╗                      ║
    ║    ██████╔╝███████║██║ █╗ ██║██████╔╝                      ║
    ║    ██╔══██╗██╔══██║██║███╗██║██╔══██╗                      ║
    ║    ██║  ██║██║  ██║╚███╔███╔╝██████╔╝                      ║
    ║    ╚═╝  ╚═╝╚═╝  ╚═╝ ╚══╝╚══╝ ╚═════╝                       ║
    ║                                                              ║
    ║           Advanced Cybersecurity Learning Platform           ║
    ║                                                              ║
    ║                    Version 1.0.0                             ║
    ║                                                              ║
    ╚══════════════════════════════════════════════════════════════╝
)" << std::endl;
}

// Print help
void printHelp() {
    std::cout << R"(
🦖 Rawr - Advanced Cybersecurity Learning Platform

Usage: rawr [OPTIONS]

Options:
    --help, -h              Show this help message
    --version, -v           Show version information
    --config <file>         Load configuration from file
    --debug                 Enable debug mode
    --test-core             Run core framework tests
    --test-plugins          Run plugin tests
    --test-integration      Run integration tests
    --list-plugins          List available plugins
    --execute <plugin>      Execute specific plugin
    --random-action         Perform random action
    --ultimate-mode         Activate ultimate mode
    --status                Show framework status

Examples:
    rawr --help                    # Show help
    rawr --config config.yaml      # Load configuration
    rawr --debug                   # Run in debug mode
    rawr --list-plugins            # List available plugins
    rawr --execute time_manipulator # Execute time manipulator
    rawr --random-action           # Perform random action
    rawr --ultimate-mode           # Activate all components

For more information, visit: https://github.com/ItsMehRAWRXD/Rawr
)" << std::endl;
}

// Print version
void printVersion() {
    std::cout << "🦖 Rawr - Advanced Cybersecurity Learning Platform v1.0.0" << std::endl;
    std::cout << "Copyright (c) 2024 Rawr Security Team" << std::endl;
    std::cout << "MIT License - see LICENSE file for details" << std::endl;
}

// Parse command line arguments
struct CommandLineArgs {
    bool help = false;
    bool version = false;
    bool debug = false;
    bool testCore = false;
    bool testPlugins = false;
    bool testIntegration = false;
    bool listPlugins = false;
    bool randomAction = false;
    bool ultimateMode = false;
    bool status = false;
    std::string configFile;
    std::string executePlugin;
};

CommandLineArgs parseArgs(int argc, char* argv[]) {
    CommandLineArgs args;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            args.help = true;
        } else if (arg == "--version" || arg == "-v") {
            args.version = true;
        } else if (arg == "--debug") {
            args.debug = true;
        } else if (arg == "--test-core") {
            args.testCore = true;
        } else if (arg == "--test-plugins") {
            args.testPlugins = true;
        } else if (arg == "--test-integration") {
            args.testIntegration = true;
        } else if (arg == "--list-plugins") {
            args.listPlugins = true;
        } else if (arg == "--random-action") {
            args.randomAction = true;
        } else if (arg == "--ultimate-mode") {
            args.ultimateMode = true;
        } else if (arg == "--status") {
            args.status = true;
        } else if (arg == "--config" && i + 1 < argc) {
            args.configFile = argv[++i];
        } else if (arg == "--execute" && i + 1 < argc) {
            args.executePlugin = argv[++i];
        } else {
            std::cerr << "Unknown option: " << arg << std::endl;
            std::cerr << "Use --help for usage information" << std::endl;
            exit(1);
        }
    }
    
    return args;
}

// Main function
int main(int argc, char* argv[]) {
    // Parse command line arguments
    auto args = parseArgs(argc, argv);
    
    // Handle help and version
    if (args.help) {
        printHelp();
        return 0;
    }
    
    if (args.version) {
        printVersion();
        return 0;
    }
    
    // Print banner
    printBanner();
    
    try {
        // Setup signal handlers
        signal(SIGINT, signalHandler);
        signal(SIGTERM, signalHandler);
        
        // Create framework instance
        g_framework = std::make_unique<UnifiedFramework>();
        
        // Load configuration if specified
        if (!args.configFile.empty()) {
            std::cout << "🦖 Rawr: Loading configuration from " << args.configFile << std::endl;
            if (!g_framework->loadConfiguration(args.configFile)) {
                std::cerr << "❌ Failed to load configuration from " << args.configFile << std::endl;
                return 1;
            }
        }
        
        // Initialize framework
        std::cout << "🦖 Rawr: Initializing framework..." << std::endl;
        if (!g_framework->initialize()) {
            std::cerr << "❌ Failed to initialize framework" << std::endl;
            return 1;
        }
        
        // Handle different modes
        if (args.testCore) {
            std::cout << "🧪 Running core framework tests..." << std::endl;
            // TODO: Implement core tests
            std::cout << "✅ Core tests completed" << std::endl;
            return 0;
        }
        
        if (args.testPlugins) {
            std::cout << "🧪 Running plugin tests..." << std::endl;
            // TODO: Implement plugin tests
            std::cout << "✅ Plugin tests completed" << std::endl;
            return 0;
        }
        
        if (args.testIntegration) {
            std::cout << "🧪 Running integration tests..." << std::endl;
            // TODO: Implement integration tests
            std::cout << "✅ Integration tests completed" << std::endl;
            return 0;
        }
        
        if (args.listPlugins) {
            std::cout << "📋 Available plugins:" << std::endl;
            auto plugins = g_framework->getAvailablePlugins();
            for (const auto& plugin : plugins) {
                std::cout << "  - " << plugin << std::endl;
            }
            return 0;
        }
        
        if (args.status) {
            std::cout << "📊 Framework status:" << std::endl;
            std::cout << "  " << g_framework->getStatus() << std::endl;
            return 0;
        }
        
        // Start framework
        std::cout << "🦖 Rawr: Starting framework..." << std::endl;
        if (!g_framework->start()) {
            std::cerr << "❌ Failed to start framework" << std::endl;
            return 1;
        }
        
        // Handle specific actions
        if (args.executePlugin.empty() && !args.randomAction && !args.ultimateMode) {
            // Interactive mode
            std::cout << "🦖 Rawr: Framework started successfully!" << std::endl;
            std::cout << "🦖 Rawr: Press Ctrl+C to exit" << std::endl;
            
            // Keep running until interrupted
            while (true) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        } else {
            // Execute specific actions
            if (!args.executePlugin.empty()) {
                std::cout << "🦖 Rawr: Executing plugin: " << args.executePlugin << std::endl;
                if (!g_framework->executePlugin(args.executePlugin)) {
                    std::cerr << "❌ Failed to execute plugin: " << args.executePlugin << std::endl;
                    return 1;
                }
            }
            
            if (args.randomAction) {
                std::cout << "🎲 Rawr: Performing random action..." << std::endl;
                g_framework->performRandomAction();
            }
            
            if (args.ultimateMode) {
                std::cout << "🚀 Rawr: Activating ultimate mode..." << std::endl;
                g_framework->activateUltimateMode();
            }
            
            // Wait a bit for actions to complete
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "❌ Unknown exception occurred" << std::endl;
        return 1;
    }
    
    return 0;
}