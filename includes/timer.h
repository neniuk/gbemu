#pragma once

#include "memory.h"
#include "registers.h"

#include <cstddef>
#include <cstdint>
#include <vector>

class Timer {
  public:
    Timer(Registers &registers, Memory &memory, bool &stopped);

    void tick(uint32_t dots);

    uint8_t get_div();
    void set_div(uint8_t value);

    uint8_t get_tima();
    void set_tima(uint8_t value);

    uint8_t get_tma();
    void set_tma(uint8_t value);

    uint8_t get_tac();
    void set_tac(uint8_t value);

  private:
    Registers &registers_;
    Memory &memory_;
    bool &stopped_;

    uint16_t div_m_cycles_counter_ = 0;
    uint16_t tima_m_cycles_counter_ = 0;

    uint16_t clock_select_increment_[4] = {256, 4, 16, 64};
};
