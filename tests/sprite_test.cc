#include "gtest/gtest.h"

#include "sprite.h"

class SpriteTest : public ::testing::Test {
 protected:
    SpriteTest() {}

    ~SpriteTest() {}
};

// TODO Re:enable sprite tests.

Sprite *testingSprite(uint8_t x, uint8_t y, uint8_t tile_number, uint8_t flags) {
    Sprite *sprite = new Sprite();
    sprite->x_ = x;
    sprite->y_ = y;
    sprite->flags_ = flags;
    sprite->tile_number_ = tile_number;
    return sprite;
}
/*
TEST(SpriteTest, Basic) {
    Sprite *sprite = testingSprite(0x12, 0x34, 0x34, 0xA0);

    EXPECT_TRUE(sprite->priority());
    EXPECT_FALSE(sprite->x_flip());
    EXPECT_TRUE(sprite->y_flip());
    EXPECT_EQ(sprite->palette(), SpritePalette0);
}
*/
TEST(SpriteTest, TileNumber) {
    Sprite *sprite = testingSprite(0x12, 0x34, 0xED, 0xA0);

    EXPECT_EQ(sprite->tile_number_, 0xED);
}
/*
TEST(SpriteTest, IntersectsRow) {
    Sprite *sprite = new Sprite(0x12, 0x34, 0xED, 0xA0);
    const int SPRITE_HEIGHT = 8;

    EXPECT_FALSE(sprite->IntersectsRow(0x33, SPRITE_HEIGHT));
    EXPECT_TRUE(sprite->IntersectsRow(0x34, SPRITE_HEIGHT));
    EXPECT_TRUE(sprite->IntersectsRow(0x35, SPRITE_HEIGHT));
    EXPECT_TRUE(sprite->IntersectsRow(0x36, SPRITE_HEIGHT));
    EXPECT_TRUE(sprite->IntersectsRow(0x37, SPRITE_HEIGHT));
    EXPECT_TRUE(sprite->IntersectsRow(0x38, SPRITE_HEIGHT));
    EXPECT_TRUE(sprite->IntersectsRow(0x39, SPRITE_HEIGHT));
    EXPECT_TRUE(sprite->IntersectsRow(0x3a, SPRITE_HEIGHT));
    EXPECT_TRUE(sprite->IntersectsRow(0x3b, SPRITE_HEIGHT));
    EXPECT_FALSE(sprite->IntersectsRow(0x3c, SPRITE_HEIGHT));
}
*/