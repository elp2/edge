#include "system.h"

#include <cassert>
#include <chrono>
#include <iostream>
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
#include "timer_controller.h"
#include "utils.h"

System::System(string rom_filename) {
  bool skip_boot_rom = true;
  mmu_ = GetMMU(rom_filename, skip_boot_rom);

  screen_ = new Screen();
  ppu_ = new PPU(screen_);

  serial_controller_ = new SerialController();
  interrupt_controller_ = new InterruptController();
  input_controller_ = new InputController();
  input_controller_->SetInterruptHandler(interrupt_controller_);
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
  cartridge_ = new Cartridge(rom_filename);
  mmu->SetCartridge(cartridge_);

  return mmu;
}

void System::Advance(int stepped) {
  input_controller_->PollAndApplyEvents();
  ppu_->Advance(stepped);
  timer_controller_->Advance(stepped);

  sound_controller_->Advance(stepped);

  interrupt_controller_->Advance(stepped);
  int interrupt_steps = interrupt_controller_->HandleInterruptRequest();
  // TODO: Refactor advancing if we advance for interrupts.
  // At the very least, we'd have to advance the other stuff, but not the
  // interrupt controller. Advancing the IC is how we turn the flags on/off.
  // Have a separate "CPU Stepped" version?
  assert(interrupt_steps == 0);

  frame_cycles_ += stepped;
  if (frame_cycles_ >= CYCLES_PER_FRAME) {
    frame_cycles_ = 0;
    static const std::chrono::duration<double> FRAME_TIME(1.0 / 60.0);

    auto current_time = std::chrono::high_resolution_clock::now();
    auto elapsed = current_time - last_frame_start_time_;

    if (elapsed < FRAME_TIME) {
      std::this_thread::sleep_for(FRAME_TIME - elapsed);
    }
    last_frame_start_time_ = std::chrono::high_resolution_clock::now();
    frame_count_++;

#ifndef BUILD_IOS
    if (frame_count_ % 1000 == 0) {
      screen_->SaveScreenshot(cartridge_->GameTitle());
    }
#endif
  }
}

void System::Main() {
  if (SUPER_DEBUG) {
    cpu_->SetDebugPrint(true);
  }
  while (true) {
    Advance(cpu_->Step());
  }
}
