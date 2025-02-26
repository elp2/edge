#pragma once

#include <cstdint>

union SDL_Event;

class PulseVoice {
 public:
  PulseVoice(int voice_number);
  ~PulseVoice();

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

  void SetNRX4(uint8_t byte);
  uint8_t GetNRX4() { return 0xBF | nrx4_; };

  bool Playing() { return enabled_; }

 private:
  void PrintDebug();

  bool VolumeSweepUp() { return (nrx2_ & 0b1000) >> 3; }
  uint8_t VolumeSweepPace() { return nrx2_ & 0b111; }

  bool PeriodSweepDown() { return (nrx0_ & 0b1000) >> 3; }
  uint8_t PeriodSweepStep() { return nrx0_ & 0x07;  }
  uint8_t PeriodSweepPace() { return (nrx0_ & 0x70) >> 4;}
  void DoPeriodSweep();

  // Cycles spent in the low duty cycle.
  int LowDutyCycles();
  // Cycles spent in the low duty cycle.
  int HighDutyCycles();

  uint8_t DutyCycle() { return (nrx1_ & 0xC0) >> 6; }

  uint16_t PeriodValue();
  float FrequencyHz();
  uint8_t nrx0_ = 0;
  uint8_t nrx1_ = 0;
  uint8_t nrx2_ = 0;
  uint8_t nrx3_ = 0;
  uint8_t nrx4_ = 0;

  int cycles_ = 0;
  int cycles_per_duty_cycle_ = 0;
  int next_duty_cycle_cycle_ = 0;
  int next_timer_cycle_ = 0;
  int next_envelope_cycle_ = 0;
  int next_period_sweep_cycle_ = 0;
  bool enabled_ = false;
  uint8_t length_ = 0;
  uint8_t length_enable_ = false;
  uint8_t volume_ = 0;

  bool duty_high_ = false;
  int voice_number_ = 0;
};
