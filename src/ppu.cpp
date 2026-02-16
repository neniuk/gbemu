#include "ppu.h"

PPU::PPU(Registers &registers, Memory &memory, Screen &screen) : registers(registers), memory(memory), screen(screen) {}

void PPU::tick(int dots) {
    const bool lcd_now_enabled = (this->get_lcdc() & 0x80) != 0;
    if (!lcd_now_enabled) {
        this->reset_lcd_off_state();
        return;
    }

    // Handle LCD enable edge: start from line 0 in mode 2.
    if (!this->lcd_enabled) {
        this->dot_in_scanline = 0;
        this->current_ly = 0;
        this->mode = 2;
        this->frame_ready = false;
        this->set_ly(0);
        this->set_ppu_mode(this->mode);
    }
    this->lcd_enabled = true;

    for (int i = 0; i < dots; ++i) {
        this->update_mode_for_current_dot();
        this->dot_in_scanline += 1;

        if (this->dot_in_scanline < this->dots_per_scanline) continue;

        this->dot_in_scanline = 0;
        this->current_ly += 1;

        if (this->current_ly == this->visible_scanlines) {
            // Entering line 144 => VBlank start.
            this->mode = 1;
            this->set_ppu_mode(this->mode);
            this->request_vblank_interrupt();
            this->frame_ready = true;
        } else if (this->current_ly >= this->total_scanlines) {
            this->current_ly = 0;
            this->mode = 2;
            this->set_ppu_mode(this->mode);
            this->frame_ready = false;
        } else if (this->current_ly < this->visible_scanlines) {
            this->mode = 2;
            this->set_ppu_mode(this->mode);
        }

        this->set_ly(static_cast<uint8_t>(this->current_ly));
    }
}

void PPU::reset_lcd_off_state() {
    this->lcd_enabled = false;
    this->dot_in_scanline = 0;
    this->current_ly = 0;
    this->mode = 0;
    this->frame_ready = false;
    this->set_ly(0);
    this->set_ppu_mode(0);
}

void PPU::request_vblank_interrupt() {
    uint8_t interrupt_flags = this->memory.get_if();
    interrupt_flags |= 0x01;
    this->memory.set_if(interrupt_flags);
}

void PPU::update_mode_for_current_dot() {
    uint8_t next_mode = 1;

    if (this->current_ly < this->visible_scanlines) {
        if (this->dot_in_scanline < this->oam_dots) {
            next_mode = 2;
        } else if (this->dot_in_scanline < (this->oam_dots + this->transfer_dots)) {
            next_mode = 3;
        } else {
            next_mode = 0;
        }
    } else {
        next_mode = 1;
    }

    if (next_mode == this->mode) return;

    this->mode = next_mode;
    this->set_ppu_mode(this->mode);
}

uint8_t PPU::get_lcdc() {
    return this->memory.read_byte(0xFF40);
}
void PPU::set_lcdc(uint8_t value) {
    this->memory.write_byte(0xFF40, value);
}

uint8_t PPU::get_ly() {
    return this->memory.read_byte(0xFF44);
}
void PPU::set_ly(uint8_t value) {
    this->memory.write_byte(0xFF44, value);
}

uint8_t PPU::get_lyc() {
    return this->memory.read_byte(0xFF45);
}
void PPU::set_lyc(uint8_t value) {
    this->memory.write_byte(0xFF45, value);
}

uint8_t PPU::get_stat() {
    return this->memory.read_byte(0xFF41);
}
void PPU::set_stat(uint8_t value) {
    this->memory.write_byte(0xFF41, value);
}

uint8_t PPU::get_ppu_mode() {
    uint8_t stat = this->get_stat();
    return stat & 0x03;
}
void PPU::set_ppu_mode(uint8_t mode) {
    uint8_t stat = this->get_stat();
    stat = (stat & 0xFC) | (mode & 0x03);
    this->set_stat(stat);
}

uint8_t PPU::get_scy() {
    return this->memory.read_byte(0xFF42);
}
void PPU::set_scy(uint8_t value) {
    this->memory.write_byte(0xFF42, value);
}

uint8_t PPU::get_scx() {
    return this->memory.read_byte(0xFF43);
}
void PPU::set_scx(uint8_t value) {
    this->memory.write_byte(0xFF43, value);
}

uint8_t PPU::get_wy() {
    return this->memory.read_byte(0xFF4A);
}
void PPU::set_wy(uint8_t value) {
    this->memory.write_byte(0xFF4A, value);
}

uint8_t PPU::get_wx() {
    return this->memory.read_byte(0xFF4B);
}
void PPU::set_wx(uint8_t value) {
    this->memory.write_byte(0xFF4B, value);
}

uint8_t PPU::get_bgp() {
    return this->memory.read_byte(0xFF47);
}
void PPU::set_bgp(uint8_t value) {
    this->memory.write_byte(0xFF47, value);
}

uint8_t PPU::get_obp0() {
    return this->memory.read_byte(0xFF48);
}
void PPU::set_obp0(uint8_t value) {
    this->memory.write_byte(0xFF48, value);
}

uint8_t PPU::get_obp1() {
    return this->memory.read_byte(0xFF49);
}
void PPU::set_obp1(uint8_t value) {
    this->memory.write_byte(0xFF49, value);
}
