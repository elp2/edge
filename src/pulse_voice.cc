#include "pulse_voice.h"

#include <cassert>
#include <iostream>

const int CYCLES_PER_SECOND = 4194304; // TODO: Global Constant.

PulseVoice::PulseVoice() {
}


void PulseVoice::SetSweepByte(uint8_t byte) {
    // TODO.
}

uint8_t PulseVoice::GetSweepByte() {
    return 0; // TODO
}

void PulseVoice::SetWavePatternDutyByte(uint8_t byte) {
    uint8_t t1 = 0b11111 & byte;
    length_steps_ = CYCLES_PER_SECOND / (256 * (64 - t1));
    wave_duty_ = (byte & (0b11000000)) >> 6;
}

uint8_t PulseVoice::GetWavePatternDutyByte() {
    // Only Wave Duty can be read.
    return wave_duty_ << 6;
}

void PulseVoice::SetEnvelopeByte(uint8_t byte) {
    // TODO.
}

uint8_t PulseVoice::GetEnvelopeByte() {
    return 0; // TODO.
}

void PulseVoice::SetFrequencyLowByte(uint8_t byte) {
    eleven_bit_frequency_ &= 0xFF00;
    eleven_bit_frequency_ |= byte;
}

uint8_t PulseVoice::GetFrequencyLowByte() {
    return eleven_bit_frequency_ & 0xFF;
}

void PulseVoice::SetFrequencyHighByte(uint8_t byte) {
    eleven_bit_frequency_ &= 0xFF;
    eleven_bit_frequency_ |= ((byte & 0b111) << 8);
}

uint8_t PulseVoice::GetFrequencyHighByte()  {
    return eleven_bit_frequency_ >> 8;
}

bool PulseVoice::Advance(int cycles) {
	return true; // TODO.
}
