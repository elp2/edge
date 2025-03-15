#pragma once

#include "constants.h"

#include <cstdint>

union SDL_Event;

class WaveVoice {
 public:
  WaveVoice();
  ~WaveVoice();

  int16_t GetSample();
  void AddSamplesToBuffer(int16_t* buffer, int samples);

  void SetNR30(uint8_t byte);
  uint8_t GetNR30() { return 0x7F | nr30_; };

  void SetNR31(uint8_t byte);
  uint8_t GetNR31() { return 0xFF | nr31_; };

  void SetNR32(uint8_t byte) { nr32_ = byte; };
  uint8_t GetNR32() { return 0x9F | nr32_; };

  void SetNR33(uint8_t byte) { nr33_ = byte; };
  uint8_t GetNR33() { return 0xFF | nr33_; };

  void SetNR34(uint8_t byte);
  uint8_t GetNR34() { return 0xBF | nr34_; };

  bool Playing() { return enabled_; };

  static const uint16_t BASE_WAVE_PATTERN_ADDRESS;

  void SetWavePatternAddress(uint16_t address, uint8_t byte);
  uint8_t GetWavePatternByte(uint16_t address);

 private:
  static const int WAVE_MAX_LENGTH = 256;

  bool DACEnabled();
  bool enabled_ = false;

  uint8_t nr30_ = 0;
  uint8_t nr31_ = 0;
  uint8_t nr32_ = 0;
  uint8_t nr33_ = 0;
  uint8_t nr34_ = 0;

  uint8_t wave_pattern_[16];

  int timer_cycles_ = CYCLES_PER_SOUND_TIMER_TICK;
  int cycles_per_sample_ = 0;
  int next_sample_cycles_ = 0;
  int sample_index_ = 0;

  int length_ = 0; // Must be able to count up to 256.
  uint8_t length_enable_ = false;
  uint8_t output_level_ = 0;
  
  uint16_t PeriodValue();
  float FrequencyHz();
  void PrintDebug();

  // Returns the current sample value centered from -1 to 1.
  float CenteredSample();
};
