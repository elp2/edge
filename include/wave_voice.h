#pragma once

#include <cstdint>

union SDL_Event;

class WaveVoice {
 public:
  WaveVoice();
  ~WaveVoice();

  // Returns true if the sound has finished playing.
  bool Advance(int cycles);

  // Returns true if there is a sound buffer to play at this moment.
  // Caller should NOT modify the sound buffer.
  bool PlaySound(float **sound_buffer, int *length);

  void SetOnOffByte(uint8_t byte);
  uint8_t GetOnOffByte() { return 0x7F | on_off_byte_; };

  void SetSoundLengthByte(uint8_t byte) { sound_length_byte_ = byte; };
  uint8_t GetSoundLengthByte() { return sound_length_byte_; };

  void SetOutputLevelByte(uint8_t byte) { output_level_byte_ = byte; };
  uint8_t GetOutputLevelByte() { return 0x9F | output_level_byte_; };

  void SetFrequencyLowByte(uint8_t byte) { frequency_low_byte_ = byte; };
  uint8_t GetFrequencyLowByte() { return frequency_low_byte_; };

  // Will also potentially make this voice ready for playback if "initial" is
  // set.
  void SetFrequencyHighByte(uint8_t byte);
  uint8_t GetFrequencyHighByte() { return frequency_high_byte_ & 0b01000000; };

  bool Playing() { return playback_steps_remaining_ > 0; };

  static const uint16_t BASE_WAVE_PATTERN_ADDRESS;

  void SetWavePatternAddress(uint16_t address, uint8_t byte);
  uint8_t GetWavePatternByte(uint16_t address);

 private:
  // Length of the voice in CPU steps (CYCLES_PER_SECOND).
  int length_steps_ = 0;

  // Pre-calculated sounds based on current state at play start.
  float *sound_buffer_;
  // Length of the internal buffer which we should play. May be less than real
  // size if we're playing a shorter sound.
  int buffer_length_ = 0;

  bool initial_ = false;
  bool loop_ = false;
  int playback_steps_remaining_ = 0;

  uint8_t on_off_byte_ = 0;
  uint8_t output_level_byte_ = 0;
  uint8_t sound_length_byte_ = 0;

  // Frequency bytes which are combined when we generate the sound.
  uint8_t frequency_low_byte_ = 0;
  uint8_t frequency_high_byte_ = 0;

  uint8_t wave_pattern_[16];

  void GenerateSoundBuffer();
};
