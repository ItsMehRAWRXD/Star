#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

void writeKeygen() {
    std::ofstream file("keygen.cpp");
    file << R"(#include <iostream>
#include <string>
#include <random>
#include <chrono>
#include <algorithm>

class KeyGen {
    std::mt19937 rng;
public:
    KeyGen() : rng(std::chrono::steady_clock::now().time_since_epoch().count()) {}
    
    std::string generate() {
        const char* chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        std::string key;
        std::uniform_int_distribution<> dist(0, 35);
        
        for(int i = 0; i < 4; ++i) {
            if(i > 0) key += "-";
            for(int j = 0; j < 4; ++j) {
                key += chars[dist(rng)];
            }
        }
        return key;
    }
};

int main() {
    KeyGen gen;
    std::cout << "Generated Key: " << gen.generate() << std::endl;
    return 0;
}
)";
    file.close();
    std::cout << "Created: keygen.cpp\n";
}

void writePasswordGen() {
    std::ofstream file("passgen.cpp");
    file << R"(#include <iostream>
#include <string>
#include <random>
#include <chrono>

int main() {
    std::string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*";
    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<> dist(0, chars.length() - 1);
    
    int length;
    std::cout << "Password length: ";
    std::cin >> length;
    
    std::string password;
    for(int i = 0; i < length; ++i) {
        password += chars[dist(rng)];
    }
    
    std::cout << "Password: " << password << std::endl;
    return 0;
}
)";
    file.close();
    std::cout << "Created: passgen.cpp\n";
}

void writePortScanner() {
    std::ofstream file("portscan.cpp");
    file << R"(#include <iostream>
#include <cstring>
#ifdef _WIN32
    #include <winsock2.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <unistd.h>
#endif

bool scanPort(const char* host, int port) {
    #ifdef _WIN32
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2,2), &wsaData);
    #endif
    
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0) return false;
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(host);
    
    bool open = (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) == 0);
    
    #ifdef _WIN32
        closesocket(sock);
        WSACleanup();
    #else
        close(sock);
    #endif
    
    return open;
}

int main() {
    std::string host;
    std::cout << "Host: ";
    std::cin >> host;
    
    std::cout << "Scanning common ports...\n";
    int ports[] = {21, 22, 23, 25, 80, 443, 3306, 3389, 8080};
    
    for(int port : ports) {
        if(scanPort(host.c_str(), port)) {
            std::cout << "Port " << port << " is OPEN\n";
        }
    }
    
    return 0;
}
)";
    file.close();
    std::cout << "Created: portscan.cpp\n";
}

void writeFileEncryptor() {
    std::ofstream file("encrypt.cpp");
    file << R"(#include <iostream>
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
)";
    file.close();
    std::cout << "Created: encrypt.cpp\n";
}

void writeHexDump() {
    std::ofstream file("hexdump.cpp");
    file << R"(#include <iostream>
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
)";
    file.close();
    std::cout << "Created: hexdump.cpp\n";
}

int main() {
    std::cout << "Code Generator - Creating real programs\n\n";
    
    std::string choice;
    std::cout << "What to generate?\n";
    std::cout << "1. keygen\n";
    std::cout << "2. password generator\n";
    std::cout << "3. port scanner\n";
    std::cout << "4. file encryptor\n";
    std::cout << "5. hex dump\n";
    std::cout << "6. all\n";
    std::cout << "> ";
    
    std::getline(std::cin, choice);
    
    if(choice == "1" || choice == "keygen") writeKeygen();
    else if(choice == "2" || choice == "password") writePasswordGen();
    else if(choice == "3" || choice == "port") writePortScanner();
    else if(choice == "4" || choice == "encrypt") writeFileEncryptor();
    else if(choice == "5" || choice == "hex") writeHexDump();
    else if(choice == "6" || choice == "all") {
        writeKeygen();
        writePasswordGen();
        writePortScanner();
        writeFileEncryptor();
        writeHexDump();
    }
    
    std::cout << "\nTo compile:\n";
    std::cout << "g++ -o keygen keygen.cpp\n";
    std::cout << "g++ -o passgen passgen.cpp\n";
    std::cout << "g++ -o portscan portscan.cpp";
    #ifdef _WIN32
        std::cout << " -lws2_32";
    #endif
    std::cout << "\n";
    std::cout << "g++ -o encrypt encrypt.cpp\n";
    std::cout << "g++ -o hexdump hexdump.cpp\n";
    
    return 0;
}