#include "stack.h"
#include "config.h"
#include "memory.h"

#include <array>
#include <assert.h>
#include <cstddef>
#include <cstring>
#include <stdint.h>

Stack::Stack(uint16_t *SP, Memory *memory) {
    this->SP = SP;
    this->memory = memory;
}

void Stack::push_byte(uint8_t value) {
    assert(this->SP != nullptr && this->memory != nullptr);
    assert(*this->SP >= 1 && *this->SP < GB_MEMORY_SIZE);

    *this->SP = static_cast<uint16_t>(*this->SP - 1);
    this->memory->write_byte(*this->SP, value);
}

uint8_t Stack::pop_byte() {
    assert(this->SP != nullptr && this->memory != nullptr);
    assert(*this->SP >= 0 && *this->SP < GB_MEMORY_SIZE);

    uint8_t byte = this->memory->read_byte(*this->SP);
    *this->SP = static_cast<uint16_t>(*this->SP + 1);

    return byte;
}

void Stack::push_word(uint16_t value) {
    assert(this->SP != nullptr && this->memory != nullptr);
    assert(*this->SP >= 2 && *this->SP < GB_MEMORY_SIZE);

    *this->SP = static_cast<uint16_t>(*this->SP - 2);
    this->memory->write_word(*this->SP, value);
}
uint16_t Stack::pop_word() {
    assert(this->SP != nullptr && this->memory != nullptr);
    assert(*this->SP >= 0 && *this->SP < GB_MEMORY_SIZE - 1);

    uint16_t word = this->memory->read_word(*this->SP);
    *this->SP = static_cast<uint16_t>(*this->SP + 2);

    return word;
}
