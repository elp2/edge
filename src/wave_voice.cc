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
    // TODO.
    if (!enabled_) {
      return;
    }

    int sample_volume = 0;
    if (DACEnabled()) {
      sample_volume = 0;
    }
    buffer[i] += sample_volume;
  }
}

bool WaveVoice::DACEnabled() {
  return bit_set(nr30_, 7);
}

void WaveVoice::SetNR34(uint8_t byte) {
  nr34_ = byte;

  if (bit_set(byte, 7)) {
    enabled_ = true;
  } else {
    enabled_ = false;
  }
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
