#include <iostream>
#include <fstream>
#include <string>

bool junkNoise(int x) {
    volatile int val = x * 0x5A5A5A5A;
    return ((val ^ (val >> 4)) & 0xF) == 0xA;
}

bool appendFile(std::ofstream& out, const std::string& filename) {
    std::ifstream in(filename, std::ios::binary);
    if (!in) return false;

    if (junkNoise(static_cast<int>(filename.size()))) {
        char junk;
        in.read(&junk, 1);
    }

    out << in.rdbuf();
    return true;
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        std::cout << "Usage: packer.exe <stub.exe> <marker.txt> <payload.bin> <output.exe>\n";
        return 1;
    }

    std::ofstream out(argv[4], std::ios::binary);
    if (!out) {
        std::cerr << "Failed to open output file.\n";
        return 1;
    }

    if (!appendFile(out, argv[1])) {
        std::cerr << "Failed to read stub file.\n";
        return 1;
    }
    if (!appendFile(out, argv[2])) {
        std::cerr << "Failed to read marker file.\n";
        return 1;
    }
    if (!appendFile(out, argv[3])) {
        std::cerr << "Failed to read payload file.\n";
        return 1;
    }

    std::cout << "Packed successfully into " << argv[4] << "\n";
    return 0;
}
