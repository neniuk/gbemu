#ifndef GB_H
#define GB_H

#include "config.h"
#include "memory.h"
#include "registers.h"
#include "screen.h"
#include "stack.h"

#include <array>
#include <stdbool.h>
#include <stdint.h>
#include <vector>

class GB {
  public:
    GB();
    Memory memory;
    Registers registers;
    Stack stack;
    Screen screen;

    void load(const std::vector<uint8_t> &buf);

    using OpFn = int (GB::*)();

    struct OpInfo {
        OpFn fn;
        const char *name;
        uint8_t length;
    };

    std::array<OpInfo, 256> ops{};

    void init();

    // opcode implementations
    int op_unimplemented();

    // Opcode
    // Mnemonic
    // Length in bytes (pc increment) & Duration in T-states (cycleCount increment)
    // Flags affected (Z, N, H, C) - Zero, Subtract, Half-carry, Carry

    // 0x00
    // NOP
    // 1 4
    // - - - -
    int op_nop();

    // 0x01
    // LD BC, n16
    // 3 12
    // - - - -
    int op_ld_bc_n16();

    // 0x02
    // LD [BC], A
    // 1 8
    // - - - -
    int op_ld_bcm_a();

    // 0x03
    // INC BC
    // 1 8
    // - - - -
    int op_inc_bc();

    // 0x04
    // INC B
    // 1 4
    // Z 0 H -
    int op_inc_b();

    // 0x05
    // DEC B
    // 1 4
    // Z 1 H -
    int op_dec_b();

    // 0x06
    // LD B, n8
    // 2 8
    // - - - -
    int op_ld_b_n8();

    // 0x07
    // RLCA
    // 1 4
    // 0 0 0 C
    int op_rlca();

    // 0x08
    // LD [a16], SP
    // 3 20
    // - - - -
    int op_ld_a16m_sp();

    // 0x09
    // ADD HL, BC
    // 1 8
    // - 0 H C
    int op_add_hl_bc();

    // 0x0a
    // LD A, [BC]
    // 1 8
    // - - - -
    int op_ld_a_bcm();

    // 0x0b
    // DEC BC
    // 1 8
    // - - - -
    int op_dec_bc();

    // 0x0c
    // INC C
    // 1 4
    // Z 0 H -
    int op_inc_c();

    // 0x0d
    // DEC C
    // 1 4
    // Z 1 H -
    int op_dec_c();

    // 0x0e
    // LD C, n8
    // 2 8
    // - - - -
    int op_ld_c_n8();

    // 0x0f
    // RRCA
    // 1 4
    // 0 0 0 C
    int op_rrca();

    // 0x10
    // STOP n8
    // 2 4
    // - - - -
    int op_stop_n8();

    // 0x11
    // LD DE, n16
    // 3 12
    // - - - -
    int op_ld_de_n16();

    // 0x12
    // LD [DE], A
    // 1 8
    // - - - -
    int op_ld_dem_a();

    // 0x13
    // INC DE
    // 1 8
    // - - - -
    int op_inc_de();

    // 0x14
    // INC D
    // 1 4
    // Z 0 H -
    int op_inc_d();

    // 0x15
    // DEC D
    // 1 4
    // Z 1 H -
    int op_dec_d();

    // 0x16
    // LD D, n8
    // 2 8
    // - - - -
    int op_ld_d_n8();

    // 0x17
    // RLA
    // 1 4
    // 0 0 0 C
    int op_rla();

    // 0x18
    // JR e8
    // 2 12
    // - - - -
    int op_jr_e8();

    // 0x19
    // ADD HL, DE
    // 1 8
    // - 0 H C
    int op_add_hl_de();

    // 0x1a
    // LD A, [DE]
    // 1 8
    // - - - -
    int op_ld_a_dem();

    // 0x1b
    // DEC DE
    // 1 8
    // - - - -
    int op_dec_de();

    // 0x1c
    // INC E
    // 1 4
    // Z 0 H -
    int op_inc_e();

    // 0x1d
    // DEC E
    // 1 4
    // Z 1 H -
    int op_dec_e();

    // 0x1e
    // LD E, n8
    // 2 8
    // - - - -
    int op_ld_e_n8();

    // 0x1f
    // RRA
    // 1 4
    // 0 0 0 C
    int op_rra();

    // 0x20
    // JR NZ, e8
    // 2 12/8
    // - - - -
    int op_jr_nz_e8();

    // 0x21
    // LD HL, n16
    // 3 12
    // - - - -
    int op_ld_hl_a16();

    // 0x22
    // LD [HL+], A
    // 1 8
    // - - - -
    int op_ld_hlim_a();

    // 0x23
    // INC HL
    // 1 8
    // - - - -
    int op_inc_hl();

    // 0x24
    // INC H
    // 1 4
    // Z 0 H -
    int op_inc_h();

    // 0x25
    // DEC H
    // 1 4
    // Z 1 H -
    int op_dec_h();

    // 0x26
    // LD H, n8
    // 2 8
    // - - - -
    int op_ld_h_n8();

    // 0x27
    // DAA
    // 1 4
    // Z - H C
    int op_daa();

    // 0x28
    // JR Z, e8
    // 2 12/8
    // - - - -
    int op_jr_z_e8();

    // 0x29
    // ADD HL, HL
    // 1 8
    // - 0 H C
    int op_add_hl_hl();

    // 0x2a
    // LD A, [HL+]
    // 1 8
    // - - - -
    int op_ld_a_hlim();

    // 0x2b
    // DEC HL
    // 1 8
    // - - - -
    int op_dec_hl();

    // 0x2c
    // INC L
    // 1 4
    // Z 0 H -
    int op_inc_l();

    // 0x2d
    // DEC L
    // 1 4
    // Z 1 H -
    int op_dec_l();

    // 0x2e
    // LD L, n8
    // 2 8
    // - - - -
    int op_ld_l_n8();

    // 0x2f
    // CPL
    // 1 4
    // - 1 1 -
    int op_cpl();

    // 0x30
    // JR NC, e8
    // 2 12/8
    // - - - -
    int op_jr_nc_e8();

    // 0x31
    // LD SP, n16
    // 3 12
    // - - - -
    int op_ld_sp_a16();

    // 0x32
    // LD [HL-], A
    // 1 8
    // - - - -
    int op_ld_hldm_a();

    // 0x33
    // INC SP
    // 1 8
    // - - - -
    int op_inc_sp();

    // 0x34
    // INC [HL]
    // 1 12
    // Z 0 H -
    int op_inc_hlm();

    // 0x35
    // DEC [HL]
    // 1 12
    // Z 1 H -
    int op_dec_hlm();

    // 0x36
    // LD [HL], n8
    // 2 12
    // - - - -
    int op_ld_hlm_n8();

    // 0x37
    // SCF
    // 1 4
    // - 0 0 1
    int op_scf();

    // 0x38
    // JR C, e8
    // 2 12/8
    // - - - -
    int op_jr_c_e8();

    // 0x39
    // ADD HL, SP
    // 1 8
    // - 0 H C
    int op_add_hl_sp();

    // 0x3a
    // LD A, [HL-]
    // 1 8
    // - - - -
    int op_ld_a_hldm();

    // 0x3b
    // DEC SP
    // 1 8
    // - - - -
    int op_dec_sp();

    // 0x3c
    // INC A
    // 1 4
    // Z 0 H -
    int op_inc_a();

    // 0x3d
    // DEC A
    // 1 4
    // Z 1 H -
    int op_dec_a();

    // 0x3e
    // LD A, n8
    // 2 8
    // - - - -
    int op_ld_a_n8();

    // 0x3f
    // CCF
    // 1 4
    // - 0 0 C
    int op_ccf();

    // 0x40
    // LD B, B
    // 1 4
    // - - - -
    int op_ld_b_b();

    // 0x41
    // LD B, C
    // 1 4
    // - - - -
    int op_ld_b_c();

    // 0x42
    // LD B, D
    // 1 4
    // - - - -
    int op_ld_b_d();

    // 0x43
    // LD B, E
    // 1 4
    // - - - -
    int op_ld_b_e();

    // 0x44
    // LD B, H
    // 1 4
    // - - - -
    int op_ld_b_h();

    // 0x45
    // LD B, L
    // 1 4
    // - - - -
    int op_ld_b_l();

    // 0x46
    // LD B, [HL]
    // 1 8
    // - - - -
    int op_ld_b_hlm();

    // 0x47
    // LD B, A
    // 1 4
    // - - - -
    int op_ld_b_a();

    // 0x48
    // LD C, B
    // 1 4
    // - - - -
    int op_ld_c_b();

    // 0x49
    // LD C, C
    // 1 4
    // - - - -
    int op_ld_c_c();

    // 0x4a
    // LD C, D
    // 1 4
    // - - - -
    int op_ld_c_d();

    // 0x4b
    // LD C, E
    // 1 4
    // - - - -
    int op_ld_c_e();

    // 0x4c
    // LD C, H
    // 1 4
    // - - - -
    int op_ld_c_h();

    // 0x4d
    // LD C, L
    // 1 4
    // - - - -
    int op_ld_c_l();

    // 0x4e
    // LD C, [HL]
    // 1 8
    // - - - -
    int op_ld_c_hlm();

    // 0x4f
    // LD C, A
    // 1 4
    // - - - -
    int op_ld_c_a();

    // 0x50
    // LD D, B
    // 1 4
    // - - - -
    int op_ld_d_b();

    // 0x51
    // LD D, C
    // 1 4
    // - - - -
    int op_ld_d_c();

    // 0x52
    // LD D, D
    // 1 4
    // - - - -
    int op_ld_d_d();

    // 0x53
    // LD D, E
    // 1 4
    // - - - -
    int op_ld_d_e();

    // 0x54
    // LD D, H
    // 1 4
    // - - - -
    int op_ld_d_h();

    // 0x55
    // LD D, L
    // 1 4
    // - - - -
    int op_ld_d_l();

    // 0x56
    // LD D, [HL]
    // 1 8
    // - - - -
    int op_ld_d_hlm();

    // 0x57
    // LD D, A
    // 1 4
    // - - - -
    int op_ld_d_a();

    // 0x58
    // LD E, B
    // 1 4
    // - - - -
    int op_ld_e_b();

    // 0x59
    // LD E, C
    // 1 4
    // - - - -
    int op_ld_e_c();

    // 0x5a
    // LD E, D
    // 1 4
    // - - - -
    int op_ld_e_d();

    // 0x5b
    // LD E, E
    // 1 4
    // - - - -
    int op_ld_e_e();

    // 0x5c
    // LD E, H
    // 1 4
    // - - - -
    int op_ld_e_h();

    // 0x5d
    // LD E, L
    // 1 4
    // - - - -
    int op_ld_e_l();

    // 0x5e
    // LD E, [HL]
    // 1 8
    // - - - -
    int op_ld_e_hlm();

    // 0x5f
    // LD E, A
    // 1 4
    // - - - -
    int op_ld_e_a();

    // 0x60
    // LD H, B
    // 1 4
    // - - - -
    int op_ld_h_b();

    // 0x61
    // LD H, C
    // 1 4
    // - - - -
    int op_ld_h_c();

    // 0x62
    // LD H, D
    // 1 4
    // - - - -
    int op_ld_h_d();

    // 0x63
    // LD H, E
    // 1 4
    // - - - -
    int op_ld_h_e();

    // 0x64
    // LD H, H
    // 1 4
    // - - - -
    int op_ld_h_h();

    // 0x65
    // LD H, L
    // 1 4
    // - - - -
    int op_ld_h_l();

    // 0x66
    // LD H, [HL]
    // 1 8
    // - - - -
    int op_ld_h_hlm();

    // 0x67
    // LD H, A
    // 1 4
    // - - - -
    int op_ld_h_a();

    // 0x68
    // LD L, B
    // 1 4
    // - - - -
    int op_ld_l_b();

    // 0x69
    // LD L, C
    // 1 4
    // - - - -
    int op_ld_l_c();

    // 0x6a
    // LD L, D
    // 1 4
    // - - - -
    int op_ld_l_d();

    // 0x6b
    // LD L, E
    // 1 4
    // - - - -
    int op_ld_l_e();

    // 0x6c
    // LD L, H
    // 1 4
    // - - - -
    int op_ld_l_h();

    // 0x6d
    // LD L, L
    // 1 4
    // - - - -
    int op_ld_l_l();

    // 0x6e
    // LD L, [HL]
    // 1 8
    // - - - -
    int op_ld_l_hlm();

    // 0x6f
    // LD L, A
    // 1 4
    // - - - -
    int op_ld_l_a();

    // 0x70
    // LD [HL], B
    // 1 8
    // - - - -
    int op_ld_hlm_b();

    // 0x71
    // LD [HL], C
    // 1 8
    // - - - -
    int op_ld_hlm_c();

    // 0x72
    // LD [HL], D
    // 1 8
    // - - - -
    int op_ld_hlm_d();

    // 0x73
    // LD [HL], E
    // 1 8
    // - - - -
    int op_ld_hlm_e();

    // 0x74
    // LD [HL], H
    // 1 8
    // - - - -
    int op_ld_hlm_h();

    // 0x75
    // LD [HL], L
    // 1 8
    // - - - -
    int op_ld_hlm_l();

    // 0x76
    // HALT
    // 1 4
    // - - - -
    int op_halt();

    // 0x77
    // LD [HL], A
    // 1 8
    // - - - -
    int op_ld_hlm_a();

    // 0x78
    // LD A, B
    // 1 4
    // - - - -
    int op_ld_a_b();

    // 0x79
    // LD A, C
    // 1 4
    // - - - -
    int op_ld_a_c();

    // 0x7a
    // LD A, D
    // 1 4
    // - - - -
    int op_ld_a_d();

    // 0x7b
    // LD A, E
    // 1 4
    // - - - -
    int op_ld_a_e();

    // 0x7c
    // LD A, H
    // 1 4
    // - - - -
    int op_ld_a_h();

    // 0x7d
    // LD A, L
    // 1 4
    // - - - -
    int op_ld_a_l();

    // 0x7e
    // LD A, [HL]
    // 1 8
    // - - - -
    int op_ld_a_hlm();

    // 0x7f
    // LD A, A
    // 1 4
    // - - - -
    int op_ld_a_a();

    // 0x80
    // ADD A, B
    // 1 4
    // Z 0 H C
    int op_add_a_b();

    // 0x81
    // ADD A, C
    // 1 4
    // Z 0 H C
    int op_add_a_c();

    // 0x82
    // ADD A, D
    // 1 4
    // Z 0 H C
    int op_add_a_d();

    // 0x83
    // ADD A, E
    // 1 4
    // Z 0 H C
    int op_add_a_e();

    // 0x84
    // ADD A, H
    // 1 4
    // Z 0 H C
    int op_add_a_h();

    // 0x85
    // ADD A, L
    // 1 4
    // Z 0 H C
    int op_add_a_l();

    // 0x86
    // ADD A, [HL]
    // 1 8
    // Z 0 H C
    int op_add_a_hlm();

    // 0x87
    // ADD A, A
    // 1 4
    // Z 0 H C
    int op_add_a_a();

    // 0x88
    // ADC A, B
    // 1 4
    // Z 0 H C
    int op_adc_a_b();

    // 0x89
    // ADC A, C
    // 1 4
    // Z 0 H C
    int op_adc_a_c();

    // 0x8a
    // ADC A, D
    // 1 4
    // Z 0 H C
    int op_adc_a_d();

    // 0x8b
    // ADC A, E
    // 1 4
    // Z 0 H C
    int op_adc_a_e();

    // 0x8c
    // ADC A, H
    // 1 4
    // Z 0 H C
    int op_adc_a_h();

    // 0x8d
    // ADC A, L
    // 1 4
    // Z 0 H C
    int op_adc_a_l();

    // 0x8e
    // ADC A, [HL]
    // 1 8
    // Z 0 H C
    int op_adc_a_hlm();

    // 0x8f
    // ADC A, A
    // 1 4
    // Z 0 H C
    int op_adc_a_a();

    // 0x90
    // SUB A, B
    // 1 4
    // Z 1 H C
    int op_sub_a_b();

    // 0x91
    // SUB A, C
    // 1 4
    // Z 1 H C
    int op_sub_a_c();

    // 0x92
    // SUB A, D
    // 1 4
    // Z 1 H C
    int op_sub_a_d();

    // 0x93
    // SUB A, E
    // 1 4
    // Z 1 H C
    int op_sub_a_e();

    // 0x94
    // SUB A, H
    // 1 4
    // Z 1 H C
    int op_sub_a_h();

    // 0x95
    // SUB A, L
    // 1 4
    // Z 1 H C
    int op_sub_a_l();

    // 0x96
    // SUB A, [HL]
    // 1 8
    // Z 1 H C
    int op_sub_a_hlm();

    // 0x97
    // SUB A, A
    // 1 4
    // Z 1 H C
    int op_sub_a_a();

    // 0x98
    // SBC A, B
    // 1 4
    // Z 1 H C
    int op_sbc_a_b();

    // 0x99
    // SBC A, C
    // 1 4
    // Z 1 H C
    int op_sbc_a_c();

    // 0x9a
    // SBC A, D
    // 1 4
    // Z 1 H C
    int op_sbc_a_d();

    // 0x9b
    // SBC A, E
    // 1 4
    // Z 1 H C
    int op_sbc_a_e();

    // 0x9c
    // SBC A, H
    // 1 4
    // Z 1 H C
    int op_sbc_a_h();

    // 0x9d
    // SBC A, L
    // 1 4
    // Z 1 H C
    int op_sbc_a_l();

    // 0x9e
    // SBC A, [HL]
    // 1 8
    // Z 1 H C
    int op_sbc_a_hlm();

    // 0x9f
    // SBC A, A
    // 1 4
    // Z 1 H C
    int op_sbc_a_a();

    // 0xa0
    // AND A, B
    // 1 4
    // Z 0 1 0
    int op_and_a_b();

    // 0xa1
    // AND A, C
    // 1 4
    // Z 0 1 0
    int op_and_a_c();

    // 0xa2
    // AND A, D
    // 1 4
    // Z 0 1 0
    int op_and_a_d();

    // 0xa3
    // AND A, E
    // 1 4
    // Z 0 1 0
    int op_and_a_e();

    // 0xa4
    // AND A, H
    // 1 4
    // Z 0 1 0
    int op_and_a_h();

    // 0xa5
    // AND A, L
    // 1 4
    // Z 0 1 0
    int op_and_a_l();

    // 0xa6
    // AND A, [HL]
    // 1 8
    // Z 0 1 0
    int op_and_a_hlm();

    // 0xa7
    // AND A, A
    // 1 4
    // Z 0 1 0
    int op_and_a_a();

    // 0xa8
    // XOR A, B
    // 1 4
    // Z 0 0 0
    int op_xor_a_b();

    // 0xa9
    // XOR A, C
    // 1 4
    // Z 0 0 0
    int op_xor_a_c();

    // 0xaa
    // XOR A, D
    // 1 4
    // Z 0 0 0
    int op_xor_a_d();

    // 0xab
    // XOR A, E
    // 1 4
    // Z 0 0 0
    int op_xor_a_e();

    // 0xac
    // XOR A, H
    // 1 4
    // Z 0 0 0
    int op_xor_a_h();

    // 0xad
    // XOR A, L
    // 1 4
    // Z 0 0 0
    int op_xor_a_l();

    // 0xae
    // XOR A, [HL]
    // 1 8
    // Z 0 0 0
    int op_xor_a_hlm();

    // 0xaf
    // XOR A, A
    // 1 4
    // Z 0 0 0
    int op_xor_a_a();

    // 0xb0
    // OR A, B
    // 1 4
    // Z 0 0 0
    int op_or_a_b();

    // 0xb1
    // OR A, C
    // 1 4
    // Z 0 0 0
    int op_or_a_c();

    // 0xb2
    // OR A, D
    // 1 4
    // Z 0 0 0
    int op_or_a_d();

    // 0xb3
    // OR A, E
    // 1 4
    // Z 0 0 0
    int op_or_a_e();

    // 0xb4
    // OR A, H
    // 1 4
    // Z 0 0 0
    int op_or_a_h();

    // 0xb5
    // OR A, L
    // 1 4
    // Z 0 0 0
    int op_or_a_l();

    // 0xb6
    // OR A, [HL]
    // 1 8
    // Z 0 0 0
    int op_or_a_hlm();

    // 0xb7
    // OR A, A
    // 1 4
    // Z 0 0 0
    int op_or_a_a();

    // 0xb8
    // CP A, B
    // 1 4
    // Z 1 H C
    int op_cp_a_b();

    // 0xb9
    // CP A, C
    // 1 4
    // Z 1 H C
    int op_cp_a_c();

    // 0xba
    // CP A, D
    // 1 4
    // Z 1 H C
    int op_cp_a_d();

    // 0xbb
    // CP A, E
    // 1 4
    // Z 1 H C
    int op_cp_a_e();

    // 0xbc
    // CP A, H
    // 1 4
    // Z 1 H C
    int op_cp_a_h();

    // 0xbd
    // CP A, L
    // 1 4
    // Z 1 H C
    int op_cp_a_l();

    // 0xbe
    // CP A, [HL]
    // 1 8
    // Z 1 H C
    int op_cp_a_hlm();

    // 0xbf
    // CP A, A
    // 1 4
    // 1 1 0 0
    int op_cp_a_a();

    // 0xc0
    // RET NZ
    // 1 20/8
    // - - - -
    int op_ret_nz();

    // 0xc1
    // POP BC
    // 1 12
    // - - - -
    int op_pop_bc();

    // 0xc2
    // JP NZ, a16
    // 3 16/12
    // - - - -
    int op_jp_nz_a16();

    // 0xc3
    // JP a16
    // 3 16
    // - - - -
    int op_jp_a16();

    // 0xc4
    // CALL NZ, a16
    // 3 24/12
    // - - - -
    int op_call_nz_a16();

    // 0xc5
    // PUSH BC
    // 1 16
    // - - - -
    int op_push_bc();

    // 0xc6
    // ADD A, n8
    // 2 8
    // Z 0 H C
    int op_add_a_n8();

    // 0xc7
    // RST $00
    // 1 16
    // - - - -
    int op_rst_00();

    // 0xc8
    // RET Z
    // 1 11/5
    // - - - -
    int op_ret_z();

    // 0xc9
    // RET
    // 1 10
    // - - - -
    int op_ret();

    // 0xca
    // JP Z, a16
    // 3 16/12
    // - - - -
    int op_jp_z_a16();

    // 0xcb
    // PREFIX
    // 1 4
    // - - - -
    int op_prefix();

    // 0xcc
    // CALL Z, a16
    // 3 24/12
    // - - - -
    int op_call_z_a16();

    // 0xcd
    // CALL a16
    // 3 24
    // - - - -
    int op_call_a16();

    // 0xce
    // ADC A, n8
    // 2 8
    // Z 0 H C
    int op_adc_a_n8();

    // 0xcf
    // RST $08
    // 1 16
    // - - - -
    int op_rst_08();

    // 0xd0
    // RET NC
    // 1 20/8
    // - - - -
    int op_ret_nc();

    // 0xd1
    // POP DE
    // 1 12
    // - - - -
    int op_pop_de();

    // 0xd2
    // JP NC, a16
    // 3 16/12
    // - - - -
    int op_jp_nc_a16();

    // 0xd3
    // Unused opcode
    // 1 4
    // - - - -
    int op_undefined_d3();

    // 0xd4
    // CALL NC, a16
    // 3 24/12
    // - - - -
    int op_call_nc_a16();

    // 0xd5
    // PUSH DE
    // 1 16
    // - - - -
    int op_push_de();

    // 0xd6
    // SUB A, n8
    // 2 8
    // Z 1 H C
    int op_sub_a_n8();

    // 0xd7
    // RST $10
    // 1 16
    // - - - -
    int op_rst_10();

    // 0xd8
    // RET C
    // 1 20/8
    // - - - -
    int op_ret_c();

    // 0xd9
    // RETI
    // 1 16
    // - - - -
    int op_reti();

    // 0xda
    // JP C, a16
    // 3 16/12
    // - - - -
    int op_jp_c_a16();

    // 0xdb
    // Unused opcode
    // - -
    // - - - -
    int op_unused_db();

    // 0xdc
    // CALL C, a16
    // 3 24/12
    // - - - -
    int op_call_c_a16();

    // 0xdd
    // Unused opcode
    // - -
    // - - - -
    int op_unused_dd();

    // 0xde
    // SBC A, n8
    // 2 8
    // Z 1 H C
    int op_sbc_a_n8();

    // 0xdf
    // RST $18
    // 1 16
    // - - - -
    int op_rst_18();

    // 0xe0
    // LDH [a8], A
    // 2 12
    // - - - -
    int op_ldh_a8m_a();

    // 0xe1
    // POP HL
    // 1 12
    // - - - -
    int op_pop_hl();

    // 0xe2
    // LDH [C], A
    // 2 8
    // - - - -
    int op_ldh_cm_a();

    // 0xe3
    // Unused opcode
    // - -
    // - - - -
    int op_unused_e3();

    // 0xe4
    // Unused opcode
    // - -
    // - - - -
    int op_unused_e4();

    // 0xe5
    // PUSH HL
    // 1 16
    // - - - -
    int op_push_hl();

    // 0xe6
    // AND A, n8
    // 2 8
    // Z 0 1 0
    int op_and_a_n8();

    // 0xe7
    // RST $20
    // 1 16
    // - - - -
    int op_rst_20();

    // 0xe8
    // ADD SP, e8
    // 2 16
    // 0 0 H C
    int op_add_sp_e8();

    // 0xe9
    // JP HL
    // 1 4
    // - - - -
    int op_jp_hl();

    // 0xea
    // LD [a16], A
    // 3 16
    // - - - -
    int op_ld_a16m_a();

    // 0xeb
    // Unused opcode
    // - -
    // - - - -
    int op_unused_eb();

    // 0xec
    // Unused opcode
    // - -
    // - - - -
    int op_unused_ec();

    // 0xed
    // Unused opcode
    // - -
    // - - - -
    int op_unused_ed();

    // 0xee
    // XOR A, n8
    // 2 8
    // Z 0 0 0
    int op_xor_a_n8();

    // 0xef
    // RST $28
    // 1 16
    // - - - -
    int op_rst_28();

    // 0xf0
    // LDH A, [a8]
    // 2 12
    // - - - -
    int op_ldh_a_a8m();

    // 0xf1
    // POP AF
    // 1 12
    // - - - -
    int op_pop_af();

    // 0xf2
    // LDH A, [C]
    // 2 8
    // - - - -
    int op_ldh_a_cm();

    // 0xf3
    // DI
    // 1 4
    // - - - -
    int op_di();

    // 0xf4
    // Unused opcode
    // - -
    // - - - -
    int op_unused_f4();

    // 0xf5
    // PUSH AF
    // 1 16
    // - - - -
    int op_push_af();

    // 0xf6
    // OR A, n8
    // 2 8
    // Z 0 0 0
    int op_or_a_n8();

    // 0xf7
    // RST $30
    // 1 16
    // - - - -
    int op_rst_30();

    // 0xf8
    // LD HL, SP+e8
    // 3 12
    // 0 0 H C
    int op_ld_hl_sp_e8();

    // 0xf9
    // LD SP, HL
    // 1 8
    // - - - -
    int op_ld_sp_hl();

    // 0xfa
    // LD A, [a16]
    // 3 16
    // - - - -
    int op_ld_a_a16m();

    // 0xfb
    // EI
    // 1 4
    // - - - -
    int op_ei();

    // 0xfc
    // Unused opcode
    // - -
    // - - - -
    int op_unused_fc();

    // 0xfd
    // Unused opcode
    // - -
    // - - - -
    int op_unused_fd();

    // 0xfe
    // CP A, n8
    // 2 8
    // Z 1 H C
    int op_cp_a_n8();

    // 0xff
    // RST $38
    // 1 16
    // - - - -
    int op_rst_38();
};

#endif
