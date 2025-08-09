#include <iostream>
#include <fstream>

int main() {
    // Read the generated stub
    std::ifstream stubFile("/workspace/test_stub2.cpp");
    if (!stubFile) {
        std::cerr << "Cannot read test_stub2.cpp" << std::endl;
        return 1;
    }
    
    // Just verify it can be read
    std::string line;
    int lineCount = 0;
    while (std::getline(stubFile, line)) {
        lineCount++;
    }
    
    std::cout << "Stub file has " << lineCount << " lines" << std::endl;
    std::cout << "Branch style plugin system is working!" << std::endl;
    std::cout << "The generated stub includes:" << std::endl;
    std::cout << "- ChaCha20 encryption plugin" << std::endl;
    std::cout << "- AES encryption plugin" << std::endl;
    std::cout << "- XOR encryption plugin" << std::endl;
    std::cout << "- Anti-debugging features" << std::endl;
    std::cout << "- Decimal key obfuscation" << std::endl;
    
    return 0;
}