#ifndef BMI_H
#define BMI_H

#include "memory.h"
#include "registers.h"

#include <cstddef>
#include <cstdint>
#include <vector>

class BMI {
  public:
    BMI(Registers *registers, Memory *memory);

    void rla();
    void rlca();
    void rl_u8(uint8_t &reg);
    void rl_u8(uint16_t address);
    void rlc_u8(uint8_t &reg);
    void rlc_u8(uint16_t address);

    void rra();
    void rrca();
    void rr_u8(uint8_t &reg);
    void rr_u8(uint16_t address);
    void rrc_u8(uint8_t &reg);
    void rrc_u8(uint16_t address);

    void sla_u8(uint8_t &reg);
    void sla_u8(uint16_t address);

    void sra_u8(uint8_t &reg);
    void sra_u8(uint16_t address);

    void srl_u8(uint8_t &reg);
    void srl_u8(uint16_t address);

    void swap_u8(uint8_t &reg);
    void swap_u8(uint16_t address);

    void bit_u8(uint8_t bit, uint8_t reg);
    void bit_u8(uint8_t bit, uint16_t address);

    void res_u8(uint8_t bit, uint8_t &reg);
    void res_u8(uint8_t bit, uint16_t address);

    void set_u8(uint8_t bit, uint8_t &reg);
    void set_u8(uint8_t bit, uint16_t address);

  private:
    Registers *registers;
    Memory *memory;
};

#endif
