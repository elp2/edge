#include "system.h"

#include <cassert>
#include <chrono>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <thread>

#include "address_router.h"
#include "bit_command.h"
#include "command_factory.h"
#include "constants.h"
#include "cpu.h"
#include "input_controller.h"
#include "interrupt_controller.h"
#include "mmu.h"
#include "ppu.h"
#include "screen.h"
#include "serial_controller.h"
#include "sound_controller.h"
#include "state.h"
#include "state_controller.h"
#include "timer_controller.h"
#include "utils.h"

System::System(string rom_filename, string game_state_dir) {
  state_controller_ = new StateController(game_state_dir);
  std::cout << "Saved state count: " << state_controller_->GetSaveStates().size() << std::endl;

  bool skip_boot_rom = true;
  mmu_ = GetMMU(skip_boot_rom);

  cartridge_ = new Cartridge(rom_filename);
  cartridge_->PrintDebugInfo();
  mmu_->SetCartridge(cartridge_);

  screen_ = new Screen();
  ppu_ = new PPU(screen_);

  serial_controller_ = new SerialController();
  interrupt_controller_ = new InterruptController();
  input_controller_ = new InputController();
  input_controller_->SetInterruptHandler(interrupt_controller_);
#ifndef BUILD_IOS
  input_controller_->SetScreenshotTaker(this);
  input_controller_->SetStateNavigator(this);
#endif  
  timer_controller_ = new TimerController();
  timer_controller_->SetInterruptHandler(interrupt_controller_);
  sound_controller_ = new SoundController();

  router_ = new AddressRouter(mmu_, ppu_, serial_controller_,
                              interrupt_controller_, input_controller_,
                              timer_controller_, sound_controller_);

  interrupt_controller_->set_input_controller(input_controller_);

  ppu_->SetInterruptHandler(interrupt_controller_);

  cpu_ = new CPU(router_);
  cpu_->SetInterruptController(interrupt_controller_);

  frame_cycles_ = 0;
  last_frame_start_time_ = std::chrono::high_resolution_clock::now();

  if (skip_boot_rom) {
    cpu_->SkipBootROM();
    router_->SkipBootROM();
  }

  frame_count_ = 0;
}

MMU *System::GetMMU(bool skip_boot_rom) {
  MMU *mmu = new MMU();

  if (!skip_boot_rom) {
    mmu->SetBootROM(UnsignedCartridgeBytes("Roms/boot.gb"));
  }

  return mmu;
}

void System::SetButtons(bool dpadUp, bool dpadDown, bool dpadLeft, bool dpadRight, bool buttonA, bool buttonB, bool buttonSelect, bool buttonStart) {
  input_controller_->SetButtons(dpadUp, dpadDown, dpadLeft, dpadRight, buttonA, buttonB, buttonSelect, buttonStart);
}

void System::AdvanceOneFrame() {
  bool entered_vsync = false;
  while (!entered_vsync) {
    int stepped = cpu_->Step();
    entered_vsync = ppu_->Advance(stepped);
    timer_controller_->Advance(stepped);

    sound_controller_->Advance(stepped);

    interrupt_controller_->Advance(stepped);
    int interrupt_steps = interrupt_controller_->HandleInterruptRequest();
    // TODO: Interrupt handling should avance everything except CPU 20 cycles. #39.
    assert(interrupt_steps == 0);

    frame_cycles_ += stepped;
  }

  input_controller_->PollAndApplyEvents();
  // std::cout << "Frame cycles: " << dec << frame_cycles_ << std::endl;

  frame_cycles_ = 0;
  frame_count_++;

#ifndef BUILD_IOS
  static const std::chrono::duration<double> FRAME_TIME(1.0 / 60.0);

  auto current_time = std::chrono::high_resolution_clock::now();
  auto elapsed = current_time - last_frame_start_time_;

  if (elapsed < FRAME_TIME) {
    std::this_thread::sleep_for(FRAME_TIME - elapsed);
  }
  last_frame_start_time_ = std::chrono::high_resolution_clock::now();
#endif

  static constexpr int MAIN_SAVE_INTERVAL_FRAMES = 1 * 60;
  if (frame_count_ % MAIN_SAVE_INTERVAL_FRAMES == 0 && frame_count_ > 0) {
      SaveMainState();
  }
}

void System::SaveState() {
  assert(state_controller_ != nullptr);
  int slot = state_controller_->GetNextRotatingSlot();
  state_controller_->SaveState(slot, cpu_, mmu_, cartridge_, router_, interrupt_controller_, ppu_, screen_);
}

void System::SaveMainState() {
  assert(state_controller_ != nullptr);
  // Save to main slot (slot 0)
  state_controller_->SaveState(0, cpu_, mmu_, cartridge_, router_, interrupt_controller_, ppu_, screen_);
}

void System::LoadMainState() {
  assert(state_controller_ != nullptr);
  // Load from main slot (slot 0)
  state_controller_->LoadState(0, cpu_, mmu_, cartridge_, router_, interrupt_controller_, ppu_);
}

void System::LoadPreviouslySavedState() {
  assert(state_controller_ != nullptr);
  std::cout << "Loading previously saved state..." << std::endl;

  int slot = state_controller_->GetRotatingSlot();
  if (slot >= 0) {
    state_controller_->LoadState(slot, cpu_, mmu_, cartridge_, router_, interrupt_controller_, ppu_);
  } else {
    std::cout << "No saved state found to load" << std::endl;
  }
}



void System::LoadStateSlot(int slot) {
  assert(state_controller_ != nullptr);
  state_controller_->LoadState(slot, cpu_, mmu_, cartridge_, router_, interrupt_controller_, ppu_);
}

void System::TakeScreenshot() {
    screen_->SaveScreenshot(cartridge_->GameTitle());
}

const uint32_t* System::pixels() { return screen_->pixels(); }

std::vector<std::unique_ptr<State>> System::GetSaveStates() {
  return state_controller_->GetSaveStates();
}

void System::Main() {
  if (SUPER_DEBUG) {
    cpu_->SetDebugPrint(true);
  }
  while (true) {
    AdvanceOneFrame();
  }
}
