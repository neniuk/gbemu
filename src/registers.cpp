#include "registers.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>

Registers::Registers() : A(0), B(0), C(0), D(0), E(0), F(0), H(0), L(0), SP(0), PC(0), IME(false) {}

void Registers::set_af(uint16_t value) {
    this->A = static_cast<uint8_t>((value & 0xFF00) >> 8);
    this->F = static_cast<uint8_t>(value & 0x00F0);
}
uint16_t Registers::get_af() { return static_cast<uint16_t>((static_cast<uint16_t>(this->A) << 8U) | static_cast<uint16_t>(this->F)); }

void Registers::set_bc(uint16_t value) {
    this->B = static_cast<uint8_t>((value & 0xFF00) >> 8);
    this->C = static_cast<uint8_t>(value & 0x00FF);
}
uint16_t Registers::get_bc() { return static_cast<uint16_t>((static_cast<uint16_t>(this->B) << 8U) | static_cast<uint16_t>(this->C)); }

void Registers::set_de(uint16_t value) {
    this->D = static_cast<uint8_t>((value & 0xFF00) >> 8);
    this->E = static_cast<uint8_t>(value & 0x00FF);
}
uint16_t Registers::get_de() { return static_cast<uint16_t>((static_cast<uint16_t>(this->D) << 8U) | static_cast<uint16_t>(this->E)); }

void Registers::set_hl(uint16_t value) {
    this->H = static_cast<uint8_t>((value & 0xFF00) >> 8);
    this->L = static_cast<uint8_t>(value & 0x00FF);
}
uint16_t Registers::get_hl() { return static_cast<uint16_t>((static_cast<uint16_t>(this->H) << 8U) | static_cast<uint16_t>(this->L)); }

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
