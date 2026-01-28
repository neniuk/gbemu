#include "gb.h"
#include "config.h"

#include <SDL2/SDL.h>
#include <assert.h>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <vector>

// TODO: CALL, JP, RET, RST etc. where PC is set directly, should not increment PC after opcode execution

GB::GB() : stack(&registers.SP, &memory), idu(&registers, &memory) {
    // clang-format off
    // Write startup logo to memory
    std::vector<uint8_t> logo = {
        0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B,
        0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D,
        0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E,
        0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99,
        0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC,
        0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E
    };
    this->memory.write_range(0x0104, logo);
    // clang-format on
};

// default unimplemented opcode handler =======
int GB::op_unimplemented() {
    std::stringstream ss;
    ss << "Unimplemented opcode at PC=0x" << std::hex << std::setw(4) << std::setfill('0') << this->registers.PC;
    throw std::runtime_error(ss.str());
    return 0;
}

// opcode implementations =====================

// 0x00
// NOP
// 1 4
// - - - -
int GB::op_nop() { return 4; }

// 0x01
// LD BC, n16
// 3 12
// - - - -
int GB::op_ld_bc_n16() {
    uint16_t n16 = this->memory.read_word(static_cast<uint16_t>(this->registers.PC + 1));
    this->registers.set_bc(n16);
    return 12;
}

// 0x02
// LD [BC], A
// 1 8
// - - - -
int GB::op_ld_bcm_a() {
    this->memory.write_byte(this->registers.get_bc(), this->registers.A);
    return 8;
}

// 0x03
// INC BC
// 1 8
// - - - -
int GB::op_inc_bc() {
    this->registers.set_bc(static_cast<uint16_t>(this->registers.get_bc() + 1));
    return 8;
}

// 0x04
// INC B
// 1 4
// Z 0 H -
int GB::op_inc_b() {
    this->idu.increment_r8(this->registers.B);
    return 4;
}

// 0x05
// DEC B
// 1 4
// Z 1 H -
int GB::op_dec_b() {
    this->idu.decrement_r8(this->registers.B);
    return 4;
}

// 0x06
// LD B, n8
// 2 8
// - - - -
int GB::op_ld_b_n8() {
    uint8_t n8 = this->memory.read_byte(static_cast<uint16_t>(this->registers.PC + 1));
    this->registers.B = n8;
    return 8;
}

// 0x07
// RLCA
// 1 4
// 0 0 0 C
int GB::op_rlca() {
    // TODO: implement RLCA
    return 0;
}

// 0x08
// LD [a16], SP
// 3 20
// - - - -
int GB::op_ld_a16m_sp() {
    uint16_t a16 = this->memory.read_word(static_cast<uint16_t>(this->registers.PC + 1));
    this->memory.write_word(a16, this->registers.SP);
    return 20;
}

// 0x09
// ADD HL, BC
// 1 8
// - 0 H C
int GB::op_add_hl_bc() {
    // TODO: implement ADD HL, BC
    return 0;
}

// 0x0a
// LD A, [BC]
// 1 8
// - - - -
int GB::op_ld_a_bcm() {
    this->registers.A = this->memory.read_byte(this->registers.get_bc());
    return 8;
}

// 0x0b
// DEC BC
// 1 8
// - - - -
int GB::op_dec_bc() {
    this->registers.set_bc(static_cast<uint16_t>(this->registers.get_bc() - 1));
    return 8;
}

// 0x0c
// INC C
// 1 4
// Z 0 H -
int GB::op_inc_c() {
    this->idu.increment_r8(this->registers.C);
    return 4;
}

// 0x0d
// DEC C
// 1 4
// Z 1 H -
int GB::op_dec_c() {
    this->idu.decrement_r8(this->registers.C);
    return 4;
}

// 0x0e
// LD C, n8
// 2 8
// - - - -
int GB::op_ld_c_n8() {
    uint8_t n8 = this->memory.read_byte(static_cast<uint16_t>(this->registers.PC + 1));
    this->registers.C = n8;
    return 8;
}

// 0x0f
// RRCA
// 1 4
// 0 0 0 C
int GB::op_rrca() {
    // TODO: implement RRCA
    return 0;
}

// 0x10
// STOP n8
// 2 4
// - - - -
int GB::op_stop_n8() {
    // TODO: implement STOP n8
    return 0;
}

// 0x11
// LD DE, n16
// 3 12
// - - - -
int GB::op_ld_de_n16() {
    uint16_t n16 = this->memory.read_word(static_cast<uint16_t>(this->registers.PC + 1));
    this->registers.set_de(n16);
    return 12;
}

// 0x12
// LD [DE], A
// 1 8
// - - - -
int GB::op_ld_dem_a() {
    this->memory.write_byte(this->registers.get_de(), this->registers.A);
    return 8;
}

// 0x13
// INC DE
// 1 8
// - - - -
int GB::op_inc_de() {
    this->registers.set_de(static_cast<uint16_t>(this->registers.get_de() + 1));
    return 8;
}

// 0x14
// INC D
// 1 4
// Z 0 H -
int GB::op_inc_d() {
    this->idu.increment_r8(this->registers.D);
    return 4;
}

// 0x15
// DEC D
// 1 4
// Z 1 H -
int GB::op_dec_d() {
    this->idu.decrement_r8(this->registers.D);
    return 4;
}

// 0x16
// LD D, n8
// 2 8
// - - - -
int GB::op_ld_d_n8() {
    uint8_t n8 = this->memory.read_byte(static_cast<uint16_t>(this->registers.PC + 1));
    this->registers.D = n8;
    return 8;
}

// 0x17
// RLA
// 1 4
// 0 0 0 C
int GB::op_rla() {
    // TODO: implement RLA
    return 0;
}

// 0x18
// JR e8
// 2 12
// - - - -
int GB::op_jr_e8() {
    int8_t offset = static_cast<int8_t>(this->memory.read_byte(static_cast<uint16_t>(this->registers.PC + 1)));
    this->registers.PC += offset;
    return 12;
}

// 0x19
// ADD HL, DE
// 1 8
// - 0 H C
int GB::op_add_hl_de() {
    // TODO: implement ADD HL, DE
    return 0;
}

// 0x1a
// LD A, [DE]
// 1 8
// - - - -
int GB::op_ld_a_dem() {
    this->registers.A = this->memory.read_byte(this->registers.get_de());
    return 8;
}

// 0x1b
// DEC DE
// 1 8
// - - - -
int GB::op_dec_de() {
    this->registers.set_de(static_cast<uint16_t>(this->registers.get_de() - 1));
    return 8;
}

// 0x1c
// INC E
// 1 4
// Z 0 H -
int GB::op_inc_e() {
    this->idu.increment_r8(this->registers.E);
    return 4;
}

// 0x1d
// DEC E
// 1 4
// Z 1 H -
int GB::op_dec_e() {
    this->idu.decrement_r8(this->registers.E);
    return 4;
}

// 0x1e
// LD E, n8
// 2 8
// - - - -
int GB::op_ld_e_n8() {
    uint8_t n8 = this->memory.read_byte(static_cast<uint16_t>(this->registers.PC + 1));
    this->registers.E = n8;
    return 8;
}

// 0x1f
// RRA
// 1 4
// 0 0 0 C
int GB::op_rra() {
    // TODO: implement RRA
    return 0;
}

// 0x20
// JR NZ, e8
// 2 12/8
// - - - -
int GB::op_jr_nz_e8() {
    if (!this->registers.get_flag_z()) {
        int8_t offset = static_cast<int8_t>(this->memory.read_byte(static_cast<uint16_t>(this->registers.PC + 1)));
        this->registers.PC += offset;
        return 12;
    }
    return 8;
}

// 0x21
// LD HL, n16
// 3 12
// - - - -
int GB::op_ld_hl_a16() {
    uint16_t n16 = this->memory.read_word(static_cast<uint16_t>(this->registers.PC + 1));
    this->registers.set_hl(n16);
    return 12;
}

// 0x22
// LD [HL+], A
// 1 8
// - - - -
int GB::op_ld_hlim_a() {
    this->memory.write_byte(this->registers.get_hl(), this->registers.A);
    this->registers.set_hl(static_cast<uint16_t>(this->registers.get_hl() + 1));
    return 8;
}

// 0x23
// INC HL
// 1 8
// - - - -
int GB::op_inc_hl() {
    this->registers.set_hl(static_cast<uint16_t>(this->registers.get_hl() + 1));
    return 8;
}

// 0x24
// INC H
// 1 4
// Z 0 H -
int GB::op_inc_h() {
    this->idu.increment_r8(this->registers.H);
    return 4;
}

// 0x25
// DEC H
// 1 4
// Z 1 H -
int GB::op_dec_h() {
    this->idu.decrement_r8(this->registers.H);
    return 4;
}

// 0x26
// LD H, n8
// 2 8
// - - - -
int GB::op_ld_h_n8() {
    uint8_t n8 = this->memory.read_byte(static_cast<uint16_t>(this->registers.PC + 1));
    this->registers.H = n8;
    return 8;
}

// 0x27
// DAA
// 1 4
// Z - H C
int GB::op_daa() {
    // TODO: implement DAA
    return 0;
}

// 0x28
// JR Z, e8
// 2 12/8
// - - - -
int GB::op_jr_z_e8() {
    if (this->registers.get_flag_z()) {
        int8_t offset = static_cast<int8_t>(this->memory.read_byte(static_cast<uint16_t>(this->registers.PC + 1)));
        this->registers.PC += offset;
        return 12;
    }
    return 8;
}

// 0x29
// ADD HL, HL
// 1 8
// - 0 H C
int GB::op_add_hl_hl() {
    // TODO: implement ADD HL, HL
    return 0;
}

// 0x2a
// LD A, [HL+]
// 1 8
// - - - -
int GB::op_ld_a_hlim() {
    this->registers.A = this->memory.read_byte(this->registers.get_hl());
    this->registers.set_hl(static_cast<uint16_t>(this->registers.get_hl() + 1));
    return 8;
}

// 0x2b
// DEC HL
// 1 8
// - - - -
int GB::op_dec_hl() {
    this->registers.set_hl(static_cast<uint16_t>(this->registers.get_hl() - 1));
    return 8;
}

// 0x2c
// INC L
// 1 4
// Z 0 H -
int GB::op_inc_l() {
    this->idu.increment_r8(this->registers.L);
    return 4;
}

// 0x2d
// DEC L
// 1 4
// Z 1 H -
int GB::op_dec_l() {
    this->idu.decrement_r8(this->registers.L);
    return 4;
}

// 0x2e
// LD L, n8
// 2 8
// - - - -
int GB::op_ld_l_n8() {
    uint8_t n8 = this->memory.read_byte(static_cast<uint16_t>(this->registers.PC + 1));
    this->registers.L = n8;
    return 8;
}

// 0x2f
// CPL
// 1 4
// - 1 1 -
int GB::op_cpl() {
    // TODO: implement CPL
    return 0;
}

// 0x30
// JR NC, e8
// 2 12/8
// - - - -
int GB::op_jr_nc_e8() {
    if (!this->registers.get_flag_c()) {
        int8_t offset = static_cast<int8_t>(this->memory.read_byte(static_cast<uint16_t>(this->registers.PC + 1)));
        this->registers.PC += offset;
        return 12;
    }
    return 8;
}

// 0x31
// LD SP, n16
// 3 12
// - - - -
int GB::op_ld_sp_a16() {
    uint16_t n16 = this->memory.read_word(static_cast<uint16_t>(this->registers.PC + 1));
    this->registers.SP = n16;
    return 12;
}

// 0x32
// LD [HL-], A
// 1 8
// - - - -
int GB::op_ld_hldm_a() {
    this->memory.write_byte(this->registers.get_hl(), this->registers.A);
    this->registers.set_hl(static_cast<uint16_t>(this->registers.get_hl() - 1));
    return 8;
}

// 0x33
// INC SP
// 1 8
// - - - -
int GB::op_inc_sp() {
    this->registers.SP = static_cast<uint16_t>(this->registers.SP + 1);
    return 8;
}

// 0x34
// INC [HL]
// 1 12
// Z 0 H -
int GB::op_inc_hlm() {
    this->idu.increment_mem8(this->registers.get_hl());
    return 12;
}

// 0x35
// DEC [HL]
// 1 12
// Z 1 H -
int GB::op_dec_hlm() {
    this->idu.decrement_mem8(this->registers.get_hl());
    return 12;
}

// 0x36
// LD [HL], n8
// 2 12
// - - - -
int GB::op_ld_hlm_n8() {
    uint8_t n8 = this->memory.read_byte(static_cast<uint16_t>(this->registers.PC + 1));
    this->memory.write_byte(this->registers.get_hl(), n8);
    return 12;
}

// 0x37
// SCF
// 1 4
// - 0 0 1
int GB::op_scf() {
    // TODO: implement SCF
    return 0;
}

// 0x38
// JR C, e8
// 2 12/8
// - - - -
int GB::op_jr_c_e8() {
    if (this->registers.get_flag_c()) {
        int8_t offset = static_cast<int8_t>(this->memory.read_byte(static_cast<uint16_t>(this->registers.PC + 1)));
        this->registers.PC += offset;
        return 12;
    }
    return 8;
}

// 0x39
// ADD HL, SP
// 1 8
// - 0 H C
int GB::op_add_hl_sp() {
    // TODO: implement ADD HL, SP
    return 0;
}

// 0x3a
// LD A, [HL-]
// 1 8
// - - - -
int GB::op_ld_a_hldm() {
    this->registers.A = this->memory.read_byte(this->registers.get_hl());
    this->registers.set_hl(static_cast<uint16_t>(this->registers.get_hl() - 1));
    return 8;
}

// 0x3b
// DEC SP
// 1 8
// - - - -
int GB::op_dec_sp() {
    this->registers.SP = static_cast<uint16_t>(this->registers.SP - 1);
    return 8;
}

// 0x3c
// INC A
// 1 4
// Z 0 H -
int GB::op_inc_a() {
    this->idu.increment_r8(this->registers.A);
    return 4;
}

// 0x3d
// DEC A
// 1 4
// Z 1 H -
int GB::op_dec_a() {
    this->idu.decrement_r8(this->registers.A);
    return 4;
}

// 0x3e
// LD A, n8
// 2 8
// - - - -
int GB::op_ld_a_n8() {
    uint8_t n8 = this->memory.read_byte(static_cast<uint16_t>(this->registers.PC + 1));
    this->registers.A = n8;
    return 8;
}

// 0x3f
// CCF
// 1 4
// - 0 0 C
int GB::op_ccf() {
    // TODO: implement CCF
    return 0;
}

// 0x40
// LD B, B
// 1 4
// - - - -
int GB::op_ld_b_b() {
    this->registers.B = this->registers.B;
    return 4;
}

// 0x41
// LD B, C
// 1 4
// - - - -
int GB::op_ld_b_c() {
    this->registers.B = this->registers.C;
    return 4;
}

// 0x42
// LD B, D
// 1 4
// - - - -
int GB::op_ld_b_d() {
    this->registers.B = this->registers.D;
    return 4;
}

// 0x43
// LD B, E
// 1 4
// - - - -
int GB::op_ld_b_e() {
    this->registers.B = this->registers.E;
    return 4;
}

// 0x44
// LD B, H
// 1 4
// - - - -
int GB::op_ld_b_h() {
    this->registers.B = this->registers.H;
    return 4;
}

// 0x45
// LD B, L
// 1 4
// - - - -
int GB::op_ld_b_l() {
    this->registers.B = this->registers.L;
    return 4;
}

// 0x46
// LD B, [HL]
// 1 8
// - - - -
int GB::op_ld_b_hlm() {
    this->registers.B = this->memory.read_byte(this->registers.get_hl());
    return 8;
}

// 0x47
// LD B, A
// 1 4
// - - - -
int GB::op_ld_b_a() {
    this->registers.B = this->registers.A;
    return 4;
}

// 0x48
// LD C, B
// 1 4
// - - - -
int GB::op_ld_c_b() {
    this->registers.C = this->registers.B;
    return 4;
}

// 0x49
// LD C, C
// 1 4
// - - - -
int GB::op_ld_c_c() {
    this->registers.C = this->registers.C;
    return 4;
}

// 0x4a
// LD C, D
// 1 4
// - - - -
int GB::op_ld_c_d() {
    this->registers.C = this->registers.D;
    return 4;
}

// 0x4b
// LD C, E
// 1 4
// - - - -
int GB::op_ld_c_e() {
    this->registers.C = this->registers.E;
    return 4;
}

// 0x4c
// LD C, H
// 1 4
// - - - -
int GB::op_ld_c_h() {
    this->registers.C = this->registers.H;
    return 4;
}

// 0x4d
// LD C, L
// 1 4
// - - - -
int GB::op_ld_c_l() {
    this->registers.C = this->registers.L;
    return 4;
}

// 0x4e
// LD C, [HL]
// 1 8
// - - - -
int GB::op_ld_c_hlm() {
    this->registers.C = this->memory.read_byte(this->registers.get_hl());
    return 8;
}

// 0x4f
// LD C, A
// 1 4
// - - - -
int GB::op_ld_c_a() {
    this->registers.C = this->registers.A;
    return 4;
}

// 0x50
// LD D, B
// 1 4
// - - - -
int GB::op_ld_d_b() {
    this->registers.D = this->registers.B;
    return 4;
}

// 0x51
// LD D, C
// 1 4
// - - - -
int GB::op_ld_d_c() {
    this->registers.D = this->registers.C;
    return 4;
}

// 0x52
// LD D, D
// 1 4
// - - - -
int GB::op_ld_d_d() {
    this->registers.D = this->registers.D;
    return 4;
}

// 0x53
// LD D, E
// 1 4
// - - - -
int GB::op_ld_d_e() {
    this->registers.D = this->registers.E;
    return 4;
}

// 0x54
// LD D, H
// 1 4
// - - - -
int GB::op_ld_d_h() {
    this->registers.D = this->registers.H;
    return 4;
}

// 0x55
// LD D, L
// 1 4
// - - - -
int GB::op_ld_d_l() {
    this->registers.D = this->registers.L;
    return 4;
}

// 0x56
// LD D, [HL]
// 1 8
// - - - -
int GB::op_ld_d_hlm() {
    this->registers.D = this->memory.read_byte(this->registers.get_hl());
    return 8;
}

// 0x57
// LD D, A
// 1 4
// - - - -
int GB::op_ld_d_a() {
    this->registers.D = this->registers.A;
    return 4;
}

// 0x58
// LD E, B
// 1 4
// - - - -
int GB::op_ld_e_b() {
    this->registers.E = this->registers.B;
    return 4;
}

// 0x59
// LD E, C
// 1 4
// - - - -
int GB::op_ld_e_c() {
    this->registers.E = this->registers.C;
    return 4;
}

// 0x5a
// LD E, D
// 1 4
// - - - -
int GB::op_ld_e_d() {
    this->registers.E = this->registers.D;
    return 4;
}

// 0x5b
// LD E, E
// 1 4
// - - - -
int GB::op_ld_e_e() {
    this->registers.E = this->registers.E;
    return 4;
}

// 0x5c
// LD E, H
// 1 4
// - - - -
int GB::op_ld_e_h() {
    this->registers.E = this->registers.H;
    return 4;
}

// 0x5d
// LD E, L
// 1 4
// - - - -
int GB::op_ld_e_l() {
    this->registers.E = this->registers.L;
    return 4;
}

// 0x5e
// LD E, [HL]
// 1 8
// - - - -
int GB::op_ld_e_hlm() {
    this->registers.E = this->memory.read_byte(this->registers.get_hl());
    return 4;
}

// 0x5f
// LD E, A
// 1 4
// - - - -
int GB::op_ld_e_a() {
    this->registers.E = this->registers.A;
    return 4;
}

// 0x60
// LD H, B
// 1 4
// - - - -
int GB::op_ld_h_b() {
    this->registers.H = this->registers.B;
    return 4;
}

// 0x61
// LD H, C
// 1 4
// - - - -
int GB::op_ld_h_c() {
    this->registers.H = this->registers.C;
    return 4;
}

// 0x62
// LD H, D
// 1 4
// - - - -
int GB::op_ld_h_d() {
    this->registers.H = this->registers.D;
    return 4;
}

// 0x63
// LD H, E
// 1 4
// - - - -
int GB::op_ld_h_e() {
    this->registers.H = this->registers.E;
    return 4;
}

// 0x64
// LD H, H
// 1 4
// - - - -
int GB::op_ld_h_h() {
    this->registers.H = this->registers.H;
    return 4;
}

// 0x65
// LD H, L
// 1 4
// - - - -
int GB::op_ld_h_l() {
    this->registers.H = this->registers.L;
    return 4;
}

// 0x66
// LD H, [HL]
// 1 8
// - - - -
int GB::op_ld_h_hlm() {
    this->registers.H = this->memory.read_byte(this->registers.get_hl());
    return 8;
}

// 0x67
// LD H, A
// 1 4
// - - - -
int GB::op_ld_h_a() {
    this->registers.H = this->registers.A;
    return 4;
}

// 0x68
// LD L, B
// 1 4
// - - - -
int GB::op_ld_l_b() {
    this->registers.L = this->registers.B;
    return 4;
}

// 0x69
// LD L, C
// 1 4
// - - - -
int GB::op_ld_l_c() {
    this->registers.L = this->registers.C;
    return 4;
}

// 0x6a
// LD L, D
// 1 4
// - - - -
int GB::op_ld_l_d() {
    this->registers.L = this->registers.D;
    return 4;
}

// 0x6b
// LD L, E
// 1 4
// - - - -
int GB::op_ld_l_e() {
    this->registers.L = this->registers.E;
    return 4;
}

// 0x6c
// LD L, H
// 1 4
// - - - -
int GB::op_ld_l_h() {
    this->registers.L = this->registers.H;
    return 4;
}

// 0x6d
// LD L, L
// 1 4
// - - - -
int GB::op_ld_l_l() {
    this->registers.L = this->registers.L;
    return 4;
}

// 0x6e
// LD L, [HL]
// 1 8
// - - - -
int GB::op_ld_l_hlm() {
    this->registers.L = this->memory.read_byte(this->registers.get_hl());
    return 8;
}

// 0x6f
// LD L, A
// 1 4
// - - - -
int GB::op_ld_l_a() {
    this->registers.L = this->registers.A;
    return 4;
}

// 0x70
// LD [HL], B
// 1 8
// - - - -
int GB::op_ld_hlm_b() {
    this->memory.write_byte(this->registers.get_hl(), this->registers.B);
    return 8;
}

// 0x71
// LD [HL], C
// 1 8
// - - - -
int GB::op_ld_hlm_c() {
    this->memory.write_byte(this->registers.get_hl(), this->registers.C);
    return 8;
}

// 0x72
// LD [HL], D
// 1 8
// - - - -
int GB::op_ld_hlm_d() {
    this->memory.write_byte(this->registers.get_hl(), this->registers.D);
    return 8;
}

// 0x73
// LD [HL], E
// 1 8
// - - - -
int GB::op_ld_hlm_e() {
    this->memory.write_byte(this->registers.get_hl(), this->registers.E);
    return 8;
}

// 0x74
// LD [HL], H
// 1 8
// - - - -
int GB::op_ld_hlm_h() {
    this->memory.write_byte(this->registers.get_hl(), this->registers.H);
    return 8;
}

// 0x75
// LD [HL], L
// 1 8
// - - - -
int GB::op_ld_hlm_l() {
    this->memory.write_byte(this->registers.get_hl(), this->registers.L);
    return 8;
}

// 0x76
// HALT
// 1 4
// - - - -
int GB::op_halt() {
    // TODO: implement HALT
    return 0;
}

// 0x77
// LD [HL], A
// 1 8
// - - - -
int GB::op_ld_hlm_a() {
    this->memory.write_byte(this->registers.get_hl(), this->registers.A);
    return 8;
}

// 0x78
// LD A, B
// 1 4
// - - - -
int GB::op_ld_a_b() {
    this->registers.A = this->registers.B;
    return 4;
}

// 0x79
// LD A, C
// 1 4
// - - - -
int GB::op_ld_a_c() {
    this->registers.A = this->registers.C;
    return 4;
}

// 0x7a
// LD A, D
// 1 4
// - - - -
int GB::op_ld_a_d() {
    this->registers.A = this->registers.D;
    return 4;
}

// 0x7b
// LD A, E
// 1 4
// - - - -
int GB::op_ld_a_e() {
    this->registers.A = this->registers.E;
    return 4;
}

// 0x7c
// LD A, H
// 1 4
// - - - -
int GB::op_ld_a_h() {
    this->registers.A = this->registers.H;
    return 4;
}

// 0x7d
// LD A, L
// 1 4
// - - - -
int GB::op_ld_a_l() {
    this->registers.A = this->registers.L;
    return 4;
}

// 0x7e
// LD A, [HL]
// 1 8
// - - - -
int GB::op_ld_a_hlm() {
    this->registers.A = this->memory.read_byte(this->registers.get_hl());
    return 8;
}

// 0x7f
// LD A, A
// 1 4
// - - - -
int GB::op_ld_a_a() {
    this->registers.A = this->registers.A;
    return 4;
}

// 0x80
// ADD A, B
// 1 4
// Z 0 H C
int GB::op_add_a_b() {
    // TODO: implement ADD A, B
    return 0;
}

// 0x81
// ADD A, C
// 1 4
// Z 0 H C
int GB::op_add_a_c() {
    // TODO: implement ADD A, C
    return 0;
}

// 0x82
// ADD A, D
// 1 4
// Z 0 H C
int GB::op_add_a_d() {
    // TODO: implement ADD A, D
    return 0;
}

// 0x83
// ADD A, E
// 1 4
// Z 0 H C
int GB::op_add_a_e() {
    // TODO: implement ADD A, E
    return 0;
}

// 0x84
// ADD A, H
// 1 4
// Z 0 H C
int GB::op_add_a_h() {
    // TODO: implement ADD A, H
    return 0;
}

// 0x85
// ADD A, L
// 1 4
// Z 0 H C
int GB::op_add_a_l() {
    // TODO: implement ADD A, L
    return 0;
}

// 0x86
// ADD A, [HL]
// 1 8
// Z 0 H C
int GB::op_add_a_hlm() {
    // TODO: implement ADD A, [HL]
    return 0;
}

// 0x87
// ADD A, A
// 1 4
// Z 0 H C
int GB::op_add_a_a() {
    // TODO: implement ADD A, A
    return 0;
}

// 0x88
// ADC A, B
// 1 4
// Z 0 H C
int GB::op_adc_a_b() {
    // TODO: implement ADC A, B
    return 0;
}

// 0x89
// ADC A, C
// 1 4
// Z 0 H C
int GB::op_adc_a_c() {
    // TODO: implement ADC A, C
    return 0;
}

// 0x8a
// ADC A, D
// 1 4
// Z 0 H C
int GB::op_adc_a_d() {
    // TODO: implement ADC A, D
    return 0;
}

// 0x8b
// ADC A, E
// 1 4
// Z 0 H C
int GB::op_adc_a_e() {
    // TODO: implement ADC A, E
    return 0;
}

// 0x8c
// ADC A, H
// 1 4
// Z 0 H C
int GB::op_adc_a_h() {
    // TODO: implement ADC A, H
    return 0;
}

// 0x8d
// ADC A, L
// 1 4
// Z 0 H C
int GB::op_adc_a_l() {
    // TODO: implement ADC A, L
    return 0;
}

// 0x8e
// ADC A, [HL]
// 1 8
// Z 0 H C
int GB::op_adc_a_hlm() {
    // TODO: implement ADC A, [HL]
    return 0;
}

// 0x8f
// ADC A, A
// 1 4
// Z 0 H C
int GB::op_adc_a_a() {
    // TODO: implement ADC A, A
    return 0;
}

// 0x90
// SUB A, B
// 1 4
// Z 1 H C
int GB::op_sub_a_b() {
    // TODO: implement SUB A, B
    return 0;
}

// 0x91
// SUB A, C
// 1 4
// Z 1 H C
int GB::op_sub_a_c() {
    // TODO: implement SUB A, C
    return 0;
}

// 0x92
// SUB A, D
// 1 4
// Z 1 H C
int GB::op_sub_a_d() {
    // TODO: implement SUB A, D
    return 0;
}

// 0x93
// SUB A, E
// 1 4
// Z 1 H C
int GB::op_sub_a_e() {
    // TODO: implement SUB A, E
    return 0;
}

// 0x94
// SUB A, H
// 1 4
// Z 1 H C
int GB::op_sub_a_h() {
    // TODO: implement SUB A, H
    return 0;
}

// 0x95
// SUB A, L
// 1 4
// Z 1 H C
int GB::op_sub_a_l() {
    // TODO: implement SUB A, L
    return 0;
}

// 0x96
// SUB A, [HL]
// 1 8
// Z 1 H C
int GB::op_sub_a_hlm() {
    // TODO: implement SUB A, [HL]
    return 0;
}

// 0x97
// SUB A, A
// 1 4
// Z 1 H C
int GB::op_sub_a_a() {
    // TODO: implement SUB A, A
    return 0;
}

// 0x98
// SBC A, B
// 1 4
// Z 1 H C
int GB::op_sbc_a_b() {
    // TODO: implement SBC A, B
    return 0;
}

// 0x99
// SBC A, C
// 1 4
// Z 1 H C
int GB::op_sbc_a_c() {
    // TODO: implement SBC A, C
    return 0;
}

// 0x9a
// SBC A, D
// 1 4
// Z 1 H C
int GB::op_sbc_a_d() {
    // TODO: implement SBC A, D
    return 0;
}

// 0x9b
// SBC A, E
// 1 4
// Z 1 H C
int GB::op_sbc_a_e() {
    // TODO: implement SBC A, E
    return 0;
}

// 0x9c
// SBC A, H
// 1 4
// Z 1 H C
int GB::op_sbc_a_h() {
    // TODO: implement SBC A, H
    return 0;
}

// 0x9d
// SBC A, L
// 1 4
// Z 1 H C
int GB::op_sbc_a_l() {
    // TODO: implement SBC A, L
    return 0;
}

// 0x9e
// SBC A, [HL]
// 1 8
// Z 1 H C
int GB::op_sbc_a_hlm() {
    // TODO: implement SBC A, [HL]
    return 0;
}

// 0x9f
// SBC A, A
// 1 4
// Z 1 H C
int GB::op_sbc_a_a() {
    // TODO: implement SBC A, A
    return 0;
}

// 0xa0
// AND A, B
// 1 4
// Z 0 1 0
int GB::op_and_a_b() {
    // TODO: implement AND A, B
    return 0;
}

// 0xa1
// AND A, C
// 1 4
// Z 0 1 0
int GB::op_and_a_c() {
    // TODO: implement AND A, C
    return 0;
}

// 0xa2
// AND A, D
// 1 4
// Z 0 1 0
int GB::op_and_a_d() {
    // TODO: implement AND A, D
    return 0;
}

// 0xa3
// AND A, E
// 1 4
// Z 0 1 0
int GB::op_and_a_e() {
    // TODO: implement AND A, E
    return 0;
}

// 0xa4
// AND A, H
// 1 4
// Z 0 1 0
int GB::op_and_a_h() {
    // TODO: implement AND A, H
    return 0;
}

// 0xa5
// AND A, L
// 1 4
// Z 0 1 0
int GB::op_and_a_l() {
    // TODO: implement AND A, L
    return 0;
}

// 0xa6
// AND A, [HL]
// 1 8
// Z 0 1 0
int GB::op_and_a_hlm() {
    // TODO: implement AND A, [HL]
    return 0;
}

// 0xa7
// AND A, A
// 1 4
// Z 0 1 0
int GB::op_and_a_a() {
    // TODO: implement AND A, A
    return 0;
}

// 0xa8
// XOR A, B
// 1 4
// Z 0 0 0
int GB::op_xor_a_b() {
    // TODO: implement XOR A, B
    return 0;
}

// 0xa9
// XOR A, C
// 1 4
// Z 0 0 0
int GB::op_xor_a_c() {
    // TODO: implement XOR A, C
    return 0;
}

// 0xaa
// XOR A, D
// 1 4
// Z 0 0 0
int GB::op_xor_a_d() {
    // TODO: implement XOR A, D
    return 0;
}

// 0xab
// XOR A, E
// 1 4
// Z 0 0 0
int GB::op_xor_a_e() {
    // TODO: implement XOR A, E
    return 0;
}

// 0xac
// XOR A, H
// 1 4
// Z 0 0 0
int GB::op_xor_a_h() {
    // TODO: implement XOR A, H
    return 0;
}

// 0xad
// XOR A, L
// 1 4
// Z 0 0 0
int GB::op_xor_a_l() {
    // TODO: implement XOR A, L
    return 0;
}

// 0xae
// XOR A, [HL]
// 1 8
// Z 0 0 0
int GB::op_xor_a_hlm() {
    // TODO: implement XOR A, [HL]
    return 0;
}

// 0xaf
// XOR A, A
// 1 4
// Z 0 0 0
int GB::op_xor_a_a() {
    // TODO: implement XOR A, A
    return 0;
}

// 0xb0
// OR A, B
// 1 4
// Z 0 0 0
int GB::op_or_a_b() {
    // TODO: implement OR A, B
    return 0;
}

// 0xb1
// OR A, C
// 1 4
// Z 0 0 0
int GB::op_or_a_c() {
    // TODO: implement OR A, C
    return 0;
}

// 0xb2
// OR A, D
// 1 4
// Z 0 0 0
int GB::op_or_a_d() {
    // TODO: implement OR A, D
    return 0;
}

// 0xb3
// OR A, E
// 1 4
// Z 0 0 0
int GB::op_or_a_e() {
    // TODO: implement OR A, E
    return 0;
}

// 0xb4
// OR A, H
// 1 4
// Z 0 0 0
int GB::op_or_a_h() {
    // TODO: implement OR A, H
    return 0;
}

// 0xb5
// OR A, L
// 1 4
// Z 0 0 0
int GB::op_or_a_l() {
    // TODO: implement OR A, L
    return 0;
}

// 0xb6
// OR A, [HL]
// 1 8
// Z 0 0 0
int GB::op_or_a_hlm() {
    // TODO: implement OR A, [HL]
    return 0;
}

// 0xb7
// OR A, A
// 1 4
// Z 0 0 0
int GB::op_or_a_a() {
    // TODO: implement OR A, A
    return 0;
}

// 0xb8
// CP A, B
// 1 4
// Z 1 H C
int GB::op_cp_a_b() {
    // TODO: implement CP A, B
    return 0;
}

// 0xb9
// CP A, C
// 1 4
// Z 1 H C
int GB::op_cp_a_c() {
    // TODO: implement CP A, C
    return 0;
}

// 0xba
// CP A, D
// 1 4
// Z 1 H C
int GB::op_cp_a_d() {
    // TODO: implement CP A, D
    return 0;
}

// 0xbb
// CP A, E
// 1 4
// Z 1 H C
int GB::op_cp_a_e() {
    // TODO: implement CP A, E
    return 0;
}

// 0xbc
// CP A, H
// 1 4
// Z 1 H C
int GB::op_cp_a_h() {
    // TODO: implement CP A, H
    return 0;
}

// 0xbd
// CP A, L
// 1 4
// Z 1 H C
int GB::op_cp_a_l() {
    uint8_t a = this->registers.A;
    uint8_t r8 = this->registers.L;
    uint8_t result = a - r8;

    // Flags
    this->registers.set_flag_z(result == 0);
    this->registers.set_flag_n(true);
    this->registers.set_flag_h((a & 0x0F) < (r8 & 0x0F));
    this->registers.set_flag_c(r8 > a);

    return 0;
}

// 0xbe
// CP A, [HL]
// 1 8
// Z 1 H C
int GB::op_cp_a_hlm() {
    // TODO: implement CP A, [HL]
    return 0;
}

// 0xbf
// CP A, A
// 1 4
// 1 1 0 0
int GB::op_cp_a_a() {
    // TODO: implement CP A, A
    return 0;
}

// 0xc0
// RET NZ
// 1 20/8
// - - - -
int GB::op_ret_nz() {
    if (!this->registers.get_flag_z()) {
        uint16_t address = this->stack.pop_word();
        this->registers.PC = address;
        return 20;
    }
    return 8;
}

// 0xc1
// POP BC
// 1 12
// - - - -
int GB::op_pop_bc() {
    uint16_t word = this->stack.pop_word();
    this->registers.set_bc(word);
    return 12;
}

// 0xc2
// JP NZ, a16
// 3 16/12
// - - - -
int GB::op_jp_nz_a16() {
    // TODO: implement JP NZ, a16
    return 0;
}

// 0xc3
// JP a16
// 3 16
// - - - -
int GB::op_jp_a16() {
    uint16_t address = this->memory.read_word(static_cast<uint16_t>(this->registers.PC + 1));
    this->registers.PC = address;
    return 16;
}

// 0xc4
// CALL NZ, a16
// 3 24/12
// - - - -
int GB::op_call_nz_a16() {
    // TODO: implement CALL NZ, a16
    return 0;
}

// 0xc5
// PUSH BC
// 1 16
// - - - -
int GB::op_push_bc() {
    this->stack.push_word(this->registers.get_bc());
    return 16;
}

// 0xc6
// ADD A, n8
// 2 8
// Z 0 H C
int GB::op_add_a_n8() {
    // TODO: implement ADD A, n8
    return 0;
}

// 0xc7
// RST $00
// 1 16
// - - - -
int GB::op_rst_00() {
    this->stack.push_word(static_cast<uint16_t>(this->registers.PC + 1));
    this->registers.PC = 0x0000;
    return 16;
}

// 0xc8
// RET Z
// 1 20/8
// - - - -
int GB::op_ret_z() {
    if (this->registers.get_flag_z()) {
        uint16_t address = this->stack.pop_word();
        this->registers.PC = address;
        return 20;
    }
    return 8;
}

// 0xc9
// RET
// 1 16
// - - - -
int GB::op_ret() {
    uint16_t address = this->stack.pop_word();
    this->registers.PC = address;
    return 16;
}

// 0xca
// JP Z, a16
// 3 16/12
// - - - -
int GB::op_jp_z_a16() {
    // TODO: implement JP Z, a16
    return 0;
}

// 0xcb
// PREFIX
// 1 4
// - - - -
int GB::op_prefix() {
    // TODO: implement PREFIX (CB-prefixed dispatch)
    return 0;
}

// 0xcc
// CALL Z, a16
// 3 24/12
// - - - -
int GB::op_call_z_a16() {
    // TODO: implement CALL Z, a16
    return 0;
}

// 0xcd
// CALL a16
// 3 24
// - - - -
int GB::op_call_a16() {
    uint16_t address = this->memory.read_word(static_cast<uint16_t>(this->registers.PC + 1));

    uint16_t return_address = static_cast<uint16_t>(this->registers.PC + 3);
    this->stack.push_word(return_address);

    this->registers.PC = address;
    return 24;
}

// 0xce
// ADC A, n8
// 2 8
// Z 0 H C
int GB::op_adc_a_n8() {
    // TODO: implement ADC A, n8
    return 0;
}

// 0xcf
// RST $08
// 1 16
// - - - -
int GB::op_rst_08() {
    this->stack.push_word(static_cast<uint16_t>(this->registers.PC + 1));
    this->registers.PC = 0x0008;
    return 16;
}

// 0xd0
// RET NC
// 1 20/8
// - - - -
int GB::op_ret_nc() {
    if (!this->registers.get_flag_c()) {
        uint16_t address = this->stack.pop_word();
        this->registers.PC = address;
        return 20;
    }
    return 8;
}

// 0xd1
// POP DE
// 1 12
// - - - -
int GB::op_pop_de() {
    uint16_t word = this->stack.pop_word();
    this->registers.set_de(word);
    return 12;
}

// 0xd2
// JP NC, a16
// 3 16/12
// - - - -
int GB::op_jp_nc_a16() {
    // TODO: implement JP NC, a16
    return 0;
}

// 0xd3
// Unused opcode
// 1 4
// - - - -
// int GB::op_unused_d3() {
//     return 0;
// }

// 0xd4
// CALL NC, a16
// 3 24/12
// - - - -
int GB::op_call_nc_a16() {
    // TODO: implement CALL NC, a16
    return 0;
}

// 0xd5
// PUSH DE
// 1 16
// - - - -
int GB::op_push_de() {
    this->stack.push_word(this->registers.get_de());
    return 16;
}

// 0xd6
// SUB A, n8
// 2 8
// Z 1 H C
int GB::op_sub_a_n8() {
    // TODO: implement SUB A, n8
    return 0;
}

// 0xd7
// RST $10
// 1 16
// - - - -
int GB::op_rst_10() {
    this->stack.push_word(static_cast<uint16_t>(this->registers.PC + 1));
    this->registers.PC = 0x0010;
    return 16;
}

// 0xd8
// RET C
// 1 20/8
// - - - -
int GB::op_ret_c() {
    if (this->registers.get_flag_c()) {
        uint16_t address = this->stack.pop_word();
        this->registers.PC = address;
        return 20;
    }
    return 8;
}

// 0xd9
// RETI
// 1 16
// - - - -
int GB::op_reti() {
    // TODO: implement RETI
    return 0;
}

// 0xda
// JP C, a16
// 3 16/12
// - - - -
int GB::op_jp_c_a16() {
    // TODO: implement JP C, a16
    return 0;
}

// 0xdb
// Unused opcode
// - -
// - - - -
// int GB::op_unused_db() {
//     return 0;
// }

// 0xdc
// CALL C, a16
// 3 24/12
// - - - -
int GB::op_call_c_a16() {
    // TODO: implement CALL C, a16
    return 0;
}

// 0xdd
// Unused opcode
// - -
// - - - -
// int GB::op_unused_dd() {
//     return 0;
// }

// 0xde
// SBC A, n8
// 2 8
// Z 1 H C
int GB::op_sbc_a_n8() {
    // TODO: implement SBC A, n8
    return 0;
}

// 0xdf
// RST $18
// 1 16
// - - - -
int GB::op_rst_18() {
    this->stack.push_word(static_cast<uint16_t>(this->registers.PC + 1));
    this->registers.PC = 0x0018;
    return 16;
}

// 0xe0
// LDH [a8], A
// 2 12
// - - - -
// Notes: a8 means 8-bit unsigned data, which is added to $FF00 in certain instructions to create a 16-bit address in HRAM (High RAM)
int GB::op_ldh_a8m_a() {
    uint16_t address = 0xFF00 + static_cast<uint16_t>(this->memory.read_byte(static_cast<uint16_t>(this->registers.PC + 1)));
    this->memory.write_byte(address, this->registers.A);
    return 12;
}

// 0xe1
// POP HL
// 1 12
// - - - -
int GB::op_pop_hl() {
    uint8_t word = this->stack.pop_word();
    this->registers.set_hl(word);
    return 12;
}

// 0xe2
// LDH [C], A
// 2 8
// - - - -
// Notes: LDH [C], A has the alternative mnemonic LD [$FF00+C], A
int GB::op_ldh_cm_a() {
    uint16_t address = 0xFF00 + static_cast<uint16_t>(this->registers.C);
    this->memory.write_byte(address, this->registers.A);
    return 8;
}

// 0xe3
// Unused opcode
// - -
// - - - -
// int GB::op_unused_e3() {
//     return 0;
// }

// 0xe4
// Unused opcode
// - -
// - - - -
// int GB::op_unused_e4() {
//     return 0;
// }

// 0xe5
// PUSH HL
// 1 16
// - - - -
int GB::op_push_hl() {
    this->stack.push_word(this->registers.get_hl());
    return 16;
}

// 0xe6
// AND A, n8
// 2 8
// Z 0 1 0
int GB::op_and_a_n8() {
    // TODO: implement AND A, n8
    return 0;
}

// 0xe7
// RST $20
// 1 16
// - - - -
int GB::op_rst_20() {
    this->stack.push_word(static_cast<uint16_t>(this->registers.PC + 1));
    this->registers.PC = 0x0020;
    return 16;
}

// 0xe8
// ADD SP, e8
// 2 16
// 0 0 H C
int GB::op_add_sp_e8() {
    // TODO: implement ADD SP, e8
    return 0;
}

// 0xe9
// JP HL
// 1 4
// - - - -
int GB::op_jp_hl() {
    this->registers.PC = this->registers.get_hl();
    return 4;
}

// 0xea
// LD [a16], A
// 3 16
// - - - -
int GB::op_ld_a16m_a() {
    this->memory.write_byte(this->memory.read_word(static_cast<uint16_t>(this->registers.PC + 1)), this->registers.A);
    return 16;
}

// 0xeb
// Unused opcode
// - -
// - - - -
// int GB::op_unused_eb() {
//     return 0;
// }

// 0xec
// Unused opcode
// - -
// - - - -
// int GB::op_unused_ec() {
//     return 0;
// }

// 0xed
// Unused opcode
// - -
// - - - -
// int GB::op_unused_ed() {
//     return 0;
// }

// 0xee
// XOR A, n8
// 2 8
// Z 0 0 0
int GB::op_xor_a_n8() {
    // TODO: implement XOR A, n8
    return 0;
}

// 0xef
// RST $28
// 1 16
// - - - -
int GB::op_rst_28() {
    this->stack.push_word(static_cast<uint16_t>(this->registers.PC + 1));
    this->registers.PC = 0x0028;
    return 16;
}

// 0xf0
// LDH A, [a8]
// 2 12
// - - - -
// Notes: a8 means 8-bit unsigned data, which is added to $FF00 in certain instructions to create a 16-bit address in HRAM (High RAM)
int GB::op_ldh_a_a8m() {
    uint16_t address = 0xFF00 + static_cast<uint16_t>(this->memory.read_byte(static_cast<uint16_t>(this->registers.PC + 1)));
    this->registers.A = this->memory.read_byte(address);
    return 12;
}

// 0xf1
// POP AF
// 1 12
// Z N H C
int GB::op_pop_af() {
    uint16_t word = this->stack.pop_word();
    this->registers.set_af(word);
    return 12;
}

// 0xf2
// LDH A, [C]
// 2 8
// - - - -
// LDH A, [C] has the alternative mnemonic LD A, [$FF00+C]
int GB::op_ldh_a_cm() {
    uint16_t address = 0xFF00 + static_cast<uint16_t>(this->registers.C);
    this->registers.A = this->memory.read_byte(address);
    return 8;
}

// 0xf3
// DI
// 1 4
// - - - -
int GB::op_di() {
    // TODO: implement DI
    return 0;
}

// 0xf4
// Unused opcode
// - -
// - - - -
// int GB::op_unused_f4() {
//     return 0;
// }

// 0xf5
// PUSH AF
// 1 16
// - - - -
int GB::op_push_af() {
    this->stack.push_word(this->registers.get_af());
    return 16;
}

// 0xf6
// OR A, n8
// 2 8
// Z 0 0 0
int GB::op_or_a_n8() {
    // TODO: implement OR A, n8
    return 0;
}

// 0xf7
// RST $30
// 1 16
// - - - -
int GB::op_rst_30() {
    this->stack.push_word(static_cast<uint16_t>(this->registers.PC + 1));
    this->registers.PC = 0x0030;
    return 16;
}

// 0xf8
// LD HL, SP+e8
// 3 12
// 0 0 H C
int GB::op_ld_hl_sp_e8() {
    // TODO: implement LD HL, SP+e8
    return 0;
}

// 0xf9
// LD SP, HL
// 1 8
// - - - -
int GB::op_ld_sp_hl() {
    this->registers.SP = this->registers.get_hl();
    return 8;
}

// 0xfa
// LD A, [a16]
// 3 16
// - - - -
int GB::op_ld_a_a16m() {
    // TODO: implement LD A, [a16]
    return 0;
}

// 0xfb
// EI
// 1 4
// - - - -
int GB::op_ei() {
    // TODO: implement EI
    return 0;
}

// 0xfc
// Unused opcode
// - -
// - - - -
// int GB::op_unused_fc() {
//     return 0;
// }

// 0xfd
// Unused opcode
// - -
// - - - -
// int GB::op_unused_fd() {
//     return 0;
// }

// 0xfe
// CP A, n8
// 2 8
// Z 1 H C
int GB::op_cp_a_n8() {
    // TODO: implement CP A, n8
    return 0;
}

// 0xff
// RST $38
// 1 16
// - - - -
int GB::op_rst_38() {
    this->stack.push_word(static_cast<uint16_t>(this->registers.PC + 1));
    this->registers.PC = 0x0038;
    return 16;
}

void GB::init() {
    for (auto &o : this->ops)
        o = {&GB::op_unimplemented, "???", 1};

    auto set = [&](uint8_t opcode, OpFn fn, const char *name, uint8_t length) { this->ops[opcode] = {fn, name, length}; };

    // clang-format off
    // 0x00 - 0x0F
    set(0x00, &GB::op_nop,          "NOP",          1);
    set(0x01, &GB::op_ld_bc_n16,    "LD BC, n16",   3);
    set(0x02, &GB::op_ld_bcm_a,     "LD [BC], A",   1);
    set(0x03, &GB::op_inc_bc,       "INC BC",       1);
    set(0x04, &GB::op_inc_b,        "INC B",        1);
    set(0x05, &GB::op_dec_b,        "DEC B",        1);
    set(0x06, &GB::op_ld_b_n8,      "LD B, n8",     2);
    set(0x07, &GB::op_rlca,         "RLCA",         1);
    set(0x08, &GB::op_ld_a16m_sp,   "LD [a16], SP", 3);
    set(0x09, &GB::op_add_hl_bc,    "ADD HL, BC",   1);
    set(0x0A, &GB::op_ld_a_bcm,     "LD A, [BC]",   1);
    set(0x0B, &GB::op_dec_bc,       "DEC BC",       1);
    set(0x0C, &GB::op_inc_c,        "INC C",        1);
    set(0x0D, &GB::op_dec_c,        "DEC C",        1);
    set(0x0E, &GB::op_ld_c_n8,      "LD C, n8",     2);
    set(0x0F, &GB::op_rrca,         "RRCA",         1);

    // 0x10 - 0x1F
    set(0x10, &GB::op_stop_n8,      "STOP n8",      2);
    set(0x11, &GB::op_ld_de_n16,    "LD DE, n16",   3);
    set(0x12, &GB::op_ld_dem_a,     "LD [DE], A",   1);
    set(0x13, &GB::op_inc_de,       "INC DE",       1);
    set(0x14, &GB::op_inc_d,        "INC D",        1);
    set(0x15, &GB::op_dec_d,        "DEC D",        1);
    set(0x16, &GB::op_ld_d_n8,      "LD D, n8",     2);
    set(0x17, &GB::op_rla,          "RLA",          1);
    set(0x18, &GB::op_jr_e8,        "JR e8",        2);
    set(0x19, &GB::op_add_hl_de,    "ADD HL, DE",   1);
    set(0x1A, &GB::op_ld_a_dem,     "LD A, [DE]",   1);
    set(0x1B, &GB::op_dec_de,       "DEC DE",       1);
    set(0x1C, &GB::op_inc_e,        "INC E",        1);
    set(0x1D, &GB::op_dec_e,        "DEC E",        1);
    set(0x1E, &GB::op_ld_e_n8,      "LD E, n8",     2);
    set(0x1F, &GB::op_rra,          "RRA",          1);

    // 0x20 - 0x2F
    set(0x20, &GB::op_jr_nz_e8,     "JR NZ, e8",    2);
    set(0x21, &GB::op_ld_hl_a16,    "LD HL, n16",   3);
    set(0x22, &GB::op_ld_hlim_a,    "LD [HL+], A",  1);
    set(0x23, &GB::op_inc_hl,       "INC HL",       1);
    set(0x24, &GB::op_inc_h,        "INC H",        1);
    set(0x25, &GB::op_dec_h,        "DEC H",        1);
    set(0x26, &GB::op_ld_h_n8,      "LD H, n8",     2);
    set(0x27, &GB::op_daa,          "DAA",          1);
    set(0x28, &GB::op_jr_z_e8,      "JR Z, e8",     2);
    set(0x29, &GB::op_add_hl_hl,    "ADD HL, HL",   1);
    set(0x2A, &GB::op_ld_a_hlim,    "LD A, [HL+]",  1);
    set(0x2B, &GB::op_dec_hl,       "DEC HL",       1);
    set(0x2C, &GB::op_inc_l,        "INC L",        1);
    set(0x2D, &GB::op_dec_l,        "DEC L",        1);
    set(0x2E, &GB::op_ld_l_n8,      "LD L, n8",     2);
    set(0x2F, &GB::op_cpl,          "CPL",          1);

    // 0x30 - 0x3F
    set(0x30, &GB::op_jr_nc_e8,     "JR NC, e8",    2);
    set(0x31, &GB::op_ld_sp_a16,    "LD SP, n16",   3);
    set(0x32, &GB::op_ld_hldm_a,    "LD [HL-], A",  1);
    set(0x33, &GB::op_inc_sp,       "INC SP",       1);
    set(0x34, &GB::op_inc_hlm,      "INC [HL]",     1);
    set(0x35, &GB::op_dec_hlm,      "DEC [HL]",     1);
    set(0x36, &GB::op_ld_hlm_n8,    "LD [HL], n8",  2);
    set(0x37, &GB::op_scf,          "SCF",          1);
    set(0x38, &GB::op_jr_c_e8,      "JR C, e8",     2);
    set(0x39, &GB::op_add_hl_sp,    "ADD HL, SP",   1);
    set(0x3A, &GB::op_ld_a_hldm,    "LD A, [HL-]",  1);
    set(0x3B, &GB::op_dec_sp,       "DEC SP",       1);
    set(0x3C, &GB::op_inc_a,        "INC A",        1);
    set(0x3D, &GB::op_dec_a,        "DEC A",        1);
    set(0x3E, &GB::op_ld_a_n8,      "LD A, n8",     2);
    set(0x3F, &GB::op_ccf,          "CCF",          1);

    // 0x40 - 0x4F
    set(0x40, &GB::op_ld_b_b,       "LD B, B",      1);
    set(0x41, &GB::op_ld_b_c,       "LD B, C",      1);
    set(0x42, &GB::op_ld_b_d,       "LD B, D",      1);
    set(0x43, &GB::op_ld_b_e,       "LD B, E",      1);
    set(0x44, &GB::op_ld_b_h,       "LD B, H",      1);
    set(0x45, &GB::op_ld_b_l,       "LD B, L",      1);
    set(0x46, &GB::op_ld_b_hlm,     "LD B, [HL]",   1);
    set(0x47, &GB::op_ld_b_a,       "LD B, A",      1);
    set(0x48, &GB::op_ld_c_b,       "LD C, B",      1);
    set(0x49, &GB::op_ld_c_c,       "LD C, C",      1);
    set(0x4A, &GB::op_ld_c_d,       "LD C, D",      1);
    set(0x4B, &GB::op_ld_c_e,       "LD C, E",      1);
    set(0x4C, &GB::op_ld_c_h,       "LD C, H",      1);
    set(0x4D, &GB::op_ld_c_l,       "LD C, L",      1);
    set(0x4E, &GB::op_ld_c_hlm,     "LD C, [HL]",   1);
    set(0x4F, &GB::op_ld_c_a,       "LD C, A",      1);

    // 0x50 - 0x5F
    set(0x50, &GB::op_ld_d_b,       "LD D, B",      1);
    set(0x51, &GB::op_ld_d_c,       "LD D, C",      1);
    set(0x52, &GB::op_ld_d_d,       "LD D, D",      1);
    set(0x53, &GB::op_ld_d_e,       "LD D, E",      1);
    set(0x54, &GB::op_ld_d_h,       "LD D, H",      1);
    set(0x55, &GB::op_ld_d_l,       "LD D, L",      1);
    set(0x56, &GB::op_ld_d_hlm,     "LD D, [HL]",   1);
    set(0x57, &GB::op_ld_d_a,       "LD D, A",      1);
    set(0x58, &GB::op_ld_e_b,       "LD E, B",      1);
    set(0x59, &GB::op_ld_e_c,       "LD E, C",      1);
    set(0x5A, &GB::op_ld_e_d,       "LD E, D",      1);
    set(0x5B, &GB::op_ld_e_e,       "LD E, E",      1);
    set(0x5C, &GB::op_ld_e_h,       "LD E, H",      1);
    set(0x5D, &GB::op_ld_e_l,       "LD E, L",      1);
    set(0x5E, &GB::op_ld_e_hlm,     "LD E, [HL]",   1);
    set(0x5F, &GB::op_ld_e_a,       "LD E, A",      1);

    // 0x60 - 0x6F
    set(0x60, &GB::op_ld_h_b,       "LD H, B",      1);
    set(0x61, &GB::op_ld_h_c,       "LD H, C",      1);
    set(0x62, &GB::op_ld_h_d,       "LD H, D",      1);
    set(0x63, &GB::op_ld_h_e,       "LD H, E",      1);
    set(0x64, &GB::op_ld_h_h,       "LD H, H",      1);
    set(0x65, &GB::op_ld_h_l,       "LD H, L",      1);
    set(0x66, &GB::op_ld_h_hlm,     "LD H, [HL]",   1);
    set(0x67, &GB::op_ld_h_a,       "LD H, A",      1);
    set(0x68, &GB::op_ld_l_b,       "LD L, B",      1);
    set(0x69, &GB::op_ld_l_c,       "LD L, C",      1);
    set(0x6A, &GB::op_ld_l_d,       "LD L, D",      1);
    set(0x6B, &GB::op_ld_l_e,       "LD L, E",      1);
    set(0x6C, &GB::op_ld_l_h,       "LD L, H",      1);
    set(0x6D, &GB::op_ld_l_l,       "LD L, L",      1);
    set(0x6E, &GB::op_ld_l_hlm,     "LD L, [HL]",   1);
    set(0x6F, &GB::op_ld_l_a,       "LD L, A",      1);

    // 0x70 - 0x7F
    set(0x70, &GB::op_ld_hlm_b,     "LD [HL], B",   1);
    set(0x71, &GB::op_ld_hlm_c,     "LD [HL], C",   1);
    set(0x72, &GB::op_ld_hlm_d,     "LD [HL], D",   1);
    set(0x73, &GB::op_ld_hlm_e,     "LD [HL], E",   1);
    set(0x74, &GB::op_ld_hlm_h,     "LD [HL], H",   1);
    set(0x75, &GB::op_ld_hlm_l,     "LD [HL], L",   1);
    set(0x76, &GB::op_halt,         "HALT",         1);
    set(0x77, &GB::op_ld_hlm_a,     "LD [HL], A",   1);
    set(0x78, &GB::op_ld_a_b,       "LD A, B",      1);
    set(0x79, &GB::op_ld_a_c,       "LD A, C",      1);
    set(0x7A, &GB::op_ld_a_d,       "LD A, D",      1);
    set(0x7B, &GB::op_ld_a_e,       "LD A, E",      1);
    set(0x7C, &GB::op_ld_a_h,       "LD A, H",      1);
    set(0x7D, &GB::op_ld_a_l,       "LD A, L",      1);
    set(0x7E, &GB::op_ld_a_hlm,     "LD A, [HL]",   1);
    set(0x7F, &GB::op_ld_a_a,       "LD A, A",      1);

    // 0x80 - 0x8F
    set(0x80, &GB::op_add_a_b,      "ADD A, B",     1);
    set(0x81, &GB::op_add_a_c,      "ADD A, C",     1);
    set(0x82, &GB::op_add_a_d,      "ADD A, D",     1);
    set(0x83, &GB::op_add_a_e,      "ADD A, E",     1);
    set(0x84, &GB::op_add_a_h,      "ADD A, H",     1);
    set(0x85, &GB::op_add_a_l,      "ADD A, L",     1);
    set(0x86, &GB::op_add_a_hlm,    "ADD A, [HL]",  1);
    set(0x87, &GB::op_add_a_a,      "ADD A, A",     1);
    set(0x88, &GB::op_adc_a_b,      "ADC A, B",     1);
    set(0x89, &GB::op_adc_a_c,      "ADC A, C",     1);
    set(0x8A, &GB::op_adc_a_d,      "ADC A, D",     1);
    set(0x8B, &GB::op_adc_a_e,      "ADC A, E",     1);
    set(0x8C, &GB::op_adc_a_h,      "ADC A, H",     1);
    set(0x8D, &GB::op_adc_a_l,      "ADC A, L",     1);
    set(0x8E, &GB::op_adc_a_hlm,    "ADC A, [HL]",  1);
    set(0x8F, &GB::op_adc_a_a,      "ADC A, A",     1);

    // 0x90 - 0x9F
    set(0x90, &GB::op_sub_a_b,      "SUB A, B",     1);
    set(0x91, &GB::op_sub_a_c,      "SUB A, C",     1);
    set(0x92, &GB::op_sub_a_d,      "SUB A, D",     1);
    set(0x93, &GB::op_sub_a_e,      "SUB A, E",     1);
    set(0x94, &GB::op_sub_a_h,      "SUB A, H",     1);
    set(0x95, &GB::op_sub_a_l,      "SUB A, L",     1);
    set(0x96, &GB::op_sub_a_hlm,    "SUB A, [HL]",  1);
    set(0x97, &GB::op_sub_a_a,      "SUB A, A",     1);
    set(0x98, &GB::op_sbc_a_b,      "SBC A, B",     1);
    set(0x99, &GB::op_sbc_a_c,      "SBC A, C",     1);
    set(0x9A, &GB::op_sbc_a_d,      "SBC A, D",     1);
    set(0x9B, &GB::op_sbc_a_e,      "SBC A, E",     1);
    set(0x9C, &GB::op_sbc_a_h,      "SBC A, H",     1);
    set(0x9D, &GB::op_sbc_a_l,      "SBC A, L",     1);
    set(0x9E, &GB::op_sbc_a_hlm,    "SBC A, [HL]",  1);
    set(0x9F, &GB::op_sbc_a_a,      "SBC A, A",     1);

    // 0xA0 - 0xAF
    set(0xA0, &GB::op_and_a_b,      "AND A, B",     1);
    set(0xA1, &GB::op_and_a_c,      "AND A, C",     1);
    set(0xA2, &GB::op_and_a_d,      "AND A, D",     1);
    set(0xA3, &GB::op_and_a_e,      "AND A, E",     1);
    set(0xA4, &GB::op_and_a_h,      "AND A, H",     1);
    set(0xA5, &GB::op_and_a_l,      "AND A, L",     1);
    set(0xA6, &GB::op_and_a_hlm,    "AND A, [HL]",  1);
    set(0xA7, &GB::op_and_a_a,      "AND A, A",     1);
    set(0xA8, &GB::op_xor_a_b,      "XOR A, B",     1);
    set(0xA9, &GB::op_xor_a_c,      "XOR A, C",     1);
    set(0xAA, &GB::op_xor_a_d,      "XOR A, D",     1);
    set(0xAB, &GB::op_xor_a_e,      "XOR A, E",     1);
    set(0xAC, &GB::op_xor_a_h,      "XOR A, H",     1);
    set(0xAD, &GB::op_xor_a_l,      "XOR A, L",     1);
    set(0xAE, &GB::op_xor_a_hlm,    "XOR A, [HL]",  1);
    set(0xAF, &GB::op_xor_a_a,      "XOR A, A",     1);

    // 0xB0 - 0xBF
    set(0xB0, &GB::op_or_a_b,       "OR A, B",      1);
    set(0xB1, &GB::op_or_a_c,       "OR A, C",      1);
    set(0xB2, &GB::op_or_a_d,       "OR A, D",      1);
    set(0xB3, &GB::op_or_a_e,       "OR A, E",      1);
    set(0xB4, &GB::op_or_a_h,       "OR A, H",      1);
    set(0xB5, &GB::op_or_a_l,       "OR A, L",      1);
    set(0xB6, &GB::op_or_a_hlm,     "OR A, [HL]",   1);
    set(0xB7, &GB::op_or_a_a,       "OR A, A",      1);
    set(0xB8, &GB::op_cp_a_b,       "CP A, B",      1);
    set(0xB9, &GB::op_cp_a_c,       "CP A, C",      1);
    set(0xBA, &GB::op_cp_a_d,       "CP A, D",      1);
    set(0xBB, &GB::op_cp_a_e,       "CP A, E",      1);
    set(0xBC, &GB::op_cp_a_h,       "CP A, H",      1);
    set(0xBD, &GB::op_cp_a_l,       "CP A, L",      1);
    set(0xBE, &GB::op_cp_a_hlm,     "CP A, [HL]",   1);
    set(0xBF, &GB::op_cp_a_a,       "CP A, A",      1);

    // 0xC0 - 0xCF
    set(0xC0, &GB::op_ret_nz,       "RET NZ",       1);
    set(0xC1, &GB::op_pop_bc,       "POP BC",       1);
    set(0xC2, &GB::op_jp_nz_a16,    "JP NZ, a16",   3);
    set(0xC3, &GB::op_jp_a16,       "JP a16",       3);
    set(0xC4, &GB::op_call_nz_a16,  "CALL NZ, a16", 3);
    set(0xC5, &GB::op_push_bc,      "PUSH BC",      1);
    set(0xC6, &GB::op_add_a_n8,     "ADD A, n8",    2);
    set(0xC7, &GB::op_rst_00,       "RST $00",      1);
    set(0xC8, &GB::op_ret_z,        "RET Z",        1);
    set(0xC9, &GB::op_ret,          "RET",          1);
    set(0xCA, &GB::op_jp_z_a16,     "JP Z, a16",    3);
    set(0xCB, &GB::op_prefix,       "PREFIX",       1);
    set(0xCC, &GB::op_call_z_a16,   "CALL Z, a16",  3);
    set(0xCD, &GB::op_call_a16,     "CALL a16",     3);
    set(0xCE, &GB::op_adc_a_n8,     "ADC A, n8",    2);
    set(0xCF, &GB::op_rst_08,       "RST $08",      1);

    // 0xD0 - 0xDF
    set(0xD0, &GB::op_ret_nc,       "RET NC",       1);
    set(0xD1, &GB::op_pop_de,       "POP DE",       1);
    set(0xD2, &GB::op_jp_nc_a16,    "JP NC, a16",   3);
    // set(0xD3, &GB::op_unused_d3, "ILLEGAL (0xD3)", 1);
    set(0xD4, &GB::op_call_nc_a16,  "CALL NC, a16", 3);
    set(0xD5, &GB::op_push_de,      "PUSH DE",      1);
    set(0xD6, &GB::op_sub_a_n8,     "SUB A, n8",    2);
    set(0xD7, &GB::op_rst_10,       "RST $10",      1);
    set(0xD8, &GB::op_ret_c,        "RET C",        1);
    set(0xD9, &GB::op_reti,         "RETI",         1);
    set(0xDA, &GB::op_jp_c_a16,     "JP C, a16",    3);
    // set(0xDB, &GB::op_unused_db,    "ILLEGAL (0xDB)", 1);
    set(0xDC, &GB::op_call_c_a16,   "CALL C, a16",  3);
    // set(0xDD, &GB::op_unused_dd,    "ILLEGAL (0xDD)", 1);
    set(0xDE, &GB::op_sbc_a_n8,     "SBC A, n8",    2);
    set(0xDF, &GB::op_rst_18,       "RST $18",      1);

    // 0xE0 - 0xEF
    set(0xE0, &GB::op_ldh_a8m_a,    "LDH [a8], A",  2);
    set(0xE1, &GB::op_pop_hl,       "POP HL",       1);
    set(0xE2, &GB::op_ldh_cm_a,    "LDH [C], A",    1);
    // set(0xE3, &GB::op_unused_e3,    "ILLEGAL (0xE3)", 1);
    // set(0xE4, &GB::op_unused_e4,    "ILLEGAL (0xE4)", 1);
    set(0xE5, &GB::op_push_hl,      "PUSH HL",      1);
    set(0xE6, &GB::op_and_a_n8,     "AND A, n8",    2);
    set(0xE7, &GB::op_rst_20,       "RST $20",      1);
    set(0xE8, &GB::op_add_sp_e8,    "ADD SP, e8",   2);
    set(0xE9, &GB::op_jp_hl,        "JP HL",        1);
    set(0xEA, &GB::op_ld_a16m_a,    "LD [a16], A",  3);
    // set(0xEB, &GB::op_unused_eb,    "ILLEGAL (0xEB)", 1);
    // set(0xEC, &GB::op_unused_ec,    "ILLEGAL (0xEC)", 1);
    // set(0xED, &GB::op_unused_ed,    "ILLEGAL (0xED)", 1);
    set(0xEE, &GB::op_xor_a_n8,     "XOR A, n8",    2);
    set(0xEF, &GB::op_rst_28,       "RST $28",      1);

    // 0xF0 - 0xFF
    set(0xF0, &GB::op_ldh_a_a8m,    "LDH A, [a8]",  2);
    set(0xF1, &GB::op_pop_af,       "POP AF",       1);
    set(0xF2, &GB::op_ldh_a_cm,      "LDH A, [C]",  1);
    set(0xF3, &GB::op_di,           "DI",           1);
    // set(0xF4, &GB::op_unused_f4,    "ILLEGAL (0xF4)", 1);
    set(0xF5, &GB::op_push_af,      "PUSH AF",      1);
    set(0xF6, &GB::op_or_a_n8,      "OR A, n8",     2);
    set(0xF7, &GB::op_rst_30,       "RST $30",      1);
    set(0xF8, &GB::op_ld_hl_sp_e8,  "LD HL, SP+e8", 2);
    set(0xF9, &GB::op_ld_sp_hl,     "LD SP, HL",    1);
    set(0xFA, &GB::op_ld_a_a16m,    "LD A, [a16]",  3);
    set(0xFB, &GB::op_ei,           "EI",           1);
    // set(0xFC, &GB::op_unused_fc,    "ILLEGAL (0xFC)", 1);
    // set(0xFD, &GB::op_unused_fd,    "ILLEGAL (0xFD)", 1);
    set(0xFE, &GB::op_cp_a_n8,      "CP A, n8",     2);
    set(0xFF, &GB::op_rst_38,       "RST $38",      1);
    // clang-format on
}
