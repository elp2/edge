#pragma once

#include <cstdint>

using namespace std;

enum Register {Register_B, Register_C, Register_D, Register_E, Register_H, Register_L, Register_BC, Register_DE, Register_HL, Register_SP};

class CPU {
    public:

    uint16_t pc;

    // Resets the CPU to base state.
    void reset();

};