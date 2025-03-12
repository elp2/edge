#include "sound_controller.h"

#include <cassert>
#include <iostream>

#include <SDL3/SDL.h>

#include "constants.h"
#include "noise_voice.h"
#include "pulse_voice.h"
#include "utils.h"
#include "wave_voice.h"


static void audioCallback(void* userdata, SDL_AudioStream* stream, int min_len, int max_len) {
    // We'll use min_len to ensure we provide at least the minimum requested samples
    int samples = min_len / sizeof(int16_t);
    int16_t* buffer = new int16_t[samples];
    
    for (int i = 0; i < samples; i++) {
        buffer[i] = 0;
    }

    static_cast<SoundController*>(userdata)->MixSamplesToBuffer(buffer, samples);
    SDL_PutAudioStreamData(stream, buffer, min_len); // Use min_len for the byte size
    delete[] buffer;
}

SoundController::SoundController() {
    voice1_ = new PulseVoice(1);
    voice2_ = new PulseVoice(2);
    voice3_ = new WaveVoice();
    voice4_ = new NoiseVoice();
    global_sound_on_ = true;

    SDL_AudioSpec spec;
    SDL_zero(spec);
    spec.freq = SAMPLE_RATE;
    spec.format = SDL_AUDIO_S16;
    spec.channels = 1;

    SDL_AudioStream* audio_stream = SDL_OpenAudioDeviceStream(
                                                              SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
        &spec,
        audioCallback,
        this
    );

    if (!audio_stream) {
        std::cerr << "Failed to open audio stream: " << SDL_GetError() << std::endl;
        return;
    }

    audio_stream_ = audio_stream;
    SDL_ResumeAudioDevice(SDL_GetAudioStreamDevice(audio_stream));
}

// Add destructor to clean up the audio stream
SoundController::~SoundController() {
    if (audio_stream_) {
        SDL_CloseAudioDevice(SDL_GetAudioStreamDevice(audio_stream_));
        audio_stream_ = nullptr;
    }
    delete voice1_;
    delete voice2_;
    delete voice3_;
    delete voice4_;
}

void SoundController::MixSamplesToBuffer(int16_t* buffer, int samples) {
    if (!global_sound_on_) {
        return;
    }
    if (ChannelLeftEnabled(0) || ChannelRightEnabled(0)) {
        voice1_->AddSamplesToBuffer(buffer, samples);
    }
    if (ChannelLeftEnabled(1) || ChannelRightEnabled(1)) {
        voice2_->AddSamplesToBuffer(buffer, samples);
    }
    if (ChannelLeftEnabled(2) || ChannelRightEnabled(2)) {
        voice3_->AddSamplesToBuffer(buffer, samples);
    }
    if (ChannelLeftEnabled(3) || ChannelRightEnabled(3)) {
        voice4_->AddSamplesToBuffer(buffer, samples);
    }
}

bool SoundController::Advance(int cycles) {
  return true;
}

void SoundController::SetByteAt(uint16_t address, uint8_t byte) {
  assert(address >= 0xFF10 && address <= 0xFF3F);

  if (address >= 0xFF30 && address <= 0xFF3F) {
    voice3_->SetWavePatternAddress(address, byte);
    return;
  }

  if (!global_sound_on_) {
    if (address < 0xFF26) {
      // Can not set registers when APU is off.
      return;
    }
  }

  switch (address) {
    case 0xFF10:
      voice1_->SetNRX0(byte);
      break;
    case 0xFF11:
      voice1_->SetNRX1(byte);
      break;
    case 0xFF12:
      voice1_->SetNRX2(byte);
      break;
    case 0xFF13:
      voice1_->SetNRX3(byte);
      break;
    case 0xFF14:
      voice1_->SetNRX4(byte);
      break;
    case 0xFF15:
      // No 0xFF15.
      break;
    case 0xFF16:
      voice2_->SetNRX1(byte);
      break;
    case 0xFF17:
      voice2_->SetNRX2(byte);
      break;
    case 0xFF18:
      voice2_->SetNRX3(byte);
      break;
    case 0xFF19:
      voice2_->SetNRX4(byte);
      break;
    case 0xFF1A:
      voice3_->SetNR30(byte);
      break;
    case 0xFF1B:
      voice3_->SetNR31(byte);
      break;
    case 0xFF1C:
      voice3_->SetNR32(byte);
      break;
    case 0xFF1D:
      voice3_->SetNR33(byte);
      break;
    case 0xFF1E:
      voice3_->SetNR34(byte);
      break;
    case 0xFF1F:
      // No 0xFF1F.
      break;
    case 0xFF20:
      voice4_->SetFF20(byte);
      break;
    case 0xFF21:
      voice4_->SetFF21(byte);
      break;
    case 0xFF22:
      voice4_->SetFF22(byte);
      break;
    case 0xFF23:
      voice4_->SetFF23(byte);
      break;
    case 0xFF24:
      channel_control_ = byte;
      break;
    case 0xFF25:
      sound_output_terminals_ = byte;
      break;
    case 0xFF26:
      SetFF26(byte);
      break;
    case 0xFF27:
    case 0xFF28:
    case 0xFF29:
    case 0xFF2A:
    case 0xFF2B:
    case 0xFF2C:
    case 0xFF2D:
    case 0xFF2E:
    case 0xFF2F:
      // 0xFF27-2F unused.
      break;

    // 0xFF30-3F Wave Pattern - covered above.

    default:
      std::cout << "Unknown SetByteAt: " << std::hex << (int)address << " to " << (int)byte << std::endl;
      assert(false);
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
      return voice1_->GetNRX0();
      break;
    case 0xFF11:
      return voice1_->GetNRX1();
      break;
    case 0xFF12:
      return voice1_->GetNRX2();
      break;
    case 0xFF13:
      return voice1_->GetNRX3();
      break;
    case 0xFF14:
      return voice1_->GetNRX4();
      break;
    case 0xFF15:
      // No Sweep NRX0 for Voice 2.
      return 0xFF;
      break;
    case 0xFF16:
      return voice2_->GetNRX1();
      break;
    case 0xFF17:
      return voice2_->GetNRX2();
      break;
    case 0xFF18:
      return voice2_->GetNRX3();
      break;
    case 0xFF19:
      return voice2_->GetNRX4();
      break;
    case 0xFF1A:
      return voice3_->GetNR30();
      break;
    case 0xFF1B:
      return voice3_->GetNR31();
      break;
    case 0xFF1C:
      return voice3_->GetNR32();
      break;
    case 0xFF1D:
      return voice3_->GetNR33();
      break;
    case 0xFF1E:
      return voice3_->GetNR34();
      break;
    case 0xFF1F:
      // Unused.
      return 0xFF;
      break;
    case 0xFF20:
      return voice4_->GetFF20();
      break;
    case 0xFF21:
      return voice4_->GetFF21();
      break;
    case 0xFF22:
      return voice4_->GetFF22();
      break;
    case 0xFF23:
      return voice4_->GetFF23();
      break;
    case 0xFF24:
      return channel_control_;
      break;
    case 0xFF25:
      return sound_output_terminals_;
      break;
    case 0xFF26:
      return GetFF26();
      break;
    case 0xFF27:
    case 0xFF28:
    case 0xFF29:
    case 0xFF2A:
    case 0xFF2B:
    case 0xFF2C:
    case 0xFF2D:
    case 0xFF2E:
    case 0xFF2F:
      // 0xFF27-2F unused.
      return 0xFF;
      break;
    // 0xFF30-3F Wave Pattern - covered above.
    default:
      break;
  }
  std::cout << "No value for GetByteAt: " << std::hex << " at " << address << std::endl;
  assert(false);
  return 0x00;
}

void SoundController::SetFF26(uint8_t byte) {
  bool new_global_sound_on = bit_set(byte, 7);
  // Other bytes are ignored.

  if (!new_global_sound_on) {
    // TODO Reset registers, but not wave RAM or NR52.

    uint16_t address = 0xFF10;
    while (address < 0xFF26) {
      SetByteAt(address, 0x00);
      address++;
    }    
  }

  global_sound_on_ = new_global_sound_on;
}

uint8_t SoundController::GetFF26() {
  uint8_t ff26 = global_sound_on_;
  ff26 <<= 4;
  ff26 |= (uint8_t)voice4_->Playing();
  ff26 <<= 1;
  ff26 |= (uint8_t)voice3_->Playing();
  ff26 <<= 1;
  ff26 |= (uint8_t)voice2_->Playing();
  ff26 <<= 1;
  ff26 |= (uint8_t)voice1_->Playing();
  return 0x70 | ff26;
}

bool SoundController::ChannelLeftEnabled(int channel) {
  return bit_set(sound_output_terminals_, channel);
}

bool SoundController::ChannelRightEnabled(int channel) {
  return bit_set(sound_output_terminals_, channel + 4);
}
