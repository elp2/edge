#include "interrupt_controller.hpp"

#include <cassert>

InterruptController::InterruptController() {

}

void InterruptController::GenerateInterrupt(Interrupt interrupt) {
    if (!interrupts_enabed_) {
        return;
    }
    if (!(interrupt_enabled_flags_ & interrupt)) {
        return;
    }

    interrupt_request_ |= interrupt;

    uint8_t rst_pc = 0x00;
    switch (interrupt) {
        case Interrupt_VBlank:
            rst_pc = 0x40;
            break;
        case Interrupt_LCDC:
            rst_pc = 0x48;
            break;
        case Interrupt_TimerOverflow:
            rst_pc = 0x50;
            break;
        case Interrupt_SerialTransferCompletion:
            rst_pc = 0x58;
            break;
        case Interrupt_Keypad:
            rst_pc = 0x60;
            break;        
        default:
            assert(false);
            break;
    }
    interrupts_enabed_ = false; // Must be re-enabled during the event loop.
    executor_->InterruptToPC(rst_pc);
}
