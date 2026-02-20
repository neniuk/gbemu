#include "bmi.hpp"

BMI::BMI(Registers &registers, Memory &memory) : registers_(registers), memory_(memory) {}

void BMI::rla() {
    uint8_t carry_in = this->registers_.get_flag_c() ? 1 : 0;
    uint8_t new_carry = (this->registers_.A & 0x80) >> 7;
    this->registers_.A = static_cast<uint8_t>((this->registers_.A << 1) | carry_in);

    // Flags 0 0 0 C
    this->registers_.set_flag_z(false);
    this->registers_.set_flag_n(false);
    this->registers_.set_flag_h(false);
    this->registers_.set_flag_c(new_carry != 0);
}
void BMI::rlca() {
    uint8_t old_a_in = this->registers_.A & 0x80;
    this->registers_.A = static_cast<uint8_t>((this->registers_.A << 1) | (old_a_in >> 7));

    // Flags 0 0 0 C
    this->registers_.set_flag_z(false);
    this->registers_.set_flag_n(false);
    this->registers_.set_flag_h(false);
    this->registers_.set_flag_c(old_a_in != 0);
}
void BMI::rl_u8(uint8_t &reg) {
    uint8_t carry_in = this->registers_.get_flag_c() ? 1 : 0;
    uint8_t new_carry = (reg & 0x80) >> 7;
    reg = static_cast<uint8_t>((reg << 1) | carry_in);

    // Flags Z 0 0 C
    this->registers_.set_flag_z(reg == 0);
    this->registers_.set_flag_n(false);
    this->registers_.set_flag_h(false);
    this->registers_.set_flag_c(new_carry != 0);
}
void BMI::rl_u8(uint16_t address) {
    uint8_t value = this->memory_.read_byte(address);
    uint8_t carry_in = this->registers_.get_flag_c() ? 1 : 0;
    uint8_t new_carry = (value & 0x80) >> 7;
    value = static_cast<uint8_t>((value << 1) | carry_in);
    this->memory_.write_byte(address, value);

    // Flags Z 0 0 C
    this->registers_.set_flag_z(value == 0);
    this->registers_.set_flag_n(false);
    this->registers_.set_flag_h(false);
    this->registers_.set_flag_c(new_carry != 0);
}
void BMI::rlc_u8(uint8_t &reg) {
    uint8_t old_reg_in = reg & 0x80;
    reg = static_cast<uint8_t>((reg << 1) | (old_reg_in >> 7));

    // Flags Z 0 0 C
    this->registers_.set_flag_z(reg == 0);
    this->registers_.set_flag_n(false);
    this->registers_.set_flag_h(false);
    this->registers_.set_flag_c(old_reg_in != 0);
}
void BMI::rlc_u8(uint16_t address) {
    uint8_t value = this->memory_.read_byte(address);
    uint8_t old_value_in = value & 0x80;
    value = static_cast<uint8_t>((value << 1) | (old_value_in >> 7));
    this->memory_.write_byte(address, value);

    // Flags Z 0 0 C
    this->registers_.set_flag_z(value == 0);
    this->registers_.set_flag_n(false);
    this->registers_.set_flag_h(false);
    this->registers_.set_flag_c(old_value_in != 0);
}

void BMI::rra() {
    uint8_t carry_in = this->registers_.get_flag_c() ? 1 : 0;
    uint8_t new_carry = this->registers_.A & 0x01;
    this->registers_.A = static_cast<uint8_t>((this->registers_.A >> 1) | (carry_in << 7));

    // Flags 0 0 0 C
    this->registers_.set_flag_z(false);
    this->registers_.set_flag_n(false);
    this->registers_.set_flag_h(false);
    this->registers_.set_flag_c(new_carry != 0);
}
void BMI::rrca() {
    uint8_t old_a_in = this->registers_.A & 0x01;
    this->registers_.A = static_cast<uint8_t>((this->registers_.A >> 1) | (old_a_in << 7));

    // Flags 0 0 0 C
    this->registers_.set_flag_z(false);
    this->registers_.set_flag_n(false);
    this->registers_.set_flag_h(false);
    this->registers_.set_flag_c(old_a_in != 0);
}
void BMI::rr_u8(uint8_t &reg) {
    uint8_t carry_in = this->registers_.get_flag_c() ? 1 : 0;
    uint8_t new_carry = reg & 0x01;
    reg = static_cast<uint8_t>((reg >> 1) | (carry_in << 7));

    // Flags Z 0 0 C
    this->registers_.set_flag_z(reg == 0);
    this->registers_.set_flag_n(false);
    this->registers_.set_flag_h(false);
    this->registers_.set_flag_c(new_carry != 0);
}
void BMI::rr_u8(uint16_t address) {
    uint8_t value = this->memory_.read_byte(address);
    uint8_t carry_in = this->registers_.get_flag_c() ? 1 : 0;
    uint8_t new_carry = value & 0x01;
    value = static_cast<uint8_t>((value >> 1) | (carry_in << 7));
    this->memory_.write_byte(address, value);

    // Flags Z 0 0 C
    this->registers_.set_flag_z(value == 0);
    this->registers_.set_flag_n(false);
    this->registers_.set_flag_h(false);
    this->registers_.set_flag_c(new_carry != 0);
}
void BMI::rrc_u8(uint8_t &reg) {
    uint8_t old_reg_in = reg & 0x01;
    reg = static_cast<uint8_t>((reg >> 1) | (old_reg_in << 7));

    // Flags Z 0 0 C
    this->registers_.set_flag_z(reg == 0);
    this->registers_.set_flag_n(false);
    this->registers_.set_flag_h(false);
    this->registers_.set_flag_c(old_reg_in != 0);
}
void BMI::rrc_u8(uint16_t address) {
    uint8_t value = this->memory_.read_byte(address);
    uint8_t old_value_in = value & 0x01;
    value = static_cast<uint8_t>((value >> 1) | (old_value_in << 7));
    this->memory_.write_byte(address, value);

    // Flags Z 0 0 C
    this->registers_.set_flag_z(value == 0);
    this->registers_.set_flag_n(false);
    this->registers_.set_flag_h(false);
    this->registers_.set_flag_c(old_value_in != 0);
}

void BMI::sla_u8(uint8_t &reg) {
    uint8_t new_carry = (reg & 0x80) >> 7;
    reg = static_cast<uint8_t>(reg << 1);

    // Flags Z 0 0 C
    this->registers_.set_flag_z(reg == 0);
    this->registers_.set_flag_n(false);
    this->registers_.set_flag_h(false);
    this->registers_.set_flag_c(new_carry != 0);
}
void BMI::sla_u8(uint16_t address) {
    uint8_t value = this->memory_.read_byte(address);
    uint8_t new_carry = (value & 0x80) >> 7;
    value = static_cast<uint8_t>(value << 1);
    this->memory_.write_byte(address, value);

    // Flags Z 0 0 C
    this->registers_.set_flag_z(value == 0);
    this->registers_.set_flag_n(false);
    this->registers_.set_flag_h(false);
    this->registers_.set_flag_c(new_carry != 0);
}

void BMI::sra_u8(uint8_t &reg) {
    uint8_t new_carry = reg & 0x01;
    uint8_t msb = reg & 0x80;
    reg = static_cast<uint8_t>((reg >> 1) | msb);

    // Flags Z 0 0 C
    this->registers_.set_flag_z(reg == 0);
    this->registers_.set_flag_n(false);
    this->registers_.set_flag_h(false);
    this->registers_.set_flag_c(new_carry != 0);
}
void BMI::sra_u8(uint16_t address) {
    uint8_t value = this->memory_.read_byte(address);
    uint8_t new_carry = value & 0x01;
    uint8_t msb = value & 0x80;
    value = static_cast<uint8_t>((value >> 1) | msb);
    this->memory_.write_byte(address, value);

    // Flags Z 0 0 C
    this->registers_.set_flag_z(value == 0);
    this->registers_.set_flag_n(false);
    this->registers_.set_flag_h(false);
    this->registers_.set_flag_c(new_carry != 0);
}

void BMI::srl_u8(uint8_t &reg) {
    uint8_t new_carry = reg & 0x01;
    reg = static_cast<uint8_t>(reg >> 1);

    // Flags Z 0 0 C
    this->registers_.set_flag_z(reg == 0);
    this->registers_.set_flag_n(false);
    this->registers_.set_flag_h(false);
    this->registers_.set_flag_c(new_carry != 0);
}
void BMI::srl_u8(uint16_t address) {
    uint8_t value = this->memory_.read_byte(address);
    uint8_t new_carry = value & 0x01;
    value = static_cast<uint8_t>(value >> 1);
    this->memory_.write_byte(address, value);

    // Flags Z 0 0 C
    this->registers_.set_flag_z(value == 0);
    this->registers_.set_flag_n(false);
    this->registers_.set_flag_h(false);
    this->registers_.set_flag_c(new_carry != 0);
}

void BMI::swap_u8(uint8_t &reg) {
    uint8_t upper_nibble = static_cast<uint8_t>((reg & 0xF0) >> 4U);
    uint8_t lower_nibble = static_cast<uint8_t>((reg & 0x0F) << 4U);
    reg = static_cast<uint8_t>(upper_nibble | lower_nibble);

    // Flags Z 0 0 0
    this->registers_.set_flag_z(reg == 0);
    this->registers_.set_flag_n(false);
    this->registers_.set_flag_h(false);
    this->registers_.set_flag_c(false);
}
void BMI::swap_u8(uint16_t address) {
    uint8_t value = this->memory_.read_byte(address);
    uint8_t upper_nibble = static_cast<uint8_t>((value & 0xF0) >> 4U);
    uint8_t lower_nibble = static_cast<uint8_t>((value & 0x0F) << 4U);
    value = static_cast<uint8_t>(upper_nibble | lower_nibble);
    this->memory_.write_byte(address, value);

    // Flags Z 0 0 0
    this->registers_.set_flag_z(value == 0);
    this->registers_.set_flag_n(false);
    this->registers_.set_flag_h(false);
    this->registers_.set_flag_c(false);
}

void BMI::bit_u8(uint8_t bit, uint8_t &reg) {
    bool bit_set = (reg & (1 << bit)) != 0;

    // Flags Z 0 1 -
    this->registers_.set_flag_z(!bit_set);
    this->registers_.set_flag_n(false);
    this->registers_.set_flag_h(true);
}
void BMI::bit_u8(uint8_t bit, uint16_t address) {
    uint8_t value = this->memory_.read_byte(address);
    bool bit_set = (value & (1 << bit)) != 0;

    // Flags Z 0 1 -
    this->registers_.set_flag_z(!bit_set);
    this->registers_.set_flag_n(false);
    this->registers_.set_flag_h(true);
}

void BMI::res_u8(uint8_t bit, uint8_t &reg) { reg &= static_cast<uint8_t>(~(1 << bit)); }
void BMI::res_u8(uint8_t bit, uint16_t address) {
    uint8_t value = this->memory_.read_byte(address);
    value &= static_cast<uint8_t>(~(1 << bit));
    this->memory_.write_byte(address, value);
}

void BMI::set_u8(uint8_t bit, uint8_t &reg) { reg |= static_cast<uint8_t>(1 << bit); }
void BMI::set_u8(uint8_t bit, uint16_t address) {
    uint8_t value = this->memory_.read_byte(address);
    value |= static_cast<uint8_t>(1 << bit);
    this->memory_.write_byte(address, value);
}
