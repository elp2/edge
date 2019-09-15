#include "timer_controller.h"

#include <cassert>
#include <iostream>

const int CYCLES_PER_SECOND = 1048576;
const int CYCLES_TO_DIV = CYCLES_PER_SECOND / 256;
const int CYCLES_FOR_4KHZ = CYCLES_PER_SECOND / 4096;
const int CYCLES_FOR_262KHZ = CYCLES_PER_SECOND / 262144;
const int CYCLES_FOR_65KHZ = CYCLES_PER_SECOND / 65536;
const int CYCLES_FOR_16KHZ = CYCLES_PER_SECOND / 16384;

TimerController::TimerController() {
}

void TimerController::SetByteAt(uint16_t address, uint8_t byte) {
    switch (address) {
    case 0xFF04:
        // Ignores the value being set and sets zero.
        div_counter_ = 0;
        break;
    case 0xFF05:
        count_ = byte;
        break;
    case 0xFF06:
        modulo_ = byte;
        break;
    case 0XFF07: {
        active_ = byte &0x04;        
        switch (byte & 0x03) {
            case 0x00:
                cycles_per_count_ = CYCLES_FOR_4KHZ;
                break;
            case 0x01:
                cycles_per_count_ = CYCLES_FOR_262KHZ;
                break;
            case 0x02:
                cycles_per_count_ = CYCLES_FOR_65KHZ;
                break;
            case 0x03:
                cycles_per_count_ = CYCLES_FOR_16KHZ;
                break;
            default:
                break;
        }
        break;
    }
    default:
        assert(false);
        break;
    }
}

uint8_t TimerController::GetByteAt(uint16_t address) {
    uint8_t ret = 0x00;
    switch (address) {
        case 0xFF04:
            return (div_counter_ >> 11) & 0xFF;
        case 0xFF05:
            return (uint8_t)count_; // TODO.
        case 0xFF06:
            return modulo_;
        case 0XFF07: {
            ret = active_ ? 0x04 : 0x00;
            switch (cycles_per_count_) {
                case CYCLES_FOR_4KHZ:
                    ret = ret | 0x00;
                    break;
                case CYCLES_FOR_262KHZ:
                    ret = ret | 0x01;
                    break;
                case CYCLES_FOR_65KHZ:
                    ret = ret | 0x02;
                    break;
                case CYCLES_FOR_16KHZ:
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
}
