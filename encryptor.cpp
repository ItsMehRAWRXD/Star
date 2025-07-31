#include <iostream>
#include <fstream>
#include <vector>

constexpr uint8_t XOR_OBFUSCATE_KEY = 0x5A;

uint8_t encKey[] = { 0x39,0x39,0x08,0x0F,0x0F,0x38,0x08,0x31,0x38,0x32,0x38 };
constexpr size_t keyLen = sizeof(encKey);

inline void decryptKey(uint8_t* keyBuf) {
    for (size_t i = 0; i < keyLen; ++i) {
        keyBuf[i] = encKey[i] ^ XOR_OBFUSCATE_KEY;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: encryptor.exe <inputfile> <outputfile>\n";
        return 1;
    }

    uint8_t key[keyLen];
    decryptKey(key);

    std::ifstream fin(argv[1], std::ios::binary);
    if (!fin) {
        std::cerr << "Failed to open input file.\n";
        return 1;
    }

    std::ofstream fout(argv[2], std::ios::binary);
    if (!fout) {
        std::cerr << "Failed to open output file.\n";
        return 1;
    }

    std::vector<char> buffer(4096);
    size_t pos = 0;
    while (fin.read(buffer.data(), buffer.size()) || fin.gcount() > 0) {
        std::streamsize bytesRead = fin.gcount();
        for (std::streamsize i = 0; i < bytesRead; ++i) {
            buffer[i] ^= key[(pos + i) % keyLen];
        }
        fout.write(buffer.data(), bytesRead);
        pos += bytesRead;
    }

    std::cout << "File encrypted successfully.\n";
    return 0;
}
