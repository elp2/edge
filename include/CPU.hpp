#pragma once

#include <cstdint>
#include "MMU.hpp"

using namespace std;

struct flags_t {
    bool z;
    bool n;
    bool h;
    bool c;
};

enum Register {Register_A, Register_B, Register_C, Register_D, Register_E, Register_F, Register_H, Register_L, Register_BC, Register_DE, Register_HL, Register_SP};

class CPU {
    public:

    // Points to the next command to be executed.
    uint16_t pc;

    // Points to the stack position.
    uint16_t sp;

    uint8_t a;

    flags_t flags;

    CPU(MMU mmu);
    ~CPU();

    // Resets the CPU to base state.
    void Reset();

    // Prints debugger info.
    void Debugger();

    // 8 bit loads.

    // 16 bit loads.

    // --push/pop count as 16 bit loads.

    // 8 bit arithmetic

    // 16 bit arithmetic

    // Misc
        // Swap nibbles.
private:
    MMU mmu;
};