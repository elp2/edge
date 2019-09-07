#pragma once

#include <cstdint>

#include "Palette.hpp"
#include "Pixel.hpp"

using namespace std;

class Screen {
 private:
    bool on_ = false;
    bool debugger_ = false;
    uint32_t *pixels_;
    int x_ = 0;
    int y_ = 0;

 public:
    Screen();
    ~Screen() = default;

    void SetTexturePixels(uint32_t *pixels);

    void DrawPixel(Pixel pixel);
    void NewLine();
    void VBlank();
	void SetPalette(Palette palette, uint8_t value);
    bool on() { return on_; };
    void set_on(bool on) { on_ = on; };
};
