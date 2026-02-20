#pragma once

#include <cstdint>

namespace config {
inline constexpr uint16_t k_pc_entrypoint = 0x0100;
inline constexpr uint32_t k_memory_size = 0x10000;
inline constexpr uint16_t k_final_address = 0xFFFF;

inline constexpr uint16_t k_screen_width = 160;
inline constexpr uint16_t k_screen_height = 144;
inline constexpr uint16_t k_screen_scale = 4;

inline constexpr char k_window_title[] = "GBEMU";

#if defined(GBEMU_DEBUG)
inline constexpr bool k_debug_mode = true;
#else
inline constexpr bool k_debug_mode = false;
#endif
} // namespace config
