#ifndef REGISTERS_H
#define REGISTERS_H

#include "config.h"
#include <cstddef>
#include <stdint.h>

static constexpr uint8_t FLAG_Z_MASK = 0x80;
static constexpr uint8_t FLAG_N_MASK = 0x40;
static constexpr uint8_t FLAG_H_MASK = 0x20;
static constexpr uint8_t FLAG_C_MASK = 0x10;

class Registers {
  public:
    Registers(uint8_t initial_value = 0);

    uint8_t A;
    uint8_t B;
    uint8_t C;
    uint8_t D;
    uint8_t E;
    uint8_t F;
    uint8_t H;
    uint8_t L;

    uint16_t SP;
    uint16_t PC;

    bool IME;

    void set_af(uint16_t value);
    uint16_t get_af();

    void set_bc(uint16_t value);
    uint16_t get_bc();

    void set_de(uint16_t value);
    uint16_t get_de();

    void set_hl(uint16_t value);
    uint16_t get_hl();

    void set_flag_z(bool value);
    bool get_flag_z();

    void set_flag_n(bool value);
    bool get_flag_n();

    void set_flag_h(bool value);
    bool get_flag_h();

    void set_flag_c(bool value);
    bool get_flag_c();
};

#endif
