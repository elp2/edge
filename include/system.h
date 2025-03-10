#pragma once

#include <cstdint>
#include <string>

class AddressRouter;
class Cartridge;
class CPU;
class InputController;
class InterruptController;
class MMU;
class PPU;
class Screen;
class SerialController;
class SoundController;
class TimerController;

using namespace std;

class System {
 public:
  System(string rom_filename);
  ~System() = default;

  void Main();

  void SetButtons(bool dpadUp, bool dpadDown, bool dpadLeft, bool dpadRight, bool buttonA, bool buttonB, bool buttonSelect, bool buttonStart);
  void AdvanceOneFrame();

  const uint32_t* pixels();

 private:
  Cartridge *cartridge_;
  MMU *mmu_;
  CPU *cpu_;
  PPU *ppu_;
  AddressRouter *router_;
  InputController *input_controller_;
  InterruptController *interrupt_controller_;
  SerialController *serial_controller_;
  SoundController *sound_controller_;
  TimerController *timer_controller_;
  Screen *screen_;

  int frame_count_;
  int frame_cycles_;
  std::chrono::high_resolution_clock::time_point last_frame_start_time_;

  MMU *GetMMU(string rom_filename, bool skip_boot_rom);
  int Advance();
  void FrameEnded();
};
