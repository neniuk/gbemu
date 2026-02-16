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
    this->screen[x][y] = color; // 0 - 3, 0 = black, 1 = dark gray, 2 = light gray, 3 = white
}
void Screen::clear() { memset(this->screen, 0, sizeof(this->screen)); }

void Screen::draw_logo(const std::vector<uint8_t> &logo) {
    if (logo.size() < 48) return;

    const size_t logo_w = 48;
    const size_t logo_h = 8;

    const size_t off_x = (GB_SCREEN_WIDTH - logo_w) / 2;
    const size_t off_y = 8;

    for (size_t y = 0; y < logo_h; ++y) {
        for (size_t xb = 0; xb < 6; ++xb) {
            const uint8_t byte = logo[y * 6 + xb];

            for (size_t bit = 0; bit < 8; ++bit) {
                const bool on = (byte & (0x80 >> bit)) != 0;
                if (on) {
                    const size_t x = off_x + xb * 8 + bit;
                    this->set(x, off_y + y, 3);
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
