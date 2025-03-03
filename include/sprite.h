#pragma once

#include <cstdint>

#include "palette.h"

struct Sprite {
  int x_;
  int y_;
  uint8_t tile_number_;
  uint8_t flags_;  // Priority, flips, palette.
};

// At one point, these were objects but it was too slow.
// Now they are just functions.
bool SpriteYIntersectsRow(int sprite_y, int row, int sprite_height);

bool SpriteOverBackgroundWindow(Sprite sprite);
bool SpriteFlippedY(Sprite sprite);
bool SpriteFlippedX(Sprite sprite);
bool SpriteUsesPalette1(Sprite sprite);
