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
}

void NoiseVoice::SetFF21(uint8_t value) { 
    ff21_ = value;
}

void NoiseVoice::SetFF22(uint8_t value) { 
    ff22_ = value;
}

void NoiseVoice::SetFF23(uint8_t value) { 
    ff23_ = value;

    if (bit_set(ff23_, 7)) {
        // Trigger.
        cycles_ = 0; // TODO: Should the clock reset each time?
        enabled_ = true;
        length_ = ff20_ & 0x3F;
        lfsr_ = 0x00;
        // TODO: Envelope.
        volume_ = (ff21_ & 0xF0) >> 4;

        next_timer_cycle_ = CYCLES_PER_SOUND_TIMER_TICK;

        cycles_per_lfsr_ = CYCLES_PER_SECOND / FrequencyHz();
        next_lfsr_cycle_ = cycles_per_lfsr_;

        next_envelope_cycle_ = CYCLES_PER_ENVELOPE_TICK * SweepPace();
    } else {
        enabled_ = false;
    }

    length_enable_ = bit_set(ff23_, 6);

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
}

bool NoiseVoice::TickLFSR() {
    bool xnored = !(bit_set(lfsr_, 0) ^ bit_set(lfsr_, 1));
    if (xnored) {
        lfsr_ |= 0x8000;
        if (LFSRShort()) {
            lfsr_ |= 0x0080;
        }
    }
    lfsr_ >>= 1;
    return 0x1 & lfsr_;
}

void NoiseVoice::AddSamplesToBuffer(int16_t* buffer, int samples) {
    if (!enabled_) {
        return;
    }

    bool current_lfsr = 0x1 & lfsr_;
    for (int i = 0; i < samples; i++) {
        if (cycles_ >= next_timer_cycle_) {
            if (length_enable_) {
                length_ += 1;
                if (length_ >= 64) {
                    enabled_ = false;
                }
            }
            next_timer_cycle_ += CYCLES_PER_SOUND_TIMER_TICK;
        }

        if (!enabled_) {
            return;
        }

        if (cycles_ >= next_lfsr_cycle_) {
            current_lfsr = TickLFSR();
            next_lfsr_cycle_ += cycles_per_lfsr_;
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

        int sample_volume = VOICE_MAX_VOLUME * volume_ / 15;
        buffer[i] += current_lfsr ? sample_volume : -sample_volume;

        cycles_ += CYCLES_PER_SAMPLE;
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
