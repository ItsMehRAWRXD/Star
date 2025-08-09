#include <iostream>
#include <fstream>
#include <iomanip>
#include <cctype>

int main(int argc, char* argv[]) {
    if(argc < 2) {
        std::cout << "Usage: " << argv[0] << " <file>\n";
        return 1;
    }
    
    std::ifstream file(argv[1], std::ios::binary);
    if(!file) {
        std::cerr << "Cannot open file\n";
        return 1;
    }
    
    unsigned char buffer[16];
    size_t offset = 0;
    
    while(file.read((char*)buffer, 16) || file.gcount() > 0) {
        std::cout << std::hex << std::setw(8) << std::setfill('0') << offset << "  ";
        
        size_t bytes = file.gcount();
        
        // Hex
        for(size_t i = 0; i < 16; ++i) {
            if(i < bytes) {
                std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)buffer[i] << " ";
            } else {
                std::cout << "   ";
            }
            if(i == 7) std::cout << " ";
        }
        
        std::cout << " |";
        
        // ASCII
        for(size_t i = 0; i < bytes; ++i) {
            std::cout << (std::isprint(buffer[i]) ? (char)buffer[i] : '.');
        }
        
        std::cout << "|\n";
        offset += bytes;
    }
    
    return 0;
}
