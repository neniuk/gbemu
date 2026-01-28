#include "screen.h"

#include <assert.h>
#include <cstddef>
#include <cstring>
#include <stdint.h>
#include <vector>

Screen::Screen() { memset(this->screen, 0, sizeof(this->screen)); }
void Screen::set(size_t x, size_t y) {
    // clang-format off
    assert(
        x >= 0 && x < GB_SCREEN_WIDTH && 
        y >= 0 && y < GB_SCREEN_HEIGHT
    );
    // clang-format on
    this->screen[x][y] = true;
}
bool Screen::is_set(size_t x, size_t y) {
    // clang-format off
    assert(
        x >= 0 && x < GB_SCREEN_WIDTH && 
        y >= 0 && y < GB_SCREEN_HEIGHT
    );
    // clang-format on
    return this->screen[x][y] == true;
}
void Screen::unset(size_t x, size_t y) {
    // clang-format off
    assert(
        x >= 0 && x < GB_SCREEN_WIDTH && 
        y >= 0 && y < GB_SCREEN_HEIGHT
    );
    // clang-format on
    this->screen[x][y] = false;
}
void Screen::clear() { memset(this->screen, 0, sizeof(this->screen)); }
