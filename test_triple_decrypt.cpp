#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <cstdint>

// Convert decimal string back to bytes
std::vector<uint8_t> decimalToBytes(const std::string& decimal, size_t expectedLength) {
    std::vector<uint8_t> bytes(expectedLength, 0);
    std::string num = decimal;
    
    for (int i = expectedLength - 1; i >= 0 && num != "0"; i--) {
        int remainder = 0;
        std::string quotient;
        
        for (char digit : num) {
            int current = remainder * 10 + (digit - '0');
            quotient += std::to_string(current / 256);
            remainder = current % 256;
        }
        
        bytes[i] = remainder;
        
        size_t firstNonZero = quotient.find_first_not_of('0');
        if (firstNonZero != std::string::npos) {
            num = quotient.substr(firstNonZero);
        } else {
            num = "0";
        }
    }
    
    return bytes;
}

// XOR implementation
void xorData(uint8_t* data, size_t len, const uint8_t* key, size_t keyLen) {
    for (size_t i = 0; i < len; i++) {
        data[i] ^= key[i % keyLen];
    }
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cout << "Usage: " << argv[0] << " <encrypted_file> <keys_file> <output_file>" << std::endl;
        return 1;
    }
    
    // Read encrypted file
    std::ifstream encFile(argv[1], std::ios::binary);
    if (!encFile) {
        std::cerr << "Failed to open encrypted file" << std::endl;
        return 1;
    }
    
    encFile.seekg(0, std::ios::end);
    size_t size = encFile.tellg();
    encFile.seekg(0, std::ios::beg);
    
    std::vector<uint8_t> data(size);
    encFile.read(reinterpret_cast<char*>(data.data()), size);
    encFile.close();
    
    // Read keys file
    std::ifstream keyFile(argv[2]);
    if (!keyFile) {
        std::cerr << "Failed to open keys file" << std::endl;
        return 1;
    }
    
    std::string line;
    std::string k1Dec, k2Dec, k3Dec;
    size_t xorKeyLen = 0;
    
    while (std::getline(keyFile, line)) {
        if (line.substr(0, 4) == "K1: ") {
            k1Dec = line.substr(4);
        } else if (line.substr(0, 4) == "K2: ") {
            k2Dec = line.substr(4);
        } else if (line.substr(0, 4) == "K3: ") {
            k3Dec = line.substr(4);
        } else if (line.substr(0, 4) == "XL: ") {
            xorKeyLen = std::stoul(line.substr(4));
        }
    }
    keyFile.close();
    
    // Convert keys from decimal
    auto k1 = decimalToBytes(k1Dec, 16);
    auto k2 = decimalToBytes(k2Dec, 32);
    auto k3 = decimalToBytes(k3Dec, xorKeyLen);
    
    std::cout << "Decrypting file of size: " << size << " bytes" << std::endl;
    
    // Reverse triple decryption (AES -> ChaCha20 -> XOR)
    // Layer 3: AES (simplified)
    for (size_t i = 0; i < data.size(); i++) {
        data[i] ^= k1[i % 16];
    }
    
    // Layer 2: ChaCha20 (simplified)
    for (size_t i = 0; i < data.size(); i++) {
        data[i] ^= k2[i % 32];
    }
    
    // Layer 1: XOR
    xorData(data.data(), data.size(), k3.data(), xorKeyLen);
    
    // Write decrypted file
    std::ofstream outFile(argv[3], std::ios::binary);
    if (!outFile) {
        std::cerr << "Failed to create output file" << std::endl;
        return 1;
    }
    
    outFile.write(reinterpret_cast<char*>(data.data()), data.size());
    outFile.close();
    
    std::cout << "File decrypted successfully!" << std::endl;
    std::cout << "Output written to: " << argv[3] << std::endl;
    
    return 0;
}