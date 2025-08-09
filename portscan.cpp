#include <iostream>
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
