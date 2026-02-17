#include "memory.h"
#include "joypad.h"

#include <algorithm>

Memory::Memory(uint8_t initial_value) : ie_(0) {
    std::fill(std::begin(vram_), std::end(vram_), initial_value);
    std::fill(std::begin(wram_), std::end(wram_), initial_value);
    std::fill(std::begin(oam_), std::end(oam_), initial_value);
    std::fill(std::begin(io_), std::end(io_), initial_value);
    std::fill(std::begin(hram_), std::end(hram_), initial_value);
}

void Memory::load_rom(const std::vector<uint8_t> &rom) {
    rom_ = rom;
    // Optional: size external RAM based on header later; keep empty for now.
}

uint8_t Memory::read_byte(uint16_t a) const { return this->read_byte_impl(a, true); }

uint8_t Memory::read_byte_unrestricted(uint16_t a) const { return this->read_byte_impl(a, false); }

uint8_t Memory::read_byte_impl(uint16_t a, bool respect_locks) const {
    // 0x0000-0x7FFF: Cartridge ROM (banking later)
    if (a <= 0x7FFF) {
        if (a < rom_.size()) return rom_[a];
        return 0xFF;
    }

    // 0x8000-0x9FFF: VRAM
    if (a >= 0x8000 && a <= 0x9FFF) {
        if (respect_locks && this->vram_blocked_) return 0xFF;
        return vram_[a - 0x8000];
    }

    // 0xA000-0xBFFF: External RAM (if present / enabled later)
    if (a >= 0xA000 && a <= 0xBFFF) {
        if (eram_.empty()) return 0xFF;
        const size_t idx = size_t(a - 0xA000) % eram_.size();
        return eram_[idx];
    }

    // 0xC000-0xDFFF: WRAM
    if (a >= 0xC000 && a <= 0xDFFF) {
        return wram_[a - 0xC000];
    }

    // 0xE000-0xFDFF: Echo RAM (mirror of 0xC000-0xDDFF)
    if (a >= 0xE000 && a <= 0xFDFF) {
        const uint16_t mirrored = uint16_t(a - 0x2000); // 0xE000->0xC000
        return wram_[mirrored - 0xC000];
    }

    // 0xFE00-0xFE9F: OAM
    if (a >= 0xFE00 && a <= 0xFE9F) {
        if (respect_locks && this->oam_blocked_) return 0xFF;
        return oam_[a - 0xFE00];
    }

    // 0xFEA0-0xFEFF: Not usable
    if (a >= 0xFEA0 && a <= 0xFEFF) {
        return 0xFF;
    }

    // 0xFF00-0xFF7F: I/O registers (later: dispatch to joypad/timer/apu/ppu/etc)
    if (a >= 0xFF00 && a <= 0xFF7F) {
        if (a == 0xFF00 && this->joypad_ != nullptr) {
            return this->joypad_->get_joyp();
        }
        return io_[a - 0xFF00];
    }

    // 0xFF80-0xFFFE: HRAM
    if (a >= 0xFF80 && a <= 0xFFFE) {
        return hram_[a - 0xFF80];
    }

    // 0xFFFF: IE
    return ie_;
}

void Memory::write_byte(uint16_t a, uint8_t v) {
    // 0000-7FFF: ROM / MBC control (later). For now ignore.
    if (a <= 0x7FFF) {
        return;
    }

    if (a >= 0x8000 && a <= 0x9FFF) {
        if (this->vram_blocked_) return;
        vram_[a - 0x8000] = v;
        return;
    }

    if (a >= 0xA000 && a <= 0xBFFF) {
        if (!eram_.empty()) {
            const size_t idx = size_t(a - 0xA000) % eram_.size();
            eram_[idx] = v;
        }
        return;
    }

    if (a >= 0xC000 && a <= 0xDFFF) {
        wram_[a - 0xC000] = v;
        return;
    }

    if (a >= 0xE000 && a <= 0xFDFF) {
        const uint16_t mirrored = uint16_t(a - 0x2000);
        wram_[mirrored - 0xC000] = v;
        return;
    }

    if (a >= 0xFE00 && a <= 0xFE9F) {
        if (this->oam_blocked_) return;
        oam_[a - 0xFE00] = v;
        return;
    }

    if (a >= 0xFEA0 && a <= 0xFEFF) {
        return; // not usable
    }

    if (a >= 0xFF00 && a <= 0xFF7F) {
        if (a == 0xFF00 && this->joypad_ != nullptr) {
            this->joypad_->set_joyp(v);
            io_[a - 0xFF00] = v;
            return;
        }
        if (a == 0xFF04) {
            // Writing any value to DIV resets it to 0
            io_[a - 0xFF00] = 0;
            this->div_reset_pending_ = true;
            return;
        }
        if (a == 0xFF46) {
            this->dma_request_pending_ = true;
            this->dma_source_high_ = v;
        }
        io_[a - 0xFF00] = v;
        return;
    }

    if (a >= 0xFF80 && a <= 0xFFFE) {
        hram_[a - 0xFF80] = v;
        return;
    }

    ie_ = v; // 0xFFFF
}

uint16_t Memory::read_word(uint16_t address) const {
    const uint8_t lo = read_byte(address);
    const uint8_t hi = read_byte(uint16_t(address + 1));
    return uint16_t(lo) | (uint16_t(hi) << 8);
}

void Memory::write_word(uint16_t address, uint16_t value) {
    write_byte(address, uint8_t(value & 0xFF));
    write_byte(uint16_t(address + 1), uint8_t(value >> 8));
}

void Memory::write_range(size_t start, size_t end, uint8_t value) {
    for (size_t a = start; a <= end; ++a)
        write_byte(uint16_t(a), value);
}

void Memory::write_range(size_t start, const std::vector<uint8_t> &data) {
    for (size_t i = 0; i < data.size(); ++i)
        write_byte(uint16_t(start + i), data[i]);
}

std::vector<uint8_t> Memory::read_range(size_t start, size_t end) {
    std::vector<uint8_t> out;
    out.reserve(end - start + 1);
    for (size_t a = start; a <= end; ++a)
        out.push_back(read_byte(uint16_t(a)));
    return out;
}

uint8_t Memory::get_ie() { return this->read_byte(0xFFFF); }
void Memory::set_ie(uint8_t value) { this->write_byte(0xFFFF, value); }

uint8_t Memory::get_if() { return this->read_byte(0xFF0F); }
void Memory::set_if(uint8_t value) { this->write_byte(0xFF0F, value); }

void Memory::attach_joypad(Joypad *joypad) { this->joypad_ = joypad; }

uint8_t Memory::read_io_reg(uint16_t address) const {
    if (address < 0xFF00 || address > 0xFF7F) return 0xFF;
    return io_[address - 0xFF00];
}

void Memory::write_io_reg(uint16_t address, uint8_t value) {
    if (address < 0xFF00 || address > 0xFF7F) return;
    io_[address - 0xFF00] = value;
}

bool Memory::consume_div_reset() {
    const bool pending = this->div_reset_pending_;
    this->div_reset_pending_ = false;
    return pending;
}

void Memory::set_vram_blocked(bool blocked) { this->vram_blocked_ = blocked; }

void Memory::set_oam_blocked(bool blocked) { this->oam_blocked_ = blocked; }

uint8_t Memory::read_vram_raw(uint16_t address) const {
    if (address < 0x8000 || address > 0x9FFF) return 0xFF;
    return this->vram_[address - 0x8000];
}

uint8_t Memory::read_oam_raw(uint16_t address) const {
    if (address < 0xFE00 || address > 0xFE9F) return 0xFF;
    return this->oam_[address - 0xFE00];
}

void Memory::write_oam_raw(uint16_t address, uint8_t value) {
    if (address < 0xFE00 || address > 0xFE9F) return;
    this->oam_[address - 0xFE00] = value;
}

bool Memory::consume_dma_request(uint8_t &source_high) {
    if (!this->dma_request_pending_) return false;
    this->dma_request_pending_ = false;
    source_high = this->dma_source_high_;
    return true;
}
