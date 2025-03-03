#include "sprite.h"

#include "utils.h"

bool SpriteYIntersectsRow(int sprite_y, int row, int sprite_height) {
  return row >= sprite_y && row < (sprite_y + sprite_height);
}

bool SpriteOverBackgroundWindow(Sprite sprite) { 
  return !bit_set(sprite.flags_, 7);
}

bool SpriteFlippedY(Sprite sprite) { 
  return bit_set(sprite.flags_, 6); 
}

bool SpriteFlippedX(Sprite sprite) {
  return bit_set(sprite.flags_, 5);
}

bool SpriteUsesPalette1(Sprite sprite) {
  return bit_set(sprite.flags_, 4);
}
