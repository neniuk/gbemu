#include "cpu.h"

#include <stdint.h>

void CPU::exec_cb(uint8_t exec_cb) {
    uint8_t octal_last_digit = exec_cb & 0b111;
    uint8_t octal_first_two_digits = (exec_cb >> 3) & 0b11111;

    // octal_last_digit -> x0 B, x1 C, x2 D, x3 E, x4 H, x5 L, x6 [HL], x7 A
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

    uint8_t *location = reg_ptrs[octal_last_digit & 0b111];
    uint8_t bit = (octal_first_two_digits >> 3) & 0b111;

    switch (octal_first_two_digits) {
    case 0: // RLC u8
        this->bmi.rlc_u8(location ? *location : this->registers.get_hl());
        break;
    case 1: // RRC u8
        this->bmi.rrc_u8(location ? *location : this->registers.get_hl());
        break;
    case 2: // RL u8
        this->bmi.rl_u8(location ? *location : this->registers.get_hl());
        break;
    case 3: // RR u8
        this->bmi.rr_u8(location ? *location : this->registers.get_hl());
        break;
    case 4: // SLA u8
        this->bmi.sla_u8(location ? *location : this->registers.get_hl());
        break;
    case 5: // SRA u8
        this->bmi.sra_u8(location ? *location : this->registers.get_hl());
        break;
    case 6: // SWAP u8
        this->bmi.swap_u8(location ? *location : this->registers.get_hl());
        break;
    case 7: // SRL u8
        this->bmi.srl_u8(location ? *location : this->registers.get_hl());
        break;
    case 10: // BIT b,u8
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
        this->bmi.bit_u8(bit, location ? *location : this->registers.get_hl());
        break;
    case 20: // RES b,u8
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
    case 27:
        this->bmi.res_u8(bit, location ? *location : this->registers.get_hl());
        break;
    case 30: // SET b,u8
    case 31:
    case 32:
    case 33:
    case 34:
    case 35:
    case 36:
    case 37:
        this->bmi.set_u8(bit, location ? *location : this->registers.get_hl());
        break;
    default:
        this->op_unimplemented();
        break;
    }

    // Update PC + T-states
    this->registers.PC += 2;
    this->tstates += (location && (octal_last_digit == 6)) ? 16 : 8;
}
