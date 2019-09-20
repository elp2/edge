#include "interrupt_controller.hpp"

#include <cassert>
#include "SDL.h"

#include "input_controller.h"

const int INTERRUPTS_ENABLE_DISABLE_LOOPS = 2;

const uint16_t IF_ADDRESS = 0xFF0F;
const uint16_t IE_ADDRESS = 0xFFFF;

InterruptController::InterruptController() {

}

void InterruptController::HandleInterrupt(Interrupt interrupt) {
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
        case Interrupt_Input:
            rst_pc = 0x60;
            break;        
        default:
            assert(false);
            break;
    }
    interrupts_enabed_ = false; // Must be re-enabled during the event loop.
    executor_->InterruptToPC(rst_pc);
}

void InterruptController::Advance(int cycles) {
    if (disable_interrupts_in_loops_ > 0) {
        if (disable_interrupts_in_loops_ == 1) {
            interrupts_enabed_ = false;
        }
        disable_interrupts_in_loops_--;
    }
    if (enable_interrupts_in_loops_ > 0) {
        if (enable_interrupts_in_loops_ == 1) {
            interrupts_enabed_ = true;
        }
        enable_interrupts_in_loops_--;
    }
}

void InterruptController::DisableInterrupts() {
    disable_interrupts_in_loops_ = INTERRUPTS_ENABLE_DISABLE_LOOPS;
    enable_interrupts_in_loops_ = 0;
}

void InterruptController::EnableInterrupts() {
    enable_interrupts_in_loops_ = INTERRUPTS_ENABLE_DISABLE_LOOPS;
    disable_interrupts_in_loops_ = 0;
}

void InterruptController::SetByteAt(uint16_t address, uint8_t byte) {
    switch (address)
    {
    case IF_ADDRESS:
        set_interrupt_request(byte);
        break;
    case IE_ADDRESS:
        set_interrupt_enabled_flags(byte);
        break;
    default:
        assert(false);
        break;
    }
}

uint8_t InterruptController::GetByteAt(uint16_t address) {
    switch (address)
    {
    case IF_ADDRESS:
        return interrupt_request();
    case IE_ADDRESS:
        return interrupt_enabled_flags();
    default:
        assert(false);
		return 0x00;
    }
}

void InterruptController::HaltUntilInput() {
	if (!interrupts_enabed_) {
		return;
	}
	while (!input_controller_->PollAndApplyEvents()) {
		SDL_Delay(5);
	}
}
