
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
