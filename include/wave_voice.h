#pragma once

#include <cstdint>

union SDL_Event;

class WaveVoice {
 public:
  WaveVoice();
  ~WaveVoice();

  void AddSamplesToBuffer(int16_t* buffer, int samples);

  void SetNR30(uint8_t byte) { nr30_ = byte; };
  uint8_t GetNR30() { return 0x7F | nr30_; };

  void SetNR31(uint8_t byte) { nr31_ = byte; };
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
  bool DACEnabled();
  bool enabled_ = false;

  uint8_t nr30_ = 0;
  uint8_t nr31_ = 0;
  uint8_t nr32_ = 0;
  uint8_t nr33_ = 0;
  uint8_t nr34_ = 0;

  uint8_t wave_pattern_[16];
};
