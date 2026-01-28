#ifndef SCREEN_H
#define SCREEN_H

#include "config.h"
#include <cstddef>
#include <stdint.h>
#include <vector>

class Screen {
  public:
    Screen();
    void set(size_t x, size_t y);
    bool is_set(size_t x, size_t y);
    void unset(size_t x, size_t y);
    void clear();

  private:
    uint8_t screen[GB_SCREEN_WIDTH][GB_SCREEN_HEIGHT];
};

#endif
