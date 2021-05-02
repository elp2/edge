#pragma once

#include <cstdint>

union SDL_Event;

class PulseVoice {
 public:
	PulseVoice();
	~PulseVoice() = default;
	
	bool Advance(int cycles);

    void SetSweepByte(uint8_t byte);
    uint8_t GetSweepByte();

    void SetWavePatternDutyByte(uint8_t byte);
    uint8_t GetWavePatternDutyByte();

    void SetEnvelopeByte(uint8_t byte);
    uint8_t GetEnvelopeByte();

    void SetFrequencyLowByte(uint8_t byte);
    uint8_t GetFrequencyLowByte();

    void SetFrequencyHighByte(uint8_t byte);
    uint8_t GetFrequencyHighByte();

 private:
     // Length of the voice in CPU steps (4194304 / sec).
    int length_steps_ = 0;
    // Length of the Sweep in CPU steps (4194304 / sec).
    int sweep_steps_ = 0;

    // 11 bit frequency sourced from the frequency_low and high bytes.
    uint16_t eleven_bit_frequency_ = 0;

	uint8_t wave_duty_ = 0;
};
