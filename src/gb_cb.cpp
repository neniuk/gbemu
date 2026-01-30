#include "gb.h"

// CB instruction stubs

// RLC r8
// 2 8
// Z 0 0 C
int GB::op_cb_rlc_b() {
    this->bmi.rlc_u8(this->registers.B);
    return 8;
}
int GB::op_cb_rlc_c() {
    this->bmi.rlc_u8(this->registers.C);
    return 8;
}
int GB::op_cb_rlc_d() {
    this->bmi.rlc_u8(this->registers.D);
    return 8;
}
int GB::op_cb_rlc_e() {
    this->bmi.rlc_u8(this->registers.E);
    return 8;
}
int GB::op_cb_rlc_h() {
    this->bmi.rlc_u8(this->registers.H);
    return 8;
}
int GB::op_cb_rlc_l() {
    this->bmi.rlc_u8(this->registers.L);
    return 8;
}
int GB::op_cb_rlc_hlm() {

    this->bmi.rlc_u8(this->registers.get_hl());
    return 16
}
int GB::op_cb_rlc_a() {
    this->bmi.rlc_u8(this->registers.A);
    return 8;
}

// Rotates (RRC)
int GB::op_cb_rrc_b() {
    this->bmi.rrc_u8(this->registers.B);
    return 8;
}
int GB::op_cb_rrc_c() {
    this->bmi.rrc_u8(this->registers.C);
    return 8;
}
int GB::op_cb_rrc_d() {
    this->bmi.rrc_u8(this->registers.D);
    return 8;
}
int GB::op_cb_rrc_e() {
    this->bmi.rrc_u8(this->registers.E);
    return 8;
}
int GB::op_cb_rrc_h() {
    this->bmi.rrc_u8(this->registers.H);
    return 8;
}
int GB::op_cb_rrc_l() {
    this->bmi.rrc_u8(this->registers.L);
    return 8;
}
int GB::op_cb_rrc_hlm() {
    this->bmi.rrc_u8(this->registers.get_hl());
    return 16;
}
int GB::op_cb_rrc_a() {
    this->bmi.rrc_u8(this->registers.A);
    return 8;
}

// Rotates (RL)
int GB::op_cb_rl_b() {
    this->bmi.rl_u8(this->registers.B);
    return 8;
}
int GB::op_cb_rl_c() {
    this->bmi.rl_u8(this->registers.C);
    return 8;
}
int GB::op_cb_rl_d() {
    this->bmi.rl_u8(this->registers.D);
    return 8;
}
int GB::op_cb_rl_e() {
    this->bmi.rl_u8(this->registers.E);
    return 8;
}
int GB::op_cb_rl_h() {
    this->bmi.rl_u8(this->registers.H);
    return 8;
}
int GB::op_cb_rl_l() {
    this->bmi.rl_u8(this->registers.L);
    return 8;
}
int GB::op_cb_rl_hlm() {
    this->bmi.rl_u8(this->registers.get_hl());
    return 16;
}
int GB::op_cb_rl_a() {
    this->bmi.rl_u8(this->registers.A);
    return 8;
}

// Rotates (RR)
int GB::op_cb_rr_b() {
    this->bmi.rr_u8(this->registers.B);
    return 8;
}
int GB::op_cb_rr_c() {
    this->bmi.rr_u8(this->registers.C);
    return 8;
}
int GB::op_cb_rr_d() {
    this->bmi.rr_u8(this->registers.D);
    return 8;
}
int GB::op_cb_rr_e() {
    this->bmi.rr_u8(this->registers.E);
    return 8;
}
int GB::op_cb_rr_h() {
    this->bmi.rr_u8(this->registers.H);
    return 8;
}
int GB::op_cb_rr_l() {
    this->bmi.rr_u8(this->registers.L);
    return 8;
}
int GB::op_cb_rr_hlm() {
    this->bmi.rr_u8(this->registers.get_hl());
    return 16;
}
int GB::op_cb_rr_a() {
    this->bmi.rr_u8(this->registers.A);
    return 8;
}

// Shifts (SLA)
int GB::op_cb_sla_b() {
    this->bmi.sla_u8(this->registers.B);
    return 8;
}
int GB::op_cb_sla_c() {
    this->bmi.sla_u8(this->registers.C);
    return 8;
}
int GB::op_cb_sla_d() {
    this->bmi.sla_u8(this->registers.D);
    return 8;
}
int GB::op_cb_sla_e() {
    this->bmi.sla_u8(this->registers.E);
    return 8;
}
int GB::op_cb_sla_h() {
    this->bmi.sla_u8(this->registers.H);
    return 8;
}
int GB::op_cb_sla_l() {
    this->bmi.sla_u8(this->registers.L);
    return 8;
}
int GB::op_cb_sla_hlm() {
    this->bmi.sla_u8(this->registers.get_hl());
    return 16;
}
int GB::op_cb_sla_a() {
    this->bmi.sla_u8(this->registers.A);
    return 8;
}

// Shifts (SRA)
int GB::op_cb_sra_b() {
    this->bmi.sra_u8(this->registers.B);
    return 8;
}
int GB::op_cb_sra_c() { /* TODO: implement op_cb_sra_c */ return 0; }
int GB::op_cb_sra_d() { /* TODO: implement op_cb_sra_d */ return 0; }
int GB::op_cb_sra_e() { /* TODO: implement op_cb_sra_e */ return 0; }
int GB::op_cb_sra_h() { /* TODO: implement op_cb_sra_h */ return 0; }
int GB::op_cb_sra_l() { /* TODO: implement op_cb_sra_l */ return 0; }
int GB::op_cb_sra_hlm() { /* TODO: implement op_cb_sra_hlm */ return 0; }
int GB::op_cb_sra_a() { /* TODO: implement op_cb_sra_a */ return 0; }

// Swap nibbles
int GB::op_cb_swap_b() { /* TODO: implement op_cb_swap_b */ return 0; }
int GB::op_cb_swap_c() { /* TODO: implement op_cb_swap_c */ return 0; }
int GB::op_cb_swap_d() { /* TODO: implement op_cb_swap_d */ return 0; }
int GB::op_cb_swap_e() { /* TODO: implement op_cb_swap_e */ return 0; }
int GB::op_cb_swap_h() { /* TODO: implement op_cb_swap_h */ return 0; }
int GB::op_cb_swap_l() { /* TODO: implement op_cb_swap_l */ return 0; }
int GB::op_cb_swap_hlm() { /* TODO: implement op_cb_swap_hlm*/ return 0; }
int GB::op_cb_swap_a() { /* TODO: implement op_cb_swap_a */ return 0; }

// Shifts logical (SRL)
int GB::op_cb_srl_b() { /* TODO: implement op_cb_srl_b */ return 0; }
int GB::op_cb_srl_c() { /* TODO: implement op_cb_srl_c */ return 0; }
int GB::op_cb_srl_d() { /* TODO: implement op_cb_srl_d */ return 0; }
int GB::op_cb_srl_e() { /* TODO: implement op_cb_srl_e */ return 0; }
int GB::op_cb_srl_h() { /* TODO: implement op_cb_srl_h */ return 0; }
int GB::op_cb_srl_l() { /* TODO: implement op_cb_srl_l */ return 0; }
int GB::op_cb_srl_hlm() { /* TODO: implement op_cb_srl_hlm */ return 0; }
int GB::op_cb_srl_a() { /* TODO: implement op_cb_srl_a */ return 0; }

// BIT b,r (bits 0..7) - each: B,C,D,E,H,L,[HL],A
int GB::op_cb_bit_0_b() { /* TODO */ return 0; }
int GB::op_cb_bit_0_c() { /* TODO */ return 0; }
int GB::op_cb_bit_0_d() { /* TODO */ return 0; }
int GB::op_cb_bit_0_e() { /* TODO */ return 0; }
int GB::op_cb_bit_0_h() { /* TODO */ return 0; }
int GB::op_cb_bit_0_l() { /* TODO */ return 0; }
int GB::op_cb_bit_0_hlm() { /* TODO */ return 0; }
int GB::op_cb_bit_0_a() { /* TODO */ return 0; }

int GB::op_cb_bit_1_b() { /* TODO */ return 0; }
int GB::op_cb_bit_1_c() { /* TODO */ return 0; }
int GB::op_cb_bit_1_d() { /* TODO */ return 0; }
int GB::op_cb_bit_1_e() { /* TODO */ return 0; }
int GB::op_cb_bit_1_h() { /* TODO */ return 0; }
int GB::op_cb_bit_1_l() { /* TODO */ return 0; }
int GB::op_cb_bit_1_hlm() { /* TODO */ return 0; }
int GB::op_cb_bit_1_a() { /* TODO */ return 0; }

int GB::op_cb_bit_2_b() { /* TODO */ return 0; }
int GB::op_cb_bit_2_c() { /* TODO */ return 0; }
int GB::op_cb_bit_2_d() { /* TODO */ return 0; }
int GB::op_cb_bit_2_e() { /* TODO */ return 0; }
int GB::op_cb_bit_2_h() { /* TODO */ return 0; }
int GB::op_cb_bit_2_l() { /* TODO */ return 0; }
int GB::op_cb_bit_2_hlm() { /* TODO */ return 0; }
int GB::op_cb_bit_2_a() { /* TODO */ return 0; }

int GB::op_cb_bit_3_b() { /* TODO */ return 0; }
int GB::op_cb_bit_3_c() { /* TODO */ return 0; }
int GB::op_cb_bit_3_d() { /* TODO */ return 0; }
int GB::op_cb_bit_3_e() { /* TODO */ return 0; }
int GB::op_cb_bit_3_h() { /* TODO */ return 0; }
int GB::op_cb_bit_3_l() { /* TODO */ return 0; }
int GB::op_cb_bit_3_hlm() { /* TODO */ return 0; }
int GB::op_cb_bit_3_a() { /* TODO */ return 0; }

int GB::op_cb_bit_4_b() { /* TODO */ return 0; }
int GB::op_cb_bit_4_c() { /* TODO */ return 0; }
int GB::op_cb_bit_4_d() { /* TODO */ return 0; }
int GB::op_cb_bit_4_e() { /* TODO */ return 0; }
int GB::op_cb_bit_4_h() { /* TODO */ return 0; }
int GB::op_cb_bit_4_l() { /* TODO */ return 0; }
int GB::op_cb_bit_4_hlm() { /* TODO */ return 0; }
int GB::op_cb_bit_4_a() { /* TODO */ return 0; }

int GB::op_cb_bit_5_b() { /* TODO */ return 0; }
int GB::op_cb_bit_5_c() { /* TODO */ return 0; }
int GB::op_cb_bit_5_d() { /* TODO */ return 0; }
int GB::op_cb_bit_5_e() { /* TODO */ return 0; }
int GB::op_cb_bit_5_h() { /* TODO */ return 0; }
int GB::op_cb_bit_5_l() { /* TODO */ return 0; }
int GB::op_cb_bit_5_hlm() { /* TODO */ return 0; }
int GB::op_cb_bit_5_a() { /* TODO */ return 0; }

int GB::op_cb_bit_6_b() { /* TODO */ return 0; }
int GB::op_cb_bit_6_c() { /* TODO */ return 0; }
int GB::op_cb_bit_6_d() { /* TODO */ return 0; }
int GB::op_cb_bit_6_e() { /* TODO */ return 0; }
int GB::op_cb_bit_6_h() { /* TODO */ return 0; }
int GB::op_cb_bit_6_l() { /* TODO */ return 0; }
int GB::op_cb_bit_6_hlm() { /* TODO */ return 0; }
int GB::op_cb_bit_6_a() { /* TODO */ return 0; }

int GB::op_cb_bit_7_b() { /* TODO */ return 0; }
int GB::op_cb_bit_7_c() { /* TODO */ return 0; }
int GB::op_cb_bit_7_d() { /* TODO */ return 0; }
int GB::op_cb_bit_7_e() { /* TODO */ return 0; }
int GB::op_cb_bit_7_h() { /* TODO */ return 0; }
int GB::op_cb_bit_7_l() { /* TODO */ return 0; }
int GB::op_cb_bit_7_hlm() { /* TODO */ return 0; }
int GB::op_cb_bit_7_a() { /* TODO */ return 0; }

// RES n,r (reset bits)
int GB::op_cb_res_0_b() { /* TODO */ return 0; }
int GB::op_cb_res_0_c() { /* TODO */ return 0; }
int GB::op_cb_res_0_d() { /* TODO */ return 0; }
int GB::op_cb_res_0_e() { /* TODO */ return 0; }
int GB::op_cb_res_0_h() { /* TODO */ return 0; }
int GB::op_cb_res_0_l() { /* TODO */ return 0; }
int GB::op_cb_res_0_hlm() { /* TODO */ return 0; }
int GB::op_cb_res_0_a() { /* TODO */ return 0; }

int GB::op_cb_res_1_b() { /* TODO */ return 0; }
int GB::op_cb_res_1_c() { /* TODO */ return 0; }
int GB::op_cb_res_1_d() { /* TODO */ return 0; }
int GB::op_cb_res_1_e() { /* TODO */ return 0; }
int GB::op_cb_res_1_h() { /* TODO */ return 0; }
int GB::op_cb_res_1_l() { /* TODO */ return 0; }
int GB::op_cb_res_1_hlm() { /* TODO */ return 0; }
int GB::op_cb_res_1_a() { /* TODO */ return 0; }

int GB::op_cb_res_2_b() { /* TODO */ return 0; }
int GB::op_cb_res_2_c() { /* TODO */ return 0; }
int GB::op_cb_res_2_d() { /* TODO */ return 0; }
int GB::op_cb_res_2_e() { /* TODO */ return 0; }
int GB::op_cb_res_2_h() { /* TODO */ return 0; }
int GB::op_cb_res_2_l() { /* TODO */ return 0; }
int GB::op_cb_res_2_hlm() { /* TODO */ return 0; }
int GB::op_cb_res_2_a() { /* TODO */ return 0; }

int GB::op_cb_res_3_b() { /* TODO */ return 0; }
int GB::op_cb_res_3_c() { /* TODO */ return 0; }
int GB::op_cb_res_3_d() { /* TODO */ return 0; }
int GB::op_cb_res_3_e() { /* TODO */ return 0; }
int GB::op_cb_res_3_h() { /* TODO */ return 0; }
int GB::op_cb_res_3_l() { /* TODO */ return 0; }
int GB::op_cb_res_3_hlm() { /* TODO */ return 0; }
int GB::op_cb_res_3_a() { /* TODO */ return 0; }

int GB::op_cb_res_4_b() { /* TODO */ return 0; }
int GB::op_cb_res_4_c() { /* TODO */ return 0; }
int GB::op_cb_res_4_d() { /* TODO */ return 0; }
int GB::op_cb_res_4_e() { /* TODO */ return 0; }
int GB::op_cb_res_4_h() { /* TODO */ return 0; }
int GB::op_cb_res_4_l() { /* TODO */ return 0; }
int GB::op_cb_res_4_hlm() { /* TODO */ return 0; }
int GB::op_cb_res_4_a() { /* TODO */ return 0; }

int GB::op_cb_res_5_b() { /* TODO */ return 0; }
int GB::op_cb_res_5_c() { /* TODO */ return 0; }
int GB::op_cb_res_5_d() { /* TODO */ return 0; }
int GB::op_cb_res_5_e() { /* TODO */ return 0; }
int GB::op_cb_res_5_h() { /* TODO */ return 0; }
int GB::op_cb_res_5_l() { /* TODO */ return 0; }
int GB::op_cb_res_5_hlm() { /* TODO */ return 0; }
int GB::op_cb_res_5_a() { /* TODO */ return 0; }

int GB::op_cb_res_6_b() { /* TODO */ return 0; }
int GB::op_cb_res_6_c() { /* TODO */ return 0; }
int GB::op_cb_res_6_d() { /* TODO */ return 0; }
int GB::op_cb_res_6_e() { /* TODO */ return 0; }
int GB::op_cb_res_6_h() { /* TODO */ return 0; }
int GB::op_cb_res_6_l() { /* TODO */ return 0; }
int GB::op_cb_res_6_hlm() { /* TODO */ return 0; }
int GB::op_cb_res_6_a() { /* TODO */ return 0; }

int GB::op_cb_res_7_b() { /* TODO */ return 0; }
int GB::op_cb_res_7_c() { /* TODO */ return 0; }
int GB::op_cb_res_7_d() { /* TODO */ return 0; }
int GB::op_cb_res_7_e() { /* TODO */ return 0; }
int GB::op_cb_res_7_h() { /* TODO */ return 0; }
int GB::op_cb_res_7_l() { /* TODO */ return 0; }
int GB::op_cb_res_7_hlm() { /* TODO */ return 0; }
int GB::op_cb_res_7_a() { /* TODO */ return 0; }

// SET n,r (set bits)
int GB::op_cb_set_0_b() { /* TODO */ return 0; }
int GB::op_cb_set_0_c() { /* TODO */ return 0; }
int GB::op_cb_set_0_d() { /* TODO */ return 0; }
int GB::op_cb_set_0_e() { /* TODO */ return 0; }
int GB::op_cb_set_0_h() { /* TODO */ return 0; }
int GB::op_cb_set_0_l() { /* TODO */ return 0; }
int GB::op_cb_set_0_hlm() { /* TODO */ return 0; }
int GB::op_cb_set_0_a() { /* TODO */ return 0; }

int GB::op_cb_set_1_b() { /* TODO */ return 0; }
int GB::op_cb_set_1_c() { /* TODO */ return 0; }
int GB::op_cb_set_1_d() { /* TODO */ return 0; }
int GB::op_cb_set_1_e() { /* TODO */ return 0; }
int GB::op_cb_set_1_h() { /* TODO */ return 0; }
int GB::op_cb_set_1_l() { /* TODO */ return 0; }
int GB::op_cb_set_1_hlm() { /* TODO */ return 0; }
int GB::op_cb_set_1_a() { /* TODO */ return 0; }

int GB::op_cb_set_2_b() { /* TODO */ return 0; }
int GB::op_cb_set_2_c() { /* TODO */ return 0; }
int GB::op_cb_set_2_d() { /* TODO */ return 0; }
int GB::op_cb_set_2_e() { /* TODO */ return 0; }
int GB::op_cb_set_2_h() { /* TODO */ return 0; }
int GB::op_cb_set_2_l() { /* TODO */ return 0; }
int GB::op_cb_set_2_hlm() { /* TODO */ return 0; }
int GB::op_cb_set_2_a() { /* TODO */ return 0; }

int GB::op_cb_set_3_b() { /* TODO */ return 0; }
int GB::op_cb_set_3_c() { /* TODO */ return 0; }
int GB::op_cb_set_3_d() { /* TODO */ return 0; }
int GB::op_cb_set_3_e() { /* TODO */ return 0; }
int GB::op_cb_set_3_h() { /* TODO */ return 0; }
int GB::op_cb_set_3_l() { /* TODO */ return 0; }
int GB::op_cb_set_3_hlm() { /* TODO */ return 0; }
int GB::op_cb_set_3_a() { /* TODO */ return 0; }

int GB::op_cb_set_4_b() { /* TODO */ return 0; }
int GB::op_cb_set_4_c() { /* TODO */ return 0; }
int GB::op_cb_set_4_d() { /* TODO */ return 0; }
int GB::op_cb_set_4_e() { /* TODO */ return 0; }
int GB::op_cb_set_4_h() { /* TODO */ return 0; }
int GB::op_cb_set_4_l() { /* TODO */ return 0; }
int GB::op_cb_set_4_hlm() { /* TODO */ return 0; }
int GB::op_cb_set_4_a() { /* TODO */ return 0; }

int GB::op_cb_set_5_b() { /* TODO */ return 0; }
int GB::op_cb_set_5_c() { /* TODO */ return 0; }
int GB::op_cb_set_5_d() { /* TODO */ return 0; }
int GB::op_cb_set_5_e() { /* TODO */ return 0; }
int GB::op_cb_set_5_h() { /* TODO */ return 0; }
int GB::op_cb_set_5_l() { /* TODO */ return 0; }
int GB::op_cb_set_5_hlm() { /* TODO */ return 0; }
int GB::op_cb_set_5_a() { /* TODO */ return 0; }

int GB::op_cb_set_6_b() { /* TODO */ return 0; }
int GB::op_cb_set_6_c() { /* TODO */ return 0; }
int GB::op_cb_set_6_d() { /* TODO */ return 0; }
int GB::op_cb_set_6_e() { /* TODO */ return 0; }
int GB::op_cb_set_6_h() { /* TODO */ return 0; }
int GB::op_cb_set_6_l() { /* TODO */ return 0; }
int GB::op_cb_set_6_hlm() { /* TODO */ return 0; }
int GB::op_cb_set_6_a() { /* TODO */ return 0; }

int GB::op_cb_set_7_b() { /* TODO */ return 0; }
int GB::op_cb_set_7_c() { /* TODO */ return 0; }
int GB::op_cb_set_7_d() { /* TODO */ return 0; }
int GB::op_cb_set_7_e() { /* TODO */ return 0; }
int GB::op_cb_set_7_h() { /* TODO */ return 0; }
int GB::op_cb_set_7_l() { /* TODO */ return 0; }
int GB::op_cb_set_7_hlm() { /* TODO */ return 0; }
int GB::op_cb_set_7_a() { /* TODO */ return 0; }

void GB::init_cb_table() {
    for (auto &o : this->ops)
        o = {&GB::op_unimplemented, "???", 1};

    auto set = [&](uint8_t opcode, OpFn fn, const char *name, uint8_t length) { this->ops[opcode] = {fn, name, length}; };

    // Set up CB opcode table
    // clang-format off
    // 0x00 - 0x0F
    set(0x00, &GB::op_nop,          "NOP",          1);
    set(0x01, &GB::op_ld_bc_n16,    "LD BC, n16",   3);
    set(0x02, &GB::op_ld_bcm_a,     "LD [BC], A",   1);
    set(0x03, &GB::op_inc_bc,       "INC BC",       1);
    // clang-format on
}
