#include "screen.h"

#include <cassert>
#include <iostream>

#ifdef BUILD_IOS
#include <SDL3/SDL.h>
#else
#include "SDL.h"
#endif

const uint8_t DEFAULT_PALETTE = 0xE4;  // 11100100.
const int PIXEL_SCALE = 4;

Screen::Screen() { InitSDL(); }

void Screen::InitSDL() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        const char* error = SDL_GetError();
        std::cout << "Error in SDL_Init: " << error << std::endl;
        assert(false);
    }
    
    /* TODO Re-enable window.
    // Window creation is mostly the same, but SDL_WINDOWPOS_CENTERED is now SDL_WINDOWPOS_CENTER
    window_ = SDL_CreateWindow("EDGE",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH * PIXEL_SCALE,
        SCREEN_HEIGHT * PIXEL_SCALE,
        0);

    // Renderer creation is similar, but driver index -1 is now nullptr
    renderer_ = SDL_CreateRenderer(window_, nullptr, SDL_RENDERER_SOFTWARE);
    */
    // Texture creation uses SDL_CreateTexture still, but pixel format constants have changed
    texture_ = SDL_CreateTexture(renderer_,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STATIC,
        SCREEN_WIDTH,
        SCREEN_HEIGHT);

    SDL_RenderClear(renderer_);
    SDL_RenderPresent(renderer_);

    pixels_ = new uint32_t[SCREEN_WIDTH * SCREEN_HEIGHT];
    palettes_ = new uint32_t[3];
    palettes_[0] = palettes_[1] = palettes_[2] = DEFAULT_PALETTE;
    frame_start_ms_ = SDL_GetTicks();  // SDL_GetTicks returns uint32_t in SDL3
    frames_ = 0;
}

void Screen::DrawPixel(Pixel pixel) {
  int pixel_index = x_ + y_ * SCREEN_WIDTH;
  assert(pixel_index < SCREEN_PIXELS);
  pixels_[pixel_index] = GetScreenColor(pixel);;
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
  // SDL_UpdateTexture signature is slightly different in SDL3 but compatible
  SDL_UpdateTexture(texture_, nullptr, pixels_, SCREEN_WIDTH * sizeof(uint32_t));
  SDL_RenderClear(renderer_);
  SDL_RenderTexture(renderer_, texture_, nullptr, nullptr);
  SDL_RenderPresent(renderer_);
        
  if (++frames_ == 60) {
    if (debugger_) {
      unsigned int sixty_frames_ms = SDL_GetTicks() - frame_start_ms_;
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
    int counter = 0;
    string filename;
    do {
        filename = base_name + "_" + to_string(counter) + ".bmp";
        counter++;
        if (counter > 1000) {
            cout << "Too many screenshots, aborting." << endl;
            return;
        }
    } while (SDL_IOFromFile(filename.c_str(), "r") != NULL);

    // (int width, int height, SDL_PixelFormat format, void *pixels, int pitch);
    SDL_Surface* surface = SDL_CreateSurfaceFrom(
                                                 SCREEN_WIDTH,
                                                 SCREEN_HEIGHT,
                                                 SDL_PIXELFORMAT_ARGB8888,
            pixels_,
            SCREEN_WIDTH * 4
        );

    if (surface == NULL) {
        cout << "Failed to create surface for screenshot: " << SDL_GetError() << endl;
        return;
    }

    if (SDL_SaveBMP(surface, filename.c_str()) != 0) {
        cout << "Failed to save screenshot: " << SDL_GetError() << endl;
    } else {
        cout << "Screenshot saved as: " << filename << endl;
    }

    SDL_DestroySurface(surface);
}
