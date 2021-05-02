#include "sound_controller.h"

#include <cassert>
#include <iostream>

#include "pulse_voice.h"

SoundController::SoundController() {
    voice1_ = new PulseVoice();
    voice2_ = new PulseVoice(); // We will not set the sweep on this voice.
	// TODO: Create voices.
}

bool SoundController::Advance(int cycles) {
	voice1_->Advance(cycles);
	voice2_->Advance(cycles);
	// TODO advance.
	// voice3_->Advance(cycles);
	// voice4_->Advance(cycles);
	return true;
}

void SoundController::SetByteAt(uint16_t address, uint8_t byte) {
    assert(address >= 0xFF10 && address <= 0xFF3F);

    if (address >= 0xFF30 && address <= 0xFF3F) {
        // TODO: Set Wave.
    }

    switch (address)
    {
        case 0xFF10:
            voice1_->SetSweepByte(byte);
            break;
        case 0xFF11:
            voice1_->SetWavePatternDutyByte(byte);
            break;
        case 0xFF12:
            voice1_->SetEnvelopeByte(byte);
            break;
        case 0xFF13:
            voice1_->SetFrequencyLowByte(byte);
            break;
        case 0xFF14:
            voice1_->SetFrequencyHighByte(byte);
            break;
        // No 0xFF15.
        case 0xFF16:
            voice2_->SetWavePatternDutyByte(byte);
            break;
        case 0xFF17:
            voice2_->SetEnvelopeByte(byte);
            break;
        case 0xFF18:
            voice2_->SetFrequencyLowByte(byte);
            break;
        case 0xFF19:
            voice2_->SetFrequencyHighByte(byte);
            break;            
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

    switch (address)
    {
        case 0xFF10:
            return voice1_->GetSweepByte();
            break;
        case 0xFF11:
			return voice1_->GetWavePatternDutyByte();
            break;
        case 0xFF12:
			return voice1_->GetEnvelopeByte();
            break;
        case 0xFF13:
			return voice1_->GetFrequencyLowByte();
            break;
        case 0xFF14:
			return voice1_->GetFrequencyHighByte();
            break;
        // No 0xFF15.
        case 0xFF16:
			return voice2_->GetWavePatternDutyByte();
            break;
        case 0xFF17:
			return voice2_->GetEnvelopeByte();
            break;
        case 0xFF18:
			return voice2_->GetFrequencyLowByte();
            break;
        case 0xFF19:
			return voice2_->GetFrequencyHighByte();
            break;            
        case 0xFF24:
            // TODO: Channel Control.
            break;
        case 0xFF25:
			// TODO.
            break;
        case 0xFF26:
			// TODO.
            break;
		// 0xFF27-2F unused.
        // 0xFF30-3F Wave Pattern - covered above.
    default:
        return 0x00;
        break;
    }
}
