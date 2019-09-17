#include "Screen.hpp"

#include <cassert>
#include <iostream>
#include "SDL.h"

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
}

void Screen::DrawPixel(Pixel pixel) {
    uint32_t color = 0xFF0000FF;
    switch(pixel.two_bit_color_) {
        case 0x00:
            color = 0xFFFF0000;
            break;
        case 0x01:
            color = 0xFF00FF00;
            break;
        case 0x02:
            color = 0xFF0000FF;
            break;
        case 0x03:
            color = 0XFFFFFFFF;
            break;
		default:
			color = 0xFF0000FF;
			break;
    }
	int pixel_index = x_ + y_ * SCREEN_WIDTH;
    assert(pixel_index < SCREEN_PIXELS);
    pixels_[pixel_index] = color;
    x_++;

    if (debugger_) {
        if (pixel.two_bit_color_ == 0x00) {
            cout << " ";
        } else {
            cout << hex << unsigned(pixel.two_bit_color_);
        }
    }
    assert(on_);
}

void Screen::NewLine() {
    y_++;
    x_ = 0;
    assert(on_);
    if (debugger_) {
        cout << "|" << endl;
    }
}

void Screen::VBlankBegan() {
    y_ = 0;    
    if (debugger_) {
        cout << "------------------------------------" << endl;
    }
}

void Screen::VBlankEnded() {
    SDL_UpdateTexture(texture_, NULL, pixels_, SCREEN_WIDTH * sizeof(Uint32));
    SDL_RenderClear(renderer_);
    SDL_RenderCopy(renderer_, texture_, NULL, NULL);
    SDL_RenderPresent(renderer_);
}

void Screen::SetPalette(Palette palette, uint8_t value) {
    (void)palette;
    (void)value;
}
