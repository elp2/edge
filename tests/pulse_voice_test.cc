#include "pulse_voice.h"

#include <cmath>

#include "gtest/gtest.h"

class PulseVoiceTest : public ::testing::Test {
 protected:
  PulseVoiceTest(){};
  ~PulseVoiceTest(){};
};

TEST(PulseVoiceTest, WaveDuty) {
  PulseVoice *pv = new PulseVoice(1);
  ASSERT_EQ(true, true); // TODO: Add tests.
}
