#include "gtest/gtest.h"

#include <cmath>

#include "pulse_voice.h"

class PulseVoiceTest : public ::testing::Test {
 protected:
    PulseVoiceTest() {};
    ~PulseVoiceTest() {};
};

TEST(PulseVoiceTest, WaveDuty) {
    PulseVoice *pv = new PulseVoice();

    pv->SetWavePatternDutyByte(0xC0);
    ASSERT_EQ(pv->GetWavePatternDutyByte(), 0xC0);

    pv->SetWavePatternDutyByte(0x0);
    ASSERT_EQ(pv->GetWavePatternDutyByte(), 0x0);

    pv->SetWavePatternDutyByte(0b00111111);
    ASSERT_EQ(pv->GetWavePatternDutyByte(), 0x0);

    pv->SetWavePatternDutyByte(0b10111111);
    ASSERT_EQ(pv->GetWavePatternDutyByte(), 0b10000000);
}
