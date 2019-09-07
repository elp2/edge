#include "Screen.hpp"

#include <cassert>
#include <iostream>

Screen::Screen() {
}

void Screen::SetTexturePixels(uint32_t *pixels) {
    pixels_ = pixels;
}

void Screen::DrawPixel(Pixel pixel) {
    uint32_t color;
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
    }
    assert(x_ + y_ * 160 < 160 * 144);
    pixels_[x_ + y_ * 160] = color;
    x_++;

    // if (pixel.two_bit_color_ != 0) {
    //     debugger_ = true;
    // }
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

void Screen::VBlank() {
    y_ = 0;    
    if (debugger_) {
        cout << "------------------------------------" << endl;
    }
}

void Screen::SetPalette(Palette palette, uint8_t value) {
    (void)palette;
    (void)value;
}