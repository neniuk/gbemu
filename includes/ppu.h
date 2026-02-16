#ifndef PPU_H
#define PPU_H

#include "memory.h"
#include "registers.h"

#include <cstddef>
#include <cstdint>
#include <vector>

class PPU {
  public:
    PPU(Registers &registers, Memory &memory);

    // LCD control & status registers
    uint8_t get_lcdc();
    void set_lcdc(uint8_t value);

    uint8_t get_ly();
    void set_ly(uint8_t value);

    uint8_t get_lyc();
    void set_lyc(uint8_t value);

    uint8_t get_stat();
    void set_stat(uint8_t value);

  private:
    Registers &registers;
    Memory &memory;
};

#endif
