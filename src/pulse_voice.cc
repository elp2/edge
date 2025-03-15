#include "pulse_voice.h"

#include <math.h>

#include <algorithm>
#include <cassert>
#include <iostream>

#include "constants.h"
#include "utils.h"

const uint8_t PulseVoice::waveform_[4][16] = {
  {1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0},  // 12.5% (2/16)
  {0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0},  // 25% (4/16)
  {0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0},  // 50% (8/16)
  {1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1}   // 75% (12/16)
};

PulseVoice::PulseVoice(int voice_number) {
  voice_number_ = voice_number;
}

PulseVoice::~PulseVoice() {}

int16_t PulseVoice::GetSample() {
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

  if (VolumeSweepPace() > 0) {
    envelope_cycles_ -= CYCLES_PER_SAMPLE;
    if (envelope_cycles_ <= 0) {
      volume_ += VolumeSweepUp() ? 1 : -1;
      volume_ = std::max(0, std::min((int)volume_, 15));
      envelope_cycles_ += CYCLES_PER_ENVELOPE_TICK * VolumeSweepPace();
    }
  }

  duty_cycle_timer_cycles_ -= CYCLES_PER_SAMPLE;
  if (duty_cycle_timer_cycles_ <= 0) {
    waveform_position_ = (waveform_position_ + 1) & 15;
    duty_cycle_timer_cycles_ += CyclesPerDutyCycle();
  }

  if (PeriodSweepPace() > 0) {
    period_sweep_cycles_ -= CYCLES_PER_SAMPLE;
    if (period_sweep_cycles_ <= 0) {
      DoPeriodSweep();
      period_sweep_cycles_ += CYCLES_PER_PERIOD_SWEEP_TICK * PeriodSweepPace();
    }
  }

  int sample_volume = (VOICE_MAX_VOLUME * volume_) / 15;
  int16_t sample = waveform_[DutyCycle()][waveform_position_] ? sample_volume : -sample_volume;

  return sample;
}

void PulseVoice::AddSamplesToBuffer(int16_t* buffer, int samples) {
  if (!enabled_) {
    return;
  }

  for (int i = 0; i < samples; i++) {
    buffer[i] += GetSample();
  }
}

void PulseVoice::DoPeriodSweep() {
  uint16_t current_period = PeriodValue();
  uint16_t shift_amount = PeriodSweepStep();
  
  uint16_t delta = current_period >> shift_amount;
  
  uint16_t new_period;
  if (PeriodSweepDown()) {
    new_period = current_period - delta;
  } else {
    new_period = current_period + delta;
    if (new_period > 0x7FF) {
      enabled_ = false;
      return;
    }
  }
  
  nrx3_ = new_period & 0xFF;
  nrx4_ &= 0xF8;
  nrx4_ |= (new_period >> 8) & 0x07;
}

void PulseVoice::PrintDebug() {
  std::cout << "** Pulse voice " << voice_number_ << " enabled: " << enabled_ << std::endl;
  std::cout << "NRX0: " << std::hex << int(nrx0_) << " 1: " << std::hex << int(nrx1_) << " 2: " << std::hex << int(nrx2_) << " 3: " << std::hex << int(nrx3_) << " 4: " << std::hex << int(nrx4_) << std::endl;
  std::cout << "Length: " << length_ << std::endl;
  std::cout << "Length enable: " << (int)length_enable_ << std::endl;
  std::cout << "Volume: " << (int)volume_ << std::endl;
  std::cout << "Volume Sweep pace: " << (int)VolumeSweepPace() << std::endl;
  std::cout << "Volume Sweep up: " << (int)VolumeSweepUp() << std::endl;
  std::cout << "Period: " << std::hex << PeriodValue() << std::endl;
  std::cout << "Frequency: " << FrequencyHz() << std::endl;
  std::cout << "Cycles per duty cycle: " << CyclesPerDutyCycle() << std::endl;
}

void PulseVoice::SetNRX1(uint8_t byte) { 
    nrx1_ = byte;
    length_ = PULSE_MAX_LENGTH - (nrx1_ & 0x3F);
};

void PulseVoice::SetNRX2(uint8_t byte) { 
    nrx2_ = byte;
    if (DACEnabled()) {
      volume_ = nrx2_ & 0xF0;
    } else {
      enabled_ = false;
      volume_ = 0;
    }
};

void PulseVoice::SetNRX4(uint8_t byte) { 
  nrx4_ = byte;

  length_enable_ = bit_set(nrx4_, 6);

  bool trigger = bit_set(nrx4_, 7);
  if (trigger) {
    enabled_ = DACEnabled();

    if (length_ == 0 || !length_enable_) {
      // "Trigger should treat 0 length as maximum"
      // "Trigger with disabled length should convert ","0 length to maximum".
      length_ = PULSE_MAX_LENGTH;
    }

    // Do not update timer_cycles_ bc Triggering does not affect the timer cycling.
    envelope_cycles_ = CYCLES_PER_ENVELOPE_TICK * VolumeSweepPace();
    period_sweep_cycles_ = CYCLES_PER_PERIOD_SWEEP_TICK * PeriodSweepPace();

    volume_ = (nrx2_ & 0xF0) >> 4;

    waveform_position_ = 0;
    duty_cycle_timer_cycles_ = CyclesPerDutyCycle();
  }
  // PrintDebug();
}

uint16_t PulseVoice::PeriodValue() {
  uint16_t period = nrx4_ & 0x07; // Lower 3 bits of NRx4 are high 3 bits of period
  period <<= 8;
  period |= nrx3_;

  return period;
}

float PulseVoice::FrequencyHz() {
  return 131072 / (2048 - PeriodValue());
}

