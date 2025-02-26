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
    
    buffer[i] += (SampleNibble() * VOICE_MAX_VOLUME) / 15;

    cycles_ += CYCLES_PER_SAMPLE;
  }
}

uint8_t WaveVoice::SampleNibble() {
  assert(sample_index_ >= 0 && sample_index_ < 32);
  uint8_t byte = wave_pattern_[sample_index_ / 2]; // First sample packed high bits.
  uint8_t nibble;
  if (sample_index_ % 2 == 0) {
    nibble = byte >> 4;
  } else {
    nibble = byte & 0x0F;
  }

  if (!DACEnabled()) {
    return 0;
  }

  switch (output_level_) {
    case 0b00:
      return 0; // Mute.
    case 0b01:
      return nibble; // 100%
    case 0b10:
      return nibble >> 1; // 50%
    case 0b11:
      return nibble >> 2; // 25%
  }
  assert(false);
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

int WaveFrequency(uint8_t low_byte, uint8_t high_byte) {
  uint16_t combined_frequency = high_byte & 0b111;
  combined_frequency <<= 8;
  combined_frequency |= low_byte;
  return 65336 /
         (2048 - combined_frequency);  // Pulse voice constant is 131072.
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
  return 65536 / (2048 - PeriodValue());
}
