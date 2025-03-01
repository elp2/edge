#include "cartridge.h"
#include "cpu.h"
#include "gtest/gtest.h"
#include "mmu.h"
#include "ppu.h"
#include "utils.h"

class JumpCommandTest : public ::testing::Test {
 protected:
  JumpCommandTest(){};
  ~JumpCommandTest(){};
};

TEST(JumpCommandTest, JPnn) {
  const uint8_t JPnn = 0xC3;
  CPU *cpu = getTestingCPUWithInstructions(vector<uint8_t>{JPnn, 0x34, 0x12});
  uint32_t cycles = cpu->cycles();
  uint16_t stack_top = 0xDEAD;
  cpu->Push16Bit(stack_top);
  cpu->Step();

  ASSERT_EQ(cpu->Get16Bit(Register_PC), 0x1234);
  ASSERT_EQ(cpu->cycles(), cycles + 16);
  // Jumps should not put a return on the stack.
  ASSERT_EQ(cpu->Pop16Bit(), stack_top);
}

TEST(JumpCommandTest, JPNCnn) {
  const uint8_t JPNCnn = 0xD2;
  CPU *cpu = getTestingCPUWithInstructions(vector<uint8_t>{JPNCnn, 0x34, 0x12});
  cpu->flags.c = false;
  cpu->Step();

  ASSERT_EQ(cpu->Get16Bit(Register_PC), 0x1234);
}

TEST(JumpCommandTest, JPNCnnNoJump) {
  const uint8_t JPNCnn = 0xD2;
  CPU *cpu = getTestingCPUWithInstructions(vector<uint8_t>{JPNCnn, 0x34, 0x12});
  uint16_t pc = cpu->Get16Bit(Register_PC);
  cpu->flags.c = true;
  cpu->Step();

  ASSERT_EQ(cpu->Get16Bit(Register_PC), pc + 3);
}

TEST(JumpCommandTest, JPHLAddress) {
  const uint8_t JPHL = 0xE9;
  CPU *cpu = getTestingCPUWithInstructions(vector<uint8_t>{JPHL, 0x34, 0x12});
  uint16_t hl = 0x9898;
  cpu->Set16Bit(Register_HL, hl);

  cpu->Step();

  ASSERT_EQ(cpu->Get16Bit(Register_PC), hl);
  ASSERT_EQ(cpu->cycles(), 4);
}

TEST(JumpCommandTest, JRn) {
  const uint8_t JRn = 0x18;
  CPU *cpu = getTestingCPUWithInstructions(vector<uint8_t>{JRn, 0x34, 0x12});
  uint16_t pc = cpu->Get16Bit(Register_PC);
  cpu->Step();

  ASSERT_EQ(cpu->Get16Bit(Register_PC), pc + 2 + 0x34);
  ASSERT_EQ(cpu->cycles(), 12);
}

TEST(JumpCommandTest, JRnNegative) {
  const uint8_t JRn = 0x18;
  int8_t signed_relative = -10;
  uint8_t unsigned_int = signed_relative;
  CPU *cpu =
      getTestingCPUWithInstructions(vector<uint8_t>{JRn, unsigned_int, 0x12});
  uint16_t pc = cpu->Get16Bit(Register_PC);
  cpu->Step();

  ASSERT_EQ(cpu->Get16Bit(Register_PC), pc + 2 + signed_relative);
  ASSERT_LT(cpu->Get16Bit(Register_PC), pc + 2);
  ASSERT_EQ(cpu->cycles(), 12);
}

TEST(JumpCommandTest, JRNZn) {
  const uint8_t JRNZ = 0x20;
  CPU *cpu = getTestingCPUWithInstructions(vector<uint8_t>{JRNZ, 0x34, 0x12});
  uint16_t pc = cpu->Get16Bit(Register_PC);
  cpu->flags.z = false;
  cpu->Step();

  ASSERT_EQ(cpu->Get16Bit(Register_PC), pc + 2 + 0x34);
  ASSERT_EQ(cpu->cycles(), 12);
}

TEST(JumpCommandTest, JRNZnNoJump) {
  const uint8_t JRNZ = 0x20;
  CPU *cpu = getTestingCPUWithInstructions(vector<uint8_t>{JRNZ, 0x34, 0x12});
  uint16_t pc = cpu->Get16Bit(Register_PC);
  cpu->flags.z = true;
  cpu->Step();

  ASSERT_EQ(cpu->Get16Bit(Register_PC), pc + 2);
  ASSERT_EQ(cpu->cycles(), 8);
}
