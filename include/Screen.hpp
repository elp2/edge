#pragma once

#include <cstdint>

#include "Palette.hpp"
#include "Pixel.hpp"

using namespace std;

class Screen {
 private:
    bool on_ = false;

 public:
    Screen();
    ~Screen() = default;

    void DrawPixel(Pixel pixel);
    void NewLine();
    void Show();
	void SetPalette(Palette palette, uint8_t value);
    bool on() { return on_; };
    void set_on(bool on) { on_ = on; };
};
