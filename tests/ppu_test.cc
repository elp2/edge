#include "ppu.h"

#include "gtest/gtest.h"
#include "screen.h"
#include "utils.h"

class PPUTest : public ::testing::Test {
 protected:
  PPUTest(){};
  ~PPUTest(){};
};

TEST(PPUTest, GetByte) {
  PPU *ppu = new PPU(new Screen());

  ppu->SetByteAt(0xFF40, 0x91);
  uint8_t lcdc = ppu->GetByteAt(0xFF40);
  ASSERT_EQ(lcdc, 0x91);

  ppu->SetByteAt(0x8000, 0x56);
  ASSERT_EQ(ppu->GetByteAt(0x8000), 0x56);
}
