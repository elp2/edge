#include "pulse_voice.h"

#include <math.h>

#include <algorithm>
#include <cassert>
#include <iostream>

#include "constants.h"
#include "utils.h"


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

        if (SweepPace() > 0 && cycles_ >= next_envelope_cycle_) {
            if (SweepUp()) {
                volume_ += 1;
            } else {
                volume_ -= 1;
            }
            if (volume_ > 15) {
                volume_ = 15;
            } else if (volume_ < 0) {
                // Volume 0 is still considered enabled.
                volume_ = 0;
            }
            next_envelope_cycle_ += CYCLES_PER_ENVELOPE_TICK * SweepPace();
        }

        if (cycles_ >= next_duty_cycle_cycle_) {
            duty_high_ = !duty_high_;
            next_duty_cycle_cycle_ += duty_high_ ? HighDutyCycles() : LowDutyCycles();
        }

        int sample_volume = VOICE_MAX_VOLUME * volume_ / 15;
        buffer[i] += duty_high_ ? sample_volume : -sample_volume;

        cycles_ += CYCLES_PER_SAMPLE;
    }
}

void PulseVoice::PrintDebug() {
  std::cout << "Pulse voice " << voice_number_ << " enabled: " << enabled_ << std::endl;
  std::cout << "Length: " << (int)length_ << std::endl;
  std::cout << "Length enable: " << (int)length_enable_ << std::endl;
  std::cout << "Volume: " << (int)volume_ << std::endl;
  std::cout << "Sweep pace: " << (int)SweepPace() << std::endl;
  std::cout << "Sweep up: " << SweepUp() << std::endl;
  std::cout << "Period: " << std::hex << PeriodValue() << std::endl;
  std::cout << "Frequency: " << FrequencyHz() << std::endl;
  std::cout << "Cycles per duty cycle: " << cycles_per_duty_cycle_ << std::endl;
}

void PulseVoice::SetNRX4(uint8_t byte) { 
  nrx4_ = byte;
  if (bit_set(nrx4_, 7)) {
    enabled_ = true;

    cycles_ = 0;
    length_ = nrx0_ & 0x3F;
    next_timer_cycle_ = CYCLES_PER_SOUND_TIMER_TICK;
    next_envelope_cycle_ = CYCLES_PER_ENVELOPE_TICK * SweepPace();
    cycles_per_duty_cycle_ = CYCLES_PER_SECOND / FrequencyHz();

    volume_ = (nrx2_ & 0xF0) >> 4;

    duty_high_ = false;
    next_duty_cycle_cycle_ = LowDutyCycles();

    PrintDebug();
  } else {
    enabled_ = false;
  }
   
  length_enable_ = bit_set(nrx4_, 6);
}

int PulseVoice::LowDutyCycles() {
  if (DutyCycle() == 0x00) {
    return cycles_per_duty_cycle_ / 8;
  } else if (DutyCycle() == 0x01) {
    return cycles_per_duty_cycle_ / 4;
  } else if (DutyCycle() == 0x02) {
    return cycles_per_duty_cycle_ / 2;
  } else if (DutyCycle() == 0x03) {
    return 3 * cycles_per_duty_cycle_ / 4;
  }
  assert(false);
}

int PulseVoice::HighDutyCycles() {
  return cycles_per_duty_cycle_ - LowDutyCycles();
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
