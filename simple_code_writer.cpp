#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <functional>

class SimpleCodeWriter {
private:
    std::map<std::string, std::function<std::string(const std::string&)>> codeTemplates;
    
public:
    SimpleCodeWriter() {
        initializeTemplates();
    }
    
    void initializeTemplates() {
        // Hello World templates
        codeTemplates["hello world c++"] = [](const std::string&) {
            return R"(#include <iostream>

int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
})";
        };
        
        codeTemplates["hello world python"] = [](const std::string&) {
            return "print(\"Hello, World!\")\n";
        };
        
        codeTemplates["hello world java"] = [](const std::string&) {
            return R"(public class HelloWorld {
    public static void main(String[] args) {
        System.out.println("Hello, World!");
    }
})";
        };
        
        codeTemplates["hello world javascript"] = [](const std::string&) {
            return "console.log(\"Hello, World!\");\n";
        };
        
        codeTemplates["hello world c"] = [](const std::string&) {
            return R"(#include <stdio.h>

int main() {
    printf("Hello, World!\n");
    return 0;
})";
        };
        
        // Basic function templates
        codeTemplates["function c++"] = [](const std::string& name) {
            return "void " + name + "() {\n    // TODO: Implement function\n}\n";
        };
        
        codeTemplates["function python"] = [](const std::string& name) {
            return "def " + name + "():\n    # TODO: Implement function\n    pass\n";
        };
        
        codeTemplates["class c++"] = [](const std::string& name) {
            return "class " + name + " {\npublic:\n    " + name + "() = default;\n    ~" + name + "() = default;\n};\n";
        };
        
        codeTemplates["class python"] = [](const std::string& name) {
            return "class " + name + ":\n    def __init__(self):\n        pass\n";
        };
    }
    
    std::string processCommand(const std::string& command) {
        std::string lowerCommand = command;
        std::transform(lowerCommand.begin(), lowerCommand.end(), lowerCommand.begin(), ::tolower);
        
        // Check for hello world requests
        if (lowerCommand.find("hello world") != std::string::npos) {
            if (lowerCommand.find("python") != std::string::npos) {
                return codeTemplates["hello world python"]("");
            } else if (lowerCommand.find("java") != std::string::npos) {
                return codeTemplates["hello world java"]("");
            } else if (lowerCommand.find("javascript") != std::string::npos || 
                       lowerCommand.find("js") != std::string::npos) {
                return codeTemplates["hello world javascript"]("");
            } else if (lowerCommand.find("c++") != std::string::npos || 
                       lowerCommand.find("cpp") != std::string::npos) {
                return codeTemplates["hello world c++"]("");
            } else if (lowerCommand.find(" c ") != std::string::npos || 
                       lowerCommand.find(" c") == lowerCommand.length() - 2) {
                return codeTemplates["hello world c"]("");
            } else {
                // Default to C++
                return codeTemplates["hello world c++"]("");
            }
        }
        
        // Check for function requests
        if (lowerCommand.find("function") != std::string::npos || 
            lowerCommand.find("method") != std::string::npos) {
            std::string funcName = "myFunction";
            if (lowerCommand.find("python") != std::string::npos) {
                return codeTemplates["function python"](funcName);
            } else {
                return codeTemplates["function c++"](funcName);
            }
        }
        
        // Check for class requests
        if (lowerCommand.find("class") != std::string::npos) {
            std::string className = "MyClass";
            if (lowerCommand.find("python") != std::string::npos) {
                return codeTemplates["class python"](className);
            } else {
                return codeTemplates["class c++"](className);
            }
        }
        
        return "I can write:\n- hello world (in C, C++, Python, Java, JavaScript)\n- function/method\n- class\n\nExample: 'write hello world in python'\n";
    }
    
    void saveToFile(const std::string& code, const std::string& filename) {
        std::ofstream file(filename);
        if (file.is_open()) {
            file << code;
            file.close();
            std::cout << "Code saved to: " << filename << std::endl;
        }
    }
};

int main() {
    SimpleCodeWriter writer;
    std::string command;
    
    std::cout << "Simple Code Writer\n";
    std::cout << "Type 'exit' to quit\n";
    std::cout << "Example: 'write hello world in c++'\n\n";
    
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, command);
        
        if (command == "exit" || command == "quit") {
            break;
        }
        
        if (command.empty()) {
            continue;
        }
        
        std::string code = writer.processCommand(command);
        std::cout << "\n" << code << "\n";
        
        // Ask if user wants to save
        std::cout << "Save to file? (y/n): ";
        std::string response;
        std::getline(std::cin, response);
        
        if (response == "y" || response == "yes") {
            std::cout << "Filename: ";
            std::string filename;
            std::getline(std::cin, filename);
            if (!filename.empty()) {
                writer.saveToFile(code, filename);
            }
        }
    }
    
    return 0;
}