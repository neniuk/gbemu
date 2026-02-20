#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

class Joypad;

class Memory {
  public:
    Memory(uint8_t initial_value = 0);

    void load_rom(const std::vector<uint8_t> &rom);

    uint8_t read_byte(uint16_t address) const;
    uint8_t read_byte_unrestricted(uint16_t address) const;
    void write_byte(uint16_t address, uint8_t value);

    uint16_t read_word(uint16_t address) const;
    void write_word(uint16_t address, uint16_t value);

    void write_range(size_t start, size_t end, uint8_t value);
    void write_range(size_t start, const std::vector<uint8_t> &data);
    std::vector<uint8_t> read_range(size_t start, size_t end);

    uint8_t get_ie();
    void set_ie(uint8_t value);

    uint8_t get_if();
    void set_if(uint8_t value);

    void attach_joypad(Joypad *joypad);
    uint8_t read_io_reg(uint16_t address) const;
    void write_io_reg(uint16_t address, uint8_t value);
    bool consume_div_reset();

    void set_vram_blocked(bool blocked);
    void set_oam_blocked(bool blocked);

    uint8_t read_vram_raw(uint16_t address) const;
    uint8_t read_oam_raw(uint16_t address) const;
    void write_oam_raw(uint16_t address, uint8_t value);

    bool consume_dma_request(uint8_t &source_high);

  private:
    uint8_t read_byte_impl(uint16_t address, bool respect_locks) const;

    std::vector<uint8_t> rom_;
    std::vector<uint8_t> eram_;

    uint8_t vram_[0x2000]; // 0x8000-0x9FFF
    uint8_t wram_[0x2000]; // 0xC000-0xDFFF (DMG: 8KiB; CGB adds banking later)
    uint8_t oam_[0x00A0];  // 0xFE00-0xFE9F
    uint8_t io_[0x0080];   // 0xFF00-0xFF7F (placeholder; later route to devices)
    uint8_t hram_[0x007F]; // 0xFF80-0xFFFE
    uint8_t ie_;           // 0xFFFF

    bool vram_blocked_ = false;
    bool oam_blocked_ = false;
    bool dma_request_pending_ = false;
    uint8_t dma_source_high_ = 0;
    bool div_reset_pending_ = false;

    Joypad *joypad_ = nullptr;
};
