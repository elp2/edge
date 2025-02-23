#include "sound_controller.h"

#include "SDL.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "interrupt_controller.h"

using testing::_;

class SoundControllerTest : public ::testing::Test {
 protected:
  SoundControllerTest(){};
  ~SoundControllerTest(){};
  void SetUp() override {
    controller_ = new SoundController();
  }

  SoundController* controller_;
};

TEST_F(SoundControllerTest, WaveMemorySetGet) {
    for (uint16_t address = 0xFF30; address <= 0xFF3F; address++) {
        controller_->SetByteAt(address, (uint8_t)address);
        EXPECT_EQ(controller_->GetByteAt(address), (uint8_t)address);
    }
}

TEST_F(SoundControllerTest, MemoryRWDefaults) {
    // Masks for readable bits (mirrors assembly)
    // ff13, ff15, ff18 are broken.
    // 12. FF13 (NR 13)
    //  Name - NR 13
    //  Contents - Sound Mode 1 register, Frequency lo (W)
    // ff15 - empty, nothing.
    // ff18 - 16. FF18 (NR 23)
    // Name - NR 23
    // Contents - Sound Mode 2 register, frequency
    //lo data (W)

    const uint8_t masks[] = {
        0x80, 0x3F, 0x00, 0xFF, 0xBF, // NR10-NR14
        0xFF, 0x3F, 0x00, 0xFF, 0xBF, // NR20-NR24
        0x7F, 0xFF, 0x9F, 0xFF, 0xBF, // NR30-NR34
        0xFF, 0xFF, 0x00, 0x00, 0xBF, // NR40-NR44
        0x00, 0x00, 0x70,             // NR50-NR52
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Wave RAM ($FF30-$FF3F)
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    // TODO: Test 0-255.
    uint8_t test_value = 0;
    uint16_t address = 0xFF10;
    const uint8_t* mask_ptr = masks;

    // TODO: Test all addresses.
    // while (address < 0xFF3F) {
    // while (address < 0xFF1F) {
    while (address < 0xFF1A) {
        // Skip NR52
        if (address == 0xFF25) {
            address++;
            mask_ptr++;
            continue;
        }

        // Expected readback value
        uint8_t expected = *mask_ptr | test_value;

        controller_->SetByteAt(address, test_value);
        uint8_t actual = controller_->GetByteAt(address);
        if (actual != expected) {
            std::cout << "Expected " << std::hex << (int)expected << " but got " << (int)actual << " on " << std::hex << (int)address << std::endl;
        }
        EXPECT_EQ(actual, expected);

        // Mute channels and disable Wave.
        controller_->SetByteAt(0xFF25, 0x00);
        controller_->SetByteAt(0xFF1A, 0x00);

        address++;
        mask_ptr++;
    }
}

// TEST_F(SoundControllerTest, SetAllAndGetAll) {
//     for (uint16_t address = 0xFF10; address <= 0xFF3F; address++) {
//         controller_->SetByteAt(address,(uint8_t)address);
//     }
//     for (uint16_t address = 0xFF10; address <= 0xFF3F; address++) {
//         EXPECT_EQ(controller_->GetByteAt(address), (uint8_t)address);
//     }
// }
