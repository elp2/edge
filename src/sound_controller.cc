#include "sound_controller.h"

#include <cassert>
#include <iostream>


SoundController::SoundController() {
	// TODO: Create voices.
}

bool SoundController::Advance(int cycles) {
	return true; // TODO.
}

void SoundController::SetByteAt(uint16_t address, uint8_t byte) {
    assert(address >= 0xFF10 && address <= 0xFF3F);

    if (address >= 0xFF30 && address <= 0xFF3F) {
        // TODO: Set Wave.
    }

    switch (address)
    {
        case 0xFF24:
            // TODO: Channel Control.
            break;
        case 0xFF25:
            sound_output_terminals_ = byte;
            break;
        case 0xFF26:
            global_sound_on_ = (bool) 0x1 & byte; // TODO ordering?
            break;

		// 0xFF27-2F unused.
        // 0xFF30-3F Wave Pattern - covered above.

    default:
        break;
    }
}

uint8_t SoundController::GetByteAt(uint16_t address) {
    assert(address >= 0xFF10 && address <= 0xFF3F);

    if (address >= 0xFF30 && address <= 0xFF3F) {
        // TODO: Get Wave.
        return 0x00;
    }

    return 0x00; // TODO
}
