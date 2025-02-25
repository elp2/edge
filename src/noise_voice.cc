#include "noise_voice.h"

#include <math.h>

#include <algorithm>
#include <cassert>
#include <iostream>

#include "constants.h"
#include "utils.h"

NoiseVoice::NoiseVoice() {
    // TODO: Implementation.
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
        enabled_ = true;
        length_ = ff20_ & 0x3F;
        lfsr_ = 0x00;
        // TODO: Envelope.
        volume_ = (ff21_ & 0xF0) >> 4;

        next_timer_cycle_ = CYCLES_PER_SOUND_TIMER_TICK;

        cycles_per_lfsr_ = CYCLES_PER_SECOND / FrequencyHz();
        next_lfsr_cycle_ = cycles_per_lfsr_;
    } else {
        std::cout << "TODO Noise voice disabled." << std::endl;
    }

    length_enable_ = bit_set(ff23_, 6);

    PrintDebug();
}

void NoiseVoice::PrintDebug() {
    std::cout << "Noise voice enabled: " << enabled_ << std::endl;
    std::cout << "Length: " << (int)length_ << std::endl;
    std::cout << "Length enable: " << length_enable_ << std::endl;
    std::cout << "LFSR: " << lfsr_ << std::endl;
    std::cout << "Frequency: " << FrequencyHz() << std::endl;
    std::cout << "Volume: " << (int)volume_ << std::endl;
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
    bool current_lfsr = 0x1 & lfsr_;
    for (int i = 0; i < samples; i++) {
        if (cycles_ >= next_timer_cycle_) {
            if (length_enable_) {
                length_ += 1;
                if (length_ == 64) {
                    enabled_ = false;
                }
            }
            next_timer_cycle_ += CYCLES_PER_SOUND_TIMER_TICK;
        }

        if (cycles_ >= next_lfsr_cycle_) {
            current_lfsr = TickLFSR();
            next_lfsr_cycle_ += cycles_per_lfsr_;
        }

        buffer[i] += current_lfsr ? volume_ : -volume_;
        std::cout << "Current LFSR: " << cycles_ << " " << buffer[i] << std::endl;
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
