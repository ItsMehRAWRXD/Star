#include <iostream>
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
