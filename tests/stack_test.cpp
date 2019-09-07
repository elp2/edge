#include "gtest/gtest.h"

#include "CPU.hpp"
#include "Utils.hpp"

class StackTest : public ::testing::Test {
 protected:
    StackTest() {}

    ~StackTest() {}
};

TEST(StackTest, PushPop16) {
    const uint16_t EXPECTED_HL = 0x1234;
    CPU *cpu = getTestingCPU();
    cpu->Set16Bit(Register_HL, EXPECTED_HL);
    cpu->Push16Bit(cpu->Get16Bit(Register_HL));
    uint16_t popped = cpu->Pop16Bit();
    EXPECT_EQ(popped, EXPECTED_HL);
    EXPECT_EQ(popped, cpu->Get16Bit(Register_HL));
}

TEST(StackTest, PushPop8) {
    const uint8_t EXPECTED_H = 0x12;
    CPU *cpu = getTestingCPU();
    cpu->Set8Bit(Register_H, EXPECTED_H);
    cpu->Push8Bit(cpu->Get8Bit(Register_H));
    uint8_t popped = cpu->Pop8Bit();
    EXPECT_EQ(popped, EXPECTED_H);
    EXPECT_EQ(popped, cpu->Get8Bit(Register_H));
}

TEST(StackTest, PushDownwards) {
    CPU *cpu = getTestingCPU();
    uint16_t initial_sp = cpu->Get16Bit(Register_SP);
    cpu->Push16Bit(0x12);
    uint16_t new_sp = cpu->Get16Bit(Register_SP);
    ASSERT_EQ(new_sp + 2, initial_sp);
}
