#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

class Joypad;

class Memory {
  public:
    void load_rom(const std::vector<uint8_t> &rom);

    uint8_t read_byte(uint16_t address) const;
    uint8_t read_byte_unrestricted(uint16_t address) const;
    void write_byte(uint16_t address, uint8_t value);

    uint16_t read_word(uint16_t address) const;
    void write_word(uint16_t address, uint16_t value);

    void write_range(size_t start, size_t end, uint8_t value);
    void write_range(size_t start, const std::vector<uint8_t> &data);
    std::vector<uint8_t> read_range(size_t start, size_t end) const;

    uint8_t get_ie() const;
    void set_ie(uint8_t value);

    uint8_t get_if() const;
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

    std::array<uint8_t, 0x2000> vram_{}; // 0x8000-0x9FFF
    std::array<uint8_t, 0x2000> wram_{}; // 0xC000-0xDFFF
    std::array<uint8_t, 0x00A0> oam_{};  // 0xFE00-0xFE9F
    std::array<uint8_t, 0x0080> io_{};   // 0xFF00-0xFF7F
    std::array<uint8_t, 0x007F> hram_{}; // 0xFF80-0xFFFE
    uint8_t ie_ = 0;                     // 0xFFFF

    bool vram_blocked_ = false;
    bool oam_blocked_ = false;
    bool dma_request_pending_ = false;
    uint8_t dma_source_high_ = 0;
    bool div_reset_pending_ = false;

    Joypad *joypad_ = nullptr;
};
