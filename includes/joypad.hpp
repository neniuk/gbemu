#pragma once

#include "memory.hpp"

#include <SDL2/SDL.h>
#include <cstdint>

class Joypad {
  public:
    explicit Joypad(Memory &memory);

    void handle_event(const SDL_Event &event);
    void set_joyp(uint8_t value);
    uint8_t get_joyp() const;
    void tick();

  private:
    struct Keys_ {
        bool right = false, left = false, up = false, down = false;
        bool a = false, b = false, select = false, start = false;
    };

    void set_key_(SDL_Scancode sc, bool pressed);
    uint8_t get_low_nibble_() const;
    uint8_t compose_joyp_() const;

    Memory &memory_;
    Keys_ keys_{};

    uint8_t select_bits_ = 0x30; // 0b00110000
    uint8_t prev_low_nibble_ = 0x0F;
};
