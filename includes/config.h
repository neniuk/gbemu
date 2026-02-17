#ifndef CONFIG_H
#define CONFIG_H

#define GB_WINDOW_TITLE "GBEMU"

#define GB_MEMORY_SIZE 0xFFFF

#define GB_SCREEN_WIDTH 160
#define GB_SCREEN_HEIGHT 144
#define GB_SCREEN_SCALE 4

#define GB_PROGRAM_COUNTER_START 0x0100

#if defined(GBEMU_DEBUG)
inline constexpr bool kDebugMode = true;
#else
inline constexpr bool kDebugMode = false;
#endif

#endif
