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

 private:
  uint8_t ff20_, ff21_, ff22_, ff23_;
};
