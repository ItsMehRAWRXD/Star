#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdint>
#include <zlib.h>

// Key handling (shared with other components)
constexpr uint8_t XOR_OBFUSCATE_KEY = 0x5A;
uint8_t encKey[] = { 0x39,0x39,0x08,0x0F,0x0F,0x38,0x08,0x31,0x38,0x32,0x38 };
constexpr size_t keyLen = sizeof(encKey);

inline void decryptKey(uint8_t* keyBuf) {
    const char* envKey = std::getenv("ENCRYPTION_KEY");
    if (envKey && strlen(envKey) >= keyLen) {
        for (size_t i = 0; i < keyLen; ++i) {
            keyBuf[i] = static_cast<uint8_t>(envKey[i]);
        }
    } else {
        for (size_t i = 0; i < keyLen; ++i) {
            keyBuf[i] = encKey[i] ^ XOR_OBFUSCATE_KEY;
        }
    }
}

// XOR encryption/decryption
std::vector<uint8_t> xorCrypt(const std::vector<uint8_t>& data, const std::vector<uint8_t>& key) {
    std::vector<uint8_t> result(data.size());
    size_t keyLen = key.size();
    
    for (size_t i = 0; i < data.size(); ++i) {
        result[i] = data[i] ^ key[i % keyLen];
    }
    
    return result;
}

// Compression using zlib
std::vector<uint8_t> compressData(const std::vector<uint8_t>& data) {
    uLong compressedSize = compressBound(data.size());
    std::vector<uint8_t> compressed(compressedSize);
    
    if (compress2(compressed.data(), &compressedSize, 
                  data.data(), data.size(), Z_BEST_COMPRESSION) != Z_OK) {
        throw std::runtime_error("Compression failed");
    }
    
    compressed.resize(compressedSize);
    return compressed;
}

// Decompression using zlib
std::vector<uint8_t> decompressData(const std::vector<uint8_t>& compressed, size_t originalSize) {
    std::vector<uint8_t> decompressed(originalSize);
    uLong decompressedSize = originalSize;
    
    if (uncompress(decompressed.data(), &decompressedSize,
                   compressed.data(), compressed.size()) != Z_OK) {
        throw std::runtime_error("Decompression failed");
    }
    
    return decompressed;
}

// File header structure
struct PackHeader {
    uint32_t magic;           // Magic number to identify packed file
    uint32_t originalSize;    // Original uncompressed size
    uint32_t compressedSize;  // Compressed size
    uint8_t encryptionType;   // 0=none, 1=XOR, 2=AES
    uint8_t reserved[3];      // Reserved for future use
};

constexpr uint32_t PACK_MAGIC = 0x4B434150; // "PACK"

class AdvancedPacker {
private:
    std::vector<uint8_t> key;
    
public:
    AdvancedPacker() {
        key.resize(keyLen);
        decryptKey(key.data());
    }
    
    // Pack: compress + encrypt
    bool packFile(const std::string& inputFile, const std::string& outputFile, 
                  uint8_t encryptionType = 1) {
        try {
            // Read input file
            std::ifstream fin(inputFile, std::ios::binary);
            if (!fin) {
                std::cerr << "Failed to open input file: " << inputFile << std::endl;
                return false;
            }
            
            std::vector<uint8_t> data((std::istreambuf_iterator<char>(fin)),
                                     std::istreambuf_iterator<char>());
            fin.close();
            
            std::cout << "Original size: " << data.size() << " bytes" << std::endl;
            
            // Compress data
            std::vector<uint8_t> compressed = compressData(data);
            std::cout << "Compressed size: " << compressed.size() << " bytes" << std::endl;
            std::cout << "Compression ratio: " << (100.0 * compressed.size() / data.size()) << "%" << std::endl;
            
            // Encrypt if requested
            std::vector<uint8_t> processed = compressed;
            if (encryptionType == 1) {
                processed = xorCrypt(compressed, key);
                std::cout << "Applied XOR encryption" << std::endl;
            }
            
            // Create header
            PackHeader header;
            header.magic = PACK_MAGIC;
            header.originalSize = static_cast<uint32_t>(data.size());
            header.compressedSize = static_cast<uint32_t>(compressed.size());
            header.encryptionType = encryptionType;
            memset(header.reserved, 0, sizeof(header.reserved));
            
            // Write output file
            std::ofstream fout(outputFile, std::ios::binary);
            if (!fout) {
                std::cerr << "Failed to open output file: " << outputFile << std::endl;
                return false;
            }
            
            fout.write(reinterpret_cast<const char*>(&header), sizeof(header));
            fout.write(reinterpret_cast<const char*>(processed.data()), processed.size());
            fout.close();
            
            std::cout << "Successfully packed to: " << outputFile << std::endl;
            return true;
            
        } catch (const std::exception& e) {
            std::cerr << "Packing failed: " << e.what() << std::endl;
            return false;
        }
    }
    
    // Unpack: decrypt + decompress
    bool unpackFile(const std::string& inputFile, const std::string& outputFile) {
        try {
            // Read input file
            std::ifstream fin(inputFile, std::ios::binary);
            if (!fin) {
                std::cerr << "Failed to open input file: " << inputFile << std::endl;
                return false;
            }
            
            // Read header
            PackHeader header;
            fin.read(reinterpret_cast<char*>(&header), sizeof(header));
            
            if (header.magic != PACK_MAGIC) {
                std::cerr << "Invalid packed file format" << std::endl;
                return false;
            }
            
            std::cout << "Packed file info:" << std::endl;
            std::cout << "  Original size: " << header.originalSize << " bytes" << std::endl;
            std::cout << "  Compressed size: " << header.compressedSize << " bytes" << std::endl;
            std::cout << "  Encryption type: " << (int)header.encryptionType << std::endl;
            
            // Read encrypted/compressed data
            std::vector<uint8_t> processed(header.compressedSize);
            fin.read(reinterpret_cast<char*>(processed.data()), header.compressedSize);
            fin.close();
            
            // Decrypt if needed
            std::vector<uint8_t> compressed = processed;
            if (header.encryptionType == 1) {
                compressed = xorCrypt(processed, key);
                std::cout << "Applied XOR decryption" << std::endl;
            }
            
            // Decompress
            std::vector<uint8_t> data = decompressData(compressed, header.originalSize);
            std::cout << "Decompressed size: " << data.size() << " bytes" << std::endl;
            
            // Write output file
            std::ofstream fout(outputFile, std::ios::binary);
            if (!fout) {
                std::cerr << "Failed to open output file: " << outputFile << std::endl;
                return false;
            }
            
            fout.write(reinterpret_cast<const char*>(data.data()), data.size());
            fout.close();
            
            std::cout << "Successfully unpacked to: " << outputFile << std::endl;
            return true;
            
        } catch (const std::exception& e) {
            std::cerr << "Unpacking failed: " << e.what() << std::endl;
            return false;
        }
    }
    
    // List packed file info without unpacking
    bool listFile(const std::string& inputFile) {
        try {
            std::ifstream fin(inputFile, std::ios::binary);
            if (!fin) {
                std::cerr << "Failed to open input file: " << inputFile << std::endl;
                return false;
            }
            
            PackHeader header;
            fin.read(reinterpret_cast<char*>(&header), sizeof(header));
            
            if (header.magic != PACK_MAGIC) {
                std::cerr << "Invalid packed file format" << std::endl;
                return false;
            }
            
            std::cout << "Packed file: " << inputFile << std::endl;
            std::cout << "  Magic: 0x" << std::hex << header.magic << std::dec << std::endl;
            std::cout << "  Original size: " << header.originalSize << " bytes" << std::endl;
            std::cout << "  Compressed size: " << header.compressedSize << " bytes" << std::endl;
            std::cout << "  Compression ratio: " << (100.0 * header.compressedSize / header.originalSize) << "%" << std::endl;
            std::cout << "  Encryption type: " << (int)header.encryptionType;
            
            switch (header.encryptionType) {
                case 0: std::cout << " (None)"; break;
                case 1: std::cout << " (XOR)"; break;
                case 2: std::cout << " (AES)"; break;
                default: std::cout << " (Unknown)"; break;
            }
            std::cout << std::endl;
            
            return true;
            
        } catch (const std::exception& e) {
            std::cerr << "Listing failed: " << e.what() << std::endl;
            return false;
        }
    }
};

void printUsage(const char* programName) {
    std::cout << "Advanced Packer - Compress and encrypt files with reversible encryption\n";
    std::cout << "Usage:\n";
    std::cout << "  " << programName << " pack <input> <output> [encryption_type]\n";
    std::cout << "  " << programName << " unpack <input> <output>\n";
    std::cout << "  " << programName << " list <input>\n";
    std::cout << "\nEncryption types:\n";
    std::cout << "  0 - No encryption (compression only)\n";
    std::cout << "  1 - XOR encryption (default)\n";
    std::cout << "  2 - AES encryption (not implemented yet)\n";
    std::cout << "\nExamples:\n";
    std::cout << "  " << programName << " pack document.txt document.pak 1\n";
    std::cout << "  " << programName << " unpack document.pak document_restored.txt\n";
    std::cout << "  " << programName << " list document.pak\n";
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printUsage(argv[0]);
        return 1;
    }
    
    std::string command = argv[1];
    AdvancedPacker packer;
    
    if (command == "pack") {
        if (argc < 4) {
            std::cerr << "Error: pack command requires input and output files\n";
            return 1;
        }
        
        std::string inputFile = argv[2];
        std::string outputFile = argv[3];
        uint8_t encryptionType = (argc > 4) ? static_cast<uint8_t>(std::stoi(argv[4])) : 1;
        
        if (!packer.packFile(inputFile, outputFile, encryptionType)) {
            return 1;
        }
        
    } else if (command == "unpack") {
        if (argc < 4) {
            std::cerr << "Error: unpack command requires input and output files\n";
            return 1;
        }
        
        std::string inputFile = argv[2];
        std::string outputFile = argv[3];
        
        if (!packer.unpackFile(inputFile, outputFile)) {
            return 1;
        }
        
    } else if (command == "list") {
        if (argc < 3) {
            std::cerr << "Error: list command requires input file\n";
            return 1;
        }
        
        std::string inputFile = argv[2];
        
        if (!packer.listFile(inputFile)) {
            return 1;
        }
        
    } else {
        std::cerr << "Unknown command: " << command << std::endl;
        printUsage(argv[0]);
        return 1;
    }
    
    return 0;
}