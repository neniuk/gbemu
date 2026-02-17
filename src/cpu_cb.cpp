#include "cpu.h"

#include <stdint.h>

void CPU::exec_cb(uint8_t exec_cb) {
    const uint8_t z = exec_cb & 0x07;
    const uint8_t y = (exec_cb >> 3) & 0x07;
    const uint8_t x = (exec_cb >> 6) & 0x03;

    // z -> 0:B 1:C 2:D 3:E 4:H 5:L 6:[HL] 7:A
    uint8_t *reg_ptrs[8] = {
        &this->registers.B,
        &this->registers.C,
        &this->registers.D,
        &this->registers.E,
        &this->registers.H,
        &this->registers.L,
        nullptr, // [HL]
        &this->registers.A,
    };

    uint8_t *location = reg_ptrs[z];
    const bool is_hlm = location == nullptr;
    const uint16_t hl = this->registers.get_hl();

    if (x == 0) {
        // Rotates/shifts: y selects operation
        switch (y) {
        case 0:
            is_hlm ? this->bmi.rlc_u8(hl) : this->bmi.rlc_u8(*location);
            break;
        case 1:
            is_hlm ? this->bmi.rrc_u8(hl) : this->bmi.rrc_u8(*location);
            break;
        case 2:
            is_hlm ? this->bmi.rl_u8(hl) : this->bmi.rl_u8(*location);
            break;
        case 3:
            is_hlm ? this->bmi.rr_u8(hl) : this->bmi.rr_u8(*location);
            break;
        case 4:
            is_hlm ? this->bmi.sla_u8(hl) : this->bmi.sla_u8(*location);
            break;
        case 5:
            is_hlm ? this->bmi.sra_u8(hl) : this->bmi.sra_u8(*location);
            break;
        case 6:
            is_hlm ? this->bmi.swap_u8(hl) : this->bmi.swap_u8(*location);
            break;
        case 7:
            is_hlm ? this->bmi.srl_u8(hl) : this->bmi.srl_u8(*location);
            break;
        }
    } else if (x == 1) {
        // BIT y,r[z]
        is_hlm ? this->bmi.bit_u8(y, hl) : this->bmi.bit_u8(y, *location);
    } else if (x == 2) {
        // RES y,r[z]
        is_hlm ? this->bmi.res_u8(y, hl) : this->bmi.res_u8(y, *location);
    } else {
        // SET y,r[z]
        is_hlm ? this->bmi.set_u8(y, hl) : this->bmi.set_u8(y, *location);
    }

    // Update PC + T-states
    this->registers.PC += 2;
    if (is_hlm) {
        this->tstates += (x == 1) ? 12 : 16; // BIT [HL]=12, others [HL]=16
    } else {
        this->tstates += 8;
    }
}
