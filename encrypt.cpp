#include <iostream>
#include <fstream>
#include <vector>
#include <string>

void xorFile(const std::string& input, const std::string& output, const std::string& key) {
    std::ifstream in(input, std::ios::binary);
    std::ofstream out(output, std::ios::binary);
    
    if(!in || !out) {
        std::cerr << "File error\n";
        return;
    }
    
    char byte;
    size_t keyIndex = 0;
    
    while(in.get(byte)) {
        byte ^= key[keyIndex % key.length()];
        out.put(byte);
        keyIndex++;
    }
    
    std::cout << "File encrypted/decrypted\n";
}

int main() {
    std::string input, output, key;
    
    std::cout << "Input file: ";
    std::cin >> input;
    std::cout << "Output file: ";
    std::cin >> output;
    std::cout << "Key: ";
    std::cin >> key;
    
    xorFile(input, output, key);
    
    return 0;
}
