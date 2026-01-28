#ifndef STACK_H
#define STACK_H

#include "config.h"
#include "memory.h"

#include <array>
#include <stdint.h>

class Stack {
  public:
    Stack(uint16_t *SP, Memory *memory);

    void push_byte(uint8_t value);
    uint8_t pop_byte();

    void push_word(uint16_t value);
    uint16_t pop_word();

  private:
    uint16_t *SP;
    Memory *memory;
};

#endif
