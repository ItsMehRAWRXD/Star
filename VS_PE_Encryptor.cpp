// VS_PE_Encryptor.cpp - Visual Studio PE File Encryptor
// Encrypts Windows PE files while preserving structure

#include <windows.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <iomanip>
#include <sstream>

#pragma comment(lib, "advapi32.lib")

class PEEncryptor {
private:
    // PE Headers
    struct PEInfo {
        IMAGE_DOS_HEADER dosHeader;
        IMAGE_NT_HEADERS ntHeaders;
        std::vector<IMAGE_SECTION_HEADER> sectionHeaders;
        DWORD fileAlignment;
        DWORD sectionAlignment;
        bool is64Bit;
    };

    // Generate cryptographically secure random bytes
    std::vector<BYTE> generateRandomBytes(size_t length) {
        std::vector<BYTE> buffer(length);
        HCRYPTPROV hCryptProv;
        
        if (CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
            CryptGenRandom(hCryptProv, (DWORD)length, buffer.data());
            CryptReleaseContext(hCryptProv, 0);
        } else {
            // Fallback to pseudo-random
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, 255);
            for (size_t i = 0; i < length; i++) {
                buffer[i] = dis(gen);
            }
        }
        
        return buffer;
    }

    // Simple XOR encryption (you can replace with AES later)
    void xorEncrypt(BYTE* data, size_t size, const BYTE* key, size_t keySize) {
        for (size_t i = 0; i < size; i++) {
            data[i] ^= key[i % keySize];
        }
    }

    // Read PE information
    bool readPEInfo(std::ifstream& file, PEInfo& info) {
        // Read DOS header
        file.read(reinterpret_cast<char*>(&info.dosHeader), sizeof(IMAGE_DOS_HEADER));
        if (info.dosHeader.e_magic != IMAGE_DOS_SIGNATURE) {
            std::cerr << "Invalid DOS signature" << std::endl;
            return false;
        }

        // Read NT headers
        file.seekg(info.dosHeader.e_lfanew);
        DWORD ntSignature;
        file.read(reinterpret_cast<char*>(&ntSignature), sizeof(DWORD));
        if (ntSignature != IMAGE_NT_SIGNATURE) {
            std::cerr << "Invalid NT signature" << std::endl;
            return false;
        }

        // Read file header
        IMAGE_FILE_HEADER fileHeader;
        file.read(reinterpret_cast<char*>(&fileHeader), sizeof(IMAGE_FILE_HEADER));

        // Determine if 32-bit or 64-bit
        WORD magic;
        file.read(reinterpret_cast<char*>(&magic), sizeof(WORD));
        file.seekg(-sizeof(WORD), std::ios::cur);

        info.is64Bit = (magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC);

        // Read appropriate optional header
        if (info.is64Bit) {
            IMAGE_OPTIONAL_HEADER64 optHeader64;
            file.read(reinterpret_cast<char*>(&optHeader64), sizeof(IMAGE_OPTIONAL_HEADER64));
            
            // Copy to 32-bit structure for compatibility
            info.ntHeaders.Signature = ntSignature;
            info.ntHeaders.FileHeader = fileHeader;
            info.ntHeaders.OptionalHeader.Magic = optHeader64.Magic;
            info.ntHeaders.OptionalHeader.SizeOfCode = optHeader64.SizeOfCode;
            info.ntHeaders.OptionalHeader.AddressOfEntryPoint = optHeader64.AddressOfEntryPoint;
            info.ntHeaders.OptionalHeader.BaseOfCode = optHeader64.BaseOfCode;
            info.ntHeaders.OptionalHeader.SectionAlignment = optHeader64.SectionAlignment;
            info.ntHeaders.OptionalHeader.FileAlignment = optHeader64.FileAlignment;
            info.ntHeaders.OptionalHeader.SizeOfImage = optHeader64.SizeOfImage;
            info.ntHeaders.OptionalHeader.SizeOfHeaders = optHeader64.SizeOfHeaders;
            info.ntHeaders.OptionalHeader.NumberOfRvaAndSizes = optHeader64.NumberOfRvaAndSizes;
        } else {
            file.seekg(info.dosHeader.e_lfanew);
            file.read(reinterpret_cast<char*>(&info.ntHeaders), sizeof(IMAGE_NT_HEADERS));
        }

        info.fileAlignment = info.ntHeaders.OptionalHeader.FileAlignment;
        info.sectionAlignment = info.ntHeaders.OptionalHeader.SectionAlignment;

        // Read section headers
        info.sectionHeaders.resize(info.ntHeaders.FileHeader.NumberOfSections);
        for (int i = 0; i < info.ntHeaders.FileHeader.NumberOfSections; i++) {
            file.read(reinterpret_cast<char*>(&info.sectionHeaders[i]), sizeof(IMAGE_SECTION_HEADER));
        }

        return true;
    }

public:
    bool encryptPE(const std::string& inputFile, const std::string& outputFile, const std::string& stubFile = "") {
        std::ifstream inFile(inputFile, std::ios::binary);
        if (!inFile) {
            std::cerr << "Cannot open input file: " << inputFile << std::endl;
            return false;
        }

        // Get file size
        inFile.seekg(0, std::ios::end);
        size_t fileSize = inFile.tellg();
        inFile.seekg(0, std::ios::beg);

        // Read entire file
        std::vector<BYTE> fileData(fileSize);
        inFile.read(reinterpret_cast<char*>(fileData.data()), fileSize);
        inFile.close();

        // Parse PE headers
        PEInfo peInfo;
        inFile.clear();
        inFile.open(inputFile, std::ios::binary);
        if (!readPEInfo(inFile, peInfo)) {
            return false;
        }
        inFile.close();

        // Generate encryption key
        std::vector<BYTE> key = generateRandomBytes(32); // 256-bit key

        std::cout << "[*] Encrypting PE file..." << std::endl;
        std::cout << "    File size: " << fileSize << " bytes" << std::endl;
        std::cout << "    Sections: " << peInfo.ntHeaders.FileHeader.NumberOfSections << std::endl;
        std::cout << "    Architecture: " << (peInfo.is64Bit ? "64-bit" : "32-bit") << std::endl;

        // Option 1: Encrypt entire file (simple)
        if (stubFile.empty()) {
            // Just encrypt the whole file
            xorEncrypt(fileData.data(), fileData.size(), key.data(), key.size());

            // Save encrypted file
            std::ofstream outFile(outputFile, std::ios::binary);
            if (!outFile) {
                std::cerr << "Cannot create output file" << std::endl;
                return false;
            }

            // Write magic header
            const char* magic = "ENCPE";
            outFile.write(magic, 5);

            // Write key size and key
            DWORD keySize = (DWORD)key.size();
            outFile.write(reinterpret_cast<char*>(&keySize), sizeof(DWORD));
            outFile.write(reinterpret_cast<char*>(key.data()), key.size());

            // Write original size
            DWORD originalSize = (DWORD)fileSize;
            outFile.write(reinterpret_cast<char*>(&originalSize), sizeof(DWORD));

            // Write encrypted data
            outFile.write(reinterpret_cast<char*>(fileData.data()), fileData.size());
            outFile.close();

            std::cout << "[+] PE encrypted successfully!" << std::endl;
            std::cout << "    Output: " << outputFile << std::endl;
            std::cout << "    Key (hex): ";
            for (size_t i = 0; i < key.size(); i++) {
                std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)key[i];
            }
            std::cout << std::dec << std::endl;

        } else {
            // Option 2: Create self-decrypting stub
            std::cout << "[*] Creating self-decrypting PE..." << std::endl;
            
            // This would require embedding a decryption stub
            // For now, we'll just create the encrypted payload
            xorEncrypt(fileData.data(), fileData.size(), key.data(), key.size());

            // Save as data file for stub to use
            std::ofstream dataFile(outputFile + ".dat", std::ios::binary);
            dataFile.write(reinterpret_cast<char*>(fileData.data()), fileData.size());
            dataFile.close();

            // Save key file
            std::ofstream keyFile(outputFile + ".key", std::ios::binary);
            keyFile.write(reinterpret_cast<char*>(key.data()), key.size());
            keyFile.close();

            std::cout << "[+] Encrypted payload saved" << std::endl;
            std::cout << "    Data: " << outputFile << ".dat" << std::endl;
            std::cout << "    Key: " << outputFile << ".key" << std::endl;
        }

        return true;
    }

    bool decryptPE(const std::string& inputFile, const std::string& outputFile) {
        std::ifstream inFile(inputFile, std::ios::binary);
        if (!inFile) {
            std::cerr << "Cannot open encrypted file" << std::endl;
            return false;
        }

        // Check magic header
        char magic[6] = {0};
        inFile.read(magic, 5);
        if (strcmp(magic, "ENCPE") != 0) {
            std::cerr << "Not a valid encrypted PE file" << std::endl;
            return false;
        }

        // Read key
        DWORD keySize;
        inFile.read(reinterpret_cast<char*>(&keySize), sizeof(DWORD));
        std::vector<BYTE> key(keySize);
        inFile.read(reinterpret_cast<char*>(key.data()), keySize);

        // Read original size
        DWORD originalSize;
        inFile.read(reinterpret_cast<char*>(&originalSize), sizeof(DWORD));

        // Read encrypted data
        std::vector<BYTE> encryptedData(originalSize);
        inFile.read(reinterpret_cast<char*>(encryptedData.data()), originalSize);
        inFile.close();

        // Decrypt
        xorEncrypt(encryptedData.data(), encryptedData.size(), key.data(), key.size());

        // Save decrypted file
        std::ofstream outFile(outputFile, std::ios::binary);
        outFile.write(reinterpret_cast<char*>(encryptedData.data()), encryptedData.size());
        outFile.close();

        std::cout << "[+] PE decrypted successfully!" << std::endl;
        std::cout << "    Output: " << outputFile << std::endl;

        return true;
    }
};

// Create a decryption stub
void generateDecryptionStub(const std::string& outputFile) {
    std::ofstream stub(outputFile);
    stub << R"(
// Decryption stub for encrypted PE
#include <windows.h>
#include <iostream>
#include <fstream>
#include <vector>

void xorDecrypt(BYTE* data, size_t size, const BYTE* key, size_t keySize) {
    for (size_t i = 0; i < size; i++) {
        data[i] ^= key[i % keySize];
    }
}

int main() {
    // Read encrypted payload (embedded or from file)
    std::ifstream dataFile("payload.dat", std::ios::binary);
    if (!dataFile) return 1;
    
    dataFile.seekg(0, std::ios::end);
    size_t dataSize = dataFile.tellg();
    dataFile.seekg(0);
    
    std::vector<BYTE> payload(dataSize);
    dataFile.read(reinterpret_cast<char*>(payload.data()), dataSize);
    dataFile.close();
    
    // Read key
    std::ifstream keyFile("payload.key", std::ios::binary);
    if (!keyFile) return 1;
    
    keyFile.seekg(0, std::ios::end);
    size_t keySize = keyFile.tellg();
    keyFile.seekg(0);
    
    std::vector<BYTE> key(keySize);
    keyFile.read(reinterpret_cast<char*>(key.data()), keySize);
    keyFile.close();
    
    // Decrypt
    xorDecrypt(payload.data(), payload.size(), key.data(), key.size());
    
    // Execute from memory
    void* exec = VirtualAlloc(0, dataSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (exec) {
        memcpy(exec, payload.data(), dataSize);
        
        // Get entry point from PE header
        IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)exec;
        IMAGE_NT_HEADERS* ntHeaders = (IMAGE_NT_HEADERS*)((BYTE*)exec + dosHeader->e_lfanew);
        DWORD entryPoint = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        
        // Execute
        ((void(*)())((BYTE*)exec + entryPoint))();
        
        VirtualFree(exec, 0, MEM_RELEASE);
    }
    
    return 0;
}
)";
    stub.close();
    std::cout << "[+] Decryption stub generated: " << outputFile << std::endl;
}

int main(int argc, char* argv[]) {
    std::cout << "=== Visual Studio PE Encryptor ===" << std::endl;
    std::cout << "Encrypts Windows PE executables\n" << std::endl;

    if (argc < 3) {
        std::cout << "Usage:" << std::endl;
        std::cout << "  Encrypt: " << argv[0] << " -e <input.exe> <output.enc>" << std::endl;
        std::cout << "  Decrypt: " << argv[0] << " -d <input.enc> <output.exe>" << std::endl;
        std::cout << "  Generate stub: " << argv[0] << " -s <stub.cpp>" << std::endl;
        return 1;
    }

    PEEncryptor encryptor;

    if (strcmp(argv[1], "-e") == 0 && argc >= 4) {
        // Encrypt PE file
        if (encryptor.encryptPE(argv[2], argv[3])) {
            std::cout << "\n[!] Remember: This creates an encrypted file, not a self-decrypting stub" << std::endl;
            std::cout << "    To create a full crypter, compile the stub with the encrypted data" << std::endl;
        }
    } else if (strcmp(argv[1], "-d") == 0 && argc >= 4) {
        // Decrypt PE file
        encryptor.decryptPE(argv[2], argv[3]);
    } else if (strcmp(argv[1], "-s") == 0 && argc >= 3) {
        // Generate decryption stub
        generateDecryptionStub(argv[2]);
    } else {
        std::cerr << "Invalid arguments" << std::endl;
        return 1;
    }

    return 0;
}