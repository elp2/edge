#include "noise_voice.h"

#include <cmath>

#include "gtest/gtest.h"

class NoiseVoiceTest : public ::testing::Test {
 protected:
  NoiseVoiceTest(){};
  ~NoiseVoiceTest(){};
};

TEST(NoiseVoiceTest, LFSRStartsZero) {
  NoiseVoice *nv = new NoiseVoice();
  ASSERT_EQ(nv->LFSR(), 0x0000);
}
