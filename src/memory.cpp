#include "memory.hpp"
#include "joypad.hpp"

namespace {
constexpr uint16_t k_rom_end = 0x7FFF;
constexpr uint16_t k_vram_start = 0x8000;
constexpr uint16_t k_vram_end = 0x9FFF;
constexpr uint16_t k_eram_start = 0xA000;
constexpr uint16_t k_eram_end = 0xBFFF;
constexpr uint16_t k_wram_start = 0xC000;
constexpr uint16_t k_wram_end = 0xDFFF;
constexpr uint16_t k_echo_start = 0xE000;
constexpr uint16_t k_echo_end = 0xFDFF;
constexpr uint16_t k_oam_start = 0xFE00;
constexpr uint16_t k_oam_end = 0xFE9F;
constexpr uint16_t k_not_usable_start = 0xFEA0;
constexpr uint16_t k_not_usable_end = 0xFEFF;
constexpr uint16_t k_io_start = 0xFF00;
constexpr uint16_t k_io_end = 0xFF7F;
constexpr uint16_t k_hram_start = 0xFF80;
constexpr uint16_t k_hram_end = 0xFFFE;

constexpr uint16_t k_joyp = 0xFF00;
constexpr uint16_t k_div = 0xFF04;
constexpr uint16_t k_if = 0xFF0F;
constexpr uint16_t k_dma = 0xFF46;
constexpr uint16_t k_ie = 0xFFFF;

constexpr bool in_range(uint16_t address, uint16_t start, uint16_t end) { return address >= start && address <= end; }

constexpr size_t range_offset(uint16_t address, uint16_t start) { return static_cast<size_t>(address - start); }
} // namespace

void Memory::load_rom(const std::vector<uint8_t> &rom) { this->rom_ = rom; }

uint8_t Memory::read_byte(uint16_t address) const { return this->read_byte_impl(address, true); }

uint8_t Memory::read_byte_unrestricted(uint16_t address) const { return this->read_byte_impl(address, false); }

uint8_t Memory::read_byte_impl(uint16_t address, bool respect_locks) const {
    if (address <= k_rom_end) {
        if (address < this->rom_.size()) return this->rom_[address];
        return 0xFF;
    }

    if (in_range(address, k_vram_start, k_vram_end)) {
        if (respect_locks && this->vram_blocked_) return 0xFF;
        return this->vram_[range_offset(address, k_vram_start)];
    }

    if (in_range(address, k_eram_start, k_eram_end)) {
        if (this->eram_.empty()) return 0xFF;
        const size_t eram_index = range_offset(address, k_eram_start) % this->eram_.size();
        return this->eram_[eram_index];
    }

    if (in_range(address, k_wram_start, k_wram_end)) {
        return this->wram_[range_offset(address, k_wram_start)];
    }

    if (in_range(address, k_echo_start, k_echo_end)) {
        const uint16_t mirrored = static_cast<uint16_t>(address - 0x2000);
        return this->wram_[range_offset(mirrored, k_wram_start)];
    }

    if (in_range(address, k_oam_start, k_oam_end)) {
        if (respect_locks && this->oam_blocked_) return 0xFF;
        return this->oam_[range_offset(address, k_oam_start)];
    }

    if (in_range(address, k_not_usable_start, k_not_usable_end)) return 0xFF;

    if (in_range(address, k_io_start, k_io_end)) {
        if (address == k_joyp && this->joypad_ != nullptr) return this->joypad_->get_joyp();
        return this->io_[range_offset(address, k_io_start)];
    }

    if (in_range(address, k_hram_start, k_hram_end)) {
        return this->hram_[range_offset(address, k_hram_start)];
    }

    return this->ie_;
}

void Memory::write_byte(uint16_t address, uint8_t value) {
    if (address <= k_rom_end) return;

    if (in_range(address, k_vram_start, k_vram_end)) {
        if (this->vram_blocked_) return;
        this->vram_[range_offset(address, k_vram_start)] = value;
        return;
    }

    if (in_range(address, k_eram_start, k_eram_end)) {
        if (!this->eram_.empty()) {
            const size_t eram_index = range_offset(address, k_eram_start) % this->eram_.size();
            this->eram_[eram_index] = value;
        }
        return;
    }

    if (in_range(address, k_wram_start, k_wram_end)) {
        this->wram_[range_offset(address, k_wram_start)] = value;
        return;
    }

    if (in_range(address, k_echo_start, k_echo_end)) {
        const uint16_t mirrored = static_cast<uint16_t>(address - 0x2000);
        this->wram_[range_offset(mirrored, k_wram_start)] = value;
        return;
    }

    if (in_range(address, k_oam_start, k_oam_end)) {
        if (this->oam_blocked_) return;
        this->oam_[range_offset(address, k_oam_start)] = value;
        return;
    }

    if (in_range(address, k_not_usable_start, k_not_usable_end)) return;

    if (in_range(address, k_io_start, k_io_end)) {
        if (address == k_joyp && this->joypad_ != nullptr) {
            this->joypad_->set_joyp(value);
            this->io_[range_offset(address, k_io_start)] = value;
            return;
        }

        if (address == k_div) {
            this->io_[range_offset(address, k_io_start)] = 0;
            this->div_reset_pending_ = true;
            return;
        }

        if (address == k_dma) {
            this->dma_request_pending_ = true;
            this->dma_source_high_ = value;
        }

        this->io_[range_offset(address, k_io_start)] = value;
        return;
    }

    if (in_range(address, k_hram_start, k_hram_end)) {
        this->hram_[range_offset(address, k_hram_start)] = value;
        return;
    }

    this->ie_ = value;
}

uint16_t Memory::read_word(uint16_t address) const {
    const uint8_t low_byte = this->read_byte(address);
    const uint8_t high_byte = this->read_byte(static_cast<uint16_t>(address + 1));
    return static_cast<uint16_t>(low_byte | (static_cast<uint16_t>(high_byte) << 8));
}

void Memory::write_word(uint16_t address, uint16_t value) {
    this->write_byte(address, static_cast<uint8_t>(value & 0x00FF));
    this->write_byte(static_cast<uint16_t>(address + 1), static_cast<uint8_t>(value >> 8));
}

void Memory::write_range(size_t start, size_t end, uint8_t value) {
    if (end < start) return;

    for (size_t address = start; address <= end; ++address) {
        this->write_byte(static_cast<uint16_t>(address), value);
    }
}

void Memory::write_range(size_t start, const std::vector<uint8_t> &data) {
    for (size_t index = 0; index < data.size(); ++index) {
        this->write_byte(static_cast<uint16_t>(start + index), data[index]);
    }
}

std::vector<uint8_t> Memory::read_range(size_t start, size_t end) const {
    std::vector<uint8_t> bytes;
    if (end < start) return bytes;

    bytes.reserve(end - start + 1);
    for (size_t address = start; address <= end; ++address) {
        bytes.push_back(this->read_byte(static_cast<uint16_t>(address)));
    }
    return bytes;
}

uint8_t Memory::get_ie() const { return this->read_byte(k_ie); }

void Memory::set_ie(uint8_t value) { this->write_byte(k_ie, value); }

uint8_t Memory::get_if() const { return this->read_byte(k_if); }

void Memory::set_if(uint8_t value) { this->write_byte(k_if, value); }

void Memory::attach_joypad(Joypad *joypad) { this->joypad_ = joypad; }

uint8_t Memory::read_io_reg(uint16_t address) const {
    if (!in_range(address, k_io_start, k_io_end)) return 0xFF;
    return this->io_[range_offset(address, k_io_start)];
}

void Memory::write_io_reg(uint16_t address, uint8_t value) {
    if (!in_range(address, k_io_start, k_io_end)) return;
    this->io_[range_offset(address, k_io_start)] = value;
}

bool Memory::consume_div_reset() {
    const bool was_pending = this->div_reset_pending_;
    this->div_reset_pending_ = false;
    return was_pending;
}

void Memory::set_vram_blocked(bool blocked) { this->vram_blocked_ = blocked; }

void Memory::set_oam_blocked(bool blocked) { this->oam_blocked_ = blocked; }

uint8_t Memory::read_vram_raw(uint16_t address) const {
    if (!in_range(address, k_vram_start, k_vram_end)) return 0xFF;
    return this->vram_[range_offset(address, k_vram_start)];
}

uint8_t Memory::read_oam_raw(uint16_t address) const {
    if (!in_range(address, k_oam_start, k_oam_end)) return 0xFF;
    return this->oam_[range_offset(address, k_oam_start)];
}

void Memory::write_oam_raw(uint16_t address, uint8_t value) {
    if (!in_range(address, k_oam_start, k_oam_end)) return;
    this->oam_[range_offset(address, k_oam_start)] = value;
}

bool Memory::consume_dma_request(uint8_t &source_high) {
    if (!this->dma_request_pending_) return false;
    this->dma_request_pending_ = false;
    source_high = this->dma_source_high_;
    return true;
}
