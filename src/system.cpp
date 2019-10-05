#include "system.hpp"

#include "SDL.h"

#include <cassert>
#include <iostream>

#include "AddressRouter.hpp"
#include "BitCommand.hpp"
#include "CommandFactory.hpp"
#include "CPU.hpp"
#include "input_controller.h"
#include "interrupt_controller.hpp"
#include "MMU.hpp"
#include "PPU.hpp"
#include "serial_controller.hpp"
#include "timer_controller.h"
#include "Screen.hpp"
#include "Utils.hpp"

System::System(string rom_filename) {
    mmu_ = GetMMU(rom_filename);
    ppu_ = new PPU();

	serial_controller_ = new SerialController();
    interrupt_controller_ = new InterruptController();
	input_controller_ = new InputController();
	input_controller_->SetInterruptHandler(interrupt_controller_);
	timer_controller_ = new TimerController();
	timer_controller_->SetInterruptHandler(interrupt_controller_);

	router_ = new AddressRouter(mmu_, ppu_, serial_controller_, interrupt_controller_, input_controller_, timer_controller_);

	interrupt_controller_->set_input_controller(input_controller_);

	ppu_->SetInterruptHandler(interrupt_controller_);

	cpu_ = new CPU(router_);
    cpu_->SetInterruptController(interrupt_controller_);
}

MMU *System::GetMMU(string rom_filename) {
    ROM *boot_rom = new ROM();
    assert(boot_rom->LoadFile("../../boot.gb"));
    ROM *cartridge_rom = new ROM();
    assert(cartridge_rom->LoadFile(rom_filename));

    MMU *mmu = new MMU();
    mmu->SetROMs(boot_rom, cartridge_rom);
    return mmu;
}

void System::Advance(int stepped) {
	if (input_controller_->Advance(stepped)) {
		input_controller_->PollAndApplyEvents();
	}
    ppu_->Advance(stepped);
	timer_controller_->Advance(stepped);
	// if (cpu_->Get16Bit(Register_PC) >= 0xC200) {
	// 	timer_controller_->Debugger();
	// }

	interrupt_controller_->Advance(stepped);
	int interrupt_steps = interrupt_controller_->HandleInterruptRequest();
	// TODO: Refactor advancing if we advance for interrupts.
	// At the very least, we'd have to advance the other stuff, but not the interrupt controller.
	// Advancing the IC is how we turn the flags on/off. Have a separate "CPU Stepped" version?
	assert(interrupt_steps == 0);
}

void System::Main() {
    // cpu_->SetDebugPrint(true);
    while(true) {
        Advance(cpu_->Step());
    }
}