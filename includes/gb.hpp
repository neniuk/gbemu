#pragma once

#include "alu.hpp"
#include "bmi.hpp"
#include "config.hpp"
#include "cpu.hpp"
#include "idu.hpp"
#include "joypad.hpp"
#include "memory.hpp"
#include "ppu.hpp"
#include "registers.hpp"
#include "screen.hpp"
#include "stack.hpp"
#include "timer.hpp"

#include <array>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

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
    Timer timer;
    Joypad joypad;

    static const std::unordered_map<uint8_t, std::string> cartridge_types;
    static const std::unordered_map<uint8_t, std::string> old_licensees;
    static const std::unordered_map<std::string, std::string> new_licensees;
};
