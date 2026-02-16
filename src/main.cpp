#include "gb.h"
#include "config.h"

#include <fstream>
#include <iterator>
#include <vector>


std::vector<uint8_t> read_file(const std::string &filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) throw std::runtime_error("Unable to open file: " + filename);

    return std::vector<uint8_t>(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("[WARNING] main > No program to load\n");
        return -1;
    }

    bool debug_flag = false;
    if (argc > 2) {
        if (strcmp(argv[2], "-d") == 0) {
            printf("[DEBUG] main > DEBUG MODE TURNED ON\n");
            debug_flag = true;
        }
    }

    const char *rom_filename = argv[1];
    std::vector<uint8_t> rom_buf = read_file(rom_filename);

    GB gb;
    gb.boot(rom_buf);

    return 0;
}
