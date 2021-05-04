#pragma once

#include <cstdint>

class PulseVoice;
class WaveVoice;

class SoundController {
 public:
	SoundController();
	~SoundController() = default;
	
	bool Advance(int cycles);

	void SetByteAt(uint16_t address, uint8_t byte);
	uint8_t GetByteAt(uint16_t);

 private:
    uint8_t sound_output_terminals_ = 0;
    bool global_sound_on_ = 0;
    uint8_t s01_volume_level_ = 0;
    uint8_t s02_volume_level_ = 0;

	PulseVoice *voice1_;
	PulseVoice *voice2_;
	WaveVoice *voice3_;
	// TODO: NoiseVoice *voice4_;

	uint32_t audio_device_ = 0;

	uint8_t FF26();
};
