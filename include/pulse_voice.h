#pragma once

#include <cstdint>

union SDL_Event;

class PulseVoice {
 public:
  PulseVoice();
  ~PulseVoice();

  // Returns true if the sound has finished playing.
  bool Advance(int cycles);

  // Returns true if there is a sound buffer to play at this moment.
  // Caller should NOT modify the sound buffer.
  bool PlaySound(float **sound_buffer, int *length);

  void SetSweepByte(uint8_t byte) { sweep_byte_ = byte; };
  uint8_t GetSweepByte() { return 0x80 | sweep_byte_; };

  void SetWavePatternDutyByte(uint8_t byte) { wave_pattern_duty_byte_ = byte; };
  uint8_t GetWavePatternDutyByte() {
    return 0x3F | wave_pattern_duty_byte_; // Only top two bits can be read.'
  };

  void SetEnvelopeByte(uint8_t byte) { envelope_byte_ = byte; };
  uint8_t GetEnvelopeByte() { return envelope_byte_; };

  void SetFrequencyLowByte(uint8_t byte) { frequency_low_byte_ = byte; };
  uint8_t GetFrequencyLowByte() { return 0xFF; /* Write only. */ };

  void SetFrequencyHighByte(uint8_t byte);
  uint8_t GetFrequencyHighByte() { return 0xBF | frequency_high_byte_; };

  bool Playing() { return playback_steps_remaining_ > 0; };

 private:
  // Length of the voice in CPU steps (CYCLES_PER_SECOND).
  int length_steps_ = 0;
  // Length of the Sweep in CPU steps (CYCLES_PER_SECOND).
  int sweep_steps_ = 0;

  float *sound_buffer_;
  // Length of the internal buffer which we should play. May be less than real
  // size if we're playing a shorter sound.
  int buffer_length_ = 0;

  bool initial_ = false;
  bool loop_ = false;
  int playback_steps_remaining_ = 0;

  // Frequency bytes which are combined when we generate the sound.
  uint8_t frequency_low_byte_ = 0;
  uint8_t frequency_high_byte_ = 0;

  uint8_t envelope_byte_ = 0;
  uint8_t sweep_byte_ = 0;
  uint8_t wave_pattern_duty_byte_ = 0;

  void GenerateSoundBuffer();
};
