#include "gtest/gtest.h"

#include "CPU.hpp"
#include "MMU.hpp"
#include "ROM.hpp"
#include "PPU.hpp"
#include "Utils.hpp"

class CPURegistersTest : public ::testing::Test {
 protected:
    CPURegistersTest() {};
    ~CPURegistersTest() {};
};

TEST(CPURegistersTest, RegisterAF) {
    CPU *cpu = getTestingCPU();
    cpu->Set16Bit(Register_AF, 0x12ff);
    ASSERT_EQ(cpu->Get8Bit(Register_A), 0x12);
    ASSERT_TRUE(cpu->flags.c);
    ASSERT_TRUE(cpu->flags.h);
    ASSERT_TRUE(cpu->flags.n);
    ASSERT_TRUE(cpu->flags.z);

    cpu->Set16Bit(Register_AF, 0x120f);
    ASSERT_FALSE(cpu->flags.c);
    ASSERT_FALSE(cpu->flags.h);
    ASSERT_FALSE(cpu->flags.n);
    ASSERT_FALSE(cpu->flags.z);
}

TEST(CPURegistersTest, CombinedRegisters) {
    CPU *cpu = getTestingCPU();
    cpu->Set8Bit(Register_A, 0xED);
    cpu->Set8Bit(Register_B, 0x12);
    cpu->Set8Bit(Register_C, 0x34);
    uint16_t bc = cpu->Get16Bit(Register_BC);
    ASSERT_EQ(cpu->Get8Bit(Register_B), 0x12);
    ASSERT_EQ(cpu->Get8Bit(Register_C), 0x34);
    ASSERT_EQ(bc, 0x1234);

    cpu->Set8Bit(Register_C, 0xED);
    ASSERT_EQ(0x12ED, cpu->Get16Bit(Register_BC));
}

TEST(CPURegistersTest, FlagsRegister) {
    CPU *cpu = getTestingCPU();
    ASSERT_EQ(cpu->Get8Bit(Register_F), 0x00);

    cpu->flags.c = true;
    ASSERT_EQ(cpu->Get8Bit(Register_F), 0x10);

    cpu->flags.h = true;
    ASSERT_EQ(cpu->Get8Bit(Register_F), 0x30);

    cpu->flags.n = true;
    ASSERT_EQ(cpu->Get8Bit(Register_F), 0x70);

    cpu->flags.z = true;
    ASSERT_EQ(cpu->Get8Bit(Register_F), 0xf0);
}
