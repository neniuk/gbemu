#ifndef REGISTERS_H
#define REGISTERS_H

#include "config.h"
#include <cstddef>
#include <stdint.h>

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

    void setAF(uint16_t value);
    uint16_t getAF();

    void setBC(uint16_t value);
    uint16_t getBC();

    void setDE(uint16_t value);
    uint16_t getDE();

    void setHL(uint16_t value);
    uint16_t getHL();
};

#endif
