#pragma once

#include <cstdint>
#include "MMU.hpp"

using namespace std;

class Command;

enum Destination {Register_A, Register_B, Register_C, Register_D, Register_E, Register_F, Register_H, Register_L, Register_BC, Register_DE, Register_HL, Register_SP, Register_PC, Eat_PC_Byte, Eat_PC_Word};

struct flags_t {
    bool z;
    bool n;
    bool h;
    bool c;
};

class CPU {
    public:

    uint8_t a,b,c,d,e,f,h,l;

    // uint16_t ReadHL();

    // Points to the next command to be executed.
    uint16_t pc;

    // Points to the stack position.
    uint16_t sp;

    flags_t flags;

    CPU(MMU mmu);
    ~CPU();

    Command *CommandForOpcode(uint8_t opcode);

    // Resets the CPU to base state.
    void Reset();

    // Prints debugger info.
    void Debugger();

    uint8_t ReadOpcodeAtPC();
    void AdvancePC();
    void Step();

    void StackDelta(int delta);


    bool Requires16Bits(Destination d);
    uint8_t Read8Bit(Destination d);
    uint16_t Read16Bit(Destination d);

    void Set8Bit(Destination d, uint8_t value);
    void Set16Bit(Destination d, uint16_t value);

    void RegisterCommand(Command *command);

    // Special Actions.
    bool haltRequested;
    bool stopRequested;
    bool disableInterruptsRequested;
    bool enableInterruptsRequested;

    // Checks the state of interrupts.
    bool InterruptsEnabled();

private:
    MMU mmu;
    Command *opcodes[256];

    void RegisterCommands();
    bool interruptsEnabled;
};