#include "Screen.hpp"

#include <cassert>
#include <iostream>

Screen::Screen() {

}

void Screen::DrawPixel(Pixel pixel) {
    if (pixel.two_bit_color_ != 0) {
        debugger_ = true;
    }
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
    assert(on_);
    if (debugger_) {
        cout << "|" << endl;
    }
}

void Screen::VBlank() {
    if (debugger_) {
        cout << "------------------------------------" << endl;
    }
}

void Screen::SetPalette(Palette palette, uint8_t value) {
    (void)palette;
    (void)value;
}