#pragma once

#include <cstdint>

namespace config {
inline constexpr uint16_t pc_entrypoint = 0x0100;
inline constexpr uint32_t memory_size = 0x10000;
inline constexpr uint16_t final_address = 0xFFFF;

inline constexpr uint16_t screen_width = 160;
inline constexpr uint16_t screen_height = 144;
inline constexpr uint16_t screen_scale = 4;

inline constexpr char window_title[] = "GBEMU";

#if defined(GBEMU_DEBUG)
inline constexpr bool debug_mode = true;
#else
inline constexpr bool debug_mode = false;
#endif
} // namespace config
