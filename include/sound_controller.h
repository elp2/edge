#pragma once

#include <cstdint>

class NoiseVoice;
class PulseVoice;
class SDL_AudioStream;
class WaveVoice;

class SoundController {
 public:
  SoundController();
  ~SoundController();

  bool Advance(int cycles);

  void SetByteAt(uint16_t address, uint8_t byte);
  uint8_t GetByteAt(uint16_t);

  int16_t GetSample();
  void MixSamplesToBuffer(int16_t* buffer, int samples);

 private:
  static const int SAMPLE_BUFFER_SIZE = 8;
  int16_t sample_buffer_[SAMPLE_BUFFER_SIZE];
  int buffer_pos_ = 0;

  uint8_t sound_output_terminals_ = 0;
  bool global_sound_on_ = 0;
  uint8_t s01_volume_level_ = 0;
  uint8_t s02_volume_level_ = 0;
  uint8_t channel_control_ = 0;
  int cycles_;

  PulseVoice *voice1_;
  PulseVoice *voice2_;
  WaveVoice *voice3_;
  NoiseVoice *voice4_;

  SDL_AudioStream* audio_stream_;

  bool ChannelLeftEnabled(int channel);
  bool ChannelRightEnabled(int channel);

  void SetFF26(uint8_t byte);
  uint8_t GetFF26();
};
