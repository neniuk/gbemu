#include "cpu.hpp"

#include <cstdint>

void CPU::exec_cb(uint8_t exec_cb) {
    const uint8_t z = exec_cb & 0x07;
    const uint8_t y = (exec_cb >> 3) & 0x07;
    const uint8_t x = (exec_cb >> 6) & 0x03;

    // z -> 0:B 1:C 2:D 3:E 4:H 5:L 6:[HL] 7:A
    uint8_t *reg_ptrs[8] = {
        &this->registers_.B,
        &this->registers_.C,
        &this->registers_.D,
        &this->registers_.E,
        &this->registers_.H,
        &this->registers_.L,
        nullptr, // [HL]
        &this->registers_.A,
    };

    uint8_t *location = reg_ptrs[z];
    const bool is_hlm = location == nullptr;
    const uint16_t hl = this->registers_.get_hl();

    if (x == 0) {
        // Rotates/shifts: y selects operation
        switch (y) {
        case 0:
            is_hlm ? this->bmi_.rlc_u8(hl) : this->bmi_.rlc_u8(*location);
            break;
        case 1:
            is_hlm ? this->bmi_.rrc_u8(hl) : this->bmi_.rrc_u8(*location);
            break;
        case 2:
            is_hlm ? this->bmi_.rl_u8(hl) : this->bmi_.rl_u8(*location);
            break;
        case 3:
            is_hlm ? this->bmi_.rr_u8(hl) : this->bmi_.rr_u8(*location);
            break;
        case 4:
            is_hlm ? this->bmi_.sla_u8(hl) : this->bmi_.sla_u8(*location);
            break;
        case 5:
            is_hlm ? this->bmi_.sra_u8(hl) : this->bmi_.sra_u8(*location);
            break;
        case 6:
            is_hlm ? this->bmi_.swap_u8(hl) : this->bmi_.swap_u8(*location);
            break;
        case 7:
            is_hlm ? this->bmi_.srl_u8(hl) : this->bmi_.srl_u8(*location);
            break;
        }
    } else if (x == 1) {
        // BIT y,r[z]
        is_hlm ? this->bmi_.bit_u8(y, hl) : this->bmi_.bit_u8(y, *location);
    } else if (x == 2) {
        // RES y,r[z]
        is_hlm ? this->bmi_.res_u8(y, hl) : this->bmi_.res_u8(y, *location);
    } else {
        // SET y,r[z]
        is_hlm ? this->bmi_.set_u8(y, hl) : this->bmi_.set_u8(y, *location);
    }

    // Update PC + T-states
    this->registers_.PC += 2;
    if (is_hlm) {
        this->tstates += (x == 1) ? 12 : 16; // BIT [HL]=12, others [HL]=16
    } else {
        this->tstates += 8;
    }
}
