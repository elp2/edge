#include "Screen.hpp"

#include <cassert>
#include <iostream>
#include "SDL.h"

const uint32_t DARKEST_GREEN = 0xFF0F380F;
const uint32_t DARK_GREEN = 0xFF306230;
const uint32_t LIGHT_GREEN = 0xFF8BAC0F;
const uint32_t LIGHEST_GREEN = 0xFF9BBC0F;
const uint8_t DEFAULT_PALETTE = 0xE4; // 11100100.

Screen::Screen() {
    InitSDL();
}

void Screen::InitSDL() {
    if (SDL_Init(SDL_INIT_VIDEO)) {
        const char * error = SDL_GetError();
        cout << "Error in SDL_Init: " << error << endl;
        assert(false);
    }
    window_ = SDL_CreateWindow(
        "EDGE",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH * 2,
        SCREEN_HEIGHT * 2,
        0
    );

    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_SOFTWARE);
    texture_ = SDL_CreateTexture(renderer_,
        SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, SCREEN_WIDTH, SCREEN_HEIGHT);
    
    SDL_RenderClear(renderer_);
    SDL_RenderPresent(renderer_);

    pixels_ = new uint32_t[SCREEN_WIDTH * SCREEN_HEIGHT];
    palettes_ = new uint32_t[3];
    palettes_[0] = palettes_[1] = palettes_[2] = DEFAULT_PALETTE;
    frame_start_ms_ = SDL_GetTicks();
    frames_ = 0;
}

void Screen::DrawPixel(Pixel pixel) {
    uint8_t palette_pixel = palettes_[pixel.palette_];
    palette_pixel >>= (pixel.two_bit_color_ * 2);

    uint32_t color = 0;
    switch(palette_pixel & 0x3) {
        case 0x00:
            color = LIGHEST_GREEN;
            break;
        case 0x01:
            color = LIGHT_GREEN;
            break;
        case 0x02:
            color = DARK_GREEN;
            break;
        case 0x03:
            color = DARKEST_GREEN;
            break;
		default:
			color = 0xFFFF69B4; // Hot pink error.
			break;
    }
	int pixel_index = x_ + y_ * SCREEN_WIDTH;
    assert(pixel_index < SCREEN_PIXELS);
    pixels_[pixel_index] = color;
    x_++;

    assert(on_);
}

void Screen::NewLine() {
    y_++;
    x_ = 0;
    assert(on_);
}

void Screen::VBlankBegan() {
    y_ = 0;
}

void Screen::VBlankEnded() {
    SDL_UpdateTexture(texture_, NULL, pixels_, SCREEN_WIDTH * sizeof(Uint32));
    SDL_RenderClear(renderer_);
    SDL_RenderCopy(renderer_, texture_, NULL, NULL);
    SDL_RenderPresent(renderer_);
    if (++frames_ == 60) {
        if (debugger_) {
            unsigned int sixty_frames_ms = SDL_GetTicks() - frame_start_ms_;
            cout << "1 second: avg " << dec << unsigned((1000 * sixty_frames_ms) / 60) << " ms / frame." << endl;
        }
        frames_ = 0;
        frame_start_ms_ = SDL_GetTicks();
    }
}

void Screen::SetPalette(Palette palette, uint8_t value) {
    palettes_[palette] = value;
}
