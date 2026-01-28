#ifndef IDU_H
#define IDU_H

#include "memory.h"
#include "registers.h"

#include <cstddef>
#include <cstdint>
#include <vector>

class IDU {
  public:
    IDU(Registers *registers, Memory *memory);

    void increment_r8(uint8_t &register_8);
    void decrement_r8(uint8_t &register_8);

    void increment_mem8(uint16_t address);
    void decrement_mem8(uint16_t address);

  private:
    Registers *registers;
    Memory *memory;
};

#endif
