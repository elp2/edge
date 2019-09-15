#pragma once

#include <cstdint>

enum Interrupt : uint8_t {
    Interrupt_VBlank = 0x1,
    Interrupt_LCDC = 0x02,
    Interrupt_TimerOverflow = 0x04,
    Interrupt_SerialTransferCompletion = 0x08,
    Interrupt_Keypad = 0x10,
};

class InterruptExecutor {
 public:
    virtual ~InterruptExecutor() = default;
    // Need to define this to fix:
    // "missing vtable usually means the first non-inline virtual member function has no definition" error.
    virtual void InterruptToPC(uint8_t pc) = 0;
};

class InterruptHandler {
 public:
    virtual void GenerateInterrupt(Interrupt interrupt) = 0; // TODO Rename to "HANDLE"!
};

// TODO: Interrupt Priority ordering 1-5.
class InterruptController : public InterruptHandler {
 public:
    InterruptController();
    ~InterruptController() = default;

    void GenerateInterrupt(Interrupt interrupt);

    void set_executor(InterruptExecutor *executor) { executor_ = executor; };
    void set_interrupt_request(uint8_t value) { interrupt_request_ = value; };
    void set_interrupt_enabled_flags(uint8_t value) { interrupt_enabled_flags_ = value; };
    uint8_t interrupt_request() { return interrupt_request_; };
    uint8_t interrupt_enabled_flags() { return interrupt_enabled_flags_; };

    void Advance(int cycles);
    void InterruptToPC(uint8_t pc);

    void SetByteAt(uint16_t address, uint8_t byte);
    uint8_t GetByteAt(uint16_t address);

    void DisableInterrupts();
    void EnableInterrupts();

    // Only from tests.
    void set_interrupts_enabled(bool enabled) { interrupts_enabed_ = enabled; };
    bool interrupts_enabled() { return interrupts_enabed_; };

 private:
    InterruptExecutor *executor_;
    bool interrupts_enabed_ = false;

    uint8_t interrupt_request_ = 0x00;
    uint8_t interrupt_enabled_flags_ = 0x00;

    int disable_interrupts_in_loops_ = 0;
    int enable_interrupts_in_loops_ = 0;    
};
