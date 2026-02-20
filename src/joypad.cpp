#include "joypad.h"

#include "config.h"

#include <iostream>

Joypad::Joypad(Memory &memory) : memory_(memory) {}

void Joypad::handle_event(const SDL_Event &event) {
    if (event.type == SDL_KEYDOWN && !event.key.repeat) {
        this->set_key_(event.key.keysym.scancode, true);
    } else if (event.type == SDL_KEYUP) {
        this->set_key_(event.key.keysym.scancode, false);
    }
}

void Joypad::set_key_(SDL_Scancode sc, bool pressed) {
    bool handled = true;
    const char *gb_btn = nullptr;

    switch (sc) {
    case SDL_SCANCODE_RIGHT:
        this->keys_.right = pressed;
        gb_btn = "Right";
        break;
    case SDL_SCANCODE_LEFT:
        this->keys_.left = pressed;
        gb_btn = "Left";
        break;
    case SDL_SCANCODE_UP:
        this->keys_.up = pressed;
        gb_btn = "Up";
        break;
    case SDL_SCANCODE_DOWN:
        this->keys_.down = pressed;
        gb_btn = "Down";
        break;
    case SDL_SCANCODE_X:
        this->keys_.a = pressed;
        gb_btn = "A";
        break;
    case SDL_SCANCODE_Z:
        this->keys_.b = pressed;
        gb_btn = "B";
        break;
    case SDL_SCANCODE_RETURN:
        this->keys_.start = pressed;
        gb_btn = "Start";
        break;
    case SDL_SCANCODE_RSHIFT:
        this->keys_.select = pressed;
        gb_btn = "Select";
        break;
    default:
        handled = false;
        break;
    }

    if constexpr (config::debug_mode) {
        if (handled) {
            std::cout << "[DEBUG] joypad > [joypad] " << (pressed ? "down " : "up   ") << gb_btn << " (scancode=" << static_cast<int>(sc)
                      << ", name=" << SDL_GetScancodeName(sc) << ")\n";
        }
    }
}

void Joypad::set_joyp(uint8_t value) {
    this->select_bits_ = value & 0x30; // only P14/P15 writable
}

uint8_t Joypad::get_low_nibble_() const {
    uint8_t low = 0x0F; // released = 1

    const bool dpad_selected = (this->select_bits_ & 0x10) == 0;
    const bool btn_selected = (this->select_bits_ & 0x20) == 0;

    if (dpad_selected) {
        if (this->keys_.right) low &= ~0x01;
        if (this->keys_.left) low &= ~0x02;
        if (this->keys_.up) low &= ~0x04;
        if (this->keys_.down) low &= ~0x08;
    }
    if (btn_selected) {
        if (this->keys_.a) low &= ~0x01;
        if (this->keys_.b) low &= ~0x02;
        if (this->keys_.select) low &= ~0x04;
        if (this->keys_.start) low &= ~0x08;
    }

    return low;
}

uint8_t Joypad::compose_joyp_() const { return static_cast<uint8_t>(0xC0 | this->select_bits_ | this->get_low_nibble_()); }

uint8_t Joypad::get_joyp() const { return this->compose_joyp_(); }

void Joypad::tick() {
    const uint8_t low = this->get_low_nibble_();

    // Joypad interrupt on 1->0 transition of selected lines
    const uint8_t falling = static_cast<uint8_t>(this->prev_low_nibble_ & ~low);
    if (falling != 0) {
        uint8_t iflags = this->memory_.get_if();
        this->memory_.set_if(static_cast<uint8_t>(iflags | 0x10)); // IF bit4
    }

    this->prev_low_nibble_ = low;
}
