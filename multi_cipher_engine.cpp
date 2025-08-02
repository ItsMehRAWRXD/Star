#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <map>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/blowfish.h>
#include <openssl/rc4.h>

// Multi-cipher encryption engine supporting multiple algorithms
class MultiCipherEngine {
private:
    std::mt19937 rng;
    std::map<std::string, std::vector<std::string>> cipherMethods;
    
public:
    enum CipherType {
        AES_256_CBC,
        AES_256_GCM,
        CHACHA20_POLY1305,
        BLOWFISH_CBC,
        RC4_STREAM,
        XOR_CUSTOM,
        XOR_ROTATING,
        XOR_MULTI_KEY,
        CUSTOM_CIPHER
    };
    
    MultiCipherEngine() : rng(std::random_device{}()) {
        initializeCipherMethods();
    }
    
    void initializeCipherMethods() {
        cipherMethods["aes_variants"] = {
            "AES-256-CBC",
            "AES-256-GCM", 
            "AES-128-CBC",
            "AES-128-GCM"
        };
        
        cipherMethods["stream_ciphers"] = {
            "ChaCha20-Poly1305",
            "ChaCha20",
            "RC4",
            "RC4-MD5"
        };
        
        cipherMethods["block_ciphers"] = {
            "Blowfish-CBC",
            "Blowfish-ECB",
            "DES-CBC",
            "3DES-CBC"
        };
        
        cipherMethods["custom_ciphers"] = {
            "XOR-Custom",
            "XOR-Rotating",
            "XOR-MultiKey",
            "Custom-Cipher"
        };
    }
    
    std::string getRandomCipherMethod() {
        std::vector<std::string> allMethods;
        for (const auto& category : cipherMethods) {
            allMethods.insert(allMethods.end(), category.second.begin(), category.second.end());
        }
        
        std::uniform_int_distribution<> dis(0, allMethods.size() - 1);
        return allMethods[dis(rng)];
    }
    
    CipherType getRandomCipherType() {
        std::uniform_int_distribution<> dis(0, 8);
        return static_cast<CipherType>(dis(rng));
    }
    
    // AES-256-CBC Encryption
    bool encryptAES256CBC(const std::vector<uint8_t>& input, std::vector<uint8_t>& output, 
                         const std::vector<uint8_t>& key, const std::vector<uint8_t>& iv) {
        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        if (!ctx) return false;
        
        if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key.data(), iv.data()) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            return false;
        }
        
        output.resize(input.size() + EVP_MAX_BLOCK_LENGTH);
        int outLen;
        
        if (EVP_EncryptUpdate(ctx, output.data(), &outLen, input.data(), input.size()) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            return false;
        }
        
        int finalLen;
        if (EVP_EncryptFinal_ex(ctx, output.data() + outLen, &finalLen) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            return false;
        }
        
        output.resize(outLen + finalLen);
        EVP_CIPHER_CTX_free(ctx);
        return true;
    }
    
    // AES-256-GCM Encryption
    bool encryptAES256GCM(const std::vector<uint8_t>& input, std::vector<uint8_t>& output,
                         const std::vector<uint8_t>& key, std::vector<uint8_t>& tag) {
        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        if (!ctx) return false;
        
        std::vector<uint8_t> iv(12);
        RAND_bytes(iv.data(), iv.size());
        
        if (EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, key.data(), iv.data()) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            return false;
        }
        
        output.resize(input.size() + EVP_MAX_BLOCK_LENGTH);
        int outLen;
        
        if (EVP_EncryptUpdate(ctx, output.data(), &outLen, input.data(), input.size()) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            return false;
        }
        
        int finalLen;
        if (EVP_EncryptFinal_ex(ctx, output.data() + outLen, &finalLen) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            return false;
        }
        
        output.resize(outLen + finalLen);
        
        tag.resize(16);
        if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, tag.data()) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            return false;
        }
        
        EVP_CIPHER_CTX_free(ctx);
        return true;
    }
    
    // ChaCha20-Poly1305 Encryption
    bool encryptChaCha20Poly1305(const std::vector<uint8_t>& input, std::vector<uint8_t>& output,
                                const std::vector<uint8_t>& key, std::vector<uint8_t>& tag) {
        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        if (!ctx) return false;
        
        std::vector<uint8_t> nonce(12);
        RAND_bytes(nonce.data(), nonce.size());
        
        if (EVP_EncryptInit_ex(ctx, EVP_chacha20_poly1305(), NULL, key.data(), nonce.data()) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            return false;
        }
        
        output.resize(input.size() + EVP_MAX_BLOCK_LENGTH);
        int outLen;
        
        if (EVP_EncryptUpdate(ctx, output.data(), &outLen, input.data(), input.size()) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            return false;
        }
        
        int finalLen;
        if (EVP_EncryptFinal_ex(ctx, output.data() + outLen, &finalLen) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            return false;
        }
        
        output.resize(outLen + finalLen);
        
        tag.resize(16);
        if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_GET_TAG, 16, tag.data()) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            return false;
        }
        
        EVP_CIPHER_CTX_free(ctx);
        return true;
    }
    
    // Blowfish Encryption
    bool encryptBlowfish(const std::vector<uint8_t>& input, std::vector<uint8_t>& output,
                        const std::vector<uint8_t>& key, const std::vector<uint8_t>& iv) {
        BF_KEY bfKey;
        BF_set_key(&bfKey, key.size(), key.data());
        
        output.resize(input.size() + 8); // Blowfish block size
        int outLen = 0;
        
        // Pad input to 8-byte blocks
        std::vector<uint8_t> paddedInput = input;
        int padding = 8 - (input.size() % 8);
        if (padding < 8) {
            paddedInput.resize(input.size() + padding, padding);
        }
        
        std::vector<uint8_t> currentIV = iv;
        
        for (size_t i = 0; i < paddedInput.size(); i += 8) {
            std::vector<uint8_t> block(8);
            std::vector<uint8_t> encryptedBlock(8);
            
            // XOR with IV
            for (int j = 0; j < 8; j++) {
                block[j] = paddedInput[i + j] ^ currentIV[j];
            }
            
            BF_ecb_encrypt(block.data(), encryptedBlock.data(), &bfKey, BF_ENCRYPT);
            
            // Copy to output
            for (int j = 0; j < 8; j++) {
                output[outLen + j] = encryptedBlock[j];
            }
            
            currentIV = encryptedBlock;
            outLen += 8;
        }
        
        output.resize(outLen);
        return true;
    }
    
    // RC4 Stream Cipher
    bool encryptRC4(const std::vector<uint8_t>& input, std::vector<uint8_t>& output,
                   const std::vector<uint8_t>& key) {
        RC4_KEY rc4Key;
        RC4_set_key(&rc4Key, key.size(), key.data());
        
        output.resize(input.size());
        RC4(&rc4Key, input.size(), input.data(), output.data());
        return true;
    }
    
    // Custom XOR with rotating key
    bool encryptXORRotating(const std::vector<uint8_t>& input, std::vector<uint8_t>& output,
                           const std::vector<uint8_t>& key) {
        output.resize(input.size());
        
        for (size_t i = 0; i < input.size(); i++) {
            uint8_t keyByte = key[i % key.size()];
            uint8_t rotation = (i % 8);
            keyByte = ((keyByte << rotation) | (keyByte >> (8 - rotation))) & 0xFF;
            output[i] = input[i] ^ keyByte;
        }
        
        return true;
    }
    
    // Multi-key XOR
    bool encryptXORMultiKey(const std::vector<uint8_t>& input, std::vector<uint8_t>& output,
                           const std::vector<std::vector<uint8_t>>& keys) {
        output.resize(input.size());
        
        for (size_t i = 0; i < input.size(); i++) {
            uint8_t result = input[i];
            for (const auto& key : keys) {
                result ^= key[i % key.size()];
            }
            output[i] = result;
        }
        
        return true;
    }
    
    // Custom cipher with multiple operations
    bool encryptCustomCipher(const std::vector<uint8_t>& input, std::vector<uint8_t>& output,
                            const std::vector<uint8_t>& key) {
        output.resize(input.size());
        
        for (size_t i = 0; i < input.size(); i++) {
            uint8_t keyByte = key[i % key.size()];
            uint8_t data = input[i];
            
            // Multiple transformations
            data = ((data << 3) | (data >> 5)) & 0xFF;  // Rotate left 3
            data ^= keyByte;                             // XOR with key
            data = ((data << 1) | (data >> 7)) & 0xFF;  // Rotate left 1
            data += keyByte;                             // Add key
            data ^= (i & 0xFF);                          // XOR with position
            
            output[i] = data;
        }
        
        return true;
    }
    
    // Key derivation function
    std::vector<uint8_t> deriveKey(const std::vector<uint8_t>& baseKey, const std::string& salt, size_t keySize) {
        std::vector<uint8_t> derivedKey(keySize);
        
        // Simple key derivation (in production, use PBKDF2 or Argon2)
        for (size_t i = 0; i < keySize; i++) {
            derivedKey[i] = baseKey[i % baseKey.size()] ^ salt[i % salt.length()] ^ (i * 0x37);
        }
        
        return derivedKey;
    }
    
    // Generate random IV/nonce
    std::vector<uint8_t> generateIV(size_t size) {
        std::vector<uint8_t> iv(size);
        RAND_bytes(iv.data(), size);
        return iv;
    }
    
    // Encrypt with specified cipher type
    bool encrypt(const std::vector<uint8_t>& input, std::vector<uint8_t>& output,
                CipherType cipherType, const std::vector<uint8_t>& key) {
        
        switch (cipherType) {
            case AES_256_CBC: {
                std::vector<uint8_t> iv = generateIV(16);
                std::vector<uint8_t> derivedKey = deriveKey(key, "AES_CBC", 32);
                bool result = encryptAES256CBC(input, output, derivedKey, iv);
                if (result) {
                    // Prepend IV to output
                    output.insert(output.begin(), iv.begin(), iv.end());
                }
                return result;
            }
            
            case AES_256_GCM: {
                std::vector<uint8_t> derivedKey = deriveKey(key, "AES_GCM", 32);
                std::vector<uint8_t> tag;
                bool result = encryptAES256GCM(input, output, derivedKey, tag);
                if (result) {
                    // Append tag to output
                    output.insert(output.end(), tag.begin(), tag.end());
                }
                return result;
            }
            
            case CHACHA20_POLY1305: {
                std::vector<uint8_t> derivedKey = deriveKey(key, "CHACHA20", 32);
                std::vector<uint8_t> tag;
                bool result = encryptChaCha20Poly1305(input, output, derivedKey, tag);
                if (result) {
                    // Append tag to output
                    output.insert(output.end(), tag.begin(), tag.end());
                }
                return result;
            }
            
            case BLOWFISH_CBC: {
                std::vector<uint8_t> iv = generateIV(8);
                std::vector<uint8_t> derivedKey = deriveKey(key, "BLOWFISH", 16);
                bool result = encryptBlowfish(input, output, derivedKey, iv);
                if (result) {
                    // Prepend IV to output
                    output.insert(output.begin(), iv.begin(), iv.end());
                }
                return result;
            }
            
            case RC4_STREAM: {
                std::vector<uint8_t> derivedKey = deriveKey(key, "RC4", 16);
                return encryptRC4(input, output, derivedKey);
            }
            
            case XOR_CUSTOM: {
                std::vector<uint8_t> derivedKey = deriveKey(key, "XOR_CUSTOM", 16);
                return encryptXORRotating(input, output, derivedKey);
            }
            
            case XOR_ROTATING: {
                std::vector<uint8_t> derivedKey = deriveKey(key, "XOR_ROT", 16);
                return encryptXORRotating(input, output, derivedKey);
            }
            
            case XOR_MULTI_KEY: {
                std::vector<std::vector<uint8_t>> keys;
                keys.push_back(deriveKey(key, "KEY1", 16));
                keys.push_back(deriveKey(key, "KEY2", 16));
                keys.push_back(deriveKey(key, "KEY3", 16));
                return encryptXORMultiKey(input, output, keys);
            }
            
            case CUSTOM_CIPHER: {
                std::vector<uint8_t> derivedKey = deriveKey(key, "CUSTOM", 16);
                return encryptCustomCipher(input, output, derivedKey);
            }
            
            default:
                return false;
        }
    }
    
    // Decrypt with specified cipher type
    bool decrypt(const std::vector<uint8_t>& input, std::vector<uint8_t>& output,
                CipherType cipherType, const std::vector<uint8_t>& key) {
        
        switch (cipherType) {
            case AES_256_CBC: {
                if (input.size() < 16) return false;
                std::vector<uint8_t> iv(input.begin(), input.begin() + 16);
                std::vector<uint8_t> encryptedData(input.begin() + 16, input.end());
                std::vector<uint8_t> derivedKey = deriveKey(key, "AES_CBC", 32);
                
                EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
                if (!ctx) return false;
                
                if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, derivedKey.data(), iv.data()) != 1) {
                    EVP_CIPHER_CTX_free(ctx);
                    return false;
                }
                
                output.resize(encryptedData.size());
                int outLen;
                
                if (EVP_DecryptUpdate(ctx, output.data(), &outLen, encryptedData.data(), encryptedData.size()) != 1) {
                    EVP_CIPHER_CTX_free(ctx);
                    return false;
                }
                
                int finalLen;
                if (EVP_DecryptFinal_ex(ctx, output.data() + outLen, &finalLen) != 1) {
                    EVP_CIPHER_CTX_free(ctx);
                    return false;
                }
                
                output.resize(outLen + finalLen);
                EVP_CIPHER_CTX_free(ctx);
                return true;
            }
            
            case RC4_STREAM: {
                std::vector<uint8_t> derivedKey = deriveKey(key, "RC4", 16);
                return encryptRC4(input, output, derivedKey); // RC4 is symmetric
            }
            
            case XOR_CUSTOM:
            case XOR_ROTATING: {
                std::vector<uint8_t> derivedKey = deriveKey(key, "XOR_ROT", 16);
                return encryptXORRotating(input, output, derivedKey); // XOR is symmetric
            }
            
            case XOR_MULTI_KEY: {
                std::vector<std::vector<uint8_t>> keys;
                keys.push_back(deriveKey(key, "KEY1", 16));
                keys.push_back(deriveKey(key, "KEY2", 16));
                keys.push_back(deriveKey(key, "KEY3", 16));
                return encryptXORMultiKey(input, output, keys); // Multi-key XOR is symmetric
            }
            
            case CUSTOM_CIPHER: {
                std::vector<uint8_t> derivedKey = deriveKey(key, "CUSTOM", 16);
                return encryptCustomCipher(input, output, derivedKey); // Custom cipher is symmetric
            }
            
            default:
                return false;
        }
    }
    
    std::string getCipherName(CipherType type) {
        switch (type) {
            case AES_256_CBC: return "AES-256-CBC";
            case AES_256_GCM: return "AES-256-GCM";
            case CHACHA20_POLY1305: return "ChaCha20-Poly1305";
            case BLOWFISH_CBC: return "Blowfish-CBC";
            case RC4_STREAM: return "RC4-Stream";
            case XOR_CUSTOM: return "XOR-Custom";
            case XOR_ROTATING: return "XOR-Rotating";
            case XOR_MULTI_KEY: return "XOR-MultiKey";
            case CUSTOM_CIPHER: return "Custom-Cipher";
            default: return "Unknown";
        }
    }
};

// Enhanced encryptor with multiple cipher support
class MultiCipherEncryptor {
private:
    MultiCipherEngine cipherEngine;
    std::vector<uint8_t> baseKey;
    
public:
    MultiCipherEncryptor() {
        // Initialize with the original XOR key
        uint8_t encKey[] = { 0x39,0x39,0x08,0x0F,0x0F,0x38,0x08,0x31,0x38,0x32,0x38 };
        uint8_t mask[] = { 0x63,0x63,0x60,0x55,0x55,0x6A,0x60,0x5B,0x6A,0x68,0x6A };
        
        baseKey.resize(sizeof(encKey));
        for (size_t i = 0; i < sizeof(encKey); i++) {
            baseKey[i] = encKey[i] ^ mask[i];
        }
    }
    
    bool encryptFile(const std::string& inputFile, const std::string& outputFile, 
                    MultiCipherEngine::CipherType cipherType = MultiCipherEngine::AES_256_CBC) {
        
        std::ifstream fin(inputFile, std::ios::binary);
        if (!fin) {
            std::cerr << "Failed to open input file." << std::endl;
            return false;
        }
        
        fin.seekg(0, std::ios::end);
        size_t fileSize = fin.tellg();
        fin.seekg(0, std::ios::beg);
        
        std::vector<uint8_t> inputData(fileSize);
        fin.read(reinterpret_cast<char*>(inputData.data()), fileSize);
        
        std::vector<uint8_t> encryptedData;
        if (!cipherEngine.encrypt(inputData, encryptedData, cipherType, baseKey)) {
            std::cerr << "Encryption failed." << std::endl;
            return false;
        }
        
        std::ofstream fout(outputFile, std::ios::binary);
        if (!fout) {
            std::cerr << "Failed to create output file." << std::endl;
            return false;
        }
        
        // Write cipher type identifier
        uint8_t cipherId = static_cast<uint8_t>(cipherType);
        fout.write(reinterpret_cast<const char*>(&cipherId), 1);
        
        // Write encrypted data
        fout.write(reinterpret_cast<const char*>(encryptedData.data()), encryptedData.size());
        
        std::cout << "File encrypted successfully using " << cipherEngine.getCipherName(cipherType) << std::endl;
        return true;
    }
    
    bool decryptFile(const std::string& inputFile, const std::string& outputFile) {
        std::ifstream fin(inputFile, std::ios::binary);
        if (!fin) {
            std::cerr << "Failed to open input file." << std::endl;
            return false;
        }
        
        // Read cipher type identifier
        uint8_t cipherId;
        fin.read(reinterpret_cast<char*>(&cipherId), 1);
        
        MultiCipherEngine::CipherType cipherType = static_cast<MultiCipherEngine::CipherType>(cipherId);
        
        // Read encrypted data
        fin.seekg(0, std::ios::end);
        size_t fileSize = fin.tellg() - 1; // Subtract 1 for cipher ID
        fin.seekg(1, std::ios::beg); // Skip cipher ID
        
        std::vector<uint8_t> encryptedData(fileSize);
        fin.read(reinterpret_cast<char*>(encryptedData.data()), fileSize);
        
        std::vector<uint8_t> decryptedData;
        if (!cipherEngine.decrypt(encryptedData, decryptedData, cipherType, baseKey)) {
            std::cerr << "Decryption failed." << std::endl;
            return false;
        }
        
        std::ofstream fout(outputFile, std::ios::binary);
        if (!fout) {
            std::cerr << "Failed to create output file." << std::endl;
            return false;
        }
        
        fout.write(reinterpret_cast<const char*>(decryptedData.data()), decryptedData.size());
        
        std::cout << "File decrypted successfully from " << cipherEngine.getCipherName(cipherType) << std::endl;
        return true;
    }
    
    void listCiphers() {
        std::cout << "Available cipher methods:\n";
        std::cout << "0 - AES-256-CBC\n";
        std::cout << "1 - AES-256-GCM\n";
        std::cout << "2 - ChaCha20-Poly1305\n";
        std::cout << "3 - Blowfish-CBC\n";
        std::cout << "4 - RC4-Stream\n";
        std::cout << "5 - XOR-Custom\n";
        std::cout << "6 - XOR-Rotating\n";
        std::cout << "7 - XOR-MultiKey\n";
        std::cout << "8 - Custom-Cipher\n";
        std::cout << "9 - Random (changes each time)\n";
    }
};

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Multi-Cipher Encryption Engine\n";
        std::cout << "Usage:\n";
        std::cout << "  multi_cipher_engine.exe <inputfile> <outputfile> [cipher_id]\n";
        std::cout << "  multi_cipher_engine.exe --decrypt <inputfile> <outputfile>\n";
        std::cout << "  multi_cipher_engine.exe --list\n";
        std::cout << "\nCipher IDs:\n";
        std::cout << "  0-8: Specific cipher methods\n";
        std::cout << "  9: Random cipher (different each time)\n";
        return 1;
    }
    
    MultiCipherEncryptor encryptor;
    
    if (std::string(argv[1]) == "--list") {
        encryptor.listCiphers();
        return 0;
    }
    
    if (std::string(argv[1]) == "--decrypt") {
        if (argc < 4) {
            std::cerr << "Missing output file for decryption." << std::endl;
            return 1;
        }
        return encryptor.decryptFile(argv[2], argv[3]) ? 0 : 1;
    }
    
    std::string inputFile = argv[1];
    std::string outputFile = argv[2];
    
    MultiCipherEngine::CipherType cipherType = MultiCipherEngine::AES_256_CBC;
    
    if (argc > 3) {
        int cipherId = std::stoi(argv[3]);
        if (cipherId == 9) {
            // Random cipher
            std::uniform_int_distribution<> dis(0, 8);
            cipherType = static_cast<MultiCipherEngine::CipherType>(dis(std::mt19937(std::random_device{}())));
        } else if (cipherId >= 0 && cipherId <= 8) {
            cipherType = static_cast<MultiCipherEngine::CipherType>(cipherId);
        } else {
            std::cerr << "Invalid cipher ID. Use --list to see available ciphers." << std::endl;
            return 1;
        }
    }
    
    return encryptor.encryptFile(inputFile, outputFile, cipherType) ? 0 : 1;
}