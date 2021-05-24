#include "sprite.h"

#include <iostream>

using namespace std;

bool SpriteYIntersectsRow(uint8_t sprite_y, int row, int sprite_height) {
  return row >= sprite_y && row < (sprite_y + sprite_height);
}