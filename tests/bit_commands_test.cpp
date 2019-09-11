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

TEST(BitCommandsTest, CP) {;
    const uint8_t CP_E = 0xBB;

    const uint8_t a = 0xE8;
    CPU *cpu = getTestingCPUWithInstructions(vector<uint8_t>{  CP_E, CP_E, CP_E });

    cpu->Set8Bit(Register_A, a);
    cpu->Set8Bit(Register_E, a);
    cpu->Step();

    EXPECT_EQ(cpu->Get8Bit(Register_A), a);
    EXPECT_FLAGS(true, false, true, false);


    cpu->Set8Bit(Register_A, a);
    cpu->Set8Bit(Register_E, a + 1);
    cpu->Step();

    EXPECT_EQ(cpu->Get8Bit(Register_A), a);
    EXPECT_FLAGS(false, true, true, true);

    cpu->Set8Bit(Register_A, a + 1);
    cpu->Set8Bit(Register_E, a);
    cpu->Step();

    EXPECT_EQ(cpu->Get8Bit(Register_A), a + 1);
    EXPECT_FLAGS(false, false, true, false);
}

TEST(BitCommandsTest, CP2) {;
    const uint8_t CP_B = 0xB8;
    const uint8_t CP_N = 0xFE;
    const uint8_t CP_HLA = 0xBE;

    CPU *cpu = getTestingCPUWithInstructions(vector<uint8_t>{ CP_B, CP_N, 0x3C, CP_HLA });

    cpu->Set8Bit(Register_A, 0x3C);
    cpu->Set8Bit(Register_B, 0x2F);
    cpu->Step();

    EXPECT_FLAGS(false, true, true, false);

    cpu->Set8Bit(Register_A, 0x3C);
    cpu->Step();

    EXPECT_FLAGS(true, false, true, false);

    cpu->Set8Bit(Register_A, 0x3C);
    cpu->Set16Bit(Register_HL, 0x9999);
    cpu->Set8Bit(Address_HL, 0x40);
    cpu->Step();

    EXPECT_EQ(cpu->Get8Bit(Register_A), 0x3C);
    EXPECT_FLAGS(false, false, true, true);
}
