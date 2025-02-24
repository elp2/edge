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
    uint8_t test_value = 0;
    while (true) {
        test_value++;
        for (uint16_t address = 0xFF30; address <= 0xFF3F; address++) {
            controller_->SetByteAt(address, (uint8_t)address);
            EXPECT_EQ(controller_->GetByteAt(address), (uint8_t)address);
            test_value++;
        }
        if (test_value == 0) {
            break;
        }
    }
}

TEST_F(SoundControllerTest, MemoryRWDefaults) {
    const uint8_t masks[] = {
        0x80, 0x3F, 0x00, 0xFF, 0xBF, // NR10-NR14
        0xFF, 0x3F, 0x00, 0xFF, 0xBF, // NR20-NR24
        0x7F, 0xFF, 0x9F, 0xFF, 0xBF, // NR30-NR34
        0xFF, 0xFF, 0x00, 0x00, 0xBF, // NR40-NR44
        0x00, 0x00, 0x70,             // NR50-NR52
        0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // Unuseds.
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Wave RAM ($FF30-$FF3F)
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    uint8_t test_value = 0;

    while (true) {
        const uint8_t* mask_ptr = masks;
        uint16_t address = 0xFF10;

        // std::cout << "Testing " << std::hex << (int)test_value << std::endl;

        while (address < 0xFF30 + 0x10) {
            // std::cout << "Address " << std::hex << (int)address << " maskptr " << std::hex << (int)*mask_ptr << std::endl;

            // Skip NR52.
            if (address == 0xFF26) {
                address++;
                mask_ptr++;
                continue;
            }

            controller_->SetByteAt(address, test_value);

            uint8_t actual = controller_->GetByteAt(address);
            uint8_t expected = *mask_ptr | test_value;
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
        test_value++;
        if (test_value == 0) {
            break;
        }
    }
}
