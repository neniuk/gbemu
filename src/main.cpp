#include "gb.h"

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

    const char *filename = argv[1];
    std::vector<uint8_t> file_buf = read_file(filename);

    return 0;
}
