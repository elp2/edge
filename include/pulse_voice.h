#pragma once

#include <cstdint>

#include "constants.h"

union SDL_Event;

class PulseVoice {
 public:
  PulseVoice(int voice_number);
  ~PulseVoice();

  // Adds this voice's samples to the buffer.
  void AddSamplesToBuffer(int16_t* buffer, int samples);
  int16_t GetSample();

  // Pulse voice 2 will never have this set by the controller.
  void SetNRX0(uint8_t byte) { nrx0_ = byte; };
  uint8_t GetNRX0() { return 0x80 | nrx0_; };

  void SetNRX1(uint8_t byte);
  uint8_t GetNRX1() { return 0x3F | nrx1_; };

  void SetNRX2(uint8_t byte);
  uint8_t GetNRX2() { return nrx2_; };

  void SetNRX3(uint8_t byte) { nrx3_ = byte; };
  uint8_t GetNRX3() { return 0xFF | nrx3_; };

  void SetNRX4(uint8_t byte);
  uint8_t GetNRX4() { return 0xBF | nrx4_; };

  bool Playing() { return enabled_; }
  void PrintDebug();

 private:
  static const int PULSE_MAX_LENGTH = 64;
  bool VolumeSweepUp() { return (nrx2_ & 0b1000) >> 3; }
  uint8_t VolumeSweepPace() { return nrx2_ & 0b111; }

  bool PeriodSweepDown() { return (nrx0_ & 0b1000) >> 3; }
  uint8_t PeriodSweepStep() { return nrx0_ & 0x07;  }
  uint8_t PeriodSweepPace() { return (nrx0_ & 0x70) >> 4;}
  void DoPeriodSweep();

  int CyclesPerDutyCycle() { return CYCLES_PER_SECOND / FrequencyHz() / 16; }

  uint8_t DutyCycle() { return (nrx1_ & 0xC0) >> 6; }

  bool DACEnabled() { return nrx2_ & 0xF8; }

  uint16_t PeriodValue();
  float FrequencyHz();
  uint8_t nrx0_ = 0;
  uint8_t nrx1_ = 0;
  uint8_t nrx2_ = 0;
  uint8_t nrx3_ = 0;
  uint8_t nrx4_ = 0;

  int duty_cycle_timer_cycles_ = CYCLES_PER_SOUND_TIMER_TICK;
  int timer_cycles_ = 0;
  int envelope_cycles_ = 0;
  int period_sweep_cycles_ = 0;
  bool enabled_ = false;
  int length_ = 0;
  uint8_t length_enable_ = false;
  int volume_ = 0;

  int voice_number_ = 0;

  uint8_t waveform_position_ = 0;
  static const uint8_t waveform_[4][16];
};
