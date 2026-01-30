#include "bmi.h"

BMI::BMI(Registers *registers, Memory *memory) : registers(registers), memory(memory) {}

void BMI::rla() {}
void BMI::rlca() {}
void BMI::rl_r8(uint8_t &reg) {}
void BMI::rlc_r8(uint8_t &reg) {}

void BMI::rra() {}
void BMI::rrca() {}
void BMI::rr_r8(uint8_t &reg) {}
void BMI::rrc_r8(uint8_t &reg) {}

void BMI::sla_u8(uint8_t &reg) {}
void BMI::sla_u8(uint16_t address) {}

void BMI::sra_u8(uint8_t &reg) {}
void BMI::sra_u8(uint16_t address) {}

void BMI::srl_u8(uint8_t &reg) {}
void BMI::srl_u8(uint16_t address) {}

void BMI::swap_u8(uint8_t &reg) {}
void BMI::swap_u8(uint16_t address) {}

void BMI::bit_u8(uint8_t bit, uint8_t reg) {}
void BMI::bit_u8(uint8_t bit, uint16_t address) {}

void BMI::res_u8(uint8_t bit, uint8_t &reg) {}
void BMI::res_u8(uint8_t bit, uint16_t address) {}

void BMI::set_u8(uint8_t bit, uint8_t &reg) {}
void BMI::set_u8(uint8_t bit, uint16_t address) {}
