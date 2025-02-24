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
}

NoiseVoice::~NoiseVoice() {}
