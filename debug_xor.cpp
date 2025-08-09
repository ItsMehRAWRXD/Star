#include <iostream>
#include <vector>
#include <iomanip>
#include <cstdint>

void print(const std::string& label, const std::vector<uint8_t>& data) {
    std::cout << label << ": ";
    for (auto b : data) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)b << " ";
    }
    std::cout << std::endl;
}

void encrypt(std::vector<uint8_t>& data, const std::vector<uint8_t>& key) {
    std::cout << "=== ENCRYPTION ===" << std::endl;
    print("Start", data);
    
    // First pass: XOR with key and position
    for (size_t i = 0; i < data.size(); i++) {
        uint8_t keyByte = key[i % key.size()];
        uint8_t posMod = (i & 0xFF) ^ ((i >> 8) & 0xFF);
        data[i] ^= keyByte ^ posMod;
    }
    print("After pass 1", data);
    
    // Second pass: Rotate bits
    for (size_t i = 0; i < data.size(); i++) {
        uint8_t rot = (i % 8);
        data[i] = (data[i] << rot) | (data[i] >> (8 - rot));
    }
    print("After pass 2", data);
    
    // Third pass: Propagate changes
    uint32_t state = 0;
    for (auto b : key) state += b;
    std::cout << "Initial state: " << std::hex << state << std::endl;
    
    for (size_t i = 0; i < data.size(); i++) {
        std::cout << "Encrypt i=" << i << " data=" << std::hex << (int)data[i];
        state = (state * 0x9E3779B1) ^ data[i];
        std::cout << " new_state=" << state << " state&0xFF=" << (state & 0xFF);
        data[i] ^= (state & 0xFF);
        std::cout << " result=" << (int)data[i] << std::endl;
    }
    print("After pass 3", data);
}

void decrypt(std::vector<uint8_t>& data, const std::vector<uint8_t>& key) {
    std::cout << "\n=== DECRYPTION ===" << std::endl;
    print("Start", data);
    
    // Reverse third pass
    uint32_t state = 0;
    for (auto b : key) state += b;
    std::cout << "Initial state: " << std::hex << state << std::endl;
    
    std::vector<uint32_t> states;
    states.push_back(state);
    
    // Build state array - we need the states AFTER each encryption step
    for (size_t i = 0; i < data.size(); i++) {
        uint8_t original = data[i] ^ (states.back() & 0xFF);
        state = (states.back() * 0x9E3779B1) ^ original;
        states.push_back(state);
        std::cout << "i=" << i << " encrypted=" << std::hex << (int)data[i] 
                  << " prev_state&0xFF=" << (states[i] & 0xFF) 
                  << " original=" << (int)original 
                  << " new_state=" << state << std::endl;
    }
    
    // Apply reverse
    for (size_t i = 0; i < data.size(); i++) {
        std::cout << "Reversing i=" << i << " data=" << std::hex << (int)data[i] 
                  << " xor with " << (states[i] & 0xFF) << std::endl;
        data[i] ^= (states[i] & 0xFF);
    }
    print("After reverse pass 3", data);
    
    // Reverse second pass
    for (size_t i = 0; i < data.size(); i++) {
        uint8_t rot = (i % 8);
        data[i] = (data[i] >> rot) | (data[i] << (8 - rot));
    }
    print("After reverse pass 2", data);
    
    // Reverse first pass
    for (size_t i = 0; i < data.size(); i++) {
        uint8_t keyByte = key[i % key.size()];
        uint8_t posMod = (i & 0xFF) ^ ((i >> 8) & 0xFF);
        data[i] ^= keyByte ^ posMod;
    }
    print("After reverse pass 1", data);
}

int main() {
    std::vector<uint8_t> original = {0x48, 0x65, 0x6c, 0x6c, 0x6f};
    std::vector<uint8_t> key(32, 0xAA);  // 32 bytes of 0xAA
    
    std::vector<uint8_t> data = original;
    
    encrypt(data, key);
    decrypt(data, key);
    
    std::cout << "\n=== RESULT ===" << std::endl;
    print("Original", original);
    print("Final", data);
    
    if (data == original) {
        std::cout << "SUCCESS!" << std::endl;
    } else {
        std::cout << "FAILED!" << std::endl;
    }
    
    return 0;
}