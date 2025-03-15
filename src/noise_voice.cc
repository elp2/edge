#include "noise_voice.h"

#include <math.h>

#include <algorithm>
#include <cassert>
#include <iostream>

#include "constants.h"
#include "utils.h"

NoiseVoice::NoiseVoice() {
}

void NoiseVoice::SetFF20(uint8_t value) { 
    ff20_ = value;
    length_ = NOISE_MAX_LENGTH - (ff20_ & 0x3F);
}

void NoiseVoice::SetFF21(uint8_t value) { 
    ff21_ = value;
    if (DACEnabled()) {
      volume_ = (ff21_ & 0xF0) >> 4;
    } else {
      enabled_ = false;
      volume_ = 0;
    }
}

bool NoiseVoice::DACEnabled() {
    return ff21_ & 0xF8;
}

void NoiseVoice::SetFF22(uint8_t value) { 
    ff22_ = value;
}

void NoiseVoice::SetFF23(uint8_t value) { 
    ff23_ = value;

    length_enable_ = bit_set(ff23_, 6);
    if (bit_set(ff23_, 7)) {
        if (length_ == 0 || !length_enable_) {
            // "Trigger should treat 0 length as maximum"
            // "Trigger with disabled length should convert ","0 length to maximum".
            length_ = NOISE_MAX_LENGTH;
        }
        enabled_ = DACEnabled();
        lfsr_ = 0x7FFF;


        // timer_ updated in the advance.
        cycles_per_lfsr_ = CYCLES_PER_SECOND / FrequencyHz();
        lsfr_cycles_ = cycles_per_lfsr_;
        envelope_cycles_ = CYCLES_PER_ENVELOPE_TICK * SweepPace();
    }
    // PrintDebug();
}

void NoiseVoice::PrintDebug() {
    std::cout << "Noise voice enabled: " << enabled_ << std::endl;
    std::cout << "Length: " << (int)length_ << std::endl;
    std::cout << "Length enable: " << length_enable_ << std::endl;
    std::cout << "LFSR: " << lfsr_ << std::endl;
    std::cout << "Frequency: " << FrequencyHz() << std::endl;
    std::cout << "Volume: " << (int)volume_ << std::endl;
    std::cout << "Sweep pace: " << (int)SweepPace() << std::endl;
    std::cout << "Sweep up: " << SweepUp() << std::endl;
    std::cout << "Cycles per LFSR: " << (int)cycles_per_lfsr_ << std::endl;
}

bool NoiseVoice::TickLFSR() {
    uint16_t xored = bit_set(lfsr_, 0) ^ bit_set(lfsr_, 1);
    if (LFSRShort()) {
        lfsr_ = ((lfsr_ >> 1) & ~0x40) | (xored << 6);
    } else {
        lfsr_ = ((lfsr_ >> 1) & ~0x4000) | (xored << 14);
    }
    return 0x1 & lfsr_;
}

int16_t NoiseVoice::GetSample() {
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

  bool current_lfsr = 0x1 & lfsr_;
  if (lsfr_cycles_ <= 0) {
    current_lfsr = TickLFSR();
    lsfr_cycles_ += cycles_per_lfsr_;
  }

  if (SweepPace() > 0) {
    envelope_cycles_ -= CYCLES_PER_SAMPLE;
    if (envelope_cycles_ <= 0) {
      volume_ += SweepUp() ? 1 : -1;
      if (volume_ > 15) {
        volume_ = 15;
      } else if (volume_ < 0) {
        volume_ = 0;
      }
      envelope_cycles_ += CYCLES_PER_ENVELOPE_TICK * SweepPace();
    }
  }

  int sample_volume = (VOICE_MAX_VOLUME * volume_) / 15;
  int16_t sample = current_lfsr ? sample_volume : -sample_volume;

  return sample;
}

void NoiseVoice::AddSamplesToBuffer(int16_t* buffer, int samples) {
    if (!enabled_) {
        return;
    }

    for (int i = 0; i < samples; i++) {
        buffer[i] += GetSample();
    }
}

float NoiseVoice::FrequencyHz() {
    float divider = ClockDivider();
    if (divider == 0) {
        // Divider 0 is actually divider 0.5.
        divider = 0.5;
    }
    return 262144.0f / float(divider * (1 << ClockShift()));
}

NoiseVoice::~NoiseVoice() {}

bool NoiseVoice::LFSRShort() { return bit_set(ff22_, 3); }
