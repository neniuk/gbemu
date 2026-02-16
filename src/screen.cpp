#include "screen.h"

#include <assert.h>
#include <cstddef>
#include <cstring>
#include <stdint.h>
#include <vector>

Screen::Screen() { memset(this->screen, 0, sizeof(this->screen)); }
void Screen::set(size_t x, size_t y, uint8_t color) {
    assert(color >= 0 && color <= 3);
    // clang-format off
    assert(
        x >= 0 && x < GB_SCREEN_WIDTH && 
        y >= 0 && y < GB_SCREEN_HEIGHT
    );
    // clang-format on
    this->screen[x][y] = color;
}
void Screen::clear() { memset(this->screen, 0, sizeof(this->screen)); }

void Screen::draw_logo(const std::vector<uint8_t> &logo) {
    if (logo.size() < 48) return;

    const size_t logo_w = 48;
    const size_t logo_h = 8;

    const size_t off_x = (GB_SCREEN_WIDTH - logo_w) / 2;
    const size_t off_y = (GB_SCREEN_HEIGHT - logo_h) / 2;

    for (size_t half = 0; half < 2; ++half) {
        for (size_t i = 0; i < 24; ++i) {
            const uint8_t byte = logo[half * 24 + i];

            for (size_t n = 0; n < 2; ++n) {
                const uint8_t nibble = (n == 0) ? static_cast<uint8_t>(byte >> 4) : static_cast<uint8_t>(byte & 0x0F);
                const size_t group = i * 2 + n;
                const size_t gx = group / 4;
                const size_t gy = group % 4;

                for (size_t bit = 0; bit < 4; ++bit) {
                    const bool on = (nibble & (0x08 >> bit)) != 0;
                    if (on) {
                        const size_t x = off_x + gx * 4 + bit;
                        const size_t y = off_y + half * 4 + gy;
                        this->set(x, y, 3);
                    }
                }
            }
        }
    }
}

void Screen::set_renderer(SDL_Renderer *renderer) { this->renderer = renderer; }
void Screen::set_texture(SDL_Texture *texture) { this->texture = texture; }
void Screen::present() {
    assert(this->renderer != nullptr);
    assert(this->texture != nullptr);

    uint32_t pixels[GB_SCREEN_WIDTH * GB_SCREEN_HEIGHT];
    for (size_t y = 0; y < GB_SCREEN_HEIGHT; ++y) {
        for (size_t x = 0; x < GB_SCREEN_WIDTH; ++x) {
            pixels[y * GB_SCREEN_WIDTH + x] = this->palette[this->screen[x][y]];
        }
    }

    SDL_UpdateTexture(this->texture, nullptr, pixels, GB_SCREEN_WIDTH * sizeof(uint32_t));

    SDL_RenderClear(this->renderer);
    SDL_RenderCopy(this->renderer, this->texture, nullptr, nullptr);
    SDL_RenderPresent(this->renderer);
}
