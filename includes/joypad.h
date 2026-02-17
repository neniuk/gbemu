#ifndef JOYPAD_H
#define JOYPAD_H

#include "memory.h"

#include <SDL2/SDL.h>
#include <cstdint>

class Joypad {
  public:
    explicit Joypad(Memory &memory);

    void handle_event(const SDL_Event &event);
    void set_joyp(uint8_t value); // CPU write to FF00
    uint8_t get_joyp() const;      // CPU read from FF00
    void tick();                    // edge detect -> IF bit4

  private:
    struct Keys {
        bool right = false, left = false, up = false, down = false;
        bool a = false, b = false, select = false, start = false;
    };

    void set_key(SDL_Scancode sc, bool pressed);
    uint8_t compute_low_nibble() const; // active-low lines
    uint8_t compose_joyp() const;       // full FF00 value

    Memory &memory;
    Keys keys_{};

    // Keep only select bits writable (P14/P15), active-low
    // default: none selected (bits high)
    uint8_t select_bits_ = 0x30;

    // For interrupt edge detection (1->0 on selected lines)
    uint8_t prev_low_nibble_ = 0x0F;
};

#endif
