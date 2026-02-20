#pragma once

#include "registers.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

class ALU {
  public:
    ALU(Registers &registers);

    void add_u8(uint8_t val);
    void adc_u8(uint8_t val);

    void add_u16(uint16_t val);

    void sub_u8(uint8_t val);
    void sbc_u8(uint8_t val);

    void and_u8(uint8_t val);
    void or_u8(uint8_t val);
    void xor_u8(uint8_t val);

    void cp_u8(uint8_t val);

  private:
    Registers &registers_;
};
