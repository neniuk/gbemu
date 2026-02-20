#include "stack.hpp"
#include "config.hpp"
#include "memory.hpp"

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>

Stack::Stack(uint16_t &SP, Memory &memory) : SP_(SP), memory_(memory) {}

void Stack::push_byte(uint8_t value) {
    assert(this->SP_ > 0U);
    this->SP_ = static_cast<uint16_t>(this->SP_ - 1U);
    this->memory_.write_byte(this->SP_, value);
}

uint8_t Stack::pop_byte() {
    assert(this->SP_ < 0xFFFFU);
    const uint8_t byte = this->memory_.read_byte(this->SP_);
    this->SP_ = static_cast<uint16_t>(this->SP_ + 1U);
    return byte;
}

void Stack::push_word(uint16_t value) {
    assert(this->SP_ > 1U);
    this->SP_ = static_cast<uint16_t>(this->SP_ - 2U);
    this->memory_.write_word(this->SP_, value);
}

uint16_t Stack::pop_word() {
    assert(this->SP_ <= 0xFFFEU);
    const uint16_t word = this->memory_.read_word(this->SP_);
    this->SP_ = static_cast<uint16_t>(this->SP_ + 2U);
    return word;
}
