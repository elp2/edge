#include "wave_voice.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <math.h>

#include "constants.h"
#include "Utils.hpp"

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
    
}

void WaveVoice::SetWavePatternAddress(uint16_t address, uint8_t byte) {
    const uint16_t BASE_WAVE_PATTERN_ADDRESS = 0xFF30;
    assert(address >= BASE_WAVE_PATTERN_ADDRESS && address <= 0xFF3F);

    wave_pattern_[address - BASE_WAVE_PATTERN_ADDRESS] = byte;
}
