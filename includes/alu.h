#ifndef ALU_H
#define ALU_H

#include "memory.h"
#include "registers.h"

#include <cstddef>
#include <cstdint>
#include <vector>

class ALU {
  public:
    ALU(Registers *registers, Memory *memory);

    void add_u8(uint8_t val);
    void adc_u8(uint8_t val);

    void sub_u8(uint8_t val);
    void sbc_u8(uint8_t val);

    void and_u8(uint8_t val);
    void or_u8(uint8_t val);
    void xor_u8(uint8_t val);

    void cp_u8(uint8_t val);

  private:
    Registers *registers;
    Memory *memory;
};

#endif
