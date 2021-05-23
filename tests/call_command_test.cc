#include "gtest/gtest.h"

#include "cpu.h"
#include "MMU.hpp"
#include "ROM.hpp"
#include "PPU.hpp"
#include "Utils.hpp"

class CallCommandTest : public ::testing::Test {
 protected:
    CallCommandTest() {};
    ~CallCommandTest() {};
};

TEST(CallCommandTest, CALLnn) {
    const uint8_t CALLnn = 0xCD;
    CPU *cpu = getTestingCPUWithInstructions(vector<uint8_t>{ CALLnn, 0x34, 0x12 });
    uint32_t cycles = cpu->cycles();
    uint16_t expected_pushed_pc = cpu->Get16Bit(Register_PC) + 3;
    cpu->Step();

    ASSERT_EQ(cpu->Get16Bit(Register_PC), 0x1234);
    ASSERT_EQ(cpu->cycles(), cycles + 24);
    ASSERT_EQ(cpu->Pop16Bit(), expected_pushed_pc);
}

TEST(CallCommandTest, CALLNZnnJump) {
    const uint8_t CALLnn = 0xC4;
    CPU *cpu = getTestingCPUWithInstructions(vector<uint8_t>{ CALLnn, 0x34,  0x12 });
    // After returning, come back to the positon afterwards the command.
    uint16_t expected_pushed_pc = cpu->Get16Bit(Register_PC) + 3;
    cpu->flags.z = false;
    uint32_t cycles = cpu->cycles();
    cpu->Step();

    ASSERT_EQ(cpu->Get16Bit(Register_PC), 0x1234);
    ASSERT_EQ(cpu->cycles(), cycles + 24);
    ASSERT_EQ(cpu->Pop16Bit(), expected_pushed_pc);
}

TEST(CallCommandTest, CALLNCnnNoJump) {
    const uint8_t CALLNCnn = 0xD4;
    CPU *cpu = getTestingCPUWithInstructions(vector<uint8_t>{ CALLNCnn, 0x12, 0x34 });
    uint16_t pc = cpu->Get16Bit(Register_PC);
    cpu->flags.c = true;
    uint32_t cycles = cpu->cycles();
    cpu->Step();

    // Advances after the call withtout jumping.
    ASSERT_EQ(cpu->Get16Bit(Register_PC), pc + 3);
    ASSERT_EQ(cpu->cycles(), cycles + 12);
}

TEST(CallCommandTest, RST18H) {
    const uint8_t RST18H = 0xDF;
    CPU *cpu = getTestingCPUWithInstructions(vector<uint8_t>{ RST18H, 0x18 });
    uint32_t cycles = cpu->cycles();
    cpu->Step();

    ASSERT_EQ(cpu->Get16Bit(Register_PC), 0x18);
    ASSERT_EQ(cpu->cycles(), cycles + 16);
}
