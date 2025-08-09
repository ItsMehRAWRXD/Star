#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <random>
#include <sstream>
#include <filesystem>
#include <map>
#include <memory>
#include <chrono>
#include <thread>
#include <atomic>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <csignal>

namespace fs = std::filesystem;

class UnlimitedCodeGenerator {
private:
    std::mt19937 rng;
    std::atomic<bool> running;
    std::atomic<int> totalGenerated;
    std::string baseDir;
    
    // Generation patterns
    enum class GenerationType {
        CPP_CLASS,
        CPP_FUNCTION,
        PYTHON_SCRIPT,
        JAVASCRIPT_MODULE,
        BASH_SCRIPT,
        HEADER_FILE
    };
    
    struct GenerationTask {
        GenerationType type;
        std::string filename;
        int complexity;
    };
    
    std::queue<GenerationTask> taskQueue;
    std::mutex queueMutex;
    std::condition_variable cv;
    
public:
    UnlimitedCodeGenerator() : 
        rng(std::chrono::steady_clock::now().time_since_epoch().count()),
        running(false),
        totalGenerated(0),
        baseDir("unlimited_output") {
        fs::create_directories(baseDir);
    }
    
    void start() {
        running = true;
        
        // Start worker threads
        std::vector<std::thread> workers;
        int threadCount = std::thread::hardware_concurrency();
        
        for (int i = 0; i < threadCount; ++i) {
            workers.emplace_back(&UnlimitedCodeGenerator::workerThread, this);
        }
        
        // Start task generator thread
        std::thread taskGen(&UnlimitedCodeGenerator::generateTasks, this);
        
        std::cout << "Unlimited Code Generator Started\n";
        std::cout << "Using " << threadCount << " worker threads\n";
        std::cout << "Output directory: " << baseDir << "\n";
        std::cout << "Press Ctrl+C to stop\n\n";
        
        // Wait for all threads
        taskGen.join();
        for (auto& w : workers) {
            w.join();
        }
    }
    
    void stop() {
        running = false;
        cv.notify_all();
    }
    
private:
    void generateTasks() {
        std::uniform_int_distribution<> typeDist(0, 5);
        std::uniform_int_distribution<> complexityDist(1, 10);
        
        while (running) {
            GenerationTask task;
            task.type = static_cast<GenerationType>(typeDist(rng));
            task.complexity = complexityDist(rng);
            task.filename = generateFilename(task.type);
            
            {
                std::lock_guard<std::mutex> lock(queueMutex);
                taskQueue.push(task);
            }
            cv.notify_one();
            
            // Control generation rate
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    
    void workerThread() {
        while (running) {
            GenerationTask task;
            
            {
                std::unique_lock<std::mutex> lock(queueMutex);
                cv.wait(lock, [this] { return !taskQueue.empty() || !running; });
                
                if (!running) break;
                
                task = taskQueue.front();
                taskQueue.pop();
            }
            
            processTask(task);
        }
    }
    
    void processTask(const GenerationTask& task) {
        std::string content;
        
        switch (task.type) {
            case GenerationType::CPP_CLASS:
                content = generateCppClass(task.complexity);
                break;
            case GenerationType::CPP_FUNCTION:
                content = generateCppFunctions(task.complexity);
                break;
            case GenerationType::PYTHON_SCRIPT:
                content = generatePythonScript(task.complexity);
                break;
            case GenerationType::JAVASCRIPT_MODULE:
                content = generateJavaScriptModule(task.complexity);
                break;
            case GenerationType::BASH_SCRIPT:
                content = generateBashScript(task.complexity);
                break;
            case GenerationType::HEADER_FILE:
                content = generateHeaderFile(task.complexity);
                break;
        }
        
        saveFile(task.filename, content);
        
        int count = ++totalGenerated;
        if (count % 100 == 0) {
            std::cout << "Generated " << count << " files..." << std::endl;
        }
    }
    
    std::string generateFilename(GenerationType type) {
        std::string prefix = "gen_" + std::to_string(totalGenerated) + "_";
        
        switch (type) {
            case GenerationType::CPP_CLASS:
            case GenerationType::CPP_FUNCTION:
                return prefix + "code.cpp";
            case GenerationType::PYTHON_SCRIPT:
                return prefix + "script.py";
            case GenerationType::JAVASCRIPT_MODULE:
                return prefix + "module.js";
            case GenerationType::BASH_SCRIPT:
                return prefix + "script.sh";
            case GenerationType::MAKEFILE:
                return "Makefile_" + std::to_string(totalGenerated);
            case GenerationType::CMAKE_FILE:
                return "CMakeLists_" + std::to_string(totalGenerated) + ".txt";
            case GenerationType::HEADER_FILE:
                return prefix + "header.h";
        }
        return prefix + "file.txt";
    }
    
    std::string generateCppClass(int complexity) {
        std::stringstream ss;
        ss << "#include <iostream>\n";
        ss << "#include <vector>\n";
        ss << "#include <memory>\n\n";
        
        for (int i = 0; i < complexity; ++i) {
            ss << "class AutoGenClass" << rng() % 10000 << " {\n";
            ss << "private:\n";
            
            // Generate members
            int memberCount = 2 + (rng() % 5);
            for (int j = 0; j < memberCount; ++j) {
                ss << "    int member" << j << ";\n";
            }
            
            ss << "public:\n";
            ss << "    AutoGenClass" << rng() % 10000 << "() = default;\n";
            
            // Generate methods
            int methodCount = 3 + (rng() % 5);
            for (int j = 0; j < methodCount; ++j) {
                ss << "    void method" << j << "() {\n";
                ss << "        // Auto-generated method\n";
                ss << "        member0 = " << rng() % 100 << ";\n";
                ss << "    }\n";
            }
            
            ss << "};\n\n";
        }
        
        return ss.str();
    }
    
    std::string generateCppFunctions(int complexity) {
        std::stringstream ss;
        ss << "#include <iostream>\n";
        ss << "#include <algorithm>\n\n";
        
        for (int i = 0; i < complexity * 2; ++i) {
            ss << "int autoFunction" << rng() % 10000 << "(";
            
            int paramCount = rng() % 4;
            for (int j = 0; j < paramCount; ++j) {
                if (j > 0) ss << ", ";
                ss << "int param" << j;
            }
            
            ss << ") {\n";
            ss << "    int result = 0;\n";
            
            // Generate function body
            int statementCount = 3 + (rng() % 7);
            for (int j = 0; j < statementCount; ++j) {
                int op = rng() % 4;
                switch (op) {
                    case 0:
                        ss << "    result += " << rng() % 100 << ";\n";
                        break;
                    case 1:
                        ss << "    for (int i = 0; i < " << rng() % 50 << "; ++i) {\n";
                        ss << "        result++;\n";
                        ss << "    }\n";
                        break;
                    case 2:
                        ss << "    if (result > " << rng() % 100 << ") {\n";
                        ss << "        result /= 2;\n";
                        ss << "    }\n";
                        break;
                    case 3:
                        ss << "    // Processing step " << j << "\n";
                        break;
                }
            }
            
            ss << "    return result;\n";
            ss << "}\n\n";
        }
        
        return ss.str();
    }
    
    std::string generatePythonScript(int complexity) {
        std::stringstream ss;
        ss << "#!/usr/bin/env python3\n";
        ss << "# Auto-generated Python script\n\n";
        ss << "import random\n";
        ss << "import sys\n\n";
        
        // Generate classes
        for (int i = 0; i < complexity; ++i) {
            ss << "class AutoGen" << rng() % 10000 << ":\n";
            ss << "    def __init__(self):\n";
            ss << "        self.data = []\n";
            ss << "        self.count = 0\n\n";
            
            // Generate methods
            int methodCount = 2 + (rng() % 4);
            for (int j = 0; j < methodCount; ++j) {
                ss << "    def method" << j << "(self, param):\n";
                ss << "        self.data.append(param)\n";
                ss << "        self.count += 1\n";
                ss << "        return self.count\n\n";
            }
        }
        
        // Generate functions
        for (int i = 0; i < complexity * 2; ++i) {
            ss << "def auto_function" << rng() % 10000 << "(";
            
            int paramCount = rng() % 3;
            for (int j = 0; j < paramCount; ++j) {
                if (j > 0) ss << ", ";
                ss << "param" << j;
            }
            
            ss << "):\n";
            ss << "    result = 0\n";
            
            int statementCount = 3 + (rng() % 5);
            for (int j = 0; j < statementCount; ++j) {
                ss << "    result += random.randint(1, 100)\n";
            }
            
            ss << "    return result\n\n";
        }
        
        ss << "if __name__ == '__main__':\n";
        ss << "    print('Auto-generated script executed')\n";
        
        return ss.str();
    }
    
    std::string generateJavaScriptModule(int complexity) {
        std::stringstream ss;
        ss << "// Auto-generated JavaScript module\n\n";
        
        // Generate classes
        for (int i = 0; i < complexity; ++i) {
            ss << "class AutoGen" << rng() % 10000 << " {\n";
            ss << "    constructor() {\n";
            ss << "        this.data = [];\n";
            ss << "        this.initialized = true;\n";
            ss << "    }\n\n";
            
            // Generate methods
            int methodCount = 3 + (rng() % 4);
            for (int j = 0; j < methodCount; ++j) {
                ss << "    method" << j << "(param) {\n";
                ss << "        this.data.push(param);\n";
                ss << "        return this.data.length;\n";
                ss << "    }\n\n";
            }
            
            ss << "}\n\n";
        }
        
        // Generate functions
        for (int i = 0; i < complexity * 2; ++i) {
            ss << "function autoFunction" << rng() % 10000 << "(";
            
            int paramCount = rng() % 3;
            for (int j = 0; j < paramCount; ++j) {
                if (j > 0) ss << ", ";
                ss << "param" << j;
            }
            
            ss << ") {\n";
            ss << "    let result = 0;\n";
            
            int statementCount = 3 + (rng() % 5);
            for (int j = 0; j < statementCount; ++j) {
                ss << "    result += Math.floor(Math.random() * 100);\n";
            }
            
            ss << "    return result;\n";
            ss << "}\n\n";
        }
        
        ss << "module.exports = {\n";
        ss << "    AutoGen" << rng() % 10000 << ",\n";
        ss << "    autoFunction" << rng() % 10000 << "\n";
        ss << "};\n";
        
        return ss.str();
    }
    
    std::string generateBashScript(int complexity) {
        std::stringstream ss;
        ss << "#!/bin/bash\n";
        ss << "# Auto-generated bash script\n\n";
        
        // Generate functions
        for (int i = 0; i < complexity; ++i) {
            ss << "function auto_func" << rng() % 10000 << "() {\n";
            ss << "    local result=0\n";
            
            int statementCount = 2 + (rng() % 4);
            for (int j = 0; j < statementCount; ++j) {
                int op = rng() % 4;
                switch (op) {
                    case 0:
                        ss << "    result=$((result + " << rng() % 100 << "))\n";
                        break;
                    case 1:
                        ss << "    echo \"Processing step " << j << "\"\n";
                        break;
                    case 2:
                        ss << "    for i in {1.." << rng() % 10 + 1 << "}; do\n";
                        ss << "        result=$((result + 1))\n";
                        ss << "    done\n";
                        break;
                    case 3:
                        ss << "    [ $result -gt " << rng() % 50 << " ] && result=0\n";
                        break;
                }
            }
            
            ss << "    echo $result\n";
            ss << "}\n\n";
        }
        
        ss << "# Main execution\n";
        ss << "echo \"Auto-generated script started\"\n";
        for (int i = 0; i < complexity; ++i) {
            ss << "auto_func" << rng() % 10000 << "\n";
        }
        ss << "echo \"Script completed\"\n";
        
        return ss.str();
    }
    
    std::string generateMakefile(int complexity) {
        std::stringstream ss;
        ss << "# Auto-generated Makefile\n\n";
        
        ss << "CC = g++\n";
        ss << "CFLAGS = -Wall -O2 -std=c++17\n";
        ss << "TARGET = auto_program_" << rng() % 10000 << "\n\n";
        
        // Generate source files
        ss << "SOURCES = \\\n";
        for (int i = 0; i < complexity; ++i) {
            ss << "    source" << i << ".cpp \\\n";
        }
        ss << "    main.cpp\n\n";
        
        ss << "OBJECTS = $(SOURCES:.cpp=.o)\n\n";
        
        ss << "all: $(TARGET)\n\n";
        
        ss << "$(TARGET): $(OBJECTS)\n";
        ss << "\t$(CC) $(CFLAGS) -o $@ $^\n\n";
        
        ss << "%.o: %.cpp\n";
        ss << "\t$(CC) $(CFLAGS) -c $<\n\n";
        
        ss << "clean:\n";
        ss << "\trm -f $(OBJECTS) $(TARGET)\n\n";
        
        ss << ".PHONY: all clean\n";
        
        return ss.str();
    }
    
    std::string generateCMakeFile(int complexity) {
        std::stringstream ss;
        ss << "# Auto-generated CMakeLists.txt\n";
        ss << "cmake_minimum_required(VERSION 3.10)\n";
        ss << "project(AutoProject" << rng() % 10000 << ")\n\n";
        
        ss << "set(CMAKE_CXX_STANDARD 17)\n";
        ss << "set(CMAKE_CXX_STANDARD_REQUIRED True)\n\n";
        
        // Add executable
        ss << "add_executable(${PROJECT_NAME}\n";
        for (int i = 0; i < complexity; ++i) {
            ss << "    src/module" << i << ".cpp\n";
        }
        ss << "    src/main.cpp\n";
        ss << ")\n\n";
        
        // Add libraries
        if (complexity > 3) {
            ss << "# Libraries\n";
            for (int i = 0; i < complexity / 2; ++i) {
                ss << "add_library(lib" << i << " STATIC\n";
                ss << "    lib/component" << i << ".cpp\n";
                ss << ")\n\n";
            }
            
            ss << "target_link_libraries(${PROJECT_NAME}\n";
            for (int i = 0; i < complexity / 2; ++i) {
                ss << "    lib" << i << "\n";
            }
            ss << ")\n";
        }
        
        return ss.str();
    }
    
    std::string generateHeaderFile(int complexity) {
        std::stringstream ss;
        std::string guard = "AUTOGEN_H_" + std::to_string(rng() % 1000000);
        
        ss << "#ifndef " << guard << "\n";
        ss << "#define " << guard << "\n\n";
        
        ss << "#include <vector>\n";
        ss << "#include <string>\n";
        ss << "#include <memory>\n\n";
        
        ss << "namespace autogen {\n\n";
        
        // Generate class declarations
        for (int i = 0; i < complexity; ++i) {
            ss << "class Component" << rng() % 10000 << " {\n";
            ss << "public:\n";
            ss << "    Component" << rng() % 10000 << "();\n";
            ss << "    ~Component" << rng() % 10000 << "();\n\n";
            
            int methodCount = 2 + (rng() % 4);
            for (int j = 0; j < methodCount; ++j) {
                ss << "    int method" << j << "(int param);\n";
            }
            
            ss << "\nprivate:\n";
            ss << "    struct Impl;\n";
            ss << "    std::unique_ptr<Impl> pImpl;\n";
            ss << "};\n\n";
        }
        
        // Generate function declarations
        for (int i = 0; i < complexity * 2; ++i) {
            ss << "int computeValue" << rng() % 10000 << "(";
            
            int paramCount = rng() % 3;
            for (int j = 0; j < paramCount; ++j) {
                if (j > 0) ss << ", ";
                ss << "int param" << j;
            }
            
            ss << ");\n";
        }
        
        ss << "\n} // namespace autogen\n\n";
        ss << "#endif // " << guard << "\n";
        
        return ss.str();
    }
    
    void saveFile(const std::string& filename, const std::string& content) {
        std::string filepath = baseDir + "/" + filename;
        
        // Create subdirectories based on file type
        std::string dir = baseDir;
        if (filename.find(".cpp") != std::string::npos) dir += "/cpp";
        else if (filename.find(".py") != std::string::npos) dir += "/python";
        else if (filename.find(".js") != std::string::npos) dir += "/javascript";
        else if (filename.find(".sh") != std::string::npos) dir += "/bash";
        else if (filename.find("Makefile") != std::string::npos) dir += "/make";
        else if (filename.find("CMake") != std::string::npos) dir += "/cmake";
        else if (filename.find(".h") != std::string::npos) dir += "/headers";
        
        fs::create_directories(dir);
        filepath = dir + "/" + filename;
        
        std::ofstream file(filepath);
        if (file.is_open()) {
            file << content;
            file.close();
        }
    }
};

int main() {
    UnlimitedCodeGenerator generator;
    
    // Set up signal handler for graceful shutdown
    std::signal(SIGINT, [](int) {
        std::cout << "\nShutting down...\n";
        std::exit(0);
    });
    
    generator.start();
    
    return 0;
}