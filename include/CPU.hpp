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
    CommandFactory *commandFactory_;
    CBCommandFactory *cbCommandFactory_;
    Command *CommandForOpcode(uint8_t opcode);

    bool interruptsEnabled_ = false;

    uint8_t a_, b_, c_, d_, e_, h_, l_ = 0;

    // Points to the next command to be executed.
    uint16_t pc_ = 0;

    // Points to the stack position.
    uint16_t sp_ = 0;

    bool disasemblerMode_ = false;
    uint32_t cycles_ = 0;
	bool debugPrint_ = false;

    bool haltNextLoop_ = false;
    bool stopNextLoop_ = false;
    bool disableInterruptsNextLoop_ = false;
    bool enableInterruptsNextLoop_ = false;
 public:
    flags_t flags;

    CPU(MMU *mmu, PPU *ppu);
    ~CPU();

    // Resets the CPU to base state.
    void Reset();

    // Prints debugger info.
    void Debugger();

    uint32_t cycles() { return cycles_; };

    uint8_t ReadOpcodeAtPC();
    void AdvancePC();
    int Step();

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