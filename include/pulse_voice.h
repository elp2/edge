#pragma once

#include <cstdint>

union SDL_Event;

class PulseVoice {
 public:
  PulseVoice();
  ~PulseVoice();

  // Returns true if the sound has finished playing.
  bool Advance(int cycles);

  // Adds this voice's samples to the buffer.
  void AddSamplesToBuffer(int16_t* buffer, int samples);

  // Pulse voice 2 will never have this set by the controller.
  void SetNRX0(uint8_t byte) { nrx0_ = byte; };
  uint8_t GetNRX0() { return 0x80 | nrx0_; };

  void SetNRX1(uint8_t byte) { nrx1_ = byte; };
  uint8_t GetNRX1() { return 0x3F | nrx1_; };

  void SetNRX2(uint8_t byte) { nrx2_ = byte; };
  uint8_t GetNRX2() { return nrx2_; };

  void SetNRX3(uint8_t byte) { nrx3_ = byte; };
  uint8_t GetNRX3() { return 0xFF | nrx3_; };

  void SetNRX4(uint8_t byte) { nrx4_ = byte; };
  uint8_t GetNRX4() { return 0xBF | nrx4_; };

  bool Playing() { return false; }

 private:
  uint8_t nrx0_ = 0;
  uint8_t nrx1_ = 0;
  uint8_t nrx2_ = 0;
  uint8_t nrx3_ = 0;
  uint8_t nrx4_ = 0;
};
