#include "pulse_voice.h"

#include <math.h>

#include <algorithm>
#include <cassert>
#include <iostream>

#include "constants.h"
#include "utils.h"


PulseVoice::PulseVoice() {
}

PulseVoice::~PulseVoice() { }

bool PulseVoice::Advance(int cycles) {
  return true;
}

int PulseFrequency(uint8_t low_byte, uint8_t high_byte) {
  uint16_t combined_frequency = high_byte & 0b111;
  combined_frequency <<= 8;
  combined_frequency |= low_byte;
  return 131072 / (2048 - combined_frequency);
}

int SweepTimeCycles(uint8_t sweep_byte) {
  uint8_t sweep_time = (sweep_byte & 0b1110000) >> 4;
  return sweep_time * SAMPLE_RATE / 8;
}

