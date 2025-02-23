#include "sound_controller.h"

#include <cassert>
#include <iostream>

#include "SDL.h"
#include "constants.h"
#include "pulse_voice.h"
#include "utils.h"
#include "wave_voice.h"

SoundController::SoundController() {
  voice1_ = new PulseVoice();
  voice2_ = new PulseVoice();  // We will not set the sweep on this voice.
  voice3_ = new WaveVoice();
  // TODO: Create voices.

  SDL_AudioSpec wanted;
  wanted.freq = SAMPLE_RATE;
  wanted.format = AUDIO_F32;
  wanted.channels = 1;  // TODO 2 (eventually)
  wanted.samples = 4096;
  wanted.callback = NULL;

  SDL_AudioSpec actual;
  audio_device_ = SDL_OpenAudioDevice(NULL, 0, &wanted, &actual,
                                      SDL_AUDIO_ALLOW_FORMAT_CHANGE);
  assert(wanted.samples == actual.samples);

  SDL_PauseAudioDevice(audio_device_, 0);
}

bool SoundController::Advance(int cycles) {
  if (voice1_->Advance(cycles)) {
    // TODO reset.
  }
  if (voice2_->Advance(cycles)) {
    // TODO reset.
  }
  if (voice3_->Advance(cycles)) {
    // TODO reset.
  }

  // TODO voice4_->Advance(cycles);

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

  if (voice2_->PlaySound(&sound_buffer, &length)) {
    bool s01 = bit_set(sound_output_terminals_, 1);
    bool s02 = bit_set(sound_output_terminals_, 5);
    if (s01 || s02) {
      SDL_QueueAudio(audio_device_, sound_buffer, length * sizeof(float));
    }
  }

  if (voice3_->PlaySound(&sound_buffer, &length)) {
    bool s01 = bit_set(sound_output_terminals_, 2);
    bool s02 = bit_set(sound_output_terminals_, 6);
    if (s01 || s02) {
      SDL_QueueAudio(audio_device_, sound_buffer, length * sizeof(float));
    }
  }

  // TODO: Play voice4.

  return true;
}

void SoundController::SetByteAt(uint16_t address, uint8_t byte) {
  assert(address >= 0xFF10 && address <= 0xFF3F);

  if (address >= 0xFF30 && address <= 0xFF3F) {
    voice3_->SetWavePatternAddress(address, byte);
    return;
  }

  switch (address) {
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
    case 0xFF1A:
      voice3_->SetOnOffByte(byte);
      break;
    case 0xFF1B:
      voice3_->SetSoundLengthByte(byte);
      break;
    case 0xFF1C:
      voice3_->SetOutputLevelByte(byte);
      break;
    case 0xFF1D:
      voice3_->SetFrequencyLowByte(byte);
      break;
    case 0xFF1E:
      voice3_->SetFrequencyHighByte(byte);
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
    return voice3_->GetWavePatternByte(address);
  }

  switch (address) {
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
    case 0xFF15:
      // No 0xFF15. Return all high.
      return 0xFF;
      break;
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
    case 0xFF1A:
      return voice3_->GetOnOffByte();
      break;
    case 0xFF1B:
      return voice3_->GetSoundLengthByte();
      break;
    case 0xFF1C:
      return voice3_->GetOutputLevelByte();
      break;
    case 0xFF1D:
      return voice3_->GetFrequencyLowByte();
      break;
    case 0xFF1E:
      return voice3_->GetFrequencyHighByte();
      break;
    case 0xFF1F:
      // Unused.
      return 0;
      break;
    case 0xFF20:
      // tODO.
      return 0;
      break;
    case 0xFF21:
      // tODO.
      return 0;
      break;
    case 0xFF22:
      // tODO.
      return 0;
      break;
    case 0xFF23:
      // tODO.
      return 0;
      break;
    case 0xFF24:
      assert(false);
    case 0xFF25:
      return sound_output_terminals_;
      break;
    case 0xFF26:
      return FF26();
      break;
      // 0xFF27-2F unused.
      // 0xFF30-3F Wave Pattern - covered above.
    default:
      break;
  }
  std::cout << "No value for GetByteAt: " << std::hex << " at " << address << std::endl;
  assert(false);
  return 0x00;
}

uint8_t SoundController::FF26() {
  uint8_t ff26 = global_sound_on_;
  ff26 <<= 4;
  ff26 |= (uint8_t)voice1_->Playing();  // TODO voice4.
  ff26 <<= 1;
  ff26 |= (uint8_t)voice1_->Playing();  // TODO voice3.
  ff26 <<= 1;
  ff26 |= (uint8_t)voice2_->Playing();
  ff26 <<= 1;
  ff26 |= (uint8_t)voice1_->Playing();
  return 0x70 |ff26;
}