#include "config.h"
#include "gb.h"

#include <fstream>
#include <iostream>
#include <iterator>
#include <string.h>
#include <vector>

std::vector<uint8_t> read_file(const std::string &filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) throw std::runtime_error("Unable to open file: " + filename);

    return std::vector<uint8_t>(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
}

int main(int argc, char **argv) {
    if (argc < 2) {
        throw std::runtime_error("No ROM to load");
    }

    if constexpr (kDebugMode) {
        std::cout << "[DEBUG] main > Debug mode enabled\n";
    }

    const char *rom_filename = argv[1];
    std::vector<uint8_t> rom_buf = read_file(rom_filename);

    GB gb;
    gb.boot(rom_buf);

    return 0;
}
