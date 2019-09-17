#pragma once

#include <cstdint>

#include "Palette.hpp"
#include "Pixel.hpp"

const int SCREEN_WIDTH = 160;
const int SCREEN_HEIGHT = 144;
const int SCREEN_PIXELS = SCREEN_WIDTH * SCREEN_HEIGHT;

struct SDL_Renderer;
struct SDL_Texture;
struct SDL_Window;

using namespace std;

class Screen {
 private:
    bool on_ = false;
    bool debugger_ = false;
    uint32_t *pixels_;
    uint32_t *palettes_;
    int x_ = 0;
    int y_ = 0;

    void InitSDL();
    SDL_Renderer *renderer_;
    SDL_Texture *texture_;  
    SDL_Window *window_;



 public:
    Screen();
    ~Screen() = default;

    void DrawPixel(Pixel pixel);
    void NewLine();
    void VBlankBegan();
    void VBlankEnded();
	void SetPalette(Palette palette, uint8_t value);
    bool on() { return on_; };
    void set_on(bool on) { on_ = on; };
};
