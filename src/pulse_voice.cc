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

void PulseVoice::AddSamplesToBuffer(int16_t* buffer, int samples) {
  if (!enabled_) {
    return;
  }

  for (int i = 0; i < samples; i++) {
    if (cycles_ >= next_timer_cycle_) {
      if (length_enable_) {
        length_ += 1;
      }
      if (length_ >= 64) {
        enabled_ = false;
      }
      next_timer_cycle_ += CYCLES_PER_SOUND_TIMER_TICK;
    }
    
    if (!enabled_) {
      return;
    }

    if (VolumeSweepPace() > 0 && cycles_ >= next_envelope_cycle_) {
      volume_ += VolumeSweepUp() ? 1 : -1;
      volume_ = std::max(0, std::min((int)volume_, 15));
      next_envelope_cycle_ += CYCLES_PER_ENVELOPE_TICK * VolumeSweepPace();
    }

    if (cycles_ >= next_duty_cycle_cycle_) {
      waveform_position_ = (waveform_position_ + 1) & 15;
      next_duty_cycle_cycle_ += CyclesPerDutyCycle();
    }

    if (PeriodSweepPace() > 0 && cycles_ >= next_period_sweep_cycle_) {
      DoPeriodSweep();
      next_period_sweep_cycle_ += CYCLES_PER_PERIOD_SWEEP_TICK * PeriodSweepPace();
    }

    int sample_volume = (VOICE_MAX_VOLUME * volume_) / 15;
    buffer[i] += waveform_[DutyCycle()][waveform_position_] ? sample_volume : -sample_volume;

    cycles_ += CYCLES_PER_SAMPLE;
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
  std::cout << "Pulse voice " << voice_number_ << " enabled: " << enabled_ << std::endl;
  std::cout << "Length: " << (int)length_ << std::endl;
  std::cout << "Length enable: " << (int)length_enable_ << std::endl;
  std::cout << "Volume: " << (int)volume_ << std::endl;
  std::cout << "Volume Sweep pace: " << (int)VolumeSweepPace() << std::endl;
  std::cout << "Volume Sweep up: " << (int)VolumeSweepUp() << std::endl;
  std::cout << "Period: " << std::hex << PeriodValue() << std::endl;
  std::cout << "Frequency: " << FrequencyHz() << std::endl;
  std::cout << "Cycles per duty cycle: " << CyclesPerDutyCycle() << std::endl;
}

void PulseVoice::SetNRX4(uint8_t byte) { 
  nrx4_ = byte;
  if (bit_set(nrx4_, 7)) {
    enabled_ = true;

    cycles_ = 0;
    length_ = nrx0_ & 0x3F;
    next_timer_cycle_ = CYCLES_PER_SOUND_TIMER_TICK;
    next_envelope_cycle_ = CYCLES_PER_ENVELOPE_TICK * VolumeSweepPace();
    next_period_sweep_cycle_ = CYCLES_PER_PERIOD_SWEEP_TICK * PeriodSweepPace();

    volume_ = (nrx2_ & 0xF0) >> 4;

    waveform_position_ = 0;
    next_duty_cycle_cycle_ = CyclesPerDutyCycle();
  } else {
    enabled_ = false;
  }
   
  length_enable_ = bit_set(nrx4_, 6);
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

