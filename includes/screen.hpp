#pragma once

#include "config.hpp"

#include <SDL2/SDL.h>
#include <cstddef>
#include <cstdint>
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
    uint8_t screen_[config::k_screen_width][config::k_screen_height];
    SDL_Renderer *renderer_ = nullptr;
    SDL_Texture *texture_ = nullptr;

    uint32_t palette_[4] = {0xFF000000, 0xFF555555, 0xFFAAAAAA, 0xFFFFFFFF};
};
