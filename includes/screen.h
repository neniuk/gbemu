#ifndef SCREEN_H
#define SCREEN_H

#include "config.h"

#include <SDL2/SDL.h>
#include <cstddef>
#include <stdint.h>
#include <vector>

class Screen {
  public:
    Screen();
    void set(size_t x, size_t y, uint8_t color);
    void clear();
    void draw_logo(const std::vector<uint8_t> &logo);

    void set_renderer(SDL_Renderer *renderer);
    void set_texture(SDL_Texture *texture);
    void present();

  private:
    uint8_t screen[GB_SCREEN_WIDTH][GB_SCREEN_HEIGHT];
    SDL_Renderer *renderer = nullptr;
    SDL_Texture *texture = nullptr;

    uint32_t palette[4] = {0xFF000000, 0xFF555555, 0xFFAAAAAA, 0xFFFFFFFF};
};

#endif
