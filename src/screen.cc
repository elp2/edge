#include "screen.h"

#include <cassert>
#include <iostream>

#include <SDL3/SDL.h>

const uint8_t DEFAULT_PALETTE = 0xE4;  // 11100100.

#ifndef BUILD_IOS
const int PIXEL_SCALE = 4;
#endif

Screen::Screen() { 
  InitSDL(); 

  pixels_front_ = new uint32_t[SCREEN_WIDTH * SCREEN_HEIGHT];
  pixels_back_ = new uint32_t[SCREEN_WIDTH * SCREEN_HEIGHT];
  palettes_ = new uint32_t[3];
  palettes_[0] = palettes_[1] = palettes_[2] = DEFAULT_PALETTE;
  frame_start_ms_ = SDL_GetTicks();
  frames_ = 0;
}

void Screen::InitSDL() {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        const char* error = SDL_GetError();
        std::cout << "Error in SDL_Init: " << error << std::endl;
        assert(false);
    }

#ifndef BUILD_IOS
    window_ = SDL_CreateWindow("EDGE",
        SCREEN_WIDTH * PIXEL_SCALE,
        SCREEN_HEIGHT * PIXEL_SCALE,
        SDL_WINDOW_HIGH_PIXEL_DENSITY);

    renderer_ = SDL_CreateRenderer(window_, 0);
#endif

    texture_ = SDL_CreateTexture(renderer_,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STATIC,
        SCREEN_WIDTH,
        SCREEN_HEIGHT);

    SDL_RenderClear(renderer_);
    SDL_RenderPresent(renderer_);
}

void Screen::DrawPixel(Pixel pixel) {
  int pixel_index = x_ + y_ * SCREEN_WIDTH;
  assert(pixel_index < SCREEN_PIXELS);
  pixels_back_[pixel_index] = GetScreenColor(pixel);
  x_++;
}

uint32_t Screen::GetScreenColor(Pixel pixel) {
  uint8_t palette_pixel = palettes_[pixel.palette_];
  palette_pixel >>= (pixel.two_bit_color_ * 2);

  if (style_ == ScreenStyle_Green) {
    switch (palette_pixel & 0x3) {
      case 0x00:
        return 0xFF9BBC0F;
      case 0x01:
        return 0xFF8BAC0F; // Light green.
      case 0x02:
        return 0xFF306230; // Dark green.
      case 0x03:
        return 0xFF0F380F; // Darkest green.
      default:
        return 0xFFFF69B4;  // Hot pink error.
    }
  } else if (style_ == ScreenStyle_White) {
    switch (palette_pixel & 0x3) {
      case 0x00:
        return 0xFFFFFFFF;
      case 0x01:
        return 0xFFAAAAAA;
      case 0x02:
        return 0xFF555555;
      case 0x03:
        return 0xFF000000;
      default:
        return 0xFFFFB469;
    }
  }
  assert(false);
  return 0;
}

void Screen::NewLine() {
  y_++;
  x_ = 0;
}

void Screen::VBlankBegan() { y_ = 0; }

void Screen::VBlankEnded() {
  std::lock_guard<std::mutex> lock(pixels_mutex_);
  std::swap(pixels_front_, pixels_back_);

  SDL_UpdateTexture(texture_, nullptr, pixels_front_, SCREEN_WIDTH * sizeof(uint32_t));
  SDL_RenderClear(renderer_);
  SDL_RenderTexture(renderer_, texture_, nullptr, nullptr);
  SDL_RenderPresent(renderer_);
        
  if (++frames_ == 60) {
    if (debugger_) {
      unsigned long long sixty_frames_ms = SDL_GetTicks() - frame_start_ms_;
      cout << "1 second: avg " << dec << unsigned((1000 * sixty_frames_ms) / 60)
           << " ms / frame." << endl;
    }
    frames_ = 0;
    frame_start_ms_ = SDL_GetTicks();
  }
}

void Screen::SetPalette(Palette palette, uint8_t value) {
  palettes_[palette] = value;
}

void Screen::SaveScreenshot(const string& base_name) {
    string filename = base_name + "_" + to_string(screenshot_++) + ".bmp";
    if (screenshot_ >= 1000) {
        screenshot_ = 0;
    }

    SDL_Surface* surface = SDL_CreateSurfaceFrom(
                                                 SCREEN_WIDTH,
                                                 SCREEN_HEIGHT,
                                                 SDL_PIXELFORMAT_ARGB8888,
            pixels_front_,
            SCREEN_WIDTH * 4
        );

    if (surface == NULL) {
        cout << "Failed to create surface for screenshot: " << SDL_GetError() << endl;
        return;
    }

    if (SDL_SaveBMP(surface, filename.c_str())) {
        cout << "Screenshot saved as: " << filename << endl;
    } else {
        cout << "Failed to save screenshot: " << SDL_GetError() << endl;
    }

    SDL_DestroySurface(surface);
}
