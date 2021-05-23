#include "gtest/gtest.h"

#include "cpu.h"
#include "utils.h"

class LoadCommandTest : public ::testing::Test {
 protected:
    LoadCommandTest() {};
    ~LoadCommandTest() {};
};

TEST(LoadCommandTest, LDnnSP) {
    const uint8_t LDnnSP = 0x08;
    CPU *cpu = getTestingCPUWithInstructions(vector<uint8_t>{ LDnnSP, 0x00, 0xC1 });
    uint16_t expected_sp = 0xFFF8;
    cpu->Set16Bit(Register_SP, expected_sp);

    cpu->Step();
    cpu->Set16Bit(Register_HL, 0xC100);
    ASSERT_EQ(cpu->Get8Bit(Address_HL), 0xF8);
    cpu->Set16Bit(Register_HL, 0xC101);
    ASSERT_EQ(cpu->Get8Bit(Address_HL), 0xFF);
}
