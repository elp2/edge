#include "pulse_voice.h"

#include <algorithm>
#include <cassert>
#include <iostream>

#include "constants.h"
#include "Utils.hpp"

const int MAX_SOUND_BUFFER_LENGTH = SAMPLE_RATE / 4;
const int SAMPLES_PER_ENVELOPE_SWEEP = SAMPLE_RATE / 64;

PulseVoice::PulseVoice() {
    // The sound can be no longer than 1/4 seconds (when t1 is set to 0).
    sound_buffer_ = new float[MAX_SOUND_BUFFER_LENGTH];
}

PulseVoice::~PulseVoice() {
    delete sound_buffer_;
}

bool PulseVoice::PlaySound(float **sound_buffer, int *length) {
	if (initial_) {
		playback_steps_remaining_ = length_steps_;
		*sound_buffer = sound_buffer_;
		*length = buffer_length_;

		initial_ = false;
		return true;
	}
	return false;
}

void PulseVoice::SetFrequencyHighByte(uint8_t byte) {
	frequency_high_byte_ = byte;
	initial_ = bit_set(byte, 7);
	if (initial_) {
		GenerateSoundBuffer();
	}
	loop_ = !bit_set(byte, 6);
}

bool PulseVoice::Advance(int cycles) {
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

int HighEights(uint8_t wave_pattern_duty_byte) {
	switch ((wave_pattern_duty_byte & (0b11000000)) >> 6)
	{
	case 0b00:
		return 1;
	case 0b01:
		return 2;
	case 0b10:
		return 4;
	case 0b11:
		return 6;
	default:
		assert(false);
		break;
	}
	return 4;
}

float VolumeForEnvelope(int envelope) {
	if (envelope <= 0) {
		return 0.0;
	}
	if (envelope >= 0xF) {
		return 1.0;
	}

	return (float)envelope / 15.0;
}

void PulseVoice::GenerateSoundBuffer() {
	uint8_t t1 = 0b11111 & wave_pattern_duty_byte_;
	length_steps_ = CYCLES_PER_SECOND / (256 * (64 - t1));
	buffer_length_ = SAMPLE_RATE * (64 - t1) / 256;

	uint16_t combined_frequency = frequency_high_byte_ & 0b111;
	combined_frequency <<= 8;
	frequency_low_byte_;
	int frequency = 131072 / (2048 - combined_frequency);

	int samples_per_wave_duty = SAMPLE_RATE / frequency;
	int high_eightths = HighEights(wave_pattern_duty_byte_);

	int envelope = envelope_byte_ >> 4;
	int envelope_sweeps = envelope_byte_ & 0b11;
	bool envelope_up = bit_set(envelope_byte_, 3);
	int samples_since_envelope = 0;

	float volume = VolumeForEnvelope(envelope);
	int highs_per_wave_duty = high_eightths * samples_per_wave_duty / 8;

	int idx = 0;
	int wave_samples = 0;
	while (idx < buffer_length_) {
		if (wave_samples <= highs_per_wave_duty) {
			sound_buffer_[idx] = volume;
		}
		else {
			sound_buffer_[idx] = -volume;
		}
		idx++;

		if (++samples_since_envelope == SAMPLES_PER_ENVELOPE_SWEEP) {
			samples_since_envelope = 0;
			if (envelope_sweeps > 0) {
				if (envelope_up) {
					envelope++;
				}
				else {
					envelope--;
				}
				volume = VolumeForEnvelope(envelope);
			}
			samples_since_envelope = 0;
		}
		if (++wave_samples == samples_per_wave_duty) {
			wave_samples = 0;
		}
	}
}
