#include "timer.h"

Timer::Timer(Registers &registers, Memory &memory, bool &stopped) : registers(registers), memory(memory), stopped(stopped) {}

void Timer::tick(int dots) {
    int m_cycles = dots / 4;
    this->m_cycles_counter += m_cycles;

    if (this->m_cycles_counter >= 64 && !this->stopped) { // 16384Hz
        this->m_cycles_counter -= 64;
        uint8_t div = this->get_div();
        this->set_div(div + 1);
    }

    uint8_t clock_select = this->get_tac() & 0x03;
    bool tima_enabled = (this->get_tac() & 0x04) != 0;
    if (!tima_enabled) return;

    uint16_t increment = this->clock_select_increment[clock_select];
    if (this->m_cycles_counter >= increment) {
        this->m_cycles_counter -= increment;
        
        uint8_t tima = this->get_tima();
        if (tima == 0xFF) {
            this->set_tima(this->get_tma());
            uint8_t IF = this->memory.get_if();
            this->memory.set_if(static_cast<uint8_t>(IF | 0x04));
        } else {
            this->set_tima(static_cast<uint8_t>(tima + 1));
        }
    }
}

uint8_t Timer::get_div() {
    return this->memory.read_byte(0xFF04);
}
void Timer::set_div(uint8_t value) {
    this->memory.write_byte(0xFF04, value);
}

uint8_t Timer::get_tima() {
    return this->memory.read_byte(0xFF05);
}
void Timer::set_tima(uint8_t value) {
    this->memory.write_byte(0xFF05, value);
}

uint8_t Timer::get_tma() {
    return this->memory.read_byte(0xFF06);
}
void Timer::set_tma(uint8_t value) {
    this->memory.write_byte(0xFF06, value);
}

uint8_t Timer::get_tac() {
    return this->memory.read_byte(0xFF07);
}
void Timer::set_tac(uint8_t value) {
    this->memory.write_byte(0xFF07, value);
}
