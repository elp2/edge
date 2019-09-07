#pragma once

#include <cstdint>

#include "Palette.hpp"

struct Sprite {
	int x_;
	int y_;
	uint8_t tile_number_;
	uint8_t flags_; // Priority, flips, palette.
};

bool SpriteYIntersectsRow(uint8_t sprite_y, int row, int sprite_height);

/*
class Sprite {
private:
    int x_;
    int y_;
    uint8_t tile_number_;
    uint8_t flags_; // Priority, flips, palette.

public:
    Sprite();
    ~Sprite() = default;

    bool IntersectsRow(int row, int sprite_height);

    // If false, only overrites color 0 of BG and window.
    bool priority() { return flags_ & 0x80; };
    uint8_t tile_number() { return tile_number_; };
    bool x_flip() { return flags_ & 0x40; };
    bool y_flip() { return flags_ & 0x20; };
    Palette palette() { return flags_ & 0x10 ?  SpritePalette1 : SpritePalette0; };
};
*/