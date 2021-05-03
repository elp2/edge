#include "sound_controller.h"

#include <cassert>
#include <iostream>

#include "SDL.h"

#include "constants.h"
#include "pulse_voice.h"
#include "Utils.hpp"

SoundController::SoundController() {
    voice1_ = new PulseVoice();
    voice2_ = new PulseVoice(); // We will not set the sweep on this voice.
	// TODO: Create voices.

    SDL_AudioSpec wanted;
    wanted.freq = SAMPLE_RATE;
    wanted.format = AUDIO_F32;
    wanted.channels = 1; // TODO 2 (eventually)
    wanted.samples = 4096;
	wanted.callback = NULL;

    SDL_AudioSpec actual;
	audio_device_ = SDL_OpenAudioDevice(NULL, 0, &wanted, &actual, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
    // TODO check SAMPLE RATE, what is .samples? assert(wanted == actual);

    SDL_PauseAudioDevice(audio_device_, 0);
}

bool SoundController::Advance(int cycles) {
	if (voice1_->Advance(cycles)) {
		// TODO reset.
	}
	if (voice2_->Advance(cycles)) {
		// TODO reset.
	}
	// TODO advance.
	// voice3_->Advance(cycles);
	// voice4_->Advance(cycles);

	if (!global_sound_on_) {
		return true;
	}

    float *sound_buffer;
    int length;
    if (voice1_->PlaySound(&sound_buffer, &length)) {
		bool s01 = bit_set(sound_output_terminals_, 0);
		bool s02 = bit_set(sound_output_terminals_, 4);
		if (s01 || s02) {
			SDL_QueueAudio(audio_device_, sound_buffer, length * sizeof(float));
		}
	}

    // TODO: Play other voices.

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
            global_sound_on_ = bit_set(byte, 7);
			// Other bytes are ignored.
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
			return sound_output_terminals_;
            break;
        case 0xFF26:
			return FF26();
            break;
		// 0xFF27-2F unused.
        // 0xFF30-3F Wave Pattern - covered above.
    default:
        return 0x00;
        break;
    }
	return 0x00;
}

uint8_t SoundController::FF26() {
	uint8_t ff26 = global_sound_on_;
	ff26 <<= 4;
	ff26 |= (uint8_t)voice1_->Playing(); // TODO voice4.
	ff26 <<= 1;
	ff26 |= (uint8_t)voice1_->Playing(); // TODO voice3.
	ff26 <<= 1;
	ff26 |= (uint8_t)voice2_->Playing();
	ff26 <<= 1;
	ff26 |= (uint8_t)voice1_->Playing();
	return ff26;
}