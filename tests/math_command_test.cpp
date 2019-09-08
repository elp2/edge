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
// TODO: SUBC

// TODO: INC
// TODO: DEC

// ADDHL

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
