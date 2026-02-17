#include "joypad.h"

Joypad::Joypad(Memory &memory) : memory(memory) {}

void Joypad::handle_event(const SDL_Event &event) {
    if (event.type == SDL_KEYDOWN && !event.key.repeat) {
        set_key(event.key.keysym.scancode, true);
    } else if (event.type == SDL_KEYUP) {
        set_key(event.key.keysym.scancode, false);
    }
}

void Joypad::set_key(SDL_Scancode sc, bool pressed) {
    switch (sc) {
    case SDL_SCANCODE_RIGHT:
        keys_.right = pressed;
        break;
    case SDL_SCANCODE_LEFT:
        keys_.left = pressed;
        break;
    case SDL_SCANCODE_UP:
        keys_.up = pressed;
        break;
    case SDL_SCANCODE_DOWN:
        keys_.down = pressed;
        break;
    case SDL_SCANCODE_X:
        keys_.a = pressed;
        break; // A
    case SDL_SCANCODE_Z:
        keys_.b = pressed;
        break; // B
    case SDL_SCANCODE_RETURN:
        keys_.start = pressed;
        break; // Start
    case SDL_SCANCODE_RSHIFT:
        keys_.select = pressed;
        break; // Select
    default:
        break;
    }
}

void Joypad::set_joyp(uint8_t value) {
    select_bits_ = value & 0x30; // only P14/P15 writable
}

uint8_t Joypad::compute_low_nibble() const {
    uint8_t low = 0x0F; // released = 1

    const bool dpad_selected = (select_bits_ & 0x10) == 0;
    const bool btn_selected = (select_bits_ & 0x20) == 0;

    if (dpad_selected) {
        if (keys_.right) low &= ~0x01;
        if (keys_.left) low &= ~0x02;
        if (keys_.up) low &= ~0x04;
        if (keys_.down) low &= ~0x08;
    }
    if (btn_selected) {
        if (keys_.a) low &= ~0x01;
        if (keys_.b) low &= ~0x02;
        if (keys_.select) low &= ~0x04;
        if (keys_.start) low &= ~0x08;
    }

    return low;
}

uint8_t Joypad::compose_joyp() const { return static_cast<uint8_t>(0xC0 | select_bits_ | compute_low_nibble()); }

uint8_t Joypad::get_joyp() const { return compose_joyp(); }

void Joypad::tick() {
    const uint8_t low = compute_low_nibble();

    // Joypad interrupt on 1->0 transition of selected lines
    const uint8_t falling = static_cast<uint8_t>(prev_low_nibble_ & ~low);
    if (falling != 0) {
        uint8_t iflags = memory.get_if();
        memory.set_if(static_cast<uint8_t>(iflags | 0x10)); // IF bit4
    }

    prev_low_nibble_ = low;
}
