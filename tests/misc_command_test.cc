#include "gtest/gtest.h"

#include "cpu.h"
#include "utils.h"

class MiscCommandTest : public ::testing::Test {
 protected:
    MiscCommandTest() {};
    ~MiscCommandTest() {};
};

TEST(MiscCommandTest, DAA) {
    const uint8_t ADD_AB = 0x80;
    const uint8_t SUB_AB = 0x90;
    const uint8_t DAA = 0x27;
    CPU *cpu = getTestingCPUWithInstructions(vector<uint8_t>{ ADD_AB, DAA, SUB_AB, DAA });

    cpu->Set8Bit(Register_A, 0x45);
    cpu->Set8Bit(Register_B, 0x38);
    cpu->Step();
    ASSERT_EQ(cpu->Get8Bit(Register_A), 0x7D);
    ASSERT_FALSE(cpu->flags.n);

    cpu->Step();
    ASSERT_EQ(cpu->Get8Bit(Register_A), 0x83);
    ASSERT_FALSE(cpu->flags.c);

    // SUB.
    cpu->Step();
    ASSERT_EQ(cpu->Get8Bit(Register_A), 0x4B);
    ASSERT_TRUE(cpu->flags.n);

    cpu->Step();
    ASSERT_EQ(cpu->Get8Bit(Register_A), 0x45);
}

TEST(MiscCommandTest, CPL) {
    const uint8_t CPL = 0x2F;
    CPU *cpu = getTestingCPUWithInstructions(vector<uint8_t>{ CPL });

    cpu->Set8Bit(Register_A, 0x35);
    cpu->Step();
    ASSERT_EQ(cpu->Get8Bit(Register_A), 0xCA);
    ASSERT_EQ(4, cpu->cycles());
    ASSERT_TRUE(cpu->flags.n);
    ASSERT_TRUE(cpu->flags.h);
}

TEST(MiscCommandTest, CCF) {
    const uint8_t CCF = 0x3F;
    CPU *cpu = getTestingCPUWithInstructions(vector<uint8_t>{ CCF, CCF });

    cpu->flags.z = true;
    cpu->flags.n = true;
    cpu->flags.h = true;
    cpu->flags.c = false;
    cpu->Step();
    EXPECT_FLAGS(true, false, false, true);
    cpu->Step();
    EXPECT_FLAGS(true, false, false, false);
}

TEST(MiscCommandTest, SCF) {
    const uint8_t SCF = 0x37;
    CPU *cpu = getTestingCPUWithInstructions(vector<uint8_t>{ SCF, SCF });

    cpu->flags.z = true;
    cpu->flags.n = true;
    cpu->flags.h = true;
    cpu->flags.c = false;
    cpu->Step();
    EXPECT_FLAGS(true, false, false, true);
    cpu->Step();
    EXPECT_FLAGS(true, false, false, true);
}

