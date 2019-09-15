#pragma once

#include <cstdint>

class InterruptHandler;

class TimerController {
 public:
    TimerController();
    ~TimerController() = default;

    void SetByteAt(uint16_t address, uint8_t byte);
    uint8_t GetByteAt(uint16_t address);

    void Advance(int cycles);

    void SetInterruptHandler(InterruptHandler *handler) { interrupt_handler_ = handler; };

 private:
    InterruptHandler *interrupt_handler_;
    // TIMA.
    int count_ = 0;

    // TMA.
    int modulo_ = 0; 

    // TAC.
    bool active_ = false;
    int cycles_per_count_ = 0;

    // DIV.
    uint16_t div_counter_ = 0;
};
