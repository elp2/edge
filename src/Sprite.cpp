#include "Sprite.hpp"

#include <iostream>

using namespace std;

Sprite::Sprite(uint8_t x_byte, uint8_t y_byte, uint8_t tile_number_byte, uint8_t flags_byte) {
    x_ = x_byte;
    y_ = y_byte;
    tile_number_ = tile_number_byte;
    flags_ = flags_byte;
}

bool Sprite::IntersectsRow(int row, int sprite_height) {
    if (x_ == 0) { 
        // Off the screen since bytes are 8 pixels wide.
        return false;
    }

    return row >= y_ && row < (y_ + sprite_height);
}
