#include "system.h"

#include <cassert>
#include <chrono>
#include <iostream>
#include <filesystem>
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
#include "timer_controller.h"
#include "utils.h"

System::System(string rom_filename, string state_root_dir) {
  bool skip_boot_rom = true;
  string cartridge_ram_dir = "";

  if (state_root_dir != "") {
    string rom_name = std::filesystem::path(rom_filename).filename().string();
    std::error_code ec;
    string game_state_dir = state_root_dir + "/" + rom_name;
    state_ = new State(game_state_dir, -1);
    cartridge_ram_dir = state_->GetStateDir();
  }

  mmu_ = GetMMU(rom_filename, skip_boot_rom);

  cartridge_ = new Cartridge(rom_filename, cartridge_ram_dir);
  cartridge_->PrintDebugInfo();
  mmu_->SetCartridge(cartridge_);
  SaveState();

  screen_ = new Screen();
  ppu_ = new PPU(screen_);

  serial_controller_ = new SerialController();
  interrupt_controller_ = new InterruptController();
  input_controller_ = new InputController();
  input_controller_->SetInterruptHandler(interrupt_controller_);
#ifndef BUILD_IOS
  input_controller_->SetScreenshotTaker(this);
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
    ppu_->SkipBootROM();
  }

  frame_count_ = 0;
}

MMU *System::GetMMU(string rom_filename, bool skip_boot_rom) {
  MMU *mmu = new MMU();

  if (!skip_boot_rom) {
    mmu->SetBootROM(UnsignedCartridgeBytes("../roms/boot.gb"));
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
    input_controller_->PollAndApplyEvents();
    entered_vsync = ppu_->Advance(stepped);
    timer_controller_->Advance(stepped);

    sound_controller_->Advance(stepped);

    interrupt_controller_->Advance(stepped);
    int interrupt_steps = interrupt_controller_->HandleInterruptRequest();
    // TODO: Interrupt handling should avance everything except CPU 20 cycles. #39.
    assert(interrupt_steps == 0);

    frame_cycles_ += stepped;
  }
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

  if (frame_count_ % 1000 == 0) {
    TakeScreenshot();
  }
#endif
}

void System::SaveState() {
  struct SaveState ss = {};
  ss.cpu.a = 10;
  ss.ppu.lcdc = 0x91;
  ss.timer.div = 0xED;

  struct SaveState ss2 = {};
  state_->SaveState(ss);
  state_->LoadState(0, ss2);
  assert(ss2.cpu.a == ss.cpu.a);
  assert(ss2.ppu.lcdc == ss.ppu.lcdc);
  assert(ss2.timer.div = ss.timer.div);
}

void System::TakeScreenshot() {
    screen_->SaveScreenshot(cartridge_->GameTitle());
}

const uint32_t* System::pixels() { return screen_->pixels(); }

void System::Main() {
  if (SUPER_DEBUG) {
    cpu_->SetDebugPrint(true);
  }
  while (true) {
    AdvanceOneFrame();
  }
}
