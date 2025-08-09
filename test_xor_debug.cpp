#include <iostream>
#include <vector>
#include <cstdint>
#include <iomanip>

class SimpleXOR {
public:
    static void encrypt(std::vector<uint8_t>& data, const std::vector<uint8_t>& key) {
        // Simple XOR without avalanche for testing
        for (size_t i = 0; i < data.size(); i++) {
            data[i] ^= key[i % key.size()];
        }
    }
    
    static void decrypt(std::vector<uint8_t>& data, const std::vector<uint8_t>& key) {
        // XOR is symmetric
        encrypt(data, key);
    }
};

void printData(const std::string& label, const std::vector<uint8_t>& data) {
    std::cout << label << ": ";
    for (auto b : data) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)b << " ";
    }
    std::cout << std::endl;
}

int main() {
    std::cout << "Testing XOR encryption/decryption" << std::endl;
    
    // Test data
    std::vector<uint8_t> original = {0x48, 0x65, 0x6c, 0x6c, 0x6f};
    std::vector<uint8_t> key = {0xAA, 0xBB, 0xCC, 0xDD};
    
    // Test simple XOR first
    std::vector<uint8_t> data = original;
    printData("Original", data);
    
    SimpleXOR::encrypt(data, key);
    printData("Encrypted", data);
    
    SimpleXOR::decrypt(data, key);
    printData("Decrypted", data);
    
    if (data == original) {
        std::cout << "Simple XOR: SUCCESS" << std::endl;
    } else {
        std::cout << "Simple XOR: FAILED" << std::endl;
    }
    
    return 0;
}