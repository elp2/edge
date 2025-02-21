#include "cpu.h"
#include "gtest/gtest.h"
#include "utils.h"

class MathCommandTest : public ::testing::Test {
 protected:
  MathCommandTest(){};
  ~MathCommandTest(){};
};

TEST(MathCommandTest, INC) {
  const uint8_t INC_A = 0x3C;
  const uint8_t INC_HL = 0x34;
  CPU *cpu = getTestingCPUWithInstructions(vector<uint8_t>{INC_A, INC_HL});
  cpu->Set8Bit(Register_A, 0xFF);

  cpu->Step();
  EXPECT_EQ(cpu->Get8Bit(Register_A), 0);
  EXPECT_FLAGS(true, true, false, false);
  EXPECT_EQ(cpu->cycles(), 4);

  // Ensure C is not affected.
  cpu->flags.c = true;
  cpu->Set16Bit(Register_HL, 0x9998);
  cpu->Set8Bit(Address_HL, 0x50);
  cpu->Step();
  EXPECT_EQ(cpu->Get8Bit(Address_HL), 0x51);
  EXPECT_FLAGS(false, false, false, true);
  EXPECT_EQ(cpu->cycles(), 16);
}

TEST(MathCommandTest, DEC) {
  const uint8_t DEC_L = 0x2D;
  const uint8_t DEC_HL = 0x35;
  CPU *cpu = getTestingCPUWithInstructions(vector<uint8_t>{DEC_L, DEC_HL});
  cpu->Set8Bit(Register_L, 0x01);

  cpu->Step();
  EXPECT_EQ(cpu->Get8Bit(Register_A), 0);
  EXPECT_FLAGS(true, false, true, false);
  EXPECT_EQ(cpu->cycles(), 4);

  // Ensure C is not affected.
  cpu->flags.c = true;
  cpu->Set16Bit(Register_HL, 0x9998);
  cpu->Set8Bit(Address_HL, 0x00);
  cpu->Step();
  EXPECT_EQ(cpu->Get8Bit(Address_HL), 0xFF);
  EXPECT_FLAGS(false, true, true, true);
  EXPECT_EQ(cpu->cycles(), 16);
}

TEST(MathCommandTest, AddHLNoCarries) {
  const uint8_t ADDHLBC = 0x09;
  const uint8_t ADDHLHL = 0x29;
  CPU *cpu =
      getTestingCPUWithInstructions(vector<uint8_t>{ADDHLBC, ADDHLBC, ADDHLHL});
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
  CPU *cpu =
      getTestingCPUWithInstructions(vector<uint8_t>{ADDHLBC, ADDHLBC, ADDHLHL});
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
  CPU *cpu =
      getTestingCPUWithInstructions(vector<uint8_t>{ADDHLBC, ADDHLBC, ADDHLHL});
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
  CPU *cpu =
      getTestingCPUWithInstructions(vector<uint8_t>{ADDHLBC, ADDHLBC, ADDHLHL});
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
  CPU *cpu = getTestingCPUWithInstructions(vector<uint8_t>{ADDHLBC, ADDHLHL});
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
  CPU *cpu = getTestingCPUWithInstructions(
      vector<uint8_t>{SBC_AH, SBC_N, 0x3A, SBC_Address_HL});

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
  CPU *cpu = getTestingCPUWithInstructions(vector<uint8_t>{ADD_SP, 0x01});
  cpu->Set16Bit(Register_SP, 0xFF00);

  cpu->Step();
  ASSERT_EQ(cpu->cycles(), 16);
  ASSERT_EQ(cpu->Get16Bit(Register_SP), 0xFF01);
  EXPECT_FLAGS(false, false, false, false);
}

TEST(MathCommandTest, AddSP2) {
  const uint8_t ADD_SP = 0xE8;
  CPU *cpu = getTestingCPUWithInstructions(vector<uint8_t>{ADD_SP, 0x02});
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
  CPU *cpu = getTestingCPUWithInstructions(
      vector<uint8_t>{ADD_SP, unsigned_negative8});
  cpu->Set16Bit(Register_SP, 0xFFF8);

  cpu->Step();
  ASSERT_EQ(cpu->cycles(), 16);
  ASSERT_EQ(cpu->Get16Bit(Register_SP), 0xFFF0);
  EXPECT_FLAGS(false, true, false, true);
}

TEST(MathCommandTest, AddSPUnderflow) {
  const uint8_t ADD_SP = 0xE8;
  const int8_t negative1 = -1;
  const uint8_t unsigned_negative1 = negative1;
  CPU *cpu = getTestingCPUWithInstructions(
      vector<uint8_t>{ADD_SP, unsigned_negative1});
  cpu->Set16Bit(Register_SP, 0x0000);

  cpu->Step();
  ASSERT_EQ(cpu->cycles(), 16);
  ASSERT_EQ(cpu->Get16Bit(Register_SP), 0xFFFF);
  EXPECT_FLAGS(false, false, false, false);
}

TEST(MathCommandTest, AddSPOverFlow) {
  const uint8_t ADD_SP = 0xE8;
  const int8_t one = 1;
  CPU *cpu = getTestingCPUWithInstructions(vector<uint8_t>{ADD_SP, one});
  cpu->Set16Bit(Register_SP, 0xFFFF);

  cpu->Step();
  ASSERT_EQ(cpu->cycles(), 16);
  ASSERT_EQ(cpu->Get16Bit(Register_SP), 0x0000);
  EXPECT_FLAGS(false, true, false, true);
}
TEST(MathCommandTest, ADD) {
  const uint8_t ADD_AB = 0x80;
  const uint8_t ADD_AN = 0xC6;
  const uint8_t ADD_AHL = 0x86;

  CPU *cpu = getTestingCPUWithInstructions(
      vector<uint8_t>{ADD_AB, ADD_AN, 0xFF, ADD_AHL});
  cpu->Set8Bit(Register_A, 0x3A);
  cpu->Set8Bit(Register_B, 0xC6);

  cpu->Step();
  ASSERT_EQ(cpu->cycles(), 4);
  ASSERT_EQ(cpu->Get8Bit(Register_A), 0x00);
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

TEST(MathCommandTest, ADC) {
  const uint8_t ADC_AE = 0x8B;
  const uint8_t ADC_AN = 0xCE;
  const uint8_t ADC_AHL = 0x8E;

  CPU *cpu = getTestingCPUWithInstructions(
      vector<uint8_t>{ADC_AE, ADC_AN, 0x3B, ADC_AHL});
  cpu->Set8Bit(Register_A, 0xE1);
  cpu->Set8Bit(Register_E, 0x0F);
  cpu->flags.c = true;

  cpu->Step();
  ASSERT_EQ(cpu->cycles(), 4);
  ASSERT_EQ(cpu->Get8Bit(Register_A), 0xF1);
  EXPECT_FLAGS(false, true, false, false);

  cpu->Set8Bit(Register_A, 0xE1);
  cpu->flags.c = true;
  cpu->Step();
  ASSERT_EQ(cpu->cycles(), 12);
  ASSERT_EQ(cpu->Get8Bit(Register_A), 0x1D);
  EXPECT_FLAGS(false, false, false, true);

  cpu->Set8Bit(Register_A, 0xE1);
  cpu->flags.c = true;
  cpu->Set16Bit(Register_HL, 0x9998);
  cpu->Set8Bit(Address_HL, 0x1E);
  cpu->Step();
  ASSERT_EQ(cpu->cycles(), 20);
  ASSERT_EQ(cpu->Get8Bit(Register_A), 0x00);
  EXPECT_FLAGS(true, true, false, true);
}

TEST(MathCommandTest, SUB) {
  const uint8_t SUB_E = 0x93;
  const uint8_t SUB_N = 0xD6;
  const uint8_t SUB_AHL = 0x96;

  CPU *cpu = getTestingCPUWithInstructions(
      vector<uint8_t>{SUB_E, SUB_N, 0x0F, SUB_AHL});
  cpu->Set8Bit(Register_A, 0x3E);
  cpu->Set8Bit(Register_E, 0x3E);

  cpu->Step();
  ASSERT_EQ(cpu->cycles(), 4);
  ASSERT_EQ(cpu->Get8Bit(Register_A), 0x00);
  EXPECT_FLAGS(true, false, true, false);

  cpu->Set8Bit(Register_A, 0x3E);
  cpu->Step();
  ASSERT_EQ(cpu->cycles(), 12);
  ASSERT_EQ(cpu->Get8Bit(Register_A), 0x2F);
  EXPECT_FLAGS(false, true, true, false);

  cpu->Set8Bit(Register_A, 0x3E);
  cpu->Set16Bit(Register_HL, 0x9998);
  cpu->Set8Bit(Address_HL, 0x40);
  cpu->Step();
  ASSERT_EQ(cpu->cycles(), 20);
  ASSERT_EQ(cpu->Get8Bit(Register_A), 0xFE);
  EXPECT_FLAGS(false, false, true, true);
}

TEST(MathCommandTest, AddDAA) {
  const uint8_t ADD_AB = 0x80;
  const uint8_t DAA = 0x27;

  CPU *cpu = getTestingCPUWithInstructions(vector<uint8_t>{ADD_AB, DAA});
  // O = ones, T = tens for a and b.
  for (uint8_t oa = 0; oa < 9; oa++) {
    for (uint8_t ta = 0; ta < 9; ta++) {
      for (uint8_t ob = 0; ob < 9; ob++) {
        for (uint8_t tb = 0; tb < 9; tb++) {
          int expected = 10 * ta + oa + 10 * tb + ob;
          bool carry = expected >= 100;
          if (carry) {
            expected -= 100;
          }
          uint8_t expected_o = expected % 10;
          uint8_t expected_t = expected / 10;
          uint8_t expected8 = (expected_t << 4) | expected_o;

          uint8_t a = (ta << 4) | oa;
          uint8_t b = (tb << 4) | ob;
          cpu->Set8Bit(Register_A, a);
          cpu->Set8Bit(Register_B, b);

          cpu->Step();
          // TODO: Explain why we need to step 2x and get Register A.
          uint8_t added = cpu->Get8Bit(Register_A);

          cpu->Step();

          EXPECT_EQ(cpu->Get8Bit(Register_A), expected8);
          EXPECT_FLAGS(expected8 == 0, false, false, carry);
          uint8_t relative = -2;
          cpu->JumpRelative(relative);
        }
      }
    }
  }
}

TEST(MathCommandTest, SUBDAA) {
  const uint8_t SUB_AB = 0x90;
  const uint8_t DAA = 0x27;

  CPU *cpu = getTestingCPUWithInstructions(vector<uint8_t>{SUB_AB, DAA});

  // O = ones, T = tens for a and b.
  for (uint8_t oa = 0; oa < 9; oa++) {
    for (uint8_t ta = 0; ta < 9; ta++) {
      for (uint8_t ob = 0; ob < 9; ob++) {
        for (uint8_t tb = 0; tb < 9; tb++) {
          int expected = 10 * ta + oa - (10 * tb + ob);
          bool carry = expected < 0;
          if (carry) {
            expected += 100;
          }
          uint8_t expected_o = expected % 10;
          uint8_t expected_t = expected / 10;
          uint8_t expected8 = (expected_t << 4) | expected_o;

          uint8_t a = (ta << 4) | oa;
          uint8_t b = (tb << 4) | ob;
          cpu->Set8Bit(Register_A, a);
          cpu->Set8Bit(Register_B, b);

          cpu->Step();
          uint8_t subbed = cpu->Get8Bit(Register_A);
          cpu->Step();

          uint8_t actual = cpu->Get8Bit(Register_A);
          if (actual != expected8) {
            cout << hex << unsigned(a) << " b: " << hex << unsigned(b)
                 << " == ? " << hex << unsigned(expected8) << " act: " << hex
                 << unsigned(actual) << " SUB: " << hex << unsigned(subbed)
                 << endl;
            assert(false);
          }
          ASSERT_EQ(actual, expected8);
          uint8_t relative = -2;
          cpu->JumpRelative(relative);
        }
      }
    }
  }
}
