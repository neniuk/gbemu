#include "idu.hpp"
#include "memory.hpp"
#include "registers.hpp"

#include <cstdint>

IDU::IDU(Registers &registers, Memory &memory) : registers_(registers), memory_(memory) {}

void IDU::increment_r8(uint8_t &register_8) {
    uint8_t result = static_cast<uint8_t>(register_8 + 1);

    // Flags Z 0 H -
    this->registers_.set_flag_z(result == 0);
    this->registers_.set_flag_n(false);
    this->registers_.set_flag_h((register_8 & 0x0F) == 0x0F);

    register_8 = result;
}
void IDU::decrement_r8(uint8_t &register_8) {
    uint8_t result = static_cast<uint8_t>(register_8 - 1);

    // Flags Z 1 H -
    this->registers_.set_flag_z(result == 0);
    this->registers_.set_flag_n(true);
    this->registers_.set_flag_h((register_8 & 0x0F) == 0x00);

    register_8 = result;
}

void IDU::increment_mem8(uint16_t address) {
    uint8_t value = this->memory_.read_byte(address);
    uint8_t result = static_cast<uint8_t>(value + 1);

    // Flags Z 0 H -
    this->registers_.set_flag_z(result == 0);
    this->registers_.set_flag_n(false);
    this->registers_.set_flag_h((value & 0x0F) == 0x0F);

    this->memory_.write_byte(address, result);
}
void IDU::decrement_mem8(uint16_t address) {
    uint8_t value = this->memory_.read_byte(address);
    uint8_t result = static_cast<uint8_t>(value - 1);

    // Flags Z 1 H -
    this->registers_.set_flag_z(result == 0);
    this->registers_.set_flag_n(true);
    this->registers_.set_flag_h((value & 0x0F) == 0x00);

    this->memory_.write_byte(address, result);
}
