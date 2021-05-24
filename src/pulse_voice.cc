#include "pulse_voice.h"

#include <math.h>

#include <algorithm>
#include <cassert>
#include <iostream>

#include "constants.h"
#include "utils.h"

const int MAX_SOUND_BUFFER_LENGTH = SAMPLE_RATE / 4;
const int SAMPLES_PER_ENVELOPE_SWEEP = SAMPLE_RATE / 64;

PulseVoice::PulseVoice() {
  // The sound can be no longer than 1/4 seconds (when t1 is set to 0).
  sound_buffer_ = new float[MAX_SOUND_BUFFER_LENGTH];
}

PulseVoice::~PulseVoice() { delete sound_buffer_; }

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
  loop_ = false;  // !bit_set(byte, 6); // TODO: The logo chime seems to
                  // indicate loop, but it clearly shouldn't loop.
}

bool PulseVoice::Advance(int cycles) {
  playback_steps_remaining_ -= cycles;
  if (playback_steps_remaining_ > 0) {
    return false;
  }
  if (loop_) {
    initial_ = true;
  } else {
    return true;
  }
  return false;
}

int HighEights(uint8_t wave_pattern_duty_byte) {
  switch ((wave_pattern_duty_byte & (0b11000000)) >> 6) {
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

int PulseFrequency(uint8_t low_byte, uint8_t high_byte) {
  uint16_t combined_frequency = high_byte & 0b111;
  combined_frequency <<= 8;
  combined_frequency |= low_byte;
  return 131072 / (2048 - combined_frequency);
}

int SweepTimeCycles(uint8_t sweep_byte) {
  uint8_t sweep_time = (sweep_byte & 0b1110000) >> 4;
  return sweep_time * SAMPLE_RATE / 8;
}

void PulseVoice::GenerateSoundBuffer() {
  uint8_t t1 = 0b11111 & wave_pattern_duty_byte_;
  length_steps_ = CYCLES_PER_SECOND / (256 * (64 - t1));
  buffer_length_ = SAMPLE_RATE * (64 - t1) / 256;

  int sweep_time_cycles = SweepTimeCycles(sweep_byte_);
  bool sweep_decreases = bit_set(sweep_byte_, 3);
  uint8_t num_sweeps = sweep_byte_ & 0b111;
  int samples_since_sweep = 0;
  int sweep_n = 0;

  int envelope = envelope_byte_ >> 4;
  int num_envelope_sweeps = envelope_byte_ & 0b111;
  bool envelope_up = bit_set(envelope_byte_, 3);
  int samples_since_envelope = 0;
  float volume = VolumeForEnvelope(envelope);

  int frequency = PulseFrequency(frequency_low_byte_, frequency_high_byte_);
  int samples_per_wave_duty = SAMPLE_RATE / frequency;
  int highs_per_wave_duty =
      HighEights(wave_pattern_duty_byte_) * samples_per_wave_duty / 8;

  int idx = 0;
  int wave_samples = 0;
  while (idx < buffer_length_) {
    if (wave_samples <= highs_per_wave_duty) {
      sound_buffer_[idx] = volume;
    } else {
      sound_buffer_[idx] = -volume;
    }
    idx++;

    if (++samples_since_envelope == SAMPLES_PER_ENVELOPE_SWEEP) {
      if (num_envelope_sweeps > 0) {
        num_envelope_sweeps--;
        if (envelope_up) {
          envelope++;
        } else {
          envelope--;
        }
        volume = VolumeForEnvelope(envelope);
      }
      samples_since_envelope = 0;
    }

    if (++samples_since_sweep == sweep_time_cycles) {
      if (num_sweeps > 0) {
        int delta = frequency / pow(2, sweep_n);
        if (sweep_decreases) {
          frequency -= delta;
          frequency = std::max(10, frequency);
        } else {
          frequency += delta;
        }

        samples_per_wave_duty = SAMPLE_RATE / frequency;
        highs_per_wave_duty =
            HighEights(wave_pattern_duty_byte_) * samples_per_wave_duty / 8;

        num_sweeps--;
        sweep_n++;
      }
      samples_since_sweep = 0;
    }

    if (++wave_samples == samples_per_wave_duty) {
      wave_samples = 0;
    }
  }
}
