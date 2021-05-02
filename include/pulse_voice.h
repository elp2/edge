#pragma once

#include <cstdint>

union SDL_Event;

class PulseVoice {
 public:
	PulseVoice();
	~PulseVoice() = default;
	
	bool Advance(int cycles);

    void SetSweepByte(uint8_t byte) { sweep_byte_ = byte; };
    uint8_t GetSweepByte() { return sweep_byte_; };

    void SetWavePatternDutyByte(uint8_t byte) { wave_pattern_duty_byte_ = byte; };
    uint8_t GetWavePatternDutyByte() { return wave_pattern_duty_byte_; };

    void SetEnvelopeByte(uint8_t byte) { envelope_byte_ = byte; };
    uint8_t GetEnvelopeByte() { return envelope_byte_; };

    void SetFrequencyLowByte(uint8_t byte) { frequency_low_byte_ = byte; };
    uint8_t GetFrequencyLowByte() { return frequency_low_byte_; };

    void SetFrequencyHighByte(uint8_t byte) { frequency_high_byte_ = byte; };
    uint8_t GetFrequencyHighByte()  { return frequency_high_byte_; };

 private:
    uint8_t sweep_byte_ = 0;
    uint8_t envelope_byte_ = 0;
    uint8_t wave_pattern_duty_byte_ = 0;
    uint8_t frequency_low_byte_ = 0;
    uint8_t frequency_high_byte_ = 0;
};
