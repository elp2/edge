#include "cpu.h"
#include "gtest/gtest.h"
#include "utils.h"

class StackTest : public ::testing::Test {
 protected:
  StackTest() {}

  ~StackTest() {}
};

TEST(StackTest, PushPop16) {
  const uint16_t EXPECTED_HL = 0x1234;
  CPU* cpu = getTestingCPU();
  cpu->Set16Bit(Register_HL, EXPECTED_HL);
  cpu->Push16Bit(cpu->Get16Bit(Register_HL));
  uint16_t popped = cpu->Pop16Bit();
  EXPECT_EQ(popped, EXPECTED_HL);
  EXPECT_EQ(popped, cpu->Get16Bit(Register_HL));
}

TEST(StackTest, PushPop8) {
  const uint8_t EXPECTED_H = 0x12;
  CPU* cpu = getTestingCPU();
  cpu->Set8Bit(Register_H, EXPECTED_H);
  cpu->Push8Bit(cpu->Get8Bit(Register_H));
  uint8_t popped = cpu->Pop8Bit();
  EXPECT_EQ(popped, EXPECTED_H);
  EXPECT_EQ(popped, cpu->Get8Bit(Register_H));
}

TEST(StackTest, PushDownwards) {
  CPU* cpu = getTestingCPU();
  uint16_t initial_sp = cpu->Get16Bit(Register_SP);
  cpu->Push16Bit(0x12);
  uint16_t new_sp = cpu->Get16Bit(Register_SP);
  ASSERT_EQ(new_sp + 2, initial_sp);
}

TEST(StackTest, PushLSBMSB) {
  CPU* cpu = getTestingCPU();
  uint16_t initial_sp = cpu->Get16Bit(Register_SP);
  cpu->Push16Bit(0x1234);
  cpu->Set16Bit(Register_HL, initial_sp - 2);
  ASSERT_EQ(cpu->Get8Bit(Address_HL), 0x34);
  cpu->Set16Bit(Register_HL, initial_sp - 1);
  ASSERT_EQ(cpu->Get8Bit(Address_HL), 0x12);
}

TEST(StackTest, PushBC) {
  const uint8_t PUSH_BC = 0xC5;
  CPU* cpu = getTestingCPUWithInstructions(vector<uint8_t>{PUSH_BC});

  uint16_t initial_sp = cpu->Get16Bit(Register_SP);
  uint16_t BC = 0xABCD;
  cpu->Set16Bit(Register_BC, BC);
  cpu->Step();

  cpu->Set16Bit(Register_HL, initial_sp - 1);
  ASSERT_EQ(cpu->Get8Bit(Address_HL), HIGHER8(BC));
  cpu->Set16Bit(Register_HL, initial_sp - 2);
  ASSERT_EQ(cpu->Get8Bit(Address_HL), LOWER8(BC));
}

TEST(StackTest, PopBC) {
  const uint8_t POP_BC = 0xC1;
  CPU* cpu = getTestingCPUWithInstructions(vector<uint8_t>{POP_BC});

  uint16_t expected = 0xFEED;
  cpu->Push16Bit(expected);
  cpu->Step();
  ASSERT_EQ(expected, cpu->Get16Bit(Register_BC));
}

TEST(StackTest, PushPopDE) {
  const uint8_t PUSH_DE = 0xD5;
  const uint8_t POP_DE = 0xD1;
  CPU* cpu = getTestingCPUWithInstructions(vector<uint8_t>{PUSH_DE, POP_DE});

  uint16_t expected = 0xFEED;
  cpu->Set16Bit(Register_DE, expected);
  cpu->Step();
  cpu->Set16Bit(Register_DE, ~expected);
  ASSERT_NE(expected, cpu->Get16Bit(Register_DE));

  cpu->Step();
  ASSERT_EQ(expected, cpu->Get16Bit(Register_DE));
}
