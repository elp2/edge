#include "gtest/gtest.h"

#include "CPU.hpp"
#include "MMU.hpp"
#include "ROM.hpp"
#include "PPU.hpp"
#include "Utils.hpp"

class BitCommandsTest : public ::testing::Test {
 protected:
    BitCommandsTest() {};
    ~BitCommandsTest() {};
};

TEST(BitCommandsTest, Basic) {
    MMU *mmu = getTestingMMU();
    PPU *ppu = new PPU();
    CPU *cpu = new CPU(mmu, ppu);

    const uint8_t RLCA = 0x07;
    const uint8_t RLA = 0x17;
    const uint8_t RRCA = 0x0F;
    const uint8_t RRA = 0x1F;

    const uint16_t ram_start = 0xC000;

    mmu->SetByteAt(ram_start, RLCA);
    cpu->Set8Bit(Register_A, 0x85);
    cpu->JumpAddress(ram_start);
    cpu->Step();
    // Examples say 0x0A but that makes no sense, since the bit7 is 1 and should be in bit0.
    // The logo works when it's like this, so gonna stick with this.
    ASSERT_EQ(cpu->Get8Bit(Register_A), 0x0B);
    EXPECT_FALSE(cpu->flags.z);
    EXPECT_TRUE(cpu->flags.c);

    mmu->SetByteAt(ram_start, RLA);
    cpu->Set8Bit(Register_A, 0x95);
    cpu->flags.c = true;
    cpu->JumpAddress(ram_start);
    cpu->Step();
    EXPECT_EQ(cpu->Get8Bit(Register_A), 0x2B);
    EXPECT_FALSE(cpu->flags.z);
    EXPECT_TRUE(cpu->flags.c);

    mmu->SetByteAt(ram_start, RRA);
    cpu->Set8Bit(Register_A, 0x81);
    cpu->flags.c = false;
    cpu->JumpAddress(ram_start);
    cpu->Step();
    EXPECT_EQ(cpu->Get8Bit(Register_A), 0x40);
    EXPECT_FALSE(cpu->flags.z);
    EXPECT_TRUE(cpu->flags.c);

    mmu->SetByteAt(ram_start, RRCA);
    cpu->Set8Bit(Register_A, 0x3B);
    cpu->flags.c = false;
    cpu->JumpAddress(ram_start);
    cpu->Step();
    EXPECT_EQ(cpu->Get8Bit(Register_A), 0x9D);
    EXPECT_FALSE(cpu->flags.z);
    EXPECT_TRUE(cpu->flags.c);
}
