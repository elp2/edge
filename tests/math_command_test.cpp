#include "gtest/gtest.h"

#include "CPU.hpp"
#include "Utils.hpp"

class MathCommandTest : public ::testing::Test {
 protected:
    MathCommandTest() {};
    ~MathCommandTest() {};
};

// TODO: ADD
// TODO: ADC

// TODO: SUB

// TODO: INC
// TODO: DEC

TEST(MathCommandTest, AddHLNoCarries) {
    const uint8_t ADDHLBC = 0x09;
    const uint8_t ADDHLHL = 0x29;
    CPU *cpu = getTestingCPUWithInstructions(vector<uint8_t>{ ADDHLBC, ADDHLBC, ADDHLHL });
    uint32_t cycles = cpu->cycles();

    cpu->Set16Bit(Register_BC, 0x00);
    cpu->Set16Bit(Register_HL, 0x00);
    cpu->flags.z = false;
    cpu->Step();

    ASSERT_EQ(cycles + 8, cpu->cycles());
    ASSERT_EQ(cpu->Get16Bit(Register_HL), 0x00);
    // Zero flags not affected.
    ASSERT_FALSE(cpu->flags.z);
    ASSERT_FALSE(cpu->flags.n);
    ASSERT_FALSE(cpu->flags.h);
    ASSERT_FALSE(cpu->flags.c);

    cpu->Set16Bit(Register_BC, 0x01);
    cpu->Step();
    ASSERT_EQ(cpu->Get16Bit(Register_HL), 0x01);
    ASSERT_FALSE(cpu->flags.z);
    ASSERT_FALSE(cpu->flags.n);
    ASSERT_FALSE(cpu->flags.h);
    ASSERT_FALSE(cpu->flags.c);

    cpu->Set16Bit(Register_HL, 0x10);
    cpu->Step();
    ASSERT_EQ(cpu->Get16Bit(Register_HL), 0x20);
    ASSERT_EQ(cpu->Get16Bit(Register_BC), 0x01);
    ASSERT_FALSE(cpu->flags.z);
    ASSERT_FALSE(cpu->flags.n);
    ASSERT_FALSE(cpu->flags.h);
    ASSERT_FALSE(cpu->flags.c);
}

TEST(MathCommandTest, AddHLHalfCarries) {
    const uint8_t ADDHLBC = 0x09;
    const uint8_t ADDHLHL = 0x29;
    CPU *cpu = getTestingCPUWithInstructions(vector<uint8_t>{ ADDHLBC, ADDHLBC, ADDHLHL });
    uint32_t cycles = cpu->cycles();

    cpu->Set16Bit(Register_BC, 0xfff);
    cpu->Set16Bit(Register_HL, 0x01);
    cpu->flags.z = false;
    cpu->Step();

    ASSERT_EQ(cycles + 8, cpu->cycles());
    ASSERT_EQ(cpu->Get16Bit(Register_HL), 0x1000);
    // Zero flags not affected.
    ASSERT_FALSE(cpu->flags.z);
    ASSERT_FALSE(cpu->flags.n);
    ASSERT_TRUE(cpu->flags.h);
    ASSERT_FALSE(cpu->flags.c);
}

TEST(MathCommandTest, AddHLFullCarries) {
    const uint8_t ADDHLBC = 0x09;
    const uint8_t ADDHLHL = 0x29;
    CPU *cpu = getTestingCPUWithInstructions(vector<uint8_t>{ ADDHLBC, ADDHLBC, ADDHLHL });
    uint32_t cycles = cpu->cycles();

    cpu->Set16Bit(Register_BC, 0xffff);
    cpu->Set16Bit(Register_HL, 0x01);
    cpu->flags.z = false;
    cpu->Step();

    ASSERT_EQ(cycles + 8, cpu->cycles());
    ASSERT_EQ(cpu->Get16Bit(Register_HL), 0x00);
    // Zero flags not affected.
    ASSERT_FALSE(cpu->flags.z);
    ASSERT_FALSE(cpu->flags.n);
    ASSERT_TRUE(cpu->flags.h);
    ASSERT_TRUE(cpu->flags.c);
}

TEST(MathCommandTest, AddHLFullNoHalfCarries) {
    const uint8_t ADDHLBC = 0x09;
    const uint8_t ADDHLHL = 0x29;
    CPU *cpu = getTestingCPUWithInstructions(vector<uint8_t>{ ADDHLBC, ADDHLBC, ADDHLHL });
    uint32_t cycles = cpu->cycles();

    cpu->Set16Bit(Register_BC, 0x8000);
    cpu->Set16Bit(Register_HL, 0x8023);
    cpu->flags.z = false;
    cpu->Step();

    ASSERT_EQ(cycles + 8, cpu->cycles());
    ASSERT_EQ(cpu->Get16Bit(Register_HL), 0x23);
    // Zero flags not affected.
    ASSERT_FALSE(cpu->flags.z);
    ASSERT_FALSE(cpu->flags.n);
    ASSERT_FALSE(cpu->flags.h);
    ASSERT_TRUE(cpu->flags.c);
}

TEST(MathCommandTest, AddHLExamples) {
    const uint8_t ADDHLBC = 0x09;
    const uint8_t ADDHLHL = 0x29;
    CPU *cpu = getTestingCPUWithInstructions(vector<uint8_t>{ ADDHLBC, ADDHLHL });
    uint32_t cycles = cpu->cycles();

    cpu->Set16Bit(Register_BC, 0x0605);
    cpu->Set16Bit(Register_HL, 0x8A23);
    cpu->flags.z = false;
    cpu->Step();

    ASSERT_EQ(cycles + 8, cpu->cycles());
    ASSERT_EQ(cpu->Get16Bit(Register_HL), 0x9028);
    // Zero flags not affected.
    ASSERT_FALSE(cpu->flags.z);
    ASSERT_FALSE(cpu->flags.n);
    ASSERT_TRUE(cpu->flags.h);
    ASSERT_FALSE(cpu->flags.c);

    cpu->Set16Bit(Register_HL, 0x8A23);
    cpu->Step();
    ASSERT_EQ(cycles + 16, cpu->cycles());
    ASSERT_EQ(cpu->Get16Bit(Register_HL), 0x1446);
    // Zero flags not affected.
    ASSERT_FALSE(cpu->flags.z);
    ASSERT_FALSE(cpu->flags.n);
    ASSERT_TRUE(cpu->flags.h);
    ASSERT_TRUE(cpu->flags.c);
}

TEST(MathCommandTest, SBC) {
    const uint8_t SBC_AH = 0x9C;
    const uint8_t SBC_N = 0xDE;
    const uint8_t SBC_Address_HL = 0x9E;
    CPU *cpu = getTestingCPUWithInstructions(vector<uint8_t>{ SBC_AH, SBC_N, 0x3A, SBC_Address_HL });
    uint32_t cycles = cpu->cycles();

    cpu->Set8Bit(Register_A, 0x3B);
    cpu->flags.c = true;

    cpu->Set8Bit(Register_H, 0x2A);
    cpu->Step();
    ASSERT_EQ(cpu->Get8Bit(Register_A), 0x10);
    EXPECT_FLAGS(false, false, true, false);

    cpu->Set8Bit(Register_A, 0x3B);
    cpu->flags.c = true;
    cpu->Step();
    ASSERT_EQ(cpu->Get8Bit(Register_A), 0x0);
    EXPECT_FLAGS(true, false, true, false);

    cpu->Set8Bit(Register_A, 0x3B);
    cpu->Set16Bit(Register_HL, 0x9999);
    cpu->Set8Bit(Address_HL, 0x4F);
    cpu->flags.c = true;
    cpu->Step();
    ASSERT_EQ(cpu->Get8Bit(Register_A), 0xEB);
    EXPECT_FLAGS(false, true, true, true);
}

TEST(MathCommandTest, AddSP1) {
    const uint8_t ADD_SP = 0xE8;
    CPU *cpu = getTestingCPUWithInstructions(vector<uint8_t>{ ADD_SP, 0x01 });
    uint32_t cycles = cpu->cycles();
    cpu->Set16Bit(Register_SP, 0xFF00);
    
    cpu->Step();
    ASSERT_EQ(cpu->cycles(), 16);
    ASSERT_EQ(cpu->Get16Bit(Register_SP), 0xFF01);
    EXPECT_FLAGS(false, false, false, false);
}

TEST(MathCommandTest, AddSP2) {
    const uint8_t ADD_SP = 0xE8;
    CPU *cpu = getTestingCPUWithInstructions(vector<uint8_t>{ ADD_SP, 0x02 });
    uint32_t cycles = cpu->cycles();
    cpu->Set16Bit(Register_SP, 0xFFF8);
    
    cpu->Step();
    ASSERT_EQ(cpu->cycles(), 16);
    ASSERT_EQ(cpu->Get16Bit(Register_SP), 0xFFFA);
    EXPECT_FLAGS(false, false, false, false);
}

TEST(MathCommandTest, AddSPNegative) {
    const uint8_t ADD_SP = 0xE8;
    const int8_t negative8 = -8;
    const uint8_t unsigned_negative8 = negative8;
    CPU *cpu = getTestingCPUWithInstructions(vector<uint8_t>{ ADD_SP, unsigned_negative8 });
    uint32_t cycles = cpu->cycles();
    cpu->Set16Bit(Register_SP, 0xFFF8);
    
    cpu->Step();
    ASSERT_EQ(cpu->cycles(), 16);
    ASSERT_EQ(cpu->Get16Bit(Register_SP), 0xFFF0);
    EXPECT_FLAGS(false, false, false, false);
}

TEST(MathCommandTest, ADD) {
    const uint8_t ADD_AB = 0x80;
    const uint8_t ADD_AN = 0xC6;
    const uint8_t ADD_AHL = 0x86;

    CPU *cpu = getTestingCPUWithInstructions(vector<uint8_t>{ ADD_AB, ADD_AN, 0xFF, ADD_AHL });
    uint32_t cycles = cpu->cycles();
    cpu->Set8Bit(Register_A, 0x3A);
    cpu->Set8Bit(Register_B, 0xC6);

    cpu->Step();
    ASSERT_EQ(cpu->cycles(), 4);
    EXPECT_FLAGS(true, true, false, true);

    cpu->Set8Bit(Register_A, 0x3C);
    cpu->Step();
    ASSERT_EQ(cpu->cycles(), 12);
    ASSERT_EQ(cpu->Get8Bit(Register_A), 0x3B);
    EXPECT_FLAGS(false, true, false, true);

    cpu->Set8Bit(Register_A, 0x3C);
    cpu->Set16Bit(Register_HL, 0x9998);
    cpu->Set8Bit(Address_HL, 0x12);
    cpu->Step();
    ASSERT_EQ(cpu->cycles(), 20);
    ASSERT_EQ(cpu->Get8Bit(Register_A), 0x4E);
    EXPECT_FLAGS(false, false, false, false);
}

