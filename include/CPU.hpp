#pragma once


#include <array>
#include <cstdint>
#include "AddressRouter.hpp"
#include "Command.hpp"
#include "CommandFactory.hpp"
#include "Destination.hpp"

using namespace std;

struct flags_t {
    bool z;
    bool n;
    bool h;
    bool c;
};

class AddressRouter;
class MMU;
class PPU;

class CPU {
 private:
    AddressRouter *addressRouter_;
    PPU *ppu_;
    CommandFactory *commandFactory_;
    CBCommandFactory *cbCommandFactory_;
    Command *CommandForOpcode(uint8_t opcode);

    bool interruptsEnabled_;

    uint8_t a_, b_, c_, d_, e_, f_, h_, l_;

    // Points to the next command to be executed.
    uint16_t pc_;

    // Points to the stack position.
    uint16_t sp_;

    bool disasemblerMode_;
    uint64_t cycles_;
    bool debugPrint_;

    bool haltNextLoop_;
    bool stopNextLoop_;
    bool disableInterruptsNextLoop_;
    bool enableInterruptsNextLoop_;
 public:
    flags_t flags;

    CPU(MMU *mmu, PPU *ppu);
    ~CPU();

    // Resets the CPU to base state.
    void Reset();

    // Prints debugger info.
    void Debugger();

    uint8_t ReadOpcodeAtPC();
    void AdvancePC();
    void Step();

    bool Requires16Bits(Destination d);
    uint8_t Get8Bit(Destination d);
    uint16_t Get16Bit(Destination d);

    void Set8Bit(Destination d, uint8_t value);
    void Set16Bit(Destination d, uint16_t value);

    void Push8Bit(uint8_t byte);
    void Push16Bit(uint16_t word);

    uint8_t Pop8Bit();
    uint16_t Pop16Bit();

    // Hacks to simulate a disassembler.
    void EnableDisassemblerMode();
    
    void JumpAddress(uint16_t address);
    void JumpRelative(uint8_t relative);

    void RegisterCommand(Command *command);

    // Special Actions.
    void HaltNextLoop() { haltNextLoop_ = true; };
    void StopNextLoop() { stopNextLoop_ = true; };
    void DisableInterruptsNextLoop() { disableInterruptsNextLoop_ = true; };
    void EnableInterruptsNextLoop() { enableInterruptsNextLoop_ = true; };

    void SetDebugPrint(bool debugPrint) { debugPrint_ = debugPrint; };

    uint64_t Cycles() { return cycles_; };
};