#include "Screen.hpp"

#include <cassert>
#include <iostream>

Screen::Screen() {

}

void Screen::DrawPixel(Pixel pixel) {
    assert(on_);
    cout << hex << unsigned(pixel.two_bit_color_);
}

void Screen::NewLine() {
    assert(on_);
    cout << endl;
}

void Screen::Show() {

}

void Screen::SetPalette(Palette palette, uint8_t value) {
    (void)palette;
    (void)value;
}