#include "interrupt_controller.hpp"

#include <cassert>
#include <iostream>
#include "SDL.h"

#include "input_controller.h"

const int INTERRUPTS_ENABLE_DISABLE_LOOPS = 2;

const uint16_t IF_ADDRESS = 0xFF0F;
const uint16_t IE_ADDRESS = 0xFFFF;

InterruptController::InterruptController() {
}

void InterruptController::RequestInterrupt(Interrupt interrupt) {
	std::cout << "Requesting Interrupt: " << std::hex << unsigned(interrupt) << std::endl;
	is_halted_ = false;

    interrupt_request_ |= interrupt;

}

int InterruptController::HandleInterruptRequest() {
	if (!interrupts_enabed_) {
		return 0;
	}

	uint8_t handleable_interrupts = interrupt_request() & interrupt_enabled_flags();

	uint8_t rst_pc = 0x00;
	if (handleable_interrupts & Interrupt_VBlank) {
		rst_pc = 0x40;
		interrupt_request_ &= ~(Interrupt_VBlank);
	}
	else if (handleable_interrupts & Interrupt_LCDC) {
		rst_pc = 0x48;
		interrupt_request_ &= ~(Interrupt_LCDC);
	}
	else if (handleable_interrupts & Interrupt_TimerOverflow) {
		rst_pc = 0x50;
		interrupt_request_ &= ~(Interrupt_TimerOverflow);
	}
	else if (handleable_interrupts & Interrupt_SerialTransferCompletion) {
		rst_pc = 0x58;
		interrupt_request_ &= ~(Interrupt_SerialTransferCompletion);
	}
	else if (handleable_interrupts & Interrupt_Input) {
		rst_pc = 0x60;
		interrupt_request_ &= ~(Interrupt_Input);
	}

	if (rst_pc == 0) {
		return 0;
	}

	assert(rst_pc != 0x00);
	std::cout << "Resetting to pc: " << std::hex << unsigned(rst_pc) << std::endl;
	interrupts_enabed_ = false; // Must be re-enabled during the event loop.
	executor_->InterruptToPC(rst_pc);
	return 0;
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
    switch (address) {
		case IF_ADDRESS:
			set_interrupt_request(byte);
			std::cout << "Write IF" << std::hex << unsigned(interrupt_request()) << std::endl;
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
    switch (address) {
		case IF_ADDRESS:
			std::cout << "GET IF" << std::endl;
			return interrupt_request();
		case IE_ADDRESS:
			return interrupt_enabled_flags();
		default:
			assert(false);
			return 0x00;
    }
}

void InterruptController::HaltUntilInterrupt() {
	is_halted_ = true;
}
