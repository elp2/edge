#pragma once

#include <cstdint>

union SDL_Event;

class NoiseVoice {
 public:
  NoiseVoice();
  ~NoiseVoice();

  // Returns true if the sound has finished playing.
  bool Advance(int cycles);

  void SetFF20(uint8_t value);
  void SetFF21(uint8_t value);
  void SetFF22(uint8_t value);
  void SetFF23(uint8_t value);

  uint8_t GetFF20() { return 0xFF; /* Write only. */ };
  uint8_t GetFF21() { return ff21_; };
  uint8_t GetFF22() { return ff22_; };
  uint8_t GetFF23() { return 0xBF | ff23_; };

  // Adds this voice's samples to the buffer.
  void AddSamplesToBuffer(int16_t* buffer, int samples);

  bool Playing() { return enabled_; }

  uint16_t LFSR() { return lfsr_; }
  bool TickLFSR();

 private:
  void PrintDebug();

  bool LFSRShort() { return (ff22_ >> 3) & 0x1; }
  uint8_t ClockDivider() { return (ff22_ & 0b111); }
  uint8_t ClockShift() { return (ff22_ & 0xF0) >> 4; }

  bool SweepUp() { return (ff21_ & 0b1000) >> 3; }
  uint8_t SweepPace() { return ff21_ & 0b111; }

  uint8_t envelope_direction_ = 0;

  float FrequencyHz();

  bool enabled_ = false;
  uint8_t ff20_, ff21_, ff22_, ff23_;

  uint8_t length_;
  uint16_t lfsr_; // https://en.wikipedia.org/wiki/Linear-feedback_shift_register
  uint8_t volume_;
  bool length_enable_ = false;
  
  int cycles_ = 0;
  int next_timer_cycle_ = 0;
  int next_lfsr_cycle_ = 0;
  int cycles_per_lfsr_ = 0;
  int next_envelope_cycle_ = 0;
};
