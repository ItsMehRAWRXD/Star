#include <iostream>
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
