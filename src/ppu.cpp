#include "ppu.h"

#include <algorithm>
#include <array>

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
        uint8_t dma_source_high = 0;
        if (this->memory.consume_dma_request(dma_source_high)) {
            this->begin_dma(dma_source_high);
        }

        this->tick_dma_one_dot();
        this->update_mode_for_current_dot();
        this->apply_memory_locks();

        if (!this->scanline_rendered && this->current_ly < this->visible_scanlines && this->mode == 0) {
            this->render_scanline();
            this->scanline_rendered = true;
        }

        this->dot_in_scanline += 1;

        if (this->dot_in_scanline < this->dots_per_scanline) continue;

        this->dot_in_scanline = 0;
        this->current_ly += 1;
        this->scanline_rendered = false;

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
        this->update_lyc_flag_and_stat_interrupt();
        this->apply_memory_locks();
    }
}

bool PPU::consume_frame_ready() {
    const bool ready = this->frame_ready;
    this->frame_ready = false;
    return ready;
}

void PPU::reset_lcd_off_state() {
    this->lcd_enabled = false;
    this->dot_in_scanline = 0;
    this->current_ly = 0;
    this->mode = 0;
    this->frame_ready = false;
    this->scanline_rendered = false;
    this->stat_irq_line = false;
    this->dma_active = false;
    this->dma_index = 0;
    this->dma_dot_counter = 0;
    this->set_ly(0);
    this->set_ppu_mode(0);
    this->update_lyc_flag_and_stat_interrupt();
    this->apply_memory_locks();
}

void PPU::request_vblank_interrupt() {
    uint8_t interrupt_flags = this->memory.get_if();
    interrupt_flags |= 0x01;
    this->memory.set_if(interrupt_flags);
}

void PPU::request_lcd_stat_interrupt() {
    uint8_t interrupt_flags = this->memory.get_if();
    interrupt_flags |= 0x02;
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
    this->update_lyc_flag_and_stat_interrupt();
}

void PPU::update_lyc_flag_and_stat_interrupt() {
    uint8_t stat = this->get_stat();
    const bool lyc_match = this->get_ly() == this->get_lyc();
    if (lyc_match) {
        stat |= 0x04;
    } else {
        stat &= static_cast<uint8_t>(~0x04);
    }
    this->set_stat(stat);

    const bool irq_on_lyc = (stat & 0x40) != 0 && lyc_match;
    const bool irq_on_mode2 = (stat & 0x20) != 0 && this->mode == 2;
    const bool irq_on_mode1 = (stat & 0x10) != 0 && this->mode == 1;
    const bool irq_on_mode0 = (stat & 0x08) != 0 && this->mode == 0;

    const bool stat_line = irq_on_lyc || irq_on_mode2 || irq_on_mode1 || irq_on_mode0;
    if (stat_line && !this->stat_irq_line) {
        this->request_lcd_stat_interrupt();
    }
    this->stat_irq_line = stat_line;
}

void PPU::apply_memory_locks() {
    const bool vram_blocked = this->mode == 3;
    const bool oam_blocked = this->mode == 2 || this->mode == 3 || this->dma_active;
    this->memory.set_vram_blocked(vram_blocked);
    this->memory.set_oam_blocked(oam_blocked);
}

void PPU::render_scanline() {
    std::array<uint8_t, GB_SCREEN_WIDTH> bg_color_ids{};
    this->render_bg_window_scanline(bg_color_ids);
    this->render_sprites_scanline(bg_color_ids);
}

void PPU::render_bg_window_scanline(std::array<uint8_t, GB_SCREEN_WIDTH> &bg_color_ids) {
    const uint8_t lcdc = this->get_lcdc();
    const bool bg_enabled = (lcdc & 0x01) != 0;
    const bool window_enabled = (lcdc & 0x20) != 0;
    const bool use_unsigned_tile_index = (lcdc & 0x10) != 0;
    const uint16_t bg_map_base = (lcdc & 0x08) ? 0x9C00 : 0x9800;
    const uint16_t win_map_base = (lcdc & 0x40) ? 0x9C00 : 0x9800;

    const uint8_t scx = this->get_scx();
    const uint8_t scy = this->get_scy();
    const uint8_t wx = this->get_wx();
    const uint8_t wy = this->get_wy();
    const uint8_t ly = this->get_ly();
    const uint8_t bgp = this->get_bgp();

    for (int x = 0; x < GB_SCREEN_WIDTH; ++x) {
        uint8_t color_id = 0;

        if (bg_enabled) {
            bool use_window = false;
            if (window_enabled && ly >= wy) {
                const int window_start_x = static_cast<int>(wx) - 7;
                if (x >= window_start_x) use_window = true;
            }

            uint16_t map_base = use_window ? win_map_base : bg_map_base;
            uint8_t pixel_x = 0;
            uint8_t pixel_y = 0;

            if (use_window) {
                pixel_x = static_cast<uint8_t>(x - (static_cast<int>(wx) - 7));
                pixel_y = static_cast<uint8_t>(ly - wy);
            } else {
                pixel_x = static_cast<uint8_t>(x + scx);
                pixel_y = static_cast<uint8_t>(ly + scy);
            }

            const uint8_t tile_x = static_cast<uint8_t>(pixel_x / 8);
            const uint8_t tile_y = static_cast<uint8_t>(pixel_y / 8);
            const uint16_t map_addr = static_cast<uint16_t>(map_base + tile_y * 32 + tile_x);
            const uint8_t tile_index = this->memory.read_byte_unrestricted(map_addr);

            const uint8_t row = static_cast<uint8_t>(pixel_y % 8);
            const uint8_t col = static_cast<uint8_t>(pixel_x % 8);
            color_id = this->read_tile_pixel(tile_index, row, col, use_unsigned_tile_index);
        }

        bg_color_ids[x] = color_id;
        const uint8_t final_color = this->apply_palette(bgp, color_id);
        this->screen.set(static_cast<size_t>(x), static_cast<size_t>(ly), final_color);
    }
}

void PPU::render_sprites_scanline(const std::array<uint8_t, GB_SCREEN_WIDTH> &bg_color_ids) {
    const uint8_t lcdc = this->get_lcdc();
    const bool sprites_enabled = (lcdc & 0x02) != 0;
    if (!sprites_enabled) return;

    const bool sprite_8x16 = (lcdc & 0x04) != 0;
    const int sprite_height = sprite_8x16 ? 16 : 8;
    const uint8_t ly = this->get_ly();

    std::array<uint8_t, 10> sprite_indices{};
    int selected_count = 0;
    for (int i = 0; i < 40 && selected_count < 10; ++i) {
        const uint16_t base = static_cast<uint16_t>(0xFE00 + i * 4);
        const int y = static_cast<int>(this->memory.read_oam_raw(base)) - 16;
        if (ly < y || ly >= y + sprite_height) continue;
        sprite_indices[selected_count++] = static_cast<uint8_t>(i);
    }

    for (int s = 0; s < selected_count; ++s) {
        const uint16_t base = static_cast<uint16_t>(0xFE00 + sprite_indices[s] * 4);
        const int y = static_cast<int>(this->memory.read_oam_raw(base)) - 16;
        const int x = static_cast<int>(this->memory.read_oam_raw(static_cast<uint16_t>(base + 1))) - 8;
        uint8_t tile = this->memory.read_oam_raw(static_cast<uint16_t>(base + 2));
        const uint8_t attrs = this->memory.read_oam_raw(static_cast<uint16_t>(base + 3));

        const bool bg_priority = (attrs & 0x80) != 0;
        const bool y_flip = (attrs & 0x40) != 0;
        const bool x_flip = (attrs & 0x20) != 0;
        const bool use_obp1 = (attrs & 0x10) != 0;

        if (sprite_8x16) tile &= 0xFE;
        uint8_t row = static_cast<uint8_t>(ly - y);
        if (y_flip) row = static_cast<uint8_t>((sprite_height - 1) - row);

        if (sprite_8x16 && row >= 8) {
            tile = static_cast<uint8_t>(tile + 1);
            row = static_cast<uint8_t>(row - 8);
        }

        for (int px = 0; px < 8; ++px) {
            const int sx = x + px;
            if (sx < 0 || sx >= GB_SCREEN_WIDTH) continue;

            uint8_t col = static_cast<uint8_t>(px);
            if (x_flip) col = static_cast<uint8_t>(7 - col);

            const uint8_t color_id = this->read_tile_pixel(tile, row, col, true);
            if (color_id == 0) continue;

            if (bg_priority && bg_color_ids[static_cast<size_t>(sx)] != 0) continue;

            const uint8_t palette = use_obp1 ? this->get_obp1() : this->get_obp0();
            const uint8_t final_color = this->apply_palette(palette, color_id);
            this->screen.set(static_cast<size_t>(sx), static_cast<size_t>(ly), final_color);
        }
    }
}

uint8_t PPU::read_tile_pixel(uint8_t tile_index, uint8_t row, uint8_t col, bool use_unsigned_tile_index) const {
    uint16_t tile_base = 0;
    if (use_unsigned_tile_index) {
        tile_base = static_cast<uint16_t>(0x8000 + static_cast<uint16_t>(tile_index) * 16);
    } else {
        const int8_t signed_index = static_cast<int8_t>(tile_index);
        tile_base = static_cast<uint16_t>(0x9000 + static_cast<int16_t>(signed_index) * 16);
    }

    const uint16_t row_addr = static_cast<uint16_t>(tile_base + static_cast<uint16_t>(row) * 2);
    const uint8_t lo = this->memory.read_byte_unrestricted(row_addr);
    const uint8_t hi = this->memory.read_byte_unrestricted(static_cast<uint16_t>(row_addr + 1));
    const uint8_t bit = static_cast<uint8_t>(7 - col);
    const uint8_t b0 = static_cast<uint8_t>((lo >> bit) & 0x01);
    const uint8_t b1 = static_cast<uint8_t>((hi >> bit) & 0x01);
    return static_cast<uint8_t>((b1 << 1) | b0);
}

uint8_t PPU::apply_palette(uint8_t palette_reg, uint8_t color_id) const {
    const uint8_t shift = static_cast<uint8_t>(color_id * 2);
    const uint8_t shade = static_cast<uint8_t>((palette_reg >> shift) & 0x03);
    // Convert DMG shade (0=white..3=black) to Screen color index (0=black..3=white).
    return static_cast<uint8_t>(3 - shade);
}

void PPU::begin_dma(uint8_t source_high) {
    this->dma_active = true;
    this->dma_source_base = static_cast<uint16_t>(source_high) << 8;
    this->dma_index = 0;
    this->dma_dot_counter = 0;
}

void PPU::tick_dma_one_dot() {
    if (!this->dma_active) return;

    this->dma_dot_counter += 1;
    if (this->dma_dot_counter < 4) return;
    this->dma_dot_counter = 0;

    if (this->dma_index < 0x00A0) {
        const uint16_t src = static_cast<uint16_t>(this->dma_source_base + this->dma_index);
        const uint8_t value = this->memory.read_byte_unrestricted(src);
        const uint16_t dst = static_cast<uint16_t>(0xFE00 + this->dma_index);
        this->memory.write_oam_raw(dst, value);
        this->dma_index += 1;
    }

    if (this->dma_index >= 0x00A0) {
        this->dma_active = false;
    }
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
