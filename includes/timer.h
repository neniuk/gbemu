#ifndef TIMER_H
#define TIMER_H

#include "memory.h"
#include "registers.h"

#include <cstddef>
#include <cstdint>
#include <vector>

class Timer {
  public:
    Timer(Registers &registers, Memory &memory, bool &stopped);

    void tick(int dots);

    uint8_t get_div();
    void set_div(uint8_t value);

    uint8_t get_tima();
    void set_tima(uint8_t value);

    uint8_t get_tma();
    void set_tma(uint8_t value);

    uint8_t get_tac();
    void set_tac(uint8_t value);

  private:
    Registers &registers;
    Memory &memory;
    bool &stopped;

    uint16_t m_cycles_counter = 0;

    uint16_t clock_select_increment[4] = {256, 4, 16, 64};
};

#endif
