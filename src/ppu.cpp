#include "ppu.h"

PPU::PPU(Registers &registers, Memory &memory) : registers(registers), memory(memory) {}

uint8_t PPU::get_lcdc() {
    return this->memory.read_byte(0xFF40);
}
void PPU::set_lcdc(uint8_t value) {
    this->memory.write_byte(0xFF40, value);
}

uint8_t PPU::get_ly() {
    return this->memory.read_byte(0xFF44);
}
void PPU::set_ly(uint8_t value) {
    this->memory.write_byte(0xFF44, value);
}

uint8_t PPU::get_lyc() {
    return this->memory.read_byte(0xFF45);
}
void PPU::set_lyc(uint8_t value) {
    this->memory.write_byte(0xFF45, value);
}

uint8_t PPU::get_stat() {
    return this->memory.read_byte(0xFF41);
}
void PPU::set_stat(uint8_t value) {
    this->memory.write_byte(0xFF41, value);
}
