#include "wave_voice.h"

#include <math.h>

#include <algorithm>
#include <cassert>
#include <iostream>

#include "constants.h"
#include "utils.h"

const uint16_t WaveVoice::BASE_WAVE_PATTERN_ADDRESS = 0xFF30;

WaveVoice::WaveVoice() {}

WaveVoice::~WaveVoice() {}

void WaveVoice::AddSamplesToBuffer(int16_t* buffer, int samples) {
  
  for (int i = 0; i < samples; i++) {
    if (cycles_ >= next_timer_cycle_) {
      if (length_enable_) {
        length_ += 1;
      }
      if (length_ >= 256) {
        enabled_ = false;
      }
      next_timer_cycle_ += CYCLES_PER_SOUND_TIMER_TICK; 
    }

    if (cycles_ >= next_sample_cycle_) {
      sample_index_ += 1;
      if (sample_index_ >= 32) {
        sample_index_ = 0;
      }
      next_sample_cycle_ += cycles_per_sample_;
    }

    if (!enabled_) {
      return;
    }

    buffer[i] += CenteredSample() * VOICE_MAX_VOLUME;

    cycles_ += CYCLES_PER_SAMPLE;
  }
}

float WaveVoice::CenteredSample() {
  assert(sample_index_ >= 0 && sample_index_ < 32);
  uint8_t byte = wave_pattern_[sample_index_ / 2]; // First sample packed high bits.
  uint8_t nibble;
  if (sample_index_ % 2 == 0) {
    nibble = byte >> 4;
  } else {
    nibble = byte & 0x0F;
  }

  if (!DACEnabled()) {
    return 0.0f;
  }
  float centered = float(nibble) - 7.5f;

  float ret;
  switch (output_level_) {
    case 0b00:
      return 0.0f; // Mute.
    case 0b01:
      centered /= 7.5f; // 100%
      break;
    case 0b10:
      centered /= 15.0f; // 50%
      break;
    case 0b11:
      centered /= 30.0f; // 25%
      break;
  }
  return std::min(1.0f, std::max(centered, -1.0f));
}

bool WaveVoice::DACEnabled() {
  return bit_set(nr30_, 7);
}

void WaveVoice::SetNR34(uint8_t byte) {
  nr34_ = byte;

  if (bit_set(byte, 7)) {
    enabled_ = true;

    length_ = nr31_;
    cycles_ = 0;
    next_timer_cycle_ = CYCLES_PER_SOUND_TIMER_TICK;
    cycles_per_sample_ = CYCLES_PER_SECOND / FrequencyHz();
    output_level_ = (nr32_ & 0x60) >> 5;

    sample_index_ = 0;
    next_sample_cycle_ = cycles_per_sample_;
  } else {
    enabled_ = false;
  }
  length_enable_ = bit_set(byte, 6);

  // PrintDebug();
}

void WaveVoice::PrintDebug() {
  std::cout << "WaveVoice::PrintDebug" << std::endl;
  std::cout << "enabled_: " << enabled_ << std::endl;
  std::cout << "length_: " << length_ << std::endl;
  std::cout << "cycles_: " << cycles_ << std::endl;
  std::cout << "sample_index_: " << (int)sample_index_ << std::endl;
  std::cout << "Frequency: " << FrequencyHz() << std::endl;
  std::cout << "Period value: " << PeriodValue() << std::endl;
  std::cout << "next_timer_cycle_: " << next_timer_cycle_ << std::endl;
  std::cout << "cycles_per_sample_: " << cycles_per_sample_ << std::endl;
  std::cout << "next_sample_cycle_: " << next_sample_cycle_ << std::endl;
  std::cout << "output_level_: " << output_level_ << std::endl;
}

void WaveVoice::SetWavePatternAddress(uint16_t address, uint8_t byte) {
  assert(address >= BASE_WAVE_PATTERN_ADDRESS && address <= 0xFF3F);
  wave_pattern_[address - BASE_WAVE_PATTERN_ADDRESS] = byte;
}

uint8_t WaveVoice::GetWavePatternByte(uint16_t address) {
  assert(address >= BASE_WAVE_PATTERN_ADDRESS && address <= 0xFF3F);
  return wave_pattern_[address - BASE_WAVE_PATTERN_ADDRESS];
}

uint16_t WaveVoice::PeriodValue() {
  uint16_t period = nr34_ & 0x07;
  period <<= 8;
  period |= nr33_;

  return period;
}

float WaveVoice::FrequencyHz() {
  return 2097152 / (2048 - PeriodValue());
}
