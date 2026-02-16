#ifndef GB_H
#define GB_H

#include "alu.h"
#include "bmi.h"
#include "config.h"
#include "idu.h"
#include "memory.h"
#include "registers.h"
#include "screen.h"
#include "stack.h"
#include "ppu.h"
#include "cpu.h"

#include <array>
#include <stdbool.h>
#include <stdint.h>
#include <vector>
#include <string>
#include <unordered_map>

struct CartridgeInfo {
    std::string title;

    std::vector<uint8_t> entry_point;
    std::vector<uint8_t> logo;

    std::string licensee;

    bool supports_sgb;

    int rom_size_kb;
    int num_rom_banks;
    int ram_size_kb;

    std::string cartridge_type;
    std::string destination;

    uint8_t game_version;

    uint8_t header_checksum;
    uint16_t global_checksum;
};

class GB {
  public:
    GB();

    CartridgeInfo read_cartridge_header();
    void boot(std::vector<uint8_t> &rom_buf);
    void run();

  private:
    Memory memory;
    Registers registers;
    Stack stack;
    Screen screen;
    IDU idu;
    ALU alu;
    BMI bmi;
    PPU ppu;
    CPU cpu;

    static const std::unordered_map<uint8_t, std::string> cartridge_types;
    static const std::unordered_map<uint8_t, std::string> old_licensees;
    static const std::unordered_map<std::string, std::string> new_licensees;
};

#endif
