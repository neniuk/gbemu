#ifndef PPU_H
#define PPU_H

#include "memory.h"
#include "registers.h"
#include "screen.h"

#include <cstddef>
#include <cstdint>
#include <vector>

class PPU {
  public:
    PPU(Registers &registers, Memory &memory, Screen &screen);

    // LCD control & status registers
    uint8_t get_lcdc();
    void set_lcdc(uint8_t value);

    uint8_t get_ly();
    void set_ly(uint8_t value);

    uint8_t get_lyc();
    void set_lyc(uint8_t value);

    uint8_t get_stat();
    void set_stat(uint8_t value);

    uint8_t get_ppu_mode();
    void set_ppu_mode(uint8_t mode);

    uint8_t get_scy();
    void set_scy(uint8_t value);

    uint8_t get_scx();
    void set_scx(uint8_t value);

    uint8_t get_wy();
    void set_wy(uint8_t value);

    uint8_t get_wx();
    void set_wx(uint8_t value);

    uint8_t get_bgp();
    void set_bgp(uint8_t value);

    uint8_t get_obp0();
    void set_obp0(uint8_t value);

    uint8_t get_obp1();
    void set_obp1(uint8_t value);

  private:
    Registers &registers;
    Memory &memory;
    Screen &screen;
};

#endif
