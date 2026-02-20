#pragma once

#include "memory.hpp"
#include "screen.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

class PPU {
  public:
    PPU(Memory &memory, Screen &screen);

    void tick(uint32_t dots);
    bool consume_frame_ready();

    // LCD control & status registers
    uint8_t get_lcdc();
    void set_lcdc(uint8_t value);

    uint8_t get_ly();
    void set_ly(uint8_t value);

    uint8_t get_lyc();
    void set_lyc(uint8_t value);

    uint8_t get_stat();
    void set_stat(uint8_t value);

    uint8_t get_ppu_mode();
    void set_ppu_mode(uint8_t mode);

    uint8_t get_scy();
    void set_scy(uint8_t value);

    uint8_t get_scx();
    void set_scx(uint8_t value);

    uint8_t get_wy();
    void set_wy(uint8_t value);

    uint8_t get_wx();
    void set_wx(uint8_t value);

    uint8_t get_bgp();
    void set_bgp(uint8_t value);

    uint8_t get_obp0();
    void set_obp0(uint8_t value);

    uint8_t get_obp1();
    void set_obp1(uint8_t value);

  private:
    void reset_lcd_off_state();
    void request_vblank_interrupt();
    void request_lcd_stat_interrupt();
    void update_mode_for_current_dot();
    void update_lyc_flag_and_stat_interrupt();
    void apply_memory_locks();
    void render_scanline();
    void render_bg_window_scanline(std::array<uint8_t, config::k_screen_width> &bg_color_ids);
    void render_sprites_scanline(const std::array<uint8_t, config::k_screen_width> &bg_color_ids);
    uint8_t read_tile_pixel(uint8_t tile_index, uint8_t row, uint8_t col, bool use_unsigned_tile_index) const;
    uint8_t apply_palette(uint8_t palette_reg, uint8_t color_id) const;
    void begin_dma(uint8_t source_high);
    void tick_dma_one_dot();

    Memory &memory_;
    Screen &screen_;

    int dot_in_scanline = 0;
    int current_ly = 0;
    uint8_t mode_ = 2;
    bool frame_ready = false;
    bool lcd_enabled = false;
    bool stat_irq_line = false;

    const uint16_t dots_per_scanline = 456;
    const uint16_t visible_scanlines = 144;
    const uint16_t total_scanlines = 154;
    const uint16_t oam_dots = 80;
    const uint16_t transfer_dots = 172;
    const uint16_t hblank_dots = 204;

    bool scanline_rendered = false;

    bool dma_active = false;
    uint16_t dma_source_base = 0;
    uint16_t dma_index = 0;
    int dma_dot_counter = 0;
};
