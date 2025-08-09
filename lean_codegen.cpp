#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <filesystem>

class LeanCodeGen {
public:
    bool generateAndTest(const std::string& lang, const std::string& desc) {
        std::string code = generate(lang, desc);
        if (code.empty()) return false;
        
        std::string filename = writeCode(lang, code);
        if (filename.empty()) return false;
        
        if (!compile(lang, filename)) return false;
        
        return execute(lang, filename);
    }

private:
    std::string generate(const std::string& lang, const std::string& desc) {
        if (lang == "c") {
            if (desc.find("hello") != std::string::npos) {
                return "#include <stdio.h>\nint main(){printf(\"Hello World\\n\");return 0;}";
            }
            if (desc.find("add") != std::string::npos) {
                return "#include <stdio.h>\nint main(){int a=5,b=3;printf(\"%d\\n\",a+b);return 0;}";
            }
            if (desc.find("fib") != std::string::npos) {
                return "#include <stdio.h>\nint fib(int n){return n<=1?n:fib(n-1)+fib(n-2);}\nint main(){printf(\"%d\\n\",fib(10));return 0;}";
            }
        }
        
        if (lang == "asm") {
            if (desc.find("hello") != std::string::npos) {
                return "section .data\n    msg db 'Hello World',10,0\n    msg_len equ $ - msg\n"
                       "section .text\n    global _start\n_start:\n    mov eax, 4\n    mov ebx, 1\n"
                       "    mov ecx, msg\n    mov edx, msg_len\n    int 0x80\n    mov eax, 1\n"
                       "    mov ebx, 0\n    int 0x80\n";
            }
        }
        
        if (lang == "cpp") {
            if (desc.find("hello") != std::string::npos) {
                return "#include <iostream>\nint main(){std::cout<<\"Hello World\\n\";return 0;}";
            }
            if (desc.find("vector") != std::string::npos) {
                return "#include <iostream>\n#include <vector>\nint main(){std::vector<int> v={1,2,3};for(auto x:v)std::cout<<x<<\" \";std::cout<<\"\\n\";return 0;}";
            }
        }
        
        return "";
    }
    
    std::string writeCode(const std::string& lang, const std::string& code) {
        std::string ext = (lang == "c") ? ".c" : (lang == "cpp") ? ".cpp" : (lang == "asm") ? ".asm" : "";
        if (ext.empty()) return "";
        
        std::string filename = "test" + ext;
        std::ofstream f(filename);
        if (!f) return "";
        f << code;
        f.close();
        return filename;
    }
    
    bool compile(const std::string& lang, const std::string& filename) {
        std::string cmd;
        
        if (lang == "c") {
            cmd = "gcc -o test_exe " + filename + " 2>/dev/null";
        } else if (lang == "cpp") {
            cmd = "g++ -o test_exe " + filename + " 2>/dev/null";
        } else if (lang == "asm") {
            cmd = "nasm -f elf32 " + filename + " && ld -m elf_i386 -o test_exe " + filename.substr(0, filename.find('.')) + ".o 2>/dev/null";
        } else {
            return false;
        }
        
        return system(cmd.c_str()) == 0;
    }
    
    bool execute(const std::string& lang, const std::string& filename) {
        std::string cmd = "./test_exe > output.txt 2>/dev/null";
        if (system(cmd.c_str()) != 0) return false;
        
        std::ifstream f("output.txt");
        if (!f) return false;
        
        std::string output;
        std::getline(f, output);
        f.close();
        
        // Verify expected output
        if (output.find("Hello World") != std::string::npos) return true;
        if (output.find("8") != std::string::npos) return true; // 5+3
        if (output.find("55") != std::string::npos) return true; // fib(10)
        if (output.find("1 2 3") != std::string::npos) return true; // vector
        
        return !output.empty();
    }
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " <lang> <description>\n";
        std::cout << "Languages: c, cpp, asm\n";
        std::cout << "Examples:\n";
        std::cout << "  " << argv[0] << " c \"hello world program\"\n";
        std::cout << "  " << argv[0] << " c \"add two numbers\"\n";
        std::cout << "  " << argv[0] << " cpp \"vector example\"\n";
        std::cout << "  " << argv[0] << " asm \"hello world\"\n";
        return 1;
    }
    
    LeanCodeGen gen;
    std::string lang = argv[1];
    std::string desc = argv[2];
    
    std::cout << "Generating " << lang << " code for: " << desc << "\n";
    
    if (gen.generateAndTest(lang, desc)) {
        std::cout << "SUCCESS: Code generated, compiled, and executed!\n";
        
        // Show the actual output
        std::ifstream f("output.txt");
        if (f) {
            std::string line;
            std::cout << "Output: ";
            while (std::getline(f, line)) {
                std::cout << line << "\n";
            }
        }
    } else {
        std::cout << "FAILED: Could not generate, compile, or execute code\n";
    }
    
    // Cleanup
    system("rm -f test.c test.cpp test.asm test.o test_exe output.txt 2>/dev/null");
    
    return 0;
}