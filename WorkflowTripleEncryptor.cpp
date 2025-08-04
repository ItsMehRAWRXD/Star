#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <thread>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#include <wincrypt.h>
#endif

class WorkflowTripleEncryptor {
private:
    std::mt19937_64 rng;
    
    struct TripleKey {
        std::vector<uint8_t> chacha_key;
        std::vector<uint8_t> chacha_nonce;
        std::vector<uint8_t> aes_key;
        std::vector<uint8_t> xor_key;
        uint32_t encryption_order;
    };

    struct WindowsEntropy {
        static std::vector<uint64_t> gather() {
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
#endif
            
            auto now = std::chrono::high_resolution_clock::now();
            entropy.push_back(now.time_since_epoch().count());
            entropy.push_back(std::hash<std::thread::id>{}(std::this_thread::get_id()));
            entropy.push_back(reinterpret_cast<uint64_t>(&entropy));
            
            return entropy;
        }
    };

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

    void xorCrypt(std::vector<uint8_t>& data, const std::vector<uint8_t>& key) {
        for (size_t i = 0; i < data.size(); i++) {
            uint8_t keyByte = key[i % key.size()];
            uint8_t posByte = (i * 0x9E3779B9) & 0xFF;
            data[i] ^= keyByte ^ posByte;
        }
    }

    TripleKey generateKeys() {
        TripleKey keys;
        auto entropy = WindowsEntropy::gather();
        
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

    void applyEncryptionLayer(std::vector<uint8_t>& data, int method, const TripleKey& keys) {
        switch (method) {
            case 0: // ChaCha20
                chacha20Crypt(data, keys.chacha_key.data(), keys.chacha_nonce.data());
                break;
            case 1: // AES Stream
                aesStreamCrypt(data, keys.aes_key);
                break;
            case 2: // XOR
                xorCrypt(data, keys.xor_key);
                break;
        }
    }

    std::string bytesToBigDecimal(const std::vector<uint8_t>& bytes) {
        std::vector<uint8_t> result = {0};
        
        for (uint8_t byte : bytes) {
            // Multiply result by 256
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
            
            // Add byte value
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

    std::string generateJunkCode() {
        std::vector<std::string> junkOps = {
            "int " + generateUniqueVarName() + " = " + std::to_string(rng() % 1000) + ";",
            "volatile int " + generateUniqueVarName() + " = GetTickCount() & 0xFF;",
            "char " + generateUniqueVarName() + "[] = \"" + std::to_string(rng()) + "\";",
            "for(int i=0; i<" + std::to_string(rng() % 10 + 1) + "; i++) { volatile int x = i * 2; }",
            "if(GetTickCount() % 2) { volatile int " + generateUniqueVarName() + " = 42; }"
        };
        
        std::string junk;
        int numOps = rng() % 3 + 1;
        for (int i = 0; i < numOps; i++) {
            junk += "    " + junkOps[rng() % junkOps.size()] + "\n";
        }
        return junk;
    }

public:
    WorkflowTripleEncryptor() : rng(std::chrono::high_resolution_clock::now().time_since_epoch().count()) {}

    bool generateStub(const std::string& stubPath) {
        std::cout << "\n=== STEP 1: Generating Encryption Stub ===" << std::endl;
        
        TripleKey keys = generateKeys();
        
        std::string varPayload = generateUniqueVarName();
        std::string varKeys = generateUniqueVarName();
        std::string funcDecrypt = generateUniqueVarName() + "Decrypt";
        std::string funcAntiDebug = generateUniqueVarName() + "Check";
        
        std::string stub = R"(
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <windows.h>
#include <cstring>

)" + generateJunkCode() + R"(

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

)" + generateJunkCode() + R"(

bool )" + funcAntiDebug + R"(() {
    if (IsDebuggerPresent()) return false;
    
    BOOL isDebugger = FALSE;
    CheckRemoteDebuggerPresent(GetCurrentProcess(), &isDebugger);
    if (isDebugger) return false;
    
    DWORD start = GetTickCount();
    Sleep(100);
    DWORD end = GetTickCount();
    if ((end - start) < 90 || (end - start) > 110) return false;
    
    return true;
}

)" + generateJunkCode() + R"(

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

)" + generateJunkCode() + R"(

void aesStreamCrypt(std::vector<uint8_t>& data, const std::vector<uint8_t>& key) {
    for (size_t i = 0; i < data.size(); i++) {
        uint8_t keyByte = key[i % key.size()];
        uint8_t nonceByte = (i >> 8) ^ (i & 0xFF);
        uint8_t mixedKey = sbox[keyByte] ^ nonceByte;
        data[i] ^= mixedKey;
    }
}

void xorCrypt(std::vector<uint8_t>& data, const std::vector<uint8_t>& key) {
    for (size_t i = 0; i < data.size(); i++) {
        uint8_t keyByte = key[i % key.size()];
        uint8_t posByte = (i * 0x9E3779B9) & 0xFF;
        data[i] ^= keyByte ^ posByte;
    }
}

)" + generateJunkCode() + R"(

std::vector<uint8_t> bigDecimalToBytes(const std::string& decimal) {
    std::vector<uint8_t> result;
    std::vector<uint8_t> temp = {0};
    
    for (char c : decimal) {
        if (c < '0' || c > '9') continue;
        
        int digit = c - '0';
        int carry = digit;
        
        for (int i = temp.size() - 1; i >= 0; i--) {
            int prod = temp[i] * 10 + carry;
            temp[i] = prod & 0xFF;
            carry = prod >> 8;
        }
        
        while (carry > 0) {
            temp.insert(temp.begin(), carry & 0xFF);
            carry >>= 8;
        }
    }
    
    return temp;
}

)" + generateJunkCode() + R"(

bool )" + funcDecrypt + R"((const std::string& inputFile, const std::string& outputFile) {
    if (!)" + funcAntiDebug + R"(()) {
        std::cout << "Security check failed!" << std::endl;
        return false;
    }

)" + generateJunkCode() + R"(

    // Embedded keys (as decimal strings for obfuscation)
    std::string chachaKeyDec = ")" + bytesToBigDecimal(keys.chacha_key) + R"(";
    std::string chachaNonceDec = ")" + bytesToBigDecimal(keys.chacha_nonce) + R"(";
    std::string aesKeyDec = ")" + bytesToBigDecimal(keys.aes_key) + R"(";
    std::string xorKeyDec = ")" + bytesToBigDecimal(keys.xor_key) + R"(";
    uint32_t encOrder = )" + std::to_string(keys.encryption_order) + R"(;

)" + generateJunkCode() + R"(

    // Convert decimal keys back to bytes
    std::vector<uint8_t> chachaKey = bigDecimalToBytes(chachaKeyDec);
    std::vector<uint8_t> chachaNonce = bigDecimalToBytes(chachaNonceDec);
    std::vector<uint8_t> aesKey = bigDecimalToBytes(aesKeyDec);
    std::vector<uint8_t> xorKey = bigDecimalToBytes(xorKeyDec);

    // Ensure correct key sizes
    chachaKey.resize(32);
    chachaNonce.resize(12);
    aesKey.resize(32);
    xorKey.resize(64);

)" + generateJunkCode() + R"(

    // Read input file
    std::ifstream inFile(inputFile, std::ios::binary);
    if (!inFile) {
        std::cout << "Error: Cannot open input file: " << inputFile << std::endl;
        return false;
    }

    std::vector<uint8_t> data((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
    inFile.close();

    if (data.empty()) {
        std::cout << "Error: Input file is empty!" << std::endl;
        return false;
    }

)" + generateJunkCode() + R"(

    // Apply triple encryption based on order
    std::vector<int> order;
    switch (encOrder) {
        case 0: order = {0, 1, 2}; break; // ChaCha20, AES, XOR
        case 1: order = {0, 2, 1}; break; // ChaCha20, XOR, AES
        case 2: order = {1, 0, 2}; break; // AES, ChaCha20, XOR
        case 3: order = {1, 2, 0}; break; // AES, XOR, ChaCha20
        case 4: order = {2, 0, 1}; break; // XOR, ChaCha20, AES
        case 5: order = {2, 1, 0}; break; // XOR, AES, ChaCha20
        default: order = {0, 1, 2}; break;
    }

)" + generateJunkCode() + R"(

    for (int method : order) {
        switch (method) {
            case 0: // ChaCha20
                chacha20Crypt(data, chachaKey.data(), chachaNonce.data());
                break;
            case 1: // AES Stream
                aesStreamCrypt(data, aesKey);
                break;
            case 2: // XOR
                xorCrypt(data, xorKey);
                break;
        }
    }

)" + generateJunkCode() + R"(

    // Write encrypted file
    std::ofstream outFile(outputFile, std::ios::binary);
    if (!outFile) {
        std::cout << "Error: Cannot create output file: " << outputFile << std::endl;
        return false;
    }

    outFile.write(reinterpret_cast<const char*>(data.data()), data.size());
    outFile.close();

    std::cout << "File encrypted successfully: " << outputFile << std::endl;
    std::cout << "Original size: " << data.size() << " bytes" << std::endl;
    std::cout << "Encryption order: " << encOrder << std::endl;

    return true;
}

)" + generateJunkCode() + R"(

int main(int argc, char* argv[]) {
    std::cout << "=== Triple Encryption Stub ===" << std::endl;
    std::cout << "Generated at: " << __DATE__ << " " << __TIME__ << std::endl;
    
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " <input_file> <output_file>" << std::endl;
        std::cout << "  input_file  - File to encrypt" << std::endl;
        std::cout << "  output_file - Encrypted output file" << std::endl;
        return 1;
    }

)" + generateJunkCode() + R"(

    std::string inputFile = argv[1];
    std::string outputFile = argv[2];

    if ()" + funcDecrypt + R"((inputFile, outputFile)) {
        std::cout << "\n=== ENCRYPTION COMPLETE ===" << std::endl;
        std::cout << "Encrypted file ready for deployment: " << outputFile << std::endl;
        return 0;
    } else {
        std::cout << "\n=== ENCRYPTION FAILED ===" << std::endl;
        return 1;
    }
}
)";

        std::ofstream stubFile(stubPath);
        if (!stubFile) {
            std::cout << "Error: Cannot create stub file: " << stubPath << std::endl;
            return false;
        }

        stubFile << stub;
        stubFile.close();

        std::cout << "✓ Encryption stub generated: " << stubPath << std::endl;
        std::cout << "✓ Unique variable names: " << varPayload << ", " << varKeys << std::endl;
        std::cout << "✓ Unique function names: " << funcDecrypt << ", " << funcAntiDebug << std::endl;
        std::cout << "✓ Encryption order: " << keys.encryption_order << std::endl;
        std::cout << "✓ Anti-debugging enabled" << std::endl;
        std::cout << "✓ Key obfuscation applied" << std::endl;

        return true;
    }

    bool compileStub(const std::string& stubPath, const std::string& exePath) {
        std::cout << "\n=== STEP 2: Compiling Stub to Executable ===" << std::endl;
        
        std::string command = "g++ -std=c++17 -O2 -s -static -o \"" + exePath + "\" \"" + stubPath + "\"";
        
        std::cout << "Compiling: " << command << std::endl;
        
        int result = system(command.c_str());
        
        if (result == 0 && std::filesystem::exists(exePath)) {
            std::cout << "✓ Stub compiled successfully: " << exePath << std::endl;
            return true;
        } else {
            std::cout << "✗ Compilation failed. Trying alternative compiler..." << std::endl;
            
            // Try with cl.exe if available
            command = "cl /std:c++17 /O2 /MT /Fe:\"" + exePath + "\" \"" + stubPath + "\" /link /SUBSYSTEM:CONSOLE";
            result = system(command.c_str());
            
            if (result == 0 && std::filesystem::exists(exePath)) {
                std::cout << "✓ Stub compiled with MSVC: " << exePath << std::endl;
                return true;
            } else {
                std::cout << "✗ Compilation failed with both compilers." << std::endl;
                std::cout << "Please ensure g++ or Visual Studio is installed." << std::endl;
                return false;
            }
        }
    }

    bool runStubWorkflow(const std::string& targetFile, const std::string& outputFile) {
        std::string stubPath = "encryption_stub_" + std::to_string(rng() % 10000) + ".cpp";
        std::string exePath = "encryption_stub_" + std::to_string(rng() % 10000) + ".exe";
        
        try {
            // Step 1: Generate stub
            if (!generateStub(stubPath)) {
                return false;
            }

            // Step 2: Compile stub
            if (!compileStub(stubPath, exePath)) {
                return false;
            }

            // Step 3: Use stub to encrypt file
            std::cout << "\n=== STEP 3: Using Stub to Encrypt File ===" << std::endl;
            std::cout << "Target file: " << targetFile << std::endl;
            std::cout << "Output file: " << outputFile << std::endl;
            
            std::string encryptCommand = "\"" + exePath + "\" \"" + targetFile + "\" \"" + outputFile + "\"";
            std::cout << "Running: " << encryptCommand << std::endl;
            
            int result = system(encryptCommand.c_str());
            
            if (result == 0 && std::filesystem::exists(outputFile)) {
                std::cout << "\n=== WORKFLOW COMPLETE ===" << std::endl;
                std::cout << "✓ Stub generated: " << stubPath << std::endl;
                std::cout << "✓ Stub compiled: " << exePath << std::endl;
                std::cout << "✓ File encrypted: " << outputFile << std::endl;
                std::cout << "✓ Ready for deployment!" << std::endl;
                
                // Clean up temporary files
                std::cout << "\nCleaning up temporary files..." << std::endl;
                std::filesystem::remove(stubPath);
                std::filesystem::remove(exePath);
                std::cout << "✓ Cleanup complete" << std::endl;
                
                return true;
            } else {
                std::cout << "✗ Encryption failed!" << std::endl;
                return false;
            }
            
        } catch (const std::exception& e) {
            std::cout << "Error during workflow: " << e.what() << std::endl;
            return false;
        }
    }
};

int main(int argc, char* argv[]) {
    std::cout << "=== Workflow Triple Encryptor ===" << std::endl;
    std::cout << "Automated workflow: Generate Stub -> Compile -> Encrypt -> Deploy" << std::endl;
    std::cout << std::endl;

    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " <input_file> <output_file>" << std::endl;
        std::cout << std::endl;
        std::cout << "Workflow:" << std::endl;
        std::cout << "  1. Generate unique encryption stub (.cpp)" << std::endl;
        std::cout << "  2. Compile stub to executable (.exe)" << std::endl;
        std::cout << "  3. Use stub to encrypt your file" << std::endl;
        std::cout << "  4. Create encrypted file ready for deployment" << std::endl;
        std::cout << "  5. Clean up temporary files" << std::endl;
        std::cout << std::endl;
        std::cout << "Example:" << std::endl;
        std::cout << "  " << argv[0] << " document.pdf encrypted_document.bin" << std::endl;
        std::cout << "  " << argv[0] << " payload.exe encrypted_payload.dat" << std::endl;
        return 1;
    }

    std::string inputFile = argv[1];
    std::string outputFile = argv[2];

    // Check if input file exists
    if (!std::filesystem::exists(inputFile)) {
        std::cout << "Error: Input file does not exist: " << inputFile << std::endl;
        return 1;
    }

    WorkflowTripleEncryptor encryptor;
    
    std::cout << "Starting automated workflow..." << std::endl;
    std::cout << "Input file: " << inputFile << std::endl;
    std::cout << "Output file: " << outputFile << std::endl;
    std::cout << std::endl;

    if (encryptor.runStubWorkflow(inputFile, outputFile)) {
        std::cout << "\n🎉 SUCCESS: Encrypted file ready for deployment!" << std::endl;
        return 0;
    } else {
        std::cout << "\n❌ FAILED: Encryption workflow failed!" << std::endl;
        return 1;
    }
}