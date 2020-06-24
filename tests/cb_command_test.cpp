#include "gtest/gtest.h"

#include <cmath>

#include "CPU.hpp"
#include "Utils.hpp"

class CBCommandTest : public ::testing::Test {
 protected:
    CBCommandTest() {};
    ~CBCommandTest() {};
};

vector<uint8_t> opcodes(uint8_t start) {
	vector<uint8_t> opcodes = vector<uint8_t>();
	for (int i = 0; i < 8; i++) {
		opcodes.push_back(0xCB);
		opcodes.push_back(start + i * 8);
	}
	return opcodes;
}

TEST(CBCommandTest, TestBitOn) {
    CPU *cpu = getTestingCPUWithInstructions(opcodes(0x40));
    uint8_t b = 1;
    // Should be unaffected by the test.
    cpu->flags.c = true;
    for (int i = 0; i < 8; i++) {
        cpu->Set8Bit(Register_B, b);
		cpu->flags.n = true; // Should be reset.
		cpu->Step();
		ASSERT_EQ(cpu->Get8Bit(Register_B), b);
        EXPECT_FLAGS(false, true, false, true);
		b <<= 1;
    } 
}

TEST(CBCommandTest, TestBitOff) {
	CPU* cpu = getTestingCPUWithInstructions(opcodes(0x44));
    uint8_t h = 1;
    // Should be unaffected by the test.
    cpu->flags.c = false;
    for (int i = 0; i < 8; i++) {
		uint8_t inverted_h = ~h;
        cpu->Set8Bit(Register_H, inverted_h);
		cpu->flags.n = true; // Should be reset.
		cpu->Step();
        ASSERT_EQ(cpu->Get8Bit(Register_H), inverted_h);
        EXPECT_FLAGS(true, true, false, false);
		h <<= 1;
    } 
}

TEST(CBCommandTest, SetBit) {
	CPU* cpu = getTestingCPUWithInstructions(opcodes(0xC6));
    cpu->Set16Bit(Register_HL, 0x9899);

	// Shouldn't be affected. We'll flip these throughout.
	bool check_flags = true;
	for (int i = 0; i < 8; i++) {
		uint8_t c_bit = pow(2, i);
        c_bit = ~c_bit;
		cpu->flags.c = cpu->flags.z = cpu->flags.h = cpu->flags.n = check_flags;

		cpu->Set8Bit(Address_HL, c_bit);
		cpu->Step();
		ASSERT_EQ(cpu->Get8Bit(Address_HL), 0xFF);
		EXPECT_FLAGS(check_flags, check_flags, check_flags, check_flags);
		check_flags = !check_flags;
	}
}

TEST(CBCommandTest, ResetBit) {
	CPU* cpu = getTestingCPUWithInstructions(opcodes(0x81));

	// Shouldn't be affected. We'll flip these throughout.
	bool check_flags = true;
	for (int i = 0; i < 8; i++) {
		uint8_t c_bit = pow(2, i);
		cpu->flags.c = cpu->flags.z = cpu->flags.h = cpu->flags.n = check_flags;

		cpu->Set8Bit(Register_C, c_bit);
		cpu->Step();
		ASSERT_EQ(cpu->Get8Bit(Register_C), 0x00);
		EXPECT_FLAGS(check_flags, check_flags, check_flags, check_flags);
		check_flags = !check_flags;
	}
}

TEST(CBCommandTest, SwapNibbles) {
	CPU* cpu = getTestingCPUWithInstructions(vector<uint8_t> { 0xCB, 0x33 });
	uint8_t e = 0;

	for (int i = 0; i < 8; i++) {
		cpu->Set8Bit(Register_E, e);
		cpu->Step();
		uint8_t swapped = cpu->Get8Bit(Register_E);

		ASSERT_EQ(NIBBLEHIGH(e), NIBBLELOW(swapped));
		ASSERT_EQ(NIBBLELOW(e), NIBBLEHIGH(swapped));
		EXPECT_FLAGS(e == 0, false, false, false);
		cpu->JumpRelative(-2);
		e++;
	}
}
