#include "alu.h"

ALU::ALU(Registers *registers, Memory *memory) : registers(registers), memory(memory) {}

void ALU::add_u8(uint8_t val) {
    uint8_t a = this->registers->A;
    uint8_t result = static_cast<uint8_t>(a + val);
    this->registers->A = result;

    // Flags Z 0 H C
    this->registers->set_flag_z(result == 0);
    this->registers->set_flag_n(false);
    this->registers->set_flag_h(((a & 0x0F) + (val & 0x0F)) > 0x0F);
    this->registers->set_flag_c(static_cast<uint16_t>(a) + static_cast<uint16_t>(val) > 0xFF);
}
void ALU::adc_u8(uint8_t val) {
    uint8_t a = this->registers->A;
    uint8_t c = this->registers->get_flag_c() ? 1 : 0;
    uint8_t result = static_cast<uint8_t>(a + val + c);
    this->registers->A = result;

    // Flags Z 0 H C
    this->registers->set_flag_z(result == 0);
    this->registers->set_flag_n(false);
    this->registers->set_flag_h(((a & 0x0F) + (val & 0x0F) + c) > 0x0F);
    this->registers->set_flag_c(static_cast<uint16_t>(a) + static_cast<uint16_t>(val) + static_cast<uint16_t>(c) > 0xFF);
}

void ALU::add_u16(uint16_t val) {
    uint16_t hl = this->registers->get_hl();
    uint16_t result = static_cast<uint16_t>(hl + val);
    this->registers->set_hl(result);

    // Flags - 0 H C
    this->registers->set_flag_n(false);
    this->registers->set_flag_h(((hl & 0x0FFF) + (val & 0x0FFF)) > 0x0FFF);
    this->registers->set_flag_c(static_cast<uint32_t>(hl) + static_cast<uint32_t>(val) > 0xFFFF);
}

void ALU::sub_u8(uint8_t val) {
    uint8_t a = this->registers->A;
    this->registers->A = static_cast<uint8_t>(a - val);

    // Flags Z 1 H C
    this->registers->set_flag_z(this->registers->A == 0);
    this->registers->set_flag_n(true);
    this->registers->set_flag_h((val & 0x0F) > (a & 0x0F));
    this->registers->set_flag_c(val > a);
}
void ALU::sbc_u8(uint8_t val) {
    uint8_t a = this->registers->A;
    uint8_t c = this->registers->get_flag_c() ? 1 : 0;
    this->registers->A = static_cast<uint8_t>(a - val - c);

    // Flags Z 1 H C
    this->registers->set_flag_z(this->registers->A == 0);
    this->registers->set_flag_n(true);
    this->registers->set_flag_h(((val & 0x0F) + c) > (a & 0x0F));
    this->registers->set_flag_c((static_cast<uint16_t>(val) + static_cast<uint16_t>(c)) > static_cast<uint16_t>(a));
}

void ALU::and_u8(uint8_t val) {
    uint8_t result = this->registers->A & val;
    this->registers->A = result;

    // Flags Z 0 1 0
    this->registers->set_flag_z(result == 0);
    this->registers->set_flag_n(false);
    this->registers->set_flag_h(true);
    this->registers->set_flag_c(false);
}
void ALU::or_u8(uint8_t val) {
    uint8_t result = this->registers->A | val;
    this->registers->A = result;

    // Flags Z 0 0 0
    this->registers->set_flag_z(result == 0);
    this->registers->set_flag_n(false);
    this->registers->set_flag_h(false);
    this->registers->set_flag_c(false);
}
void ALU::xor_u8(uint8_t val) {
    uint8_t result = this->registers->A ^ val;
    this->registers->A = result;

    // Flags Z 0 0 0
    this->registers->set_flag_z(result == 0);
    this->registers->set_flag_n(false);
    this->registers->set_flag_h(false);
    this->registers->set_flag_c(false);
}

void ALU::cp_u8(uint8_t val) {
    uint8_t a = this->registers->A;
    uint8_t result = static_cast<uint8_t>(a - val);

    // Flags Z 1 H C
    this->registers->set_flag_z(result == 0);
    this->registers->set_flag_n(true);
    this->registers->set_flag_h((val & 0x0F) > (a & 0x0F));
    this->registers->set_flag_c(val > a);
}
