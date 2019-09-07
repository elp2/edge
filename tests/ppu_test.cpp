#include "gtest/gtest.h"

#include "PPU.hpp"
#include "Utils.hpp"

class PPUTest : public ::testing::Test {
 protected:
    PPUTest() {};
    ~PPUTest() {};
};

TEST(PPUTest, GetByte) {
    PPU *ppu = new PPU();

    ppu->SetByteAt(0xFF40, 0x05);
    uint8_t lcdc = ppu->GetByteAt(0xFF40);
    ASSERT_EQ(lcdc, 0x05);

    ppu->SetByteAt(0x8000, 0x56);
    ASSERT_EQ(ppu->GetByteAt(0x8000), 0x56);
}
