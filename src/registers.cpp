#include "registers.h"

#include <assert.h>
#include <cstddef>
#include <cstring>
#include <stdint.h>

Registers::Registers(uint8_t initial_value) {
    this->A = initial_value;
    this->B = initial_value;
    this->C = initial_value;
    this->D = initial_value;
    this->E = initial_value;
    this->F = initial_value;
    this->H = initial_value;
    this->L = initial_value;
    this->SP = static_cast<uint16_t>(initial_value);
    this->PC = GB_PROGRAM_COUNTER_START;
}

void Registers::set_af(uint16_t value) {
    this->A = static_cast<uint8_t>((value & 0xFF00) >> 8);
    this->F = static_cast<uint8_t>(value & 0x00FF);
}
uint16_t Registers::get_af() { return (static_cast<uint16_t>(this->A) << 8) | static_cast<uint16_t>(this->F); }

void Registers::set_bc(uint16_t value) {
    this->B = static_cast<uint8_t>((value & 0xFF00) >> 8);
    this->C = static_cast<uint8_t>(value & 0x00FF);
}
uint16_t Registers::get_bc() { return (static_cast<uint16_t>(this->B) << 8) | static_cast<uint16_t>(this->C); }

void Registers::set_de(uint16_t value) {
    this->D = static_cast<uint8_t>((value & 0xFF00) >> 8);
    this->E = static_cast<uint8_t>(value & 0x00FF);
}
uint16_t Registers::get_de() { return (static_cast<uint16_t>(this->D) << 8) | static_cast<uint16_t>(this->E); }

void Registers::set_hl(uint16_t value) {
    this->H = static_cast<uint8_t>((value & 0xFF00) >> 8);
    this->L = static_cast<uint8_t>(value & 0x00FF);
}
uint16_t Registers::get_hl() { return (static_cast<uint16_t>(this->H) << 8) | static_cast<uint16_t>(this->L); }

void Registers::set_flag_z(bool value) {
    this->F = static_cast<uint8_t>((this->F & static_cast<uint8_t>(~FLAG_Z_MASK)) | (value ? FLAG_Z_MASK : 0x00));
}
bool Registers::get_flag_z() { return (this->F & FLAG_Z_MASK) != 0; }

void Registers::set_flag_n(bool value) {
    this->F = static_cast<uint8_t>((this->F & static_cast<uint8_t>(~FLAG_N_MASK)) | (value ? FLAG_N_MASK : 0x00));
}
bool Registers::get_flag_n() { return (this->F & FLAG_N_MASK) != 0; }

void Registers::set_flag_h(bool value) {
    this->F = static_cast<uint8_t>((this->F & static_cast<uint8_t>(~FLAG_H_MASK)) | (value ? FLAG_H_MASK : 0x00));
}
bool Registers::get_flag_h() { return (this->F & FLAG_H_MASK) != 0; }

void Registers::set_flag_c(bool value) {
    this->F = static_cast<uint8_t>((this->F & static_cast<uint8_t>(~FLAG_C_MASK)) | (value ? FLAG_C_MASK : 0x00));
}
bool Registers::get_flag_c() { return (this->F & FLAG_C_MASK) != 0; }
