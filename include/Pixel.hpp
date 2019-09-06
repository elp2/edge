#pragma once

#include <cstdint>

#include "Palette.hpp"

struct Pixel
{
    uint8_t two_bit_color_;
    Palette palette_;
};
