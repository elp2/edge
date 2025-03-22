#pragma once

#include <cstdint>
#include <string>

#include "input_controller.h"

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
class State;
class TimerController;

using namespace std;

class System : public ScreenshotTaker {
 public:
  System(string rom_filename, string state_dir);
  ~System() = default;

  void Main();

  void SetButtons(bool dpadUp, bool dpadDown, bool dpadLeft, bool dpadRight, bool buttonA, bool buttonB, bool buttonSelect, bool buttonStart);
  void AdvanceOneFrame();

  const uint32_t* pixels();

  void SaveState();

  // ScreenshotTaker abstract class functions.
  void TakeScreenshot();

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
  State *state_;

  int frame_count_;
  int frame_cycles_;
  std::chrono::high_resolution_clock::time_point last_frame_start_time_;

  MMU *GetMMU(string rom_filename, bool skip_boot_rom);
};
