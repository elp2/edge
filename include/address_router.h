#pragma once

#include <cstdint>

using namespace std;

class InputController;
class InterruptController;
class MMU;
class PPU;
class SerialController;
class SoundController;
class TimerController;

struct DeviceMemorySaveState;

enum AddressOwner {
  AdressOwner_Unknown = 0,
  AddressOwner_MMU,
  AddressOwner_PPU,
  AddressOwner_Serial,
  AddressOwner_Interrupt,
  AddressOwner_Input,
  AddressOwner_DMA,
  AddressOwner_Timer,
  AddressOwner_Sound,
};

// Redirects reads and writes to the MMU, PPU, Sound, and potentially other
// controllers.
class AddressRouter {
 public:
  AddressRouter(MMU* mmu, PPU* ppu, SerialController* serial_controller,
                InterruptController* interrupt_controller,
                InputController* input_controller,
                TimerController* timer_controller,
                SoundController* sound_controller);
  ~AddressRouter() = default;

  uint8_t GetByteAt(uint16_t address);
  void SetByteAt(uint16_t address, uint8_t byte);

  uint16_t GetWordAt(uint16_t address);
  void SetWordAt(uint16_t address, uint16_t word);

  void EnableDisassemblerMode(bool disassemblerMode);

  void SaveState(struct DeviceMemorySaveState &state);
  void LoadState(const struct DeviceMemorySaveState &state);
  void SkipBootROM();
 private:
  MMU* mmu_;
  PPU* ppu_;
  InputController* input_controller_;
  InterruptController* interrupt_controller_;
  SerialController* serial_controller_;
  SoundController* sound_controller_;
  TimerController* timer_controller_;

  bool disassemblerMode_ = false;
  uint8_t dma_base_ = 0x00;

  uint8_t GetByteAtAddressFromOwner(AddressOwner owner, uint16_t address);
  void SetByteAtAddressInOwner(AddressOwner owner, uint16_t address,
                               uint8_t byte);
  void PerformDMA(uint8_t dma_base);
  bool ShouldSaveLoadAddress(uint16_t address);
};
