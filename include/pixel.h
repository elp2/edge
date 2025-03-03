#pragma once

#include <cstdint>

#include "palette.h"

struct Pixel {
  uint8_t two_bit_color_;
  Palette palette_;
  bool sprite_over_background_window_;
};
