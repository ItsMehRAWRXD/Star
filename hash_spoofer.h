#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <random>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>

#ifdef _WIN32
    #include <windows.h>
    #include <winreg.h>
    #include <shlobj.h>
    #include <imagehlp.h>
    #include <wincrypt.h>
    #include <psapi.h>
    #pragma comment(lib, "imagehlp.lib")
    #pragma comment(lib, "crypt32.lib")
#elif defined(__APPLE__)
    #include <mach-o/loader.h>
    #include <mach-o/fat.h>
    #include <mach-o/swap.h>
    #include <sys/stat.h>
    #include <unistd.h>
#elif defined(__linux__)
    #include <elf.h>
    #include <sys/stat.h>
    #include <unistd.h>
#endif

namespace fs = std::filesystem;

class HashSpoofer {
public:
    HashSpoofer();
    ~HashSpoofer();
    
    // Core spoofing methods
    void spoofFile(const std::string& filePath);
    void spoofFileWithCustomHashes(const std::string& filePath, const std::map<std::string, std::string>& targetHashes);
    void createPolymorphicFile(const std::string& originalFile, const std::string& outputFile);
    void createMetamorphicFile(const std::string& originalFile, const std::string& outputFile);
    
    // Hash manipulation
    void manipulateCRC32(const std::string& filePath, uint32_t targetCRC32);
    void manipulateMD5(const std::string& filePath, const std::string& targetMD5);
    void manipulateSHA1(const std::string& filePath, const std::string& targetSHA1);
    void manipulateSHA256(const std::string& filePath, const std::string& targetSHA256);
    void manipulateSSDEEP(const std::string& filePath, const std::string& targetSSDEEP);
    
    // File signature manipulation
    void spoofFileSignature(const std::string& filePath, const std::string& targetSignature);
    void spoofFileExtension(const std::string& filePath, const std::string& newExtension);
    void spoofFileHeader(const std::string& filePath, const std::vector<uint8_t>& newHeader);
    void spoofFileFooter(const std::string& filePath, const std::vector<uint8_t>& newFooter);
    
    // Advanced spoofing techniques
    void createCollisionFile(const std::string& filePath, const std::string& targetFile);
    void createHashCollision(const std::string& filePath, const std::map<std::string, std::string>& targetHashes);
    void createTimeStampSpoof(const std::string& filePath, const std::string& targetTime);
    void createSizeSpoof(const std::string& filePath, size_t targetSize);
    
    // PE file specific (Windows)
    void spoofPEFile(const std::string& filePath);
    void manipulatePESignature(const std::string& filePath);
    void manipulatePEHeaders(const std::string& filePath);
    void manipulatePEImports(const std::string& filePath);
    void manipulatePEResources(const std::string& filePath);
    void manipulatePEChecksum(const std::string& filePath);
    void manipulatePETimestamp(const std::string& filePath);
    
    // ELF file specific (Linux)
    void spoofELFFile(const std::string& filePath);
    void manipulateELFSignature(const std::string& filePath);
    void manipulateELFHeaders(const std::string& filePath);
    void manipulateELFSections(const std::string& filePath);
    
    // Mach-O file specific (macOS)
    void spoofMachOFile(const std::string& filePath);
    void manipulateMachOSignature(const std::string& filePath);
    void manipulateMachOHeaders(const std::string& filePath);
    void manipulateMachOLoadCommands(const std::string& filePath);
    
    // Dynamic spoofing
    void createDynamicSpoofer(const std::string& filePath);
    void enableRuntimeSpoofing(const std::string& filePath);
    void createSelfModifyingFile(const std::string& filePath);
    void createEncryptedFile(const std::string& filePath, const std::string& key);
    
    // Utility methods
    std::string calculateCRC32(const std::string& filePath);
    std::string calculateMD5(const std::string& filePath);
    std::string calculateSHA1(const std::string& filePath);
    std::string calculateSHA256(const std::string& filePath);
    std::string calculateSSDEEP(const std::string& filePath);
    
    std::map<std::string, std::string> getAllHashes(const std::string& filePath);
    bool verifyHash(const std::string& filePath, const std::string& hashType, const std::string& expectedHash);
    
    // File analysis
    std::string getFileType(const std::string& filePath);
    std::string getFileSignature(const std::string& filePath);
    size_t getFileSize(const std::string& filePath);
    std::string getFileTimestamp(const std::string& filePath);
    
    // Getters
    std::vector<std::string> getSpoofedFiles() const;
    std::string getLastError() const;

private:
    std::vector<std::string> spoofedFiles;
    std::string lastError;
    std::random_device rd;
    std::mt19937 gen;
    
    // Internal methods
    void setError(const std::string& error);
    void clearError();
    
    // Hash calculation methods
    uint32_t calculateCRC32Internal(const std::vector<uint8_t>& data);
    std::string calculateMD5Internal(const std::vector<uint8_t>& data);
    std::string calculateSHA1Internal(const std::vector<uint8_t>& data);
    std::string calculateSHA256Internal(const std::vector<uint8_t>& data);
    std::string calculateSSDEEPInternal(const std::vector<uint8_t>& data);
    
    // File manipulation methods
    std::vector<uint8_t> readFileBytes(const std::string& filePath);
    bool writeFileBytes(const std::string& filePath, const std::vector<uint8_t>& data);
    bool backupFile(const std::string& filePath);
    bool restoreFile(const std::string& filePath);
    
    // Collision methods
    std::vector<uint8_t> findCollision(const std::vector<uint8_t>& originalData, const std::string& targetHash, const std::string& hashType);
    std::vector<uint8_t> createHashCollisionInternal(const std::vector<uint8_t>& data, const std::map<std::string, std::string>& targetHashes);
    
    // PE manipulation methods
    bool isValidPEFile(const std::vector<uint8_t>& data);
    std::vector<uint8_t> manipulatePEFileInternal(const std::vector<uint8_t>& data);
    std::vector<uint8_t> addPEPadding(const std::vector<uint8_t>& data);
    std::vector<uint8_t> modifyPESections(const std::vector<uint8_t>& data);
    std::vector<uint8_t> modifyPEImports(const std::vector<uint8_t>& data);
    std::vector<uint8_t> modifyPEResources(const std::vector<uint8_t>& data);
    
    // ELF manipulation methods
    bool isValidELFFile(const std::vector<uint8_t>& data);
    std::vector<uint8_t> manipulateELFFileInternal(const std::vector<uint8_t>& data);
    std::vector<uint8_t> addELFPadding(const std::vector<uint8_t>& data);
    std::vector<uint8_t> modifyELFSections(const std::vector<uint8_t>& data);
    
    // Mach-O manipulation methods
    bool isValidMachOFile(const std::vector<uint8_t>& data);
    std::vector<uint8_t> manipulateMachOFileInternal(const std::vector<uint8_t>& data);
    std::vector<uint8_t> addMachOPadding(const std::vector<uint8_t>& data);
    std::vector<uint8_t> modifyMachOLoadCommands(const std::vector<uint8_t>& data);
    
    // Polymorphism methods
    std::vector<uint8_t> applyPolymorphicTransform(const std::vector<uint8_t>& data);
    std::vector<uint8_t> applyMetamorphicTransform(const std::vector<uint8_t>& data);
    std::vector<uint8_t> addJunkData(const std::vector<uint8_t>& data);
    std::vector<uint8_t> reorderSections(const std::vector<uint8_t>& data);
    std::vector<uint8_t> modifyStrings(const std::vector<uint8_t>& data);
    
    // Encryption methods
    std::vector<uint8_t> encryptData(const std::vector<uint8_t>& data, const std::string& key);
    std::vector<uint8_t> decryptData(const std::vector<uint8_t>& data, const std::string& key);
    std::string generateRandomKey(int length);
    
    // Utility methods
    std::string bytesToHex(const std::vector<uint8_t>& data);
    std::vector<uint8_t> hexToBytes(const std::string& hex);
    std::string generateRandomString(int length);
    std::vector<uint8_t> generateRandomBytes(int length);
    
    // Platform-specific methods
    #ifdef _WIN32
    bool manipulateWindowsFileAttributes(const std::string& filePath);
    bool manipulateWindowsFileVersion(const std::string& filePath);
    bool manipulateWindowsFileDescription(const std::string& filePath);
    #elif defined(__APPLE__)
    bool manipulateMacOSFileAttributes(const std::string& filePath);
    bool manipulateMacOSFileBundle(const std::string& filePath);
    #elif defined(__linux__)
    bool manipulateLinuxFileAttributes(const std::string& filePath);
    bool manipulateLinuxFilePermissions(const std::string& filePath);
    #endif
    
    // Configuration
    std::map<std::string, std::string> knownSignatures;
    std::map<std::string, std::vector<uint8_t>> knownHeaders;
    std::map<std::string, std::vector<uint8_t>> knownFooters;
    
    void initializeKnownSignatures();
    void initializeKnownHeaders();
    void initializeKnownFooters();
};