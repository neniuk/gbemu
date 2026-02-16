#include "ppu.h"

PPU::PPU(Registers &registers, Memory &memory, Screen &screen) : registers(registers), memory(memory), screen(screen) {}

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

uint8_t PPU::get_ppu_mode() {
    uint8_t stat = this->get_stat();
    return stat & 0x03;
}
void PPU::set_ppu_mode(uint8_t mode) {
    uint8_t stat = this->get_stat();
    stat = (stat & 0xFC) | (mode & 0x03);
    this->set_stat(stat);
}

uint8_t PPU::get_scy() {
    return this->memory.read_byte(0xFF42);
}
void PPU::set_scy(uint8_t value) {
    this->memory.write_byte(0xFF42, value);
}

uint8_t PPU::get_scx() {
    return this->memory.read_byte(0xFF43);
}
void PPU::set_scx(uint8_t value) {
    this->memory.write_byte(0xFF43, value);
}

uint8_t PPU::get_wy() {
    return this->memory.read_byte(0xFF4A);
}
void PPU::set_wy(uint8_t value) {
    this->memory.write_byte(0xFF4A, value);
}

uint8_t PPU::get_wx() {
    return this->memory.read_byte(0xFF4B);
}
void PPU::set_wx(uint8_t value) {
    this->memory.write_byte(0xFF4B, value);
}

uint8_t PPU::get_bgp() {
    return this->memory.read_byte(0xFF47);
}
void PPU::set_bgp(uint8_t value) {
    this->memory.write_byte(0xFF47, value);
}

uint8_t PPU::get_obp0() {
    return this->memory.read_byte(0xFF48);
}
void PPU::set_obp0(uint8_t value) {
    this->memory.write_byte(0xFF48, value);
}

uint8_t PPU::get_obp1() {
    return this->memory.read_byte(0xFF49);
}
void PPU::set_obp1(uint8_t value) {
    this->memory.write_byte(0xFF49, value);
}
