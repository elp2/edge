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

int16_t WaveVoice::GetSample() {
  timer_cycles_ -= CYCLES_PER_SAMPLE;
  if (timer_cycles_ <= 0) {
    timer_cycles_ += CYCLES_PER_SOUND_TIMER_TICK;
    if (length_enable_) {
      length_ = std::max(length_ - 1, 0);
      if (length_ == 0) {
        enabled_ = false;
        return 0;
      }
    }
  }

  if (!enabled_) {
    return 0;
  }

  if (next_sample_cycles_ <= 0) {
    sample_index_ += 1;
    if (sample_index_ >= 32) {
      sample_index_ = 0;
    }
    next_sample_cycles_ += cycles_per_sample_;
  }

  int16_t sample = CenteredSample() * VOICE_MAX_VOLUME;

  return sample;
}

void WaveVoice::AddSamplesToBuffer(int16_t* buffer, int samples) {
  for (int i = 0; i < samples; i++) {
    buffer[i] += GetSample();
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

void WaveVoice::SetNR30(uint8_t byte) {
  nr30_ = byte;
  if (!bit_set(nr30_, 7)) {
    enabled_ = false;
  }
}

void WaveVoice::SetNR31(uint8_t byte) {
  nr31_ = byte;
  length_ = WAVE_MAX_LENGTH - nr31_;
}

void WaveVoice::SetNR34(uint8_t byte) {
  nr34_ = byte;

  length_enable_ = bit_set(byte, 6);

  bool trigger = bit_set(byte, 7);
  if (trigger) {
    enabled_ = DACEnabled();

    if (length_ == 0 || !length_enable_) {
      // "Trigger should treat 0 length as maximum"
      // "Trigger with disabled length should convert ","0 length to maximum".
      length_ = WAVE_MAX_LENGTH;
    }

    cycles_per_sample_ = CYCLES_PER_SECOND / FrequencyHz();
    output_level_ = (nr32_ & 0x60) >> 5;

    sample_index_ = 0;
    // Do not adjust timer_cycles_. It is independent of trigger.
    next_sample_cycles_ = cycles_per_sample_;
  }

  // PrintDebug();
}

void WaveVoice::PrintDebug() {
  std::cout << "WaveVoice::PrintDebug" << std::endl;
  std::cout << "enabled_: " << enabled_ << std::endl;
  std::cout << "length_: " << length_ << std::endl;
  std::cout << "sample_index_: " << (int)sample_index_ << std::endl;
  std::cout << "Frequency: " << FrequencyHz() << std::endl;
  std::cout << "Period value: " << PeriodValue() << std::endl;
  std::cout << "cycles_per_sample_: " << cycles_per_sample_ << std::endl;
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
