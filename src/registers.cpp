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

void Registers::setAF(uint16_t value) {
    this->A = static_cast<uint8_t>((value & 0xFF00) >> 8);
    this->F = static_cast<uint8_t>(value & 0x00FF);
}
uint16_t Registers::getAF() { return (static_cast<uint16_t>(this->A) << 8) | static_cast<uint16_t>(this->F); }

void Registers::setBC(uint16_t value) {
    this->B = static_cast<uint8_t>((value & 0xFF00) >> 8);
    this->C = static_cast<uint8_t>(value & 0x00FF);
}
uint16_t Registers::getBC() { return (static_cast<uint16_t>(this->B) << 8) | static_cast<uint16_t>(this->C); }

void Registers::setDE(uint16_t value) {
    this->D = static_cast<uint8_t>((value & 0xFF00) >> 8);
    this->E = static_cast<uint8_t>(value & 0x00FF);
}
uint16_t Registers::getDE() { return (static_cast<uint16_t>(this->D) << 8) | static_cast<uint16_t>(this->E); }

void Registers::setHL(uint16_t value) {
    this->H = static_cast<uint8_t>((value & 0xFF00) >> 8);
    this->L = static_cast<uint8_t>(value & 0x00FF);
}
uint16_t Registers::getHL() { return (static_cast<uint16_t>(this->H) << 8) | static_cast<uint16_t>(this->L); }
