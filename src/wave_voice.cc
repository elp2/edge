#include "wave_voice.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <math.h>

#include "constants.h"
#include "utils.h"

const int MAX_SOUND_BUFFER_LENGTH = SAMPLE_RATE;

WaveVoice::WaveVoice() {
    // The sound can be no longer than 1/4 seconds (when t1 is set to 0).
    sound_buffer_ = new float[MAX_SOUND_BUFFER_LENGTH];
}

WaveVoice::~WaveVoice() {
    delete sound_buffer_;
}

bool WaveVoice::PlaySound(float **sound_buffer, int *length) {
	if (initial_) {
		playback_steps_remaining_ = length_steps_;
		*sound_buffer = sound_buffer_;
		*length = buffer_length_;

		initial_ = false;
		return true;
	}
	return false;
}

void WaveVoice::SetOnOffByte(uint8_t byte) {
    on_off_byte_ = byte;
    if (on_off_byte_ & 0b10000000) {
        // TODO handle turn on voice.
    } else {
        // TODO handle turn OFF voice.
    }
}

void WaveVoice::SetFrequencyHighByte(uint8_t byte) {
	frequency_high_byte_ = byte;

	loop_ = false; // !bit_set(byte, 6); // TODO: The logo chime seems to indicate loop, but it clearly shouldn't loop.
	initial_ = bit_set(byte, 7);
	if (initial_) {
		GenerateSoundBuffer();
	}
}

bool WaveVoice::Advance(int cycles) {
	playback_steps_remaining_ -= cycles;
	if (playback_steps_remaining_ > 0) {
		return false;
	}
	if (loop_) {
		initial_ = true;
	}
	else {
		return true;
	}
	return false;
}

int WaveFrequency(uint8_t low_byte, uint8_t high_byte) {
	uint16_t combined_frequency = high_byte & 0b111;
	combined_frequency <<= 8;
	combined_frequency |= low_byte;
	return 65336 / (2048 - combined_frequency); // Pulse voice constant is 131072.
}

void WaveVoice::GenerateSoundBuffer() {
	int frequency = WaveFrequency(frequency_low_byte_, frequency_high_byte_);
    int samples_per_frequency = SAMPLE_RATE / frequency;
	int samples_per_fragment = samples_per_frequency / 32;

	float sound_length = (256 - sound_length_byte_) / 256.0;

	int i = 0;
	int fi = 0;
	while (i < std::min(int(sound_length * SAMPLE_RATE), MAX_SOUND_BUFFER_LENGTH)) {
		uint8_t val = fi % 2 == 0 ? NIBBLEHIGH(wave_pattern_[fi/2]) : NIBBLELOW(wave_pattern_[fi/2]);
		
		float sound = (float)val / (float)(0xf / 2.0) - 1.0;
		sound = std::max(std::min(1.0f, sound), -1.0f);
		sound_buffer_[i] = sound;

		i += 1;
		if (i % samples_per_fragment == 0) {
			fi = (fi + 1) % 32;
		}
	}
}

void WaveVoice::SetWavePatternAddress(uint16_t address, uint8_t byte) {
    const uint16_t BASE_WAVE_PATTERN_ADDRESS = 0xFF30;
    assert(address >= BASE_WAVE_PATTERN_ADDRESS && address <= 0xFF3F);

    wave_pattern_[address - BASE_WAVE_PATTERN_ADDRESS] = byte;
}
