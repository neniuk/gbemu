#include "timer.h"

Timer::Timer(Registers &registers, Memory &memory, bool &stopped) : registers_(registers), memory_(memory), stopped_(stopped) {}

void Timer::tick(uint32_t dots) {
    const int m_cycles = dots / 4;

    if (this->memory_.consume_div_reset()) {
        this->div_m_cycles_counter_ = 0;
    }

    this->div_m_cycles_counter_ += static_cast<uint16_t>(m_cycles);
    this->tima_m_cycles_counter_ += static_cast<uint16_t>(m_cycles);

    while (this->div_m_cycles_counter_ >= 64 && !this->stopped_) { // 16384Hz
        this->div_m_cycles_counter_ -= 64;
        uint8_t div = this->get_div();
        this->set_div(div + 1);
    }

    uint8_t clock_select = this->get_tac() & 0x03;
    bool tima_enabled = (this->get_tac() & 0x04) != 0;
    if (!tima_enabled) return;

    uint16_t increment = this->clock_select_increment_[clock_select];
    while (this->tima_m_cycles_counter_ >= increment) {
        this->tima_m_cycles_counter_ -= increment;

        uint8_t tima = this->get_tima();
        if (tima == 0xFF) {
            this->set_tima(this->get_tma());
            uint8_t IF = this->memory_.get_if();
            this->memory_.set_if(static_cast<uint8_t>(IF | 0x04));
        } else {
            this->set_tima(static_cast<uint8_t>(tima + 1));
        }
    }
}

uint8_t Timer::get_div() { return this->memory_.read_byte(0xFF04); }
void Timer::set_div(uint8_t value) { this->memory_.write_io_reg(0xFF04, value); }

uint8_t Timer::get_tima() { return this->memory_.read_byte(0xFF05); }
void Timer::set_tima(uint8_t value) { this->memory_.write_byte(0xFF05, value); }

uint8_t Timer::get_tma() { return this->memory_.read_byte(0xFF06); }
void Timer::set_tma(uint8_t value) { this->memory_.write_byte(0xFF06, value); }

uint8_t Timer::get_tac() { return this->memory_.read_byte(0xFF07); }
void Timer::set_tac(uint8_t value) { this->memory_.write_byte(0xFF07, value); }
