#pragma once

#include <cstdint>

#include "Palette.hpp"

class Screen {
 private:

 public:
    Screen();
    ~Screen() = default;

    void DrawPixel(uint8_t pixel, Palette palette);
    void NewLine();
    void Show();
	void SetPalette(Palette palette, uint8_t value);
};
