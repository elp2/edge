#pragma once


#include <array>
#include <cstdint>
#include "Command.hpp"
#include "Destination.hpp"
#include "MMU.hpp"

using namespace std;

struct flags_t {
    bool z;
    bool n;
    bool h;
    bool c;
};

class CPU {
    public:

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

    bool Requires16Bits(Destination d);
    uint8_t Get8Bit(Destination d);
    uint16_t Read16Bit(Destination d);

    void Set8Bit(Destination d, uint8_t value);
    void Set16Bit(Destination d, uint16_t value);

    void Push8Bit(uint8_t byte);
    void Push16Bit(uint16_t word);

    uint8_t Pop8Bit();
    uint16_t Pop16Bit();

    void JumpAddress(uint16_t address);
    void JumpRelative(uint8_t relative);

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
    array<Command *, 256> commands;

    void RegisterCommands();
    bool interruptsEnabled;

    uint8_t a,b,c,d,e,f,h,l;

    // Points to the next command to be executed.
    uint16_t pc;

    // Points to the stack position.
    uint16_t sp;

};