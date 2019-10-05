#include "timer_controller.h"

#include <cassert>
#include <iostream>

#include "interrupt_controller.hpp"

using std::cout;
using std::hex;
using std::endl;

const int CYCLES_PER_SECOND = 4194304;
const int STEPS_FOR_4KHZ = CYCLES_PER_SECOND / 4096;
const int STEPS_FOR_262KHZ = CYCLES_PER_SECOND / 262144;
const int STEPS_FOR_65KHZ = CYCLES_PER_SECOND / 65536;
const int STEPS_FOR_16KHZ = CYCLES_PER_SECOND / 16384;

TimerController::TimerController() {
    advance_per_cycle_ = STEPS_FOR_4KHZ;
    advanced_ = advance_per_cycle_;
}

void TimerController::SetByteAt(uint16_t address, uint8_t byte) {
    switch (address) {
    case 0xFF04:
        // Ignores the value being set and sets zero.
        div_counter_ = 0;
        break;
    case 0xFF05:
        tima_ = byte;
        break;
    case 0xFF06:
        modulo_ = byte;
        break;
    case 0XFF07: {
        active_ = byte & 0x04;      
        switch (byte & 0x03) {
            case 0x00:
                advance_per_cycle_ = STEPS_FOR_4KHZ;
                break;
            case 0x01:
                advance_per_cycle_ = STEPS_FOR_262KHZ;
                break;
            case 0x02:
                advance_per_cycle_ = STEPS_FOR_65KHZ;
                break;
            case 0x03:
                advance_per_cycle_ = STEPS_FOR_16KHZ;
                break;
            default:
                break;
        }
        advanced_ = advance_per_cycle_;
        break;
    }
    default:
        assert(false);
        break;
    }
	// cout << "Timer updated  " << hex << unsigned(address) << " = " << hex << unsigned(byte);
	// cout << " Active: " << active_ << " TIMA : " << hex << unsigned(tima_);
	// cout << " div:" << hex << unsigned(div_counter_) << " APC: " << advance_per_cycle_ << endl;
}

uint8_t TimerController::GetByteAt(uint16_t address) {
    uint8_t ret = 0x00;
    switch (address) {
        case 0xFF04:
            return (div_counter_ >> 8) & 0xFF;
        case 0xFF05:
            return tima_;
        case 0xFF06:
            return modulo_;
        case 0XFF07: {
            ret = active_ ? 0x04 : 0x00;
            switch (advance_per_cycle_) {
                case STEPS_FOR_4KHZ:
                    ret = ret | 0x00;
                    break;
                case STEPS_FOR_262KHZ:
                    ret = ret | 0x01;
                    break;
                case STEPS_FOR_65KHZ:
                    ret = ret | 0x02;
                    break;
                case STEPS_FOR_16KHZ:
                    ret = ret | 0x03;
                    break;
            }
            return ret;
        }
        default:
            assert(false);
            return 0x00;
        }
}

void TimerController::Advance(int cycles) {
    // Overflow is ignored.
    div_counter_ += cycles;

    if (!active_) {
        return;
    }
    int ticks = 0;
    // We will scale appropriately on fetch.
    while (cycles > advance_per_cycle_) {
		ticks++;
        cycles -= advance_per_cycle_;
    }
    advanced_ -= cycles;
    if (advanced_ <= 0) {
        advanced_ += advance_per_cycle_;
		ticks++;
    }
    if (!active_) {
        return;
    }

    while (ticks) {
        tima_++;
        if (tima_ == 0x00) {
            interrupt_handler_->RequestInterrupt(Interrupt_TimerOverflow);
            tima_ = modulo_;
        }
        --ticks;
    }
}

void TimerController::Debugger() {
    cout << "DIV: " << hex << unsigned(GetByteAt(0xFF04)) << endl;
    cout << "TIMA: " << hex << unsigned(tima_) << endl;
}
