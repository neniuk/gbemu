#include "cpu.h"
#include "alu.h"
#include "config.h"
#include "idu.h"
#include "memory.h"
#include "registers.h"
#include "screen.h"
#include "stack.h"
#include "bmi.h"
#include "ppu.h"

#include <SDL2/SDL.h>
#include <assert.h>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <vector>

CPU::CPU(Registers &registers, Memory &memory, Stack &stack, IDU &idu, ALU &alu, BMI &bmi, PPU &ppu) : registers(registers), memory(memory), stack(stack), idu(idu), alu(alu), bmi(bmi), ppu(ppu) {
    this->init(); // Initialize opcode tables
}

uint32_t CPU::step() {
    if (this->halted || this->stopped) {
        this->tstates += 4;
        return 4;
    }

    const uint64_t before = this->tstates;

    uint8_t opcode = this->memory.read_byte(this->registers.PC);
    this->exec(opcode);

    return static_cast<uint32_t>(this->tstates - before);
}

void CPU::exec(uint8_t opcode) {
    OpInfo op_info = this->ops[opcode];
    (this->*op_info.fn)();
}

void CPU::service_interrupts() {
    const uint8_t IE = this->memory.get_ie();
    const uint8_t IF = this->memory.get_if();

    if ((IE & IF) == 0) return;
    this->halted = false;

    if (!this->registers.IME) return;
    this->registers.IME = false;

    auto service = [&](uint8_t bit, uint16_t vector) -> bool {
        if (((IE & IF) & (1u << bit)) == 0) return false; // Not requested / enabled
        this->memory.write_byte(0xFF0F, static_cast<uint8_t>(IF & ~(1u << bit))); // Clear IF bit

        this->stack.push_word(this->registers.PC);
        this->registers.PC = vector;

        this->tstates += 20;
        return true;
    };

    if (service(0, 0x0040)) return; // VBlank
    if (service(1, 0x0048)) return; // STAT
    if (service(2, 0x0050)) return; // Timer
    if (service(3, 0x0058)) return; // Serial
    if (service(4, 0x0060)) return; // Joypad
};

// default unimplemented opcode handler =======
void CPU::op_unimplemented() {
    std::stringstream ss;
    ss << "Unimplemented opcode at PC=0x" << std::hex << std::setw(4) << std::setfill('0') << this->registers.PC;
    throw std::runtime_error(ss.str());
    return;
}

// opcode implementations =====================

// Opcode
// Mnemonic
// Length in bytes (pc increment) & Duration in T-states (cycleCount increment)
// Flags affected (Z, N, H, C) - Zero, Subtract, Half-carry, Carry

// Return time taken to complete in T-states

// 0x00
// NOP
// 1 4
// - - - -
void CPU::op_nop() {
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x01
// LD BC, n16
// 3 12
// - - - -
void CPU::op_ld_bc_n16() {
    uint16_t n16 = this->memory.read_word(static_cast<uint16_t>(this->registers.PC + 1));
    this->registers.set_bc(n16);

    this->registers.PC += 3;
    this->tstates += 12;
}

// 0x02
// LD [BC], A
// 1 8
// - - - -
void CPU::op_ld_bcm_a() {
    this->memory.write_byte(this->registers.get_bc(), this->registers.A);

    this->registers.PC += 1;
    this->tstates += 8;
}

// 0x03
// INC BC
// 1 8
// - - - -
void CPU::op_inc_bc() {
    this->registers.set_bc(static_cast<uint16_t>(this->registers.get_bc() + 1));

    this->registers.PC += 1;
    this->tstates += 8;
}

// 0x04
// INC B
// 1 4
// Z 0 H -
void CPU::op_inc_b() {
    this->idu.increment_r8(this->registers.B);

    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x05
// DEC B
// 1 4
// Z 1 H -
void CPU::op_dec_b() {
    this->idu.decrement_r8(this->registers.B);

    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x06
// LD B, n8
// 2 8
// - - - -
void CPU::op_ld_b_n8() {
    uint8_t n8 = this->memory.read_byte(static_cast<uint16_t>(this->registers.PC + 1));
    this->registers.B = n8;

    this->registers.PC += 2;
    this->tstates += 8;
}

// 0x07
// RLCA
// 1 4
// 0 0 0 C
void CPU::op_rlca() {
    this->bmi.rlca();

    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x08
// LD [a16], SP
// 3 20
// - - - -
void CPU::op_ld_a16m_sp() {
    uint16_t a16 = this->memory.read_word(static_cast<uint16_t>(this->registers.PC + 1));
    this->memory.write_word(a16, this->registers.SP);

    this->registers.PC += 3;
    this->tstates += 20;
}

// 0x09
// ADD HL, BC
// 1 8
// - 0 H C
void CPU::op_add_hl_bc() {
    this->alu.add_u16(this->registers.get_bc());

    this->registers.PC += 1;
    this->tstates += 8;
}

// 0x0a
// LD A, [BC]
// 1 8
// - - - -
void CPU::op_ld_a_bcm() {
    this->registers.A = this->memory.read_byte(this->registers.get_bc());

    this->registers.PC += 1;
    this->tstates += 8;
}

// 0x0b
// DEC BC
// 1 8
// - - - -
void CPU::op_dec_bc() {
    this->registers.set_bc(static_cast<uint16_t>(this->registers.get_bc() - 1));

    this->registers.PC += 1;
    this->tstates += 8;
}

// 0x0c
// INC C
// 1 4
// Z 0 H -
void CPU::op_inc_c() {
    this->idu.increment_r8(this->registers.C);

    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x0d
// DEC C
// 1 4
// Z 1 H -
void CPU::op_dec_c() {
    this->idu.decrement_r8(this->registers.C);

    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x0e
// LD C, n8
// 2 8
// - - - -
void CPU::op_ld_c_n8() {
    uint8_t n8 = this->memory.read_byte(static_cast<uint16_t>(this->registers.PC + 1));
    this->registers.C = n8;

    this->registers.PC += 2;
    this->tstates += 8;
}

// 0x0f
// RRCA
// 1 4
// 0 0 0 C
void CPU::op_rrca() {
    this->bmi.rrca();

    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x10
// STOP n8
// 2 4
// - - - -
void CPU::op_stop_n8() {
    this->stopped = true;
    this->memory.write_byte(0xFF04, 0x00); // reset DIV register

    this->registers.PC += 2;
    this->tstates += 4;
}

// 0x11
// LD DE, n16
// 3 12
// - - - -
void CPU::op_ld_de_n16() {
    uint16_t n16 = this->memory.read_word(static_cast<uint16_t>(this->registers.PC + 1));
    this->registers.set_de(n16);

    this->registers.PC += 3;
    this->tstates += 12;
}

// 0x12
// LD [DE], A
// 1 8
// - - - -
void CPU::op_ld_dem_a() {
    this->memory.write_byte(this->registers.get_de(), this->registers.A);

    this->registers.PC += 1;
    this->tstates += 8;
}

// 0x13
// INC DE
// 1 8
// - - - -
void CPU::op_inc_de() {
    this->registers.set_de(static_cast<uint16_t>(this->registers.get_de() + 1));

    this->registers.PC += 1;
    this->tstates += 8;
}

// 0x14
// INC D
// 1 4
// Z 0 H -
void CPU::op_inc_d() {
    this->idu.increment_r8(this->registers.D);

    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x15
// DEC D
// 1 4
// Z 1 H -
void CPU::op_dec_d() {
    this->idu.decrement_r8(this->registers.D);

    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x16
// LD D, n8
// 2 8
// - - - -
void CPU::op_ld_d_n8() {
    uint8_t n8 = this->memory.read_byte(static_cast<uint16_t>(this->registers.PC + 1));
    this->registers.D = n8;

    this->registers.PC += 2;
    this->tstates += 8;
}

// 0x17
// RLA
// 1 4
// 0 0 0 C
void CPU::op_rla() {
    this->bmi.rla();

    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x18
// JR e8
// 2 12
// - - - -
void CPU::op_jr_e8() {
    int8_t offset = static_cast<int8_t>(this->memory.read_byte(static_cast<uint16_t>(this->registers.PC + 1)));
    this->registers.PC += offset;

    this->tstates += 12;
}

// 0x19
// ADD HL, DE
// 1 8
// - 0 H C
void CPU::op_add_hl_de() {
    this->alu.add_u16(this->registers.get_de());

    this->registers.PC += 1;
    this->tstates += 8;
}

// 0x1a
// LD A, [DE]
// 1 8
// - - - -
void CPU::op_ld_a_dem() {
    this->registers.A = this->memory.read_byte(this->registers.get_de());

    this->registers.PC += 1;
    this->tstates += 8;
}

// 0x1b
// DEC DE
// 1 8
// - - - -
void CPU::op_dec_de() {
    this->registers.set_de(static_cast<uint16_t>(this->registers.get_de() - 1));

    this->registers.PC += 1;
    this->tstates += 8;
}

// 0x1c
// INC E
// 1 4
// Z 0 H -
void CPU::op_inc_e() {
    this->idu.increment_r8(this->registers.E);

    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x1d
// DEC E
// 1 4
// Z 1 H -
void CPU::op_dec_e() {
    this->idu.decrement_r8(this->registers.E);

    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x1e
// LD E, n8
// 2 8
// - - - -
void CPU::op_ld_e_n8() {
    uint8_t n8 = this->memory.read_byte(static_cast<uint16_t>(this->registers.PC + 1));
    this->registers.E = n8;

    this->registers.PC += 2;
    this->tstates += 8;
}

// 0x1f
// RRA
// 1 4
// 0 0 0 C
void CPU::op_rra() {
    this->bmi.rra();

    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x20
// JR NZ, e8
// 2 12/8
// - - - -
void CPU::op_jr_nz_e8() {
    if (!this->registers.get_flag_z()) {
        int8_t offset = static_cast<int8_t>(this->memory.read_byte(static_cast<uint16_t>(this->registers.PC + 1)));
        this->registers.PC += offset;
        this->tstates += 12;
        return;
    }
    this->tstates += 8;
}

// 0x21
// LD HL, n16
// 3 12
// - - - -
void CPU::op_ld_hl_a16() {
    uint16_t n16 = this->memory.read_word(static_cast<uint16_t>(this->registers.PC + 1));
    this->registers.set_hl(n16);

    this->registers.PC += 3;
    this->tstates += 12;
}

// 0x22
// LD [HL+], A
// 1 8
// - - - -
void CPU::op_ld_hlim_a() {
    this->memory.write_byte(this->registers.get_hl(), this->registers.A);
    this->registers.set_hl(static_cast<uint16_t>(this->registers.get_hl() + 1));

    this->registers.PC += 1;
    this->tstates += 8;
}

// 0x23
// INC HL
// 1 8
// - - - -
void CPU::op_inc_hl() {
    this->registers.set_hl(static_cast<uint16_t>(this->registers.get_hl() + 1));

    this->registers.PC += 1;
    this->tstates += 8;
}

// 0x24
// INC H
// 1 4
// Z 0 H -
void CPU::op_inc_h() {
    this->idu.increment_r8(this->registers.H);

    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x25
// DEC H
// 1 4
// Z 1 H -
void CPU::op_dec_h() {
    this->idu.decrement_r8(this->registers.H);

    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x26
// LD H, n8
// 2 8
// - - - -
void CPU::op_ld_h_n8() {
    uint8_t n8 = this->memory.read_byte(static_cast<uint16_t>(this->registers.PC + 1));
    this->registers.H = n8;

    this->registers.PC += 2;
    this->tstates += 8;
}

// 0x27
// DAA
// 1 4
// Z - H C
void CPU::op_daa() {
    uint8_t a = this->registers.A;
    uint8_t adjust = 0;
    bool carry = this->registers.get_flag_c();
    bool subtract = this->registers.get_flag_n();

    if (subtract) {
        if (this->registers.get_flag_h()) adjust |= 0x06;
        if (carry) adjust |= 0x60;
        a = static_cast<uint8_t>(a - adjust);
        // carry flag unaffected (remains as before)
    } else {
        if (this->registers.get_flag_h() || (a & 0x0F) > 9) adjust |= 0x06;
        if (carry || a > 0x99) {
            adjust |= 0x60;
            carry = true;
        } else {
            carry = false;
        }
        a = static_cast<uint8_t>(a + adjust);
    }

    this->registers.A = a;
    this->registers.set_flag_z(a == 0);
    this->registers.set_flag_n(subtract);
    this->registers.set_flag_h(false);
    this->registers.set_flag_c(carry);

    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x28
// JR Z, e8
// 2 12/8
// - - - -
void CPU::op_jr_z_e8() {
    if (this->registers.get_flag_z()) {
        int8_t offset = static_cast<int8_t>(this->memory.read_byte(static_cast<uint16_t>(this->registers.PC + 1)));
        this->registers.PC += offset;
        this->tstates += 12;
        return;
    }
    this->tstates += 8;
}

// 0x29
// ADD HL, HL
// 1 8
// - 0 H C
void CPU::op_add_hl_hl() {
    this->alu.add_u16(this->registers.get_hl());

    this->registers.PC += 1;
    this->tstates += 8;
}

// 0x2a
// LD A, [HL+]
// 1 8
// - - - -
void CPU::op_ld_a_hlim() {
    this->registers.A = this->memory.read_byte(this->registers.get_hl());
    this->registers.set_hl(static_cast<uint16_t>(this->registers.get_hl() + 1));

    this->registers.PC += 1;
    this->tstates += 8;
}

// 0x2b
// DEC HL
// 1 8
// - - - -
void CPU::op_dec_hl() {
    this->registers.set_hl(static_cast<uint16_t>(this->registers.get_hl() - 1));
    this->registers.PC += 1;
    this->tstates += 8;
}

// 0x2c
// INC L
// 1 4
// Z 0 H -
void CPU::op_inc_l() {
    this->idu.increment_r8(this->registers.L);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x2d
// DEC L
// 1 4
// Z 1 H -
void CPU::op_dec_l() {
    this->idu.decrement_r8(this->registers.L);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x2e
// LD L, n8
// 2 8
// - - - -
void CPU::op_ld_l_n8() {
    uint8_t n8 = this->memory.read_byte(static_cast<uint16_t>(this->registers.PC + 1));
    this->registers.L = n8;
    this->registers.PC += 2;
    this->tstates += 8;
}

// 0x2f
// CPL
// 1 4
// - 1 1 -
void CPU::op_cpl() {
    this->registers.A = ~this->registers.A;

    // Flags
    this->registers.set_flag_n(true);
    this->registers.set_flag_h(true);

    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x30
// JR NC, e8
// 2 12/8
// - - - -
void CPU::op_jr_nc_e8() {
    if (!this->registers.get_flag_c()) {
        int8_t offset = static_cast<int8_t>(this->memory.read_byte(static_cast<uint16_t>(this->registers.PC + 1)));
        this->registers.PC += offset;
        this->tstates += 12;
        return;
    }
    this->registers.PC += 2;
    this->tstates += 8;
}

// 0x31
// LD SP, n16
// 3 12
// - - - -
void CPU::op_ld_sp_a16() {
    uint16_t n16 = this->memory.read_word(static_cast<uint16_t>(this->registers.PC + 1));
    this->registers.SP = n16;
    this->registers.PC += 3;
    this->tstates += 12;
}

// 0x32
// LD [HL-], A
// 1 8
// - - - -
void CPU::op_ld_hldm_a() {
    this->memory.write_byte(this->registers.get_hl(), this->registers.A);
    this->registers.set_hl(static_cast<uint16_t>(this->registers.get_hl() - 1));
    this->registers.PC += 1;
    this->tstates += 8;
}

// 0x33
// INC SP
// 1 8
// - - - -
void CPU::op_inc_sp() {
    this->registers.SP = static_cast<uint16_t>(this->registers.SP + 1);
    this->registers.PC += 1;
    this->tstates += 8;
}

// 0x34
// INC [HL]
// 1 12
// Z 0 H -
void CPU::op_inc_hlm() {
    this->idu.increment_mem8(this->registers.get_hl());
    this->registers.PC += 1;
    this->tstates += 12;
}

// 0x35
// DEC [HL]
// 1 12
// Z 1 H -
void CPU::op_dec_hlm() {
    this->idu.decrement_mem8(this->registers.get_hl());
    this->registers.PC += 1;
    this->tstates += 12;
}

// 0x36
// LD [HL], n8
// 2 12
// - - - -
void CPU::op_ld_hlm_n8() {
    uint8_t n8 = this->memory.read_byte(static_cast<uint16_t>(this->registers.PC + 1));
    this->memory.write_byte(this->registers.get_hl(), n8);
    this->registers.PC += 2;
    this->tstates += 12;
}

// 0x37
// SCF
// 1 4
// - 0 0 1
void CPU::op_scf() {
    this->registers.set_flag_n(false);
    this->registers.set_flag_h(false);
    this->registers.set_flag_c(true);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x38
// JR C, e8
// 2 12/8
// - - - -
void CPU::op_jr_c_e8() {
    if (this->registers.get_flag_c()) {
        int8_t offset = static_cast<int8_t>(this->memory.read_byte(static_cast<uint16_t>(this->registers.PC + 1)));
        this->registers.PC += offset;
        this->tstates += 12;
        return;
    }
    this->registers.PC += 2;
    this->tstates += 8;
}

// 0x39
// ADD HL, SP
// 1 8
// - 0 H C
void CPU::op_add_hl_sp() {
    this->alu.add_u16(this->registers.SP);
    this->registers.PC += 1;
    this->tstates += 8;
}

// 0x3a
// LD A, [HL-]
// 1 8
// - - - -
void CPU::op_ld_a_hldm() {
    this->registers.A = this->memory.read_byte(this->registers.get_hl());
    this->registers.set_hl(static_cast<uint16_t>(this->registers.get_hl() - 1));
    this->registers.PC += 1;
    this->tstates += 8;
}

// 0x3b
// DEC SP
// 1 8
// - - - -
void CPU::op_dec_sp() {
    this->registers.SP = static_cast<uint16_t>(this->registers.SP - 1);
    this->registers.PC += 1;
    this->tstates += 8;
}

// 0x3c
// INC A
// 1 4
// Z 0 H -
void CPU::op_inc_a() {
    this->idu.increment_r8(this->registers.A);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x3d
// DEC A
// 1 4
// Z 1 H -
void CPU::op_dec_a() {
    this->idu.decrement_r8(this->registers.A);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x3e
// LD A, n8
// 2 8
// - - - -
void CPU::op_ld_a_n8() {
    uint8_t n8 = this->memory.read_byte(static_cast<uint16_t>(this->registers.PC + 1));
    this->registers.A = n8;
    this->registers.PC += 2;
    this->tstates += 8;
}

// 0x3f
// CCF
// 1 4
// - 0 0 C
void CPU::op_ccf() {
    this->registers.set_flag_n(false);
    this->registers.set_flag_h(false);
    this->registers.set_flag_c(!this->registers.get_flag_c());
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x40
// LD B, B
// 1 4
// - - - -
void CPU::op_ld_b_b() {
    this->registers.B = this->registers.B;
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x41
// LD B, C
// 1 4
// - - - -
void CPU::op_ld_b_c() {
    this->registers.B = this->registers.C;
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x42
// LD B, D
// 1 4
// - - - -
void CPU::op_ld_b_d() {
    this->registers.B = this->registers.D;
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x43
// LD B, E
// 1 4
// - - - -
void CPU::op_ld_b_e() {
    this->registers.B = this->registers.E;
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x44
// LD B, H
// 1 4
// - - - -
void CPU::op_ld_b_h() {
    this->registers.B = this->registers.H;
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x45
// LD B, L
// 1 4
// - - - -
void CPU::op_ld_b_l() {
    this->registers.B = this->registers.L;
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x46
// LD B, [HL]
// 1 8
// - - - -
void CPU::op_ld_b_hlm() {
    this->registers.B = this->memory.read_byte(this->registers.get_hl());
    this->registers.PC += 1;
    this->tstates += 8;
}

// 0x47
// LD B, A
// 1 4
// - - - -
void CPU::op_ld_b_a() {
    this->registers.B = this->registers.A;
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x48
// LD C, B
// 1 4
// - - - -
void CPU::op_ld_c_b() {
    this->registers.C = this->registers.B;
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x49
// LD C, C
// 1 4
// - - - -
void CPU::op_ld_c_c() {
    this->registers.C = this->registers.C;
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x4a
// LD C, D
// 1 4
// - - - -
void CPU::op_ld_c_d() {
    this->registers.C = this->registers.D;
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x4b
// LD C, E
// 1 4
// - - - -
void CPU::op_ld_c_e() {
    this->registers.C = this->registers.E;
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x4c
// LD C, H
// 1 4
// - - - -
void CPU::op_ld_c_h() {
    this->registers.C = this->registers.H;
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x4d
// LD C, L
// 1 4
// - - - -
void CPU::op_ld_c_l() {
    this->registers.C = this->registers.L;
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x4e
// LD C, [HL]
// 1 8
// - - - -
void CPU::op_ld_c_hlm() {
    this->registers.C = this->memory.read_byte(this->registers.get_hl());
    this->registers.PC += 1;
    this->tstates += 8;
}

// 0x4f
// LD C, A
// 1 4
// - - - -
void CPU::op_ld_c_a() {
    this->registers.C = this->registers.A;
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x50
// LD D, B
// 1 4
// - - - -
void CPU::op_ld_d_b() {
    this->registers.D = this->registers.B;
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x51
// LD D, C
// 1 4
// - - - -
void CPU::op_ld_d_c() {
    this->registers.D = this->registers.C;
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x52
// LD D, D
// 1 4
// - - - -
void CPU::op_ld_d_d() {
    this->registers.D = this->registers.D;
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x53
// LD D, E
// 1 4
// - - - -
void CPU::op_ld_d_e() {
    this->registers.D = this->registers.E;
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x54
// LD D, H
// 1 4
// - - - -
void CPU::op_ld_d_h() {
    this->registers.D = this->registers.H;
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x55
// LD D, L
// 1 4
// - - - -
void CPU::op_ld_d_l() {
    this->registers.D = this->registers.L;
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x56
// LD D, [HL]
// 1 8
// - - - -
void CPU::op_ld_d_hlm() {
    this->registers.D = this->memory.read_byte(this->registers.get_hl());
    this->registers.PC += 1;
    this->tstates += 8;
}

// 0x57
// LD D, A
// 1 4
// - - - -
void CPU::op_ld_d_a() {
    this->registers.D = this->registers.A;
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x58
// LD E, B
// 1 4
// - - - -
void CPU::op_ld_e_b() {
    this->registers.E = this->registers.B;
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x59
// LD E, C
// 1 4
// - - - -
void CPU::op_ld_e_c() {
    this->registers.E = this->registers.C;
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x5a
// LD E, D
// 1 4
// - - - -
void CPU::op_ld_e_d() {
    this->registers.E = this->registers.D;
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x5b
// LD E, E
// 1 4
// - - - -
void CPU::op_ld_e_e() {
    this->registers.E = this->registers.E;
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x5c
// LD E, H
// 1 4
// - - - -
void CPU::op_ld_e_h() {
    this->registers.E = this->registers.H;
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x5d
// LD E, L
// 1 4
// - - - -
void CPU::op_ld_e_l() {
    this->registers.E = this->registers.L;
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x5e
// LD E, [HL]
// 1 8
// - - - -
void CPU::op_ld_e_hlm() {
    this->registers.E = this->memory.read_byte(this->registers.get_hl());
    this->registers.PC += 1;
    this->tstates += 8;
}

// 0x5f
// LD E, A
// 1 4
// - - - -
void CPU::op_ld_e_a() {
    this->registers.E = this->registers.A;
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x60
// LD H, B
// 1 4
// - - - -
void CPU::op_ld_h_b() {
    this->registers.H = this->registers.B;
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x61
// LD H, C
// 1 4
// - - - -
void CPU::op_ld_h_c() {
    this->registers.H = this->registers.C;
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x62
// LD H, D
// 1 4
// - - - -
void CPU::op_ld_h_d() {
    this->registers.H = this->registers.D;
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x63
// LD H, E
// 1 4
// - - - -
void CPU::op_ld_h_e() {
    this->registers.H = this->registers.E;
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x64
// LD H, H
// 1 4
// - - - -
void CPU::op_ld_h_h() {
    this->registers.H = this->registers.H;
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x65
// LD H, L
// 1 4
// - - - -
void CPU::op_ld_h_l() {
    this->registers.H = this->registers.L;
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x66
// LD H, [HL]
// 1 8
// - - - -
void CPU::op_ld_h_hlm() {
    this->registers.H = this->memory.read_byte(this->registers.get_hl());
    this->registers.PC += 1;
    this->tstates += 8;
}

// 0x67
// LD H, A
// 1 4
// - - - -
void CPU::op_ld_h_a() {
    this->registers.H = this->registers.A;
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x68
// LD L, B
// 1 4
// - - - -
void CPU::op_ld_l_b() {
    this->registers.L = this->registers.B;
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x69
// LD L, C
// 1 4
// - - - -
void CPU::op_ld_l_c() {
    this->registers.L = this->registers.C;
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x6a
// LD L, D
// 1 4
// - - - -
void CPU::op_ld_l_d() {
    this->registers.L = this->registers.D;
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x6b
// LD L, E
// 1 4
// - - - -
void CPU::op_ld_l_e() {
    this->registers.L = this->registers.E;
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x6c
// LD L, H
// 1 4
// - - - -
void CPU::op_ld_l_h() {
    this->registers.L = this->registers.H;
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x6d
// LD L, L
// 1 4
// - - - -
void CPU::op_ld_l_l() {
    this->registers.L = this->registers.L;
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x6e
// LD L, [HL]
// 1 8
// - - - -
void CPU::op_ld_l_hlm() {
    this->registers.L = this->memory.read_byte(this->registers.get_hl());
    this->registers.PC += 1;
    this->tstates += 8;
}

// 0x6f
// LD L, A
// 1 4
// - - - -
void CPU::op_ld_l_a() {
    this->registers.L = this->registers.A;
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x70
// LD [HL], B
// 1 8
// - - - -
void CPU::op_ld_hlm_b() {
    this->memory.write_byte(this->registers.get_hl(), this->registers.B);
    this->registers.PC += 1;
    this->tstates += 8;
}

// 0x71
// LD [HL], C
// 1 8
// - - - -
void CPU::op_ld_hlm_c() {
    this->memory.write_byte(this->registers.get_hl(), this->registers.C);
    this->registers.PC += 1;
    this->tstates += 8;
}

// 0x72
// LD [HL], D
// 1 8
// - - - -
void CPU::op_ld_hlm_d() {
    this->memory.write_byte(this->registers.get_hl(), this->registers.D);
    this->registers.PC += 1;
    this->tstates += 8;
}

// 0x73
// LD [HL], E
// 1 8
// - - - -
void CPU::op_ld_hlm_e() {
    this->memory.write_byte(this->registers.get_hl(), this->registers.E);
    this->registers.PC += 1;
    this->tstates += 8;
}

// 0x74
// LD [HL], H
// 1 8
// - - - -
void CPU::op_ld_hlm_h() {
    this->memory.write_byte(this->registers.get_hl(), this->registers.H);
    this->registers.PC += 1;
    this->tstates += 8;
}

// 0x75
// LD [HL], L
// 1 8
// - - - -
void CPU::op_ld_hlm_l() {
    this->memory.write_byte(this->registers.get_hl(), this->registers.L);
    this->registers.PC += 1;
    this->tstates += 8;
}

// 0x76
// HALT
// 1 4
// - - - -
void CPU::op_halt() {
    this->halted = true;
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x77
// LD [HL], A
// 1 8
// - - - -
void CPU::op_ld_hlm_a() {
    this->memory.write_byte(this->registers.get_hl(), this->registers.A);
    this->registers.PC += 1;
    this->tstates += 8;
}

// 0x78
// LD A, B
// 1 4
// - - - -
void CPU::op_ld_a_b() {
    this->registers.A = this->registers.B;
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x79
// LD A, C
// 1 4
// - - - -
void CPU::op_ld_a_c() {
    this->registers.A = this->registers.C;
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x7a
// LD A, D
// 1 4
// - - - -
void CPU::op_ld_a_d() {
    this->registers.A = this->registers.D;
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x7b
// LD A, E
// 1 4
// - - - -
void CPU::op_ld_a_e() {
    this->registers.A = this->registers.E;
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x7c
// LD A, H
// 1 4
// - - - -
void CPU::op_ld_a_h() {
    this->registers.A = this->registers.H;
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x7d
// LD A, L
// 1 4
// - - - -
void CPU::op_ld_a_l() {
    this->registers.A = this->registers.L;
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x7e
// LD A, [HL]
// 1 8
// - - - -
void CPU::op_ld_a_hlm() {
    this->registers.A = this->memory.read_byte(this->registers.get_hl());
    this->registers.PC += 1;
    this->tstates += 8;
}

// 0x7f
// LD A, A
// 1 4
// - - - -
void CPU::op_ld_a_a() {
    this->registers.A = this->registers.A;
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x80
// ADD A, B
// 1 4
// Z 0 H C
void CPU::op_add_a_b() {
    this->alu.add_u8(this->registers.B);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x81
// ADD A, C
// 1 4
// Z 0 H C
void CPU::op_add_a_c() {
    this->alu.add_u8(this->registers.C);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x82
// ADD A, D
// 1 4
// Z 0 H C
void CPU::op_add_a_d() {
    this->alu.add_u8(this->registers.D);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x83
// ADD A, E
// 1 4
// Z 0 H C
void CPU::op_add_a_e() {
    this->alu.add_u8(this->registers.E);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x84
// ADD A, H
// 1 4
// Z 0 H C
void CPU::op_add_a_h() {
    this->alu.add_u8(this->registers.H);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x85
// ADD A, L
// 1 4
// Z 0 H C
void CPU::op_add_a_l() {
    this->alu.add_u8(this->registers.L);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x86
// ADD A, [HL]
// 1 8
// Z 0 H C
void CPU::op_add_a_hlm() {
    uint8_t value = this->memory.read_byte(this->registers.get_hl());
    this->alu.add_u8(value);
    this->registers.PC += 1;
    this->tstates += 8;
}

// 0x87
// ADD A, A
// 1 4
// Z 0 H C
void CPU::op_add_a_a() {
    this->alu.add_u8(this->registers.A);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x88
// ADC A, B
// 1 4
// Z 0 H C
void CPU::op_adc_a_b() {
    this->alu.adc_u8(this->registers.B);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x89
// ADC A, C
// 1 4
// Z 0 H C
void CPU::op_adc_a_c() {
    this->alu.adc_u8(this->registers.C);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x8a
// ADC A, D
// 1 4
// Z 0 H C
void CPU::op_adc_a_d() {
    this->alu.adc_u8(this->registers.D);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x8b
// ADC A, E
// 1 4
// Z 0 H C
void CPU::op_adc_a_e() {
    this->alu.adc_u8(this->registers.E);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x8c
// ADC A, H
// 1 4
// Z 0 H C
void CPU::op_adc_a_h() {
    this->alu.adc_u8(this->registers.H);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x8d
// ADC A, L
// 1 4
// Z 0 H C
void CPU::op_adc_a_l() {
    this->alu.adc_u8(this->registers.L);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x8e
// ADC A, [HL]
// 1 8
// Z 0 H C
void CPU::op_adc_a_hlm() {
    uint8_t value = this->memory.read_byte(this->registers.get_hl());
    this->alu.adc_u8(value);
    this->registers.PC += 1;
    this->tstates += 8;
}

// 0x8f
// ADC A, A
// 1 4
// Z 0 H C
void CPU::op_adc_a_a() {
    this->alu.adc_u8(this->registers.A);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x90
// SUB A, B
// 1 4
// Z 1 H C
void CPU::op_sub_a_b() {
    this->alu.sub_u8(this->registers.B);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x91
// SUB A, C
// 1 4
// Z 1 H C
void CPU::op_sub_a_c() {
    this->alu.sub_u8(this->registers.C);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x92
// SUB A, D
// 1 4
// Z 1 H C
void CPU::op_sub_a_d() {
    this->alu.sub_u8(this->registers.D);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x93
// SUB A, E
// 1 4
// Z 1 H C
void CPU::op_sub_a_e() {
    this->alu.sub_u8(this->registers.E);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x94
// SUB A, H
// 1 4
// Z 1 H C
void CPU::op_sub_a_h() {
    this->alu.sub_u8(this->registers.H);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x95
// SUB A, L
// 1 4
// Z 1 H C
void CPU::op_sub_a_l() {
    this->alu.sub_u8(this->registers.L);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x96
// SUB A, [HL]
// 1 8
// Z 1 H C
void CPU::op_sub_a_hlm() {
    uint8_t value = this->memory.read_byte(this->registers.get_hl());
    this->alu.sub_u8(value);
    this->registers.PC += 1;
    this->tstates += 8;
}

// 0x97
// SUB A, A
// 1 4
// Z 1 H C
void CPU::op_sub_a_a() {
    this->alu.sub_u8(this->registers.A);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x98
// SBC A, B
// 1 4
// Z 1 H C
void CPU::op_sbc_a_b() {
    this->alu.sbc_u8(this->registers.B);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x99
// SBC A, C
// 1 4
// Z 1 H C
void CPU::op_sbc_a_c() {
    this->alu.sbc_u8(this->registers.C);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x9a
// SBC A, D
// 1 4
// Z 1 H C
void CPU::op_sbc_a_d() {
    this->alu.sbc_u8(this->registers.D);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x9b
// SBC A, E
// 1 4
// Z 1 H C
void CPU::op_sbc_a_e() {
    this->alu.sbc_u8(this->registers.E);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x9c
// SBC A, H
// 1 4
// Z 1 H C
void CPU::op_sbc_a_h() {
    this->alu.sbc_u8(this->registers.H);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x9d
// SBC A, L
// 1 4
// Z 1 H C
void CPU::op_sbc_a_l() {
    this->alu.sbc_u8(this->registers.L);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0x9e
// SBC A, [HL]
// 1 8
// Z 1 H C
void CPU::op_sbc_a_hlm() {
    uint8_t value = this->memory.read_byte(this->registers.get_hl());
    this->alu.sbc_u8(value);
    this->registers.PC += 1;
    this->tstates += 8;
}

// 0x9f
// SBC A, A
// 1 4
// Z 1 H C
void CPU::op_sbc_a_a() {
    this->alu.sbc_u8(this->registers.A);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0xa0
// AND A, B
// 1 4
// Z 0 1 0
void CPU::op_and_a_b() {
    this->alu.and_u8(this->registers.B);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0xa1
// AND A, C
// 1 4
// Z 0 1 0
void CPU::op_and_a_c() {
    this->alu.and_u8(this->registers.C);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0xa2
// AND A, D
// 1 4
// Z 0 1 0
void CPU::op_and_a_d() {
    this->alu.and_u8(this->registers.D);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0xa3
// AND A, E
// 1 4
// Z 0 1 0
void CPU::op_and_a_e() {
    this->alu.and_u8(this->registers.E);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0xa4
// AND A, H
// 1 4
// Z 0 1 0
void CPU::op_and_a_h() {
    this->alu.and_u8(this->registers.H);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0xa5
// AND A, L
// 1 4
// Z 0 1 0
void CPU::op_and_a_l() {
    this->alu.and_u8(this->registers.L);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0xa6
// AND A, [HL]
// 1 8
// Z 0 1 0
void CPU::op_and_a_hlm() {
    uint8_t value = this->memory.read_byte(this->registers.get_hl());
    this->alu.and_u8(value);
    this->registers.PC += 1;
    this->tstates += 8;
}

// 0xa7
// AND A, A
// 1 4
// Z 0 1 0
void CPU::op_and_a_a() {
    this->alu.and_u8(this->registers.A);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0xa8
// XOR A, B
// 1 4
// Z 0 0 0
void CPU::op_xor_a_b() {
    this->alu.xor_u8(this->registers.B);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0xa9
// XOR A, C
// 1 4
// Z 0 0 0
void CPU::op_xor_a_c() {
    this->alu.xor_u8(this->registers.C);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0xaa
// XOR A, D
// 1 4
// Z 0 0 0
void CPU::op_xor_a_d() {
    this->alu.xor_u8(this->registers.D);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0xab
// XOR A, E
// 1 4
// Z 0 0 0
void CPU::op_xor_a_e() {
    this->alu.xor_u8(this->registers.E);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0xac
// XOR A, H
// 1 4
// Z 0 0 0
void CPU::op_xor_a_h() {
    this->alu.xor_u8(this->registers.H);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0xad
// XOR A, L
// 1 4
// Z 0 0 0
void CPU::op_xor_a_l() {
    this->alu.xor_u8(this->registers.L);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0xae
// XOR A, [HL]
// 1 8
// Z 0 0 0
void CPU::op_xor_a_hlm() {
    uint8_t value = this->memory.read_byte(this->registers.get_hl());
    this->alu.xor_u8(value);
    this->registers.PC += 1;
    this->tstates += 8;
}

// 0xaf
// XOR A, A
// 1 4
// Z 0 0 0
void CPU::op_xor_a_a() {
    this->alu.xor_u8(this->registers.A);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0xb0
// OR A, B
// 1 4
// Z 0 0 0
void CPU::op_or_a_b() {
    this->alu.or_u8(this->registers.B);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0xb1
// OR A, C
// 1 4
// Z 0 0 0
void CPU::op_or_a_c() {
    this->alu.or_u8(this->registers.C);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0xb2
// OR A, D
// 1 4
// Z 0 0 0
void CPU::op_or_a_d() {
    this->alu.or_u8(this->registers.D);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0xb3
// OR A, E
// 1 4
// Z 0 0 0
void CPU::op_or_a_e() {
    this->alu.or_u8(this->registers.E);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0xb4
// OR A, H
// 1 4
// Z 0 0 0
void CPU::op_or_a_h() {
    this->alu.or_u8(this->registers.H);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0xb5
// OR A, L
// 1 4
// Z 0 0 0
void CPU::op_or_a_l() {
    this->alu.or_u8(this->registers.L);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0xb6
// OR A, [HL]
// 1 8
// Z 0 0 0
void CPU::op_or_a_hlm() {
    uint8_t value = this->memory.read_byte(this->registers.get_hl());
    this->alu.or_u8(value);
    this->registers.PC += 1;
    this->tstates += 8;
}

// 0xb7
// OR A, A
// 1 4
// Z 0 0 0
void CPU::op_or_a_a() {
    this->alu.or_u8(this->registers.A);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0xb8
// CP A, B
// 1 4
// Z 1 H C
void CPU::op_cp_a_b() {
    this->alu.cp_u8(this->registers.B);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0xb9
// CP A, C
// 1 4
// Z 1 H C
void CPU::op_cp_a_c() {
    this->alu.cp_u8(this->registers.C);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0xba
// CP A, D
// 1 4
// Z 1 H C
void CPU::op_cp_a_d() {
    this->alu.cp_u8(this->registers.D);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0xbb
// CP A, E
// 1 4
// Z 1 H C
void CPU::op_cp_a_e() {
    this->alu.cp_u8(this->registers.E);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0xbc
// CP A, H
// 1 4
// Z 1 H C
void CPU::op_cp_a_h() {
    this->alu.cp_u8(this->registers.H);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0xbd
// CP A, L
// 1 4
// Z 1 H C
void CPU::op_cp_a_l() {
    this->alu.cp_u8(this->registers.L);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0xbe
// CP A, [HL]
// 1 8
// Z 1 H C
void CPU::op_cp_a_hlm() {
    uint8_t value = this->memory.read_byte(this->registers.get_hl());
    this->alu.cp_u8(value);
    this->registers.PC += 1;
    this->tstates += 8;
}

// 0xbf
// CP A, A
// 1 4
// 1 1 0 0
void CPU::op_cp_a_a() {
    this->alu.cp_u8(this->registers.A);
    this->registers.set_flag_z(true);
    this->registers.set_flag_n(true);
    this->registers.set_flag_h(false);
    this->registers.set_flag_c(false);
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0xc0
// RET NZ
// 1 20/8
// - - - -
void CPU::op_ret_nz() {
    if (!this->registers.get_flag_z()) {
        uint16_t address = this->stack.pop_word();
        this->registers.PC = address;
        this->tstates += 20;
        return;
    }
    this->registers.PC += 1;
    this->tstates += 8;
}

// 0xc1
// POP BC
// 1 12
// - - - -
void CPU::op_pop_bc() {
    uint16_t word = this->stack.pop_word();
    this->registers.set_bc(word);
    this->registers.PC += 1;
    this->tstates += 12;
}

// 0xc2
// JP NZ, a16
// 3 16/12
// - - - -
void CPU::op_jp_nz_a16() {
    uint16_t address = this->memory.read_word(static_cast<uint16_t>(this->registers.PC + 1));
    if (!this->registers.get_flag_z()) {
        this->registers.PC = address;
        this->tstates += 16;
        return;
    }
    this->registers.PC += 3;
    this->tstates += 12;
}

// 0xc3
// JP a16
// 3 16
// - - - -
void CPU::op_jp_a16() {
    uint16_t address = this->memory.read_word(static_cast<uint16_t>(this->registers.PC + 1));
    this->registers.PC = address;
    this->tstates += 16;
}

// 0xc4
// CALL NZ, a16
// 3 24/12
// - - - -
void CPU::op_call_nz_a16() {
    uint16_t address = this->memory.read_word(static_cast<uint16_t>(this->registers.PC + 1));

    if (!this->registers.get_flag_z()) {
        uint16_t return_address = static_cast<uint16_t>(this->registers.PC + 3);
        this->stack.push_word(return_address);

        this->registers.PC = address;
        this->tstates += 24;
        return;
    }
    this->registers.PC += 3;
    this->tstates += 12;
}

// 0xc5
// PUSH BC
// 1 16
// - - - -
void CPU::op_push_bc() {
    this->stack.push_word(this->registers.get_bc());
    this->registers.PC += 1;
    this->tstates += 16;
}

// 0xc6
// ADD A, n8
// 2 8
// Z 0 H C
void CPU::op_add_a_n8() {
    uint8_t value = this->memory.read_byte(static_cast<uint16_t>(this->registers.PC + 1));
    this->alu.add_u8(value);
    this->registers.PC += 2;
    this->tstates += 8;
}

// 0xc7
// RST $00
// 1 16
// - - - -
void CPU::op_rst_00() {
    this->stack.push_word(static_cast<uint16_t>(this->registers.PC + 1));
    this->registers.PC = 0x0000;
    this->tstates += 16;
}

// 0xc8
// RET Z
// 1 20/8
// - - - -
void CPU::op_ret_z() {
    if (this->registers.get_flag_z()) {
        uint16_t address = this->stack.pop_word();
        this->registers.PC = address;
        this->tstates += 20;
        return;
    }
    this->registers.PC += 1;
    this->tstates += 8;
}

// 0xc9
// RET
// 1 16
// - - - -
void CPU::op_ret() {
    uint16_t address = this->stack.pop_word();
    this->registers.PC = address;
    this->tstates += 16;
}

// 0xca
// JP Z, a16
// 3 16/12
// - - - -
void CPU::op_jp_z_a16() {
    uint16_t address = this->memory.read_word(static_cast<uint16_t>(this->registers.PC + 1));
    if (this->registers.get_flag_z()) {
        this->registers.PC = address;
        this->tstates += 16;
        return;
    }
    this->registers.PC += 3;
    this->tstates += 12;
}

// 0xcb
// PREFIX
// 1 4
// - - - -
void CPU::op_prefix() {
    uint8_t cb = this->memory.read_byte(static_cast<uint16_t>(this->registers.PC + 1));
    this->exec_cb(cb); // PC + Cycles handled in exec_cb
}

// 0xcc
// CALL Z, a16
// 3 24/12
// - - - -
void CPU::op_call_z_a16() {
    uint16_t address = this->memory.read_word(static_cast<uint16_t>(this->registers.PC + 1));

    if (this->registers.get_flag_z()) {
        uint16_t return_address = static_cast<uint16_t>(this->registers.PC + 3);
        this->stack.push_word(return_address);

        this->registers.PC = address;
        this->tstates += 24;
        return;
    }
    this->registers.PC += 3;
    this->tstates += 12;
}

// 0xcd
// CALL a16
// 3 24
// - - - -
void CPU::op_call_a16() {
    uint16_t address = this->memory.read_word(static_cast<uint16_t>(this->registers.PC + 1));

    uint16_t return_address = static_cast<uint16_t>(this->registers.PC + 3);
    this->stack.push_word(return_address);

    this->registers.PC = address;
    this->tstates += 24;
}

// 0xce
// ADC A, n8
// 2 8
// Z 0 H C
void CPU::op_adc_a_n8() {
    uint8_t value = this->memory.read_byte(static_cast<uint16_t>(this->registers.PC + 1));
    this->alu.adc_u8(value);
    this->registers.PC += 2;
    this->tstates += 8;
}

// 0xcf
// RST $08
// 1 16
// - - - -
void CPU::op_rst_08() {
    this->stack.push_word(static_cast<uint16_t>(this->registers.PC + 1));
    this->registers.PC = 0x0008;
    this->tstates += 16;
}

// 0xd0
// RET NC
// 1 20/8
// - - - -
void CPU::op_ret_nc() {
    if (!this->registers.get_flag_c()) {
        uint16_t address = this->stack.pop_word();
        this->registers.PC = address;
        this->tstates += 20;
        return;
    }
    this->registers.PC += 1;
    this->tstates += 8;
}

// 0xd1
// POP DE
// 1 12
// - - - -
void CPU::op_pop_de() {
    uint16_t word = this->stack.pop_word();
    this->registers.set_de(word);
    this->registers.PC += 1;
    this->tstates += 12;
}

// 0xd2
// JP NC, a16
// 3 16/12
// - - - -
void CPU::op_jp_nc_a16() {
    uint16_t address = this->memory.read_word(static_cast<uint16_t>(this->registers.PC + 1));
    if (!this->registers.get_flag_c()) {
        this->registers.PC = address;
        this->tstates += 16;
        return;
    }
    this->registers.PC += 3;
    this->tstates += 12;
}

// 0xd3
// Unused opcode
// 1 4
// - - - -
// void CPU::op_unused_d3() {
//     return 0;
// }

// 0xd4
// CALL NC, a16
// 3 24/12
// - - - -
void CPU::op_call_nc_a16() {
    uint16_t address = this->memory.read_word(static_cast<uint16_t>(this->registers.PC + 1));

    if (!this->registers.get_flag_c()) {
        uint16_t return_address = static_cast<uint16_t>(this->registers.PC + 3);
        this->stack.push_word(return_address);

        this->registers.PC = address;
        this->tstates += 24;
        return;
    }
    this->registers.PC += 3;
    this->tstates += 12;
}

// 0xd5
// PUSH DE
// 1 16
// - - - -
void CPU::op_push_de() {
    this->stack.push_word(this->registers.get_de());
    this->registers.PC += 1;
    this->tstates += 16;
}

// 0xd6
// SUB A, n8
// 2 8
// Z 1 H C
void CPU::op_sub_a_n8() {
    uint8_t value = this->memory.read_byte(static_cast<uint16_t>(this->registers.PC + 1));
    this->alu.sub_u8(value);
    this->registers.PC += 2;
    this->tstates += 8;
}

// 0xd7
// RST $10
// 1 16
// - - - -
void CPU::op_rst_10() {
    this->stack.push_word(static_cast<uint16_t>(this->registers.PC + 1));
    this->registers.PC = 0x0010;
    this->tstates += 16;
}

// 0xd8
// RET C
// 1 20/8
// - - - -
void CPU::op_ret_c() {
    if (this->registers.get_flag_c()) {
        uint16_t address = this->stack.pop_word();
        this->registers.PC = address;
        this->tstates += 20;
        return;
    }
    this->registers.PC += 1;
    this->tstates += 8;
}

// 0xd9
// RETI
// 1 16
// - - - -
void CPU::op_reti() {
    uint16_t address = this->stack.pop_word();
    this->registers.PC = address;
    this->registers.IME = true;
    this->tstates += 16;
}

// 0xda
// JP C, a16
// 3 16/12
// - - - -
void CPU::op_jp_c_a16() {
    uint16_t address = this->memory.read_word(static_cast<uint16_t>(this->registers.PC + 1));
    if (this->registers.get_flag_c()) {
        this->registers.PC = address;
        this->tstates += 16;
        return;
    }
    this->registers.PC += 3;
    this->tstates += 12;
}

// 0xdb
// Unused opcode
// - -
// - - - -
// void CPU::op_unused_db() {
//     return 0;
// }

// 0xdc
// CALL C, a16
// 3 24/12
// - - - -
void CPU::op_call_c_a16() {
    uint16_t address = this->memory.read_word(static_cast<uint16_t>(this->registers.PC + 1));

    if (this->registers.get_flag_c()) {
        uint16_t return_address = static_cast<uint16_t>(this->registers.PC + 3);
        this->stack.push_word(return_address);

        this->registers.PC = address;
        this->tstates += 24;
        return;
    }
    this->registers.PC += 3;
    this->tstates += 12;
}

// 0xdd
// Unused opcode
// - -
// - - - -
// void CPU::op_unused_dd() {
//     return 0;
// }

// 0xde
// SBC A, n8
// 2 8
// Z 1 H C
void CPU::op_sbc_a_n8() {
    uint8_t value = this->memory.read_byte(static_cast<uint16_t>(this->registers.PC + 1));
    this->alu.sbc_u8(value);
    this->registers.PC += 2;
    this->tstates += 8;
}

// 0xdf
// RST $18
// 1 16
// - - - -
void CPU::op_rst_18() {
    this->stack.push_word(static_cast<uint16_t>(this->registers.PC + 1));
    this->registers.PC = 0x0018;
    this->tstates += 16;
}

// 0xe0
// LDH [a8], A
// 2 12
// - - - -
// Notes: a8 means 8-bit unsigned data, which is added to $FF00 in certain instructions to create a 16-bit address in HRAM (High RAM)
void CPU::op_ldh_a8m_a() {
    uint16_t address = 0xFF00 + static_cast<uint16_t>(this->memory.read_byte(static_cast<uint16_t>(this->registers.PC + 1)));
    this->memory.write_byte(address, this->registers.A);
    this->registers.PC += 2;
    this->tstates += 12;
}

// 0xe1
// POP HL
// 1 12
// - - - -
void CPU::op_pop_hl() {
    uint8_t word = this->stack.pop_word();
    this->registers.set_hl(word);
    this->registers.PC += 1;
    this->tstates += 12;
}

// 0xe2
// LDH [C], A
// 2 8
// - - - -
// Notes: LDH [C], A has the alternative mnemonic LD [$FF00+C], A
void CPU::op_ldh_cm_a() {
    uint16_t address = 0xFF00 + static_cast<uint16_t>(this->registers.C);
    this->memory.write_byte(address, this->registers.A);
    this->registers.PC += 2;
    this->tstates += 8;
}

// 0xe3
// Unused opcode
// - -
// - - - -
// void CPU::op_unused_e3() {
//     return 0;
// }

// 0xe4
// Unused opcode
// - -
// - - - -
// void CPU::op_unused_e4() {
//     return 0;
// }

// 0xe5
// PUSH HL
// 1 16
// - - - -
void CPU::op_push_hl() {
    this->stack.push_word(this->registers.get_hl());
    this->registers.PC += 1;
    this->tstates += 16;
}

// 0xe6
// AND A, n8
// 2 8
// Z 0 1 0
void CPU::op_and_a_n8() {
    uint8_t value = this->memory.read_byte(static_cast<uint16_t>(this->registers.PC + 1));
    this->alu.and_u8(value);
    this->registers.PC += 2;
    this->tstates += 8;
}

// 0xe7
// RST $20
// 1 16
// - - - -
void CPU::op_rst_20() {
    this->stack.push_word(static_cast<uint16_t>(this->registers.PC + 1));
    this->registers.PC = 0x0020;
    this->tstates += 16;
}

// 0xe8
// ADD SP, e8
// 2 16
// 0 0 H C
void CPU::op_add_sp_e8() {
    int8_t value = static_cast<int8_t>(this->memory.read_byte(static_cast<uint16_t>(this->registers.PC + 1)));
    uint16_t sp = this->registers.SP;
    uint16_t result = static_cast<uint16_t>(static_cast<int32_t>(sp) + static_cast<int32_t>(value));
    this->registers.SP = result;

    uint8_t value_u = static_cast<uint8_t>(value);

    // Flags
    this->registers.set_flag_z(false);
    this->registers.set_flag_n(false);
    this->registers.set_flag_h(((sp & 0x0F) + (value_u & 0x0F)) > 0x0F);
    this->registers.set_flag_c((static_cast<uint16_t>(sp & 0xFF) + value_u) > 0xFF);

    this->registers.PC += 2;
    this->tstates += 16;
}

// 0xe9
// JP HL
// 1 4
// - - - -
void CPU::op_jp_hl() {
    this->registers.PC = this->registers.get_hl();
    this->tstates += 4;
}

// 0xea
// LD [a16], A
// 3 16
// - - - -
void CPU::op_ld_a16m_a() {
    this->memory.write_byte(this->memory.read_word(static_cast<uint16_t>(this->registers.PC + 1)), this->registers.A);
    this->registers.PC += 3;
    this->tstates += 16;
}

// 0xeb
// Unused opcode
// - -
// - - - -
// void CPU::op_unused_eb() {
//     return 0;
// }

// 0xec
// Unused opcode
// - -
// - - - -
// void CPU::op_unused_ec() {
//     return 0;
// }

// 0xed
// Unused opcode
// - -
// - - - -
// void CPU::op_unused_ed() {
//     return 0;
// }

// 0xee
// XOR A, n8
// 2 8
// Z 0 0 0
void CPU::op_xor_a_n8() {
    uint8_t value = this->memory.read_byte(static_cast<uint16_t>(this->registers.PC + 1));
    this->alu.xor_u8(value);
    this->registers.PC += 2;
    this->tstates += 8;
}

// 0xef
// RST $28
// 1 16
// - - - -
void CPU::op_rst_28() {
    this->stack.push_word(static_cast<uint16_t>(this->registers.PC + 1));
    this->registers.PC = 0x0028;
    this->tstates += 16;
}

// 0xf0
// LDH A, [a8]
// 2 12
// - - - -
// Notes: a8 means 8-bit unsigned data, which is added to $FF00 in certain instructions to create a 16-bit address in HRAM (High RAM)
void CPU::op_ldh_a_a8m() {
    uint16_t address = 0xFF00 + static_cast<uint16_t>(this->memory.read_byte(static_cast<uint16_t>(this->registers.PC + 1)));
    this->registers.A = this->memory.read_byte(address);
    this->registers.PC += 2;
    this->tstates += 12;
}

// 0xf1
// POP AF
// 1 12
// Z N H C
void CPU::op_pop_af() {
    uint16_t word = this->stack.pop_word();
    this->registers.set_af(word);
    this->registers.PC += 1;
    this->tstates += 12;
}

// 0xf2
// LDH A, [C]
// 2 8
// - - - -
// LDH A, [C] has the alternative mnemonic LD A, [$FF00+C]
void CPU::op_ldh_a_cm() {
    uint16_t address = 0xFF00 + static_cast<uint16_t>(this->registers.C);
    this->registers.A = this->memory.read_byte(address);
    this->registers.PC += 2;
    this->tstates += 8;
}

// 0xf3
// DI
// 1 4
// - - - -
void CPU::op_di() {
    this->registers.IME = false;
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0xf4
// Unused opcode
// - -
// - - - -
// void CPU::op_unused_f4() {
//     return 0;
// }

// 0xf5
// PUSH AF
// 1 16
// - - - -
void CPU::op_push_af() {
    this->stack.push_word(this->registers.get_af());
    this->registers.PC += 1;
    this->tstates += 16;
}

// 0xf6
// OR A, n8
// 2 8
// Z 0 0 0
void CPU::op_or_a_n8() {
    uint8_t value = this->memory.read_byte(static_cast<uint16_t>(this->registers.PC + 1));
    this->alu.or_u8(value);
    this->registers.PC += 2;
    this->tstates += 8;
}

// 0xf7
// RST $30
// 1 16
// - - - -
void CPU::op_rst_30() {
    this->stack.push_word(static_cast<uint16_t>(this->registers.PC + 1));
    this->registers.PC = 0x0030;
    this->tstates += 16;
}

// 0xf8
// LD HL, SP+e8
// 3 12
// 0 0 H C
void CPU::op_ld_hl_sp_e8() {
    int8_t offset = static_cast<int8_t>(this->memory.read_byte(static_cast<uint16_t>(this->registers.PC + 1)));
    uint16_t sp = this->registers.SP;
    uint16_t result = static_cast<uint16_t>(static_cast<int32_t>(sp) + static_cast<int32_t>(offset));
    this->registers.set_hl(result);

    // Flags
    this->registers.set_flag_z(false);
    this->registers.set_flag_n(false);
    this->registers.set_flag_h(((sp & 0x0F) + (static_cast<uint8_t>(offset) & 0x0F)) > 0x0F);
    this->registers.set_flag_c((static_cast<uint16_t>(sp & 0xFF) + static_cast<uint8_t>(offset)) > 0xFF);

    this->registers.PC += 2;
    this->tstates += 12;
}

// 0xf9
// LD SP, HL
// 1 8
// - - - -
void CPU::op_ld_sp_hl() {
    this->registers.SP = this->registers.get_hl();
    this->registers.PC += 1;
    this->tstates += 8;
}

// 0xfa
// LD A, [a16]
// 3 16
// - - - -
void CPU::op_ld_a_a16m() {
    uint16_t address = this->memory.read_word(static_cast<uint16_t>(this->registers.PC + 1));
    uint8_t value = this->memory.read_byte(address);
    this->registers.A = value;
    this->registers.PC += 3;
    this->tstates += 16;
}

// 0xfb
// EI
// 1 4
// - - - -
void CPU::op_ei() {
    this->registers.IME = true;
    this->registers.PC += 1;
    this->tstates += 4;
}

// 0xfc
// Unused opcode
// - -
// - - - -
// void CPU::op_unused_fc() {
//     return 0;
// }

// 0xfd
// Unused opcode
// - -
// - - - -
// void CPU::op_unused_fd() {
//     return 0;
// }

// 0xfe
// CP A, n8
// 2 8
// Z 1 H C
void CPU::op_cp_a_n8() {
    uint8_t value = this->memory.read_byte(static_cast<uint16_t>(this->registers.PC + 1));
    this->alu.cp_u8(value);
    this->registers.PC += 2;
    this->tstates += 8;
}

// 0xff
// RST $38
// 1 16
// - - - -
void CPU::op_rst_38() {
    this->stack.push_word(static_cast<uint16_t>(this->registers.PC + 1));
    this->registers.PC = 0x0038;
    this->tstates += 16;
}

void CPU::init() {
    for (auto &o : this->ops)
        o = {&CPU::op_unimplemented, "???"};

    auto set = [&](uint8_t opcode, OpFn fn, const char *name) { this->ops[opcode] = {fn, name}; };

    // Set up opcode table
    // clang-format off
    // 0x00 - 0x0F
    set(0x00, &CPU::op_nop,          "NOP");
    set(0x01, &CPU::op_ld_bc_n16,    "LD BC, n16");
    set(0x02, &CPU::op_ld_bcm_a,     "LD [BC], A");
    set(0x03, &CPU::op_inc_bc,       "INC BC");
    set(0x04, &CPU::op_inc_b,        "INC B");
    set(0x05, &CPU::op_dec_b,        "DEC B");
    set(0x06, &CPU::op_ld_b_n8,      "LD B, n8");
    set(0x07, &CPU::op_rlca,         "RLCA");
    set(0x08, &CPU::op_ld_a16m_sp,   "LD [a16], SP");
    set(0x09, &CPU::op_add_hl_bc,    "ADD HL, BC");
    set(0x0A, &CPU::op_ld_a_bcm,     "LD A, [BC]");
    set(0x0B, &CPU::op_dec_bc,       "DEC BC");
    set(0x0C, &CPU::op_inc_c,        "INC C");
    set(0x0D, &CPU::op_dec_c,        "DEC C");
    set(0x0E, &CPU::op_ld_c_n8,      "LD C, n8");
    set(0x0F, &CPU::op_rrca,         "RRCA");

    // 0x10 - 0x1F
    set(0x10, &CPU::op_stop_n8,      "STOP n8");
    set(0x11, &CPU::op_ld_de_n16,    "LD DE, n16");
    set(0x12, &CPU::op_ld_dem_a,     "LD [DE], A");
    set(0x13, &CPU::op_inc_de,       "INC DE");
    set(0x14, &CPU::op_inc_d,        "INC D");
    set(0x15, &CPU::op_dec_d,        "DEC D");
    set(0x16, &CPU::op_ld_d_n8,      "LD D, n8");
    set(0x17, &CPU::op_rla,          "RLA");
    set(0x18, &CPU::op_jr_e8,        "JR e8");
    set(0x19, &CPU::op_add_hl_de,    "ADD HL, DE");
    set(0x1A, &CPU::op_ld_a_dem,     "LD A, [DE]");
    set(0x1B, &CPU::op_dec_de,       "DEC DE");
    set(0x1C, &CPU::op_inc_e,        "INC E");
    set(0x1D, &CPU::op_dec_e,        "DEC E");
    set(0x1E, &CPU::op_ld_e_n8,      "LD E, n8");
    set(0x1F, &CPU::op_rra,          "RRA");

    // 0x20 - 0x2F
    set(0x20, &CPU::op_jr_nz_e8,     "JR NZ, e8");
    set(0x21, &CPU::op_ld_hl_a16,    "LD HL, n16");
    set(0x22, &CPU::op_ld_hlim_a,    "LD [HL+], A");
    set(0x23, &CPU::op_inc_hl,       "INC HL");
    set(0x24, &CPU::op_inc_h,        "INC H");
    set(0x25, &CPU::op_dec_h,        "DEC H");
    set(0x26, &CPU::op_ld_h_n8,      "LD H, n8");
    set(0x27, &CPU::op_daa,          "DAA");
    set(0x28, &CPU::op_jr_z_e8,      "JR Z, e8");
    set(0x29, &CPU::op_add_hl_hl,    "ADD HL, HL");
    set(0x2A, &CPU::op_ld_a_hlim,    "LD A, [HL+]");
    set(0x2B, &CPU::op_dec_hl,       "DEC HL");
    set(0x2C, &CPU::op_inc_l,        "INC L");
    set(0x2D, &CPU::op_dec_l,        "DEC L");
    set(0x2E, &CPU::op_ld_l_n8,      "LD L, n8");
    set(0x2F, &CPU::op_cpl,          "CPL");

    // 0x30 - 0x3F
    set(0x30, &CPU::op_jr_nc_e8,     "JR NC, e8");
    set(0x31, &CPU::op_ld_sp_a16,    "LD SP, n16");
    set(0x32, &CPU::op_ld_hldm_a,    "LD [HL-], A");
    set(0x33, &CPU::op_inc_sp,       "INC SP");
    set(0x34, &CPU::op_inc_hlm,      "INC [HL]");
    set(0x35, &CPU::op_dec_hlm,      "DEC [HL]");
    set(0x36, &CPU::op_ld_hlm_n8,    "LD [HL], n8");
    set(0x37, &CPU::op_scf,          "SCF");
    set(0x38, &CPU::op_jr_c_e8,      "JR C, e8");
    set(0x39, &CPU::op_add_hl_sp,    "ADD HL, SP");
    set(0x3A, &CPU::op_ld_a_hldm,    "LD A, [HL-]");
    set(0x3B, &CPU::op_dec_sp,       "DEC SP");
    set(0x3C, &CPU::op_inc_a,        "INC A");
    set(0x3D, &CPU::op_dec_a,        "DEC A");
    set(0x3E, &CPU::op_ld_a_n8,      "LD A, n8");
    set(0x3F, &CPU::op_ccf,          "CCF");

    // 0x40 - 0x4F
    set(0x40, &CPU::op_ld_b_b,       "LD B, B");
    set(0x41, &CPU::op_ld_b_c,       "LD B, C");
    set(0x42, &CPU::op_ld_b_d,       "LD B, D");
    set(0x43, &CPU::op_ld_b_e,       "LD B, E");
    set(0x44, &CPU::op_ld_b_h,       "LD B, H");
    set(0x45, &CPU::op_ld_b_l,       "LD B, L");
    set(0x46, &CPU::op_ld_b_hlm,     "LD B, [HL]");
    set(0x47, &CPU::op_ld_b_a,       "LD B, A");
    set(0x48, &CPU::op_ld_c_b,       "LD C, B");
    set(0x49, &CPU::op_ld_c_c,       "LD C, C");
    set(0x4A, &CPU::op_ld_c_d,       "LD C, D");
    set(0x4B, &CPU::op_ld_c_e,       "LD C, E");
    set(0x4C, &CPU::op_ld_c_h,       "LD C, H");
    set(0x4D, &CPU::op_ld_c_l,       "LD C, L");
    set(0x4E, &CPU::op_ld_c_hlm,     "LD C, [HL]");
    set(0x4F, &CPU::op_ld_c_a,       "LD C, A");

    // 0x50 - 0x5F
    set(0x50, &CPU::op_ld_d_b,       "LD D, B");
    set(0x51, &CPU::op_ld_d_c,       "LD D, C");
    set(0x52, &CPU::op_ld_d_d,       "LD D, D");
    set(0x53, &CPU::op_ld_d_e,       "LD D, E");
    set(0x54, &CPU::op_ld_d_h,       "LD D, H");
    set(0x55, &CPU::op_ld_d_l,       "LD D, L");
    set(0x56, &CPU::op_ld_d_hlm,     "LD D, [HL]");
    set(0x57, &CPU::op_ld_d_a,       "LD D, A");
    set(0x58, &CPU::op_ld_e_b,       "LD E, B");
    set(0x59, &CPU::op_ld_e_c,       "LD E, C");
    set(0x5A, &CPU::op_ld_e_d,       "LD E, D");
    set(0x5B, &CPU::op_ld_e_e,       "LD E, E");
    set(0x5C, &CPU::op_ld_e_h,       "LD E, H");
    set(0x5D, &CPU::op_ld_e_l,       "LD E, L");
    set(0x5E, &CPU::op_ld_e_hlm,     "LD E, [HL]");
    set(0x5F, &CPU::op_ld_e_a,       "LD E, A");

    // 0x60 - 0x6F
    set(0x60, &CPU::op_ld_h_b,       "LD H, B");
    set(0x61, &CPU::op_ld_h_c,       "LD H, C");
    set(0x62, &CPU::op_ld_h_d,       "LD H, D");
    set(0x63, &CPU::op_ld_h_e,       "LD H, E");
    set(0x64, &CPU::op_ld_h_h,       "LD H, H");
    set(0x65, &CPU::op_ld_h_l,       "LD H, L");
    set(0x66, &CPU::op_ld_h_hlm,     "LD H, [HL]");
    set(0x67, &CPU::op_ld_h_a,       "LD H, A");
    set(0x68, &CPU::op_ld_l_b,       "LD L, B");
    set(0x69, &CPU::op_ld_l_c,       "LD L, C");
    set(0x6A, &CPU::op_ld_l_d,       "LD L, D");
    set(0x6B, &CPU::op_ld_l_e,       "LD L, E");
    set(0x6C, &CPU::op_ld_l_h,       "LD L, H");
    set(0x6D, &CPU::op_ld_l_l,       "LD L, L");
    set(0x6E, &CPU::op_ld_l_hlm,     "LD L, [HL]");
    set(0x6F, &CPU::op_ld_l_a,       "LD L, A");

    // 0x70 - 0x7F
    set(0x70, &CPU::op_ld_hlm_b,     "LD [HL], B");
    set(0x71, &CPU::op_ld_hlm_c,     "LD [HL], C");
    set(0x72, &CPU::op_ld_hlm_d,     "LD [HL], D");
    set(0x73, &CPU::op_ld_hlm_e,     "LD [HL], E");
    set(0x74, &CPU::op_ld_hlm_h,     "LD [HL], H");
    set(0x75, &CPU::op_ld_hlm_l,     "LD [HL], L");
    set(0x76, &CPU::op_halt,         "HALT");
    set(0x77, &CPU::op_ld_hlm_a,     "LD [HL], A");
    set(0x78, &CPU::op_ld_a_b,       "LD A, B");
    set(0x79, &CPU::op_ld_a_c,       "LD A, C");
    set(0x7A, &CPU::op_ld_a_d,       "LD A, D");
    set(0x7B, &CPU::op_ld_a_e,       "LD A, E");
    set(0x7C, &CPU::op_ld_a_h,       "LD A, H");
    set(0x7D, &CPU::op_ld_a_l,       "LD A, L");
    set(0x7E, &CPU::op_ld_a_hlm,     "LD A, [HL]");
    set(0x7F, &CPU::op_ld_a_a,       "LD A, A");

    // 0x80 - 0x8F
    set(0x80, &CPU::op_add_a_b,      "ADD A, B");
    set(0x81, &CPU::op_add_a_c,      "ADD A, C");
    set(0x82, &CPU::op_add_a_d,      "ADD A, D");
    set(0x83, &CPU::op_add_a_e,      "ADD A, E");
    set(0x84, &CPU::op_add_a_h,      "ADD A, H");
    set(0x85, &CPU::op_add_a_l,      "ADD A, L");
    set(0x86, &CPU::op_add_a_hlm,    "ADD A, [HL]");
    set(0x87, &CPU::op_add_a_a,      "ADD A, A");
    set(0x88, &CPU::op_adc_a_b,      "ADC A, B");
    set(0x89, &CPU::op_adc_a_c,      "ADC A, C");
    set(0x8A, &CPU::op_adc_a_d,      "ADC A, D");
    set(0x8B, &CPU::op_adc_a_e,      "ADC A, E");
    set(0x8C, &CPU::op_adc_a_h,      "ADC A, H");
    set(0x8D, &CPU::op_adc_a_l,      "ADC A, L");
    set(0x8E, &CPU::op_adc_a_hlm,    "ADC A, [HL]");
    set(0x8F, &CPU::op_adc_a_a,      "ADC A, A");

    // 0x90 - 0x9F
    set(0x90, &CPU::op_sub_a_b,      "SUB A, B");
    set(0x91, &CPU::op_sub_a_c,      "SUB A, C");
    set(0x92, &CPU::op_sub_a_d,      "SUB A, D");
    set(0x93, &CPU::op_sub_a_e,      "SUB A, E");
    set(0x94, &CPU::op_sub_a_h,      "SUB A, H");
    set(0x95, &CPU::op_sub_a_l,      "SUB A, L");
    set(0x96, &CPU::op_sub_a_hlm,    "SUB A, [HL]");
    set(0x97, &CPU::op_sub_a_a,      "SUB A, A");
    set(0x98, &CPU::op_sbc_a_b,      "SBC A, B");
    set(0x99, &CPU::op_sbc_a_c,      "SBC A, C");
    set(0x9A, &CPU::op_sbc_a_d,      "SBC A, D");
    set(0x9B, &CPU::op_sbc_a_e,      "SBC A, E");
    set(0x9C, &CPU::op_sbc_a_h,      "SBC A, H");
    set(0x9D, &CPU::op_sbc_a_l,      "SBC A, L");
    set(0x9E, &CPU::op_sbc_a_hlm,    "SBC A, [HL]");
    set(0x9F, &CPU::op_sbc_a_a,      "SBC A, A");

    // 0xA0 - 0xAF
    set(0xA0, &CPU::op_and_a_b,      "AND A, B");
    set(0xA1, &CPU::op_and_a_c,      "AND A, C");
    set(0xA2, &CPU::op_and_a_d,      "AND A, D");
    set(0xA3, &CPU::op_and_a_e,      "AND A, E");
    set(0xA4, &CPU::op_and_a_h,      "AND A, H");
    set(0xA5, &CPU::op_and_a_l,      "AND A, L");
    set(0xA6, &CPU::op_and_a_hlm,    "AND A, [HL]");
    set(0xA7, &CPU::op_and_a_a,      "AND A, A");
    set(0xA8, &CPU::op_xor_a_b,      "XOR A, B");
    set(0xA9, &CPU::op_xor_a_c,      "XOR A, C");
    set(0xAA, &CPU::op_xor_a_d,      "XOR A, D");
    set(0xAB, &CPU::op_xor_a_e,      "XOR A, E");
    set(0xAC, &CPU::op_xor_a_h,      "XOR A, H");
    set(0xAD, &CPU::op_xor_a_l,      "XOR A, L");
    set(0xAE, &CPU::op_xor_a_hlm,    "XOR A, [HL]");
    set(0xAF, &CPU::op_xor_a_a,      "XOR A, A");

    // 0xB0 - 0xBF
    set(0xB0, &CPU::op_or_a_b,       "OR A, B");
    set(0xB1, &CPU::op_or_a_c,       "OR A, C");
    set(0xB2, &CPU::op_or_a_d,       "OR A, D");
    set(0xB3, &CPU::op_or_a_e,       "OR A, E");
    set(0xB4, &CPU::op_or_a_h,       "OR A, H");
    set(0xB5, &CPU::op_or_a_l,       "OR A, L");
    set(0xB6, &CPU::op_or_a_hlm,     "OR A, [HL]");
    set(0xB7, &CPU::op_or_a_a,       "OR A, A");
    set(0xB8, &CPU::op_cp_a_b,       "CP A, B");
    set(0xB9, &CPU::op_cp_a_c,       "CP A, C");
    set(0xBA, &CPU::op_cp_a_d,       "CP A, D");
    set(0xBB, &CPU::op_cp_a_e,       "CP A, E");
    set(0xBC, &CPU::op_cp_a_h,       "CP A, H");
    set(0xBD, &CPU::op_cp_a_l,       "CP A, L");
    set(0xBE, &CPU::op_cp_a_hlm,     "CP A, [HL]");
    set(0xBF, &CPU::op_cp_a_a,       "CP A, A");

    // 0xC0 - 0xCF
    set(0xC0, &CPU::op_ret_nz,       "RET NZ");
    set(0xC1, &CPU::op_pop_bc,       "POP BC");
    set(0xC2, &CPU::op_jp_nz_a16,    "JP NZ, a16");
    set(0xC3, &CPU::op_jp_a16,       "JP a16");
    set(0xC4, &CPU::op_call_nz_a16,  "CALL NZ, a16");
    set(0xC5, &CPU::op_push_bc,      "PUSH BC");
    set(0xC6, &CPU::op_add_a_n8,     "ADD A, n8");
    set(0xC7, &CPU::op_rst_00,       "RST $00");
    set(0xC8, &CPU::op_ret_z,        "RET Z");
    set(0xC9, &CPU::op_ret,          "RET");
    set(0xCA, &CPU::op_jp_z_a16,     "JP Z, a16");
    set(0xCB, &CPU::op_prefix,       "PREFIX");
    set(0xCC, &CPU::op_call_z_a16,   "CALL Z, a16");
    set(0xCD, &CPU::op_call_a16,     "CALL a16");
    set(0xCE, &CPU::op_adc_a_n8,     "ADC A, n8");
    set(0xCF, &CPU::op_rst_08,       "RST $08");

    // 0xD0 - 0xDF
    set(0xD0, &CPU::op_ret_nc,       "RET NC");
    set(0xD1, &CPU::op_pop_de,       "POP DE");
    set(0xD2, &CPU::op_jp_nc_a16,    "JP NC, a16");
    // set(0xD3, &CPU::op_unused_d3, "ILLEGAL (0xD3)");
    set(0xD4, &CPU::op_call_nc_a16,  "CALL NC, a16");
    set(0xD5, &CPU::op_push_de,      "PUSH DE");
    set(0xD6, &CPU::op_sub_a_n8,     "SUB A, n8");
    set(0xD7, &CPU::op_rst_10,       "RST $10");
    set(0xD8, &CPU::op_ret_c,        "RET C");
    set(0xD9, &CPU::op_reti,         "RETI");
    set(0xDA, &CPU::op_jp_c_a16,     "JP C, a16");
    // set(0xDB, &CPU::op_unused_db,    "ILLEGAL (0xDB)");
    set(0xDC, &CPU::op_call_c_a16,   "CALL C, a16");
    // set(0xDD, &CPU::op_unused_dd,    "ILLEGAL (0xDD)");
    set(0xDE, &CPU::op_sbc_a_n8,     "SBC A, n8");
    set(0xDF, &CPU::op_rst_18,       "RST $18");

    // 0xE0 - 0xEF
    set(0xE0, &CPU::op_ldh_a8m_a,    "LDH [a8], A");
    set(0xE1, &CPU::op_pop_hl,       "POP HL");
    set(0xE2, &CPU::op_ldh_cm_a,    "LDH [C], A");
    // set(0xE3, &CPU::op_unused_e3,    "ILLEGAL (0xE3)");
    // set(0xE4, &CPU::op_unused_e4,    "ILLEGAL (0xE4)");
    set(0xE5, &CPU::op_push_hl,      "PUSH HL");
    set(0xE6, &CPU::op_and_a_n8,     "AND A, n8");
    set(0xE7, &CPU::op_rst_20,       "RST $20");
    set(0xE8, &CPU::op_add_sp_e8,    "ADD SP, e8");
    set(0xE9, &CPU::op_jp_hl,        "JP HL");
    set(0xEA, &CPU::op_ld_a16m_a,    "LD [a16], A");
    // set(0xEB, &CPU::op_unused_eb,    "ILLEGAL (0xEB)");
    // set(0xEC, &CPU::op_unused_ec,    "ILLEGAL (0xEC)");
    // set(0xED, &CPU::op_unused_ed,    "ILLEGAL (0xED)");
    set(0xEE, &CPU::op_xor_a_n8,     "XOR A, n8");
    set(0xEF, &CPU::op_rst_28,       "RST $28");

    // 0xF0 - 0xFF
    set(0xF0, &CPU::op_ldh_a_a8m,    "LDH A, [a8]");
    set(0xF1, &CPU::op_pop_af,       "POP AF");
    set(0xF2, &CPU::op_ldh_a_cm,      "LDH A, [C]");
    set(0xF3, &CPU::op_di,           "DI");
    // set(0xF4, &CPU::op_unused_f4,    "ILLEGAL (0xF4)");
    set(0xF5, &CPU::op_push_af,      "PUSH AF");
    set(0xF6, &CPU::op_or_a_n8,      "OR A, n8");
    set(0xF7, &CPU::op_rst_30,       "RST $30");
    set(0xF8, &CPU::op_ld_hl_sp_e8,  "LD HL, SP+e8");
    set(0xF9, &CPU::op_ld_sp_hl,     "LD SP, HL");
    set(0xFA, &CPU::op_ld_a_a16m,    "LD A, [a16]");
    set(0xFB, &CPU::op_ei,           "EI");
    // set(0xFC, &CPU::op_unused_fc,    "ILLEGAL (0xFC)");
    // set(0xFD, &CPU::op_unused_fd,    "ILLEGAL (0xFD)");
    set(0xFE, &CPU::op_cp_a_n8,      "CP A, n8");
    set(0xFF, &CPU::op_rst_38,       "RST $38");
    // clang-format on
}
