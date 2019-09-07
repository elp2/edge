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

TEST(BitCommandsTest, RLCA) {
    const uint8_t RLCA = 0x07;    
    CPU *cpu = getTestingCPUWithInstructions(vector<uint8_t>{  RLCA });
    cpu->Set8Bit(Register_A, 0x85);
    cpu->Step();

    // Examples say 0x0A but that makes no sense, since the bit7 is 1 and should be in bit0.
    // The logo works when it's like this, so gonna stick with this.
    ASSERT_EQ(cpu->Get8Bit(Register_A), 0x0B);
    EXPECT_FALSE(cpu->flags.z);
    EXPECT_TRUE(cpu->flags.c);
}

TEST(BitCommandsTest, RLCA2) {
    const uint8_t RLCA = 0x07;    
    CPU *cpu = getTestingCPUWithInstructions(vector<uint8_t>{  RLCA });
    cpu->Set8Bit(Register_A, 0x0);
    cpu->Step();

    ASSERT_EQ(cpu->Get8Bit(Register_A), 0x00);
    EXPECT_TRUE(cpu->flags.z);
    EXPECT_FALSE(cpu->flags.c);
}

TEST(BitCommandsTest, RLA) {;
    const uint8_t RLA = 0x17;

    CPU *cpu = getTestingCPUWithInstructions(vector<uint8_t>{  RLA });
    cpu->Set8Bit(Register_A, 0x95);
    cpu->flags.c = true;
    cpu->Step();

    EXPECT_EQ(cpu->Get8Bit(Register_A), 0x2B);
    EXPECT_FALSE(cpu->flags.z);
    EXPECT_TRUE(cpu->flags.c);
}

TEST(BitCommandsTest, RRA) {;
    const uint8_t RRA = 0x1F;

    CPU *cpu = getTestingCPUWithInstructions(vector<uint8_t>{  RRA });
    cpu->Set8Bit(Register_A, 0x81);
    cpu->flags.c = false;
    cpu->Step();

    EXPECT_EQ(cpu->Get8Bit(Register_A), 0x40);
    EXPECT_FALSE(cpu->flags.z);
    EXPECT_TRUE(cpu->flags.c);
}

TEST(BitCommandsTest, RRCA) {;
    const uint8_t RRCA = 0x0F;

    CPU *cpu = getTestingCPUWithInstructions(vector<uint8_t>{  RRCA });
    cpu->Set8Bit(Register_A, 0x3B);
    cpu->flags.c = false;
    cpu->Step();

    EXPECT_EQ(cpu->Get8Bit(Register_A), 0x9D);
    EXPECT_FALSE(cpu->flags.z);
    EXPECT_TRUE(cpu->flags.c);
}
