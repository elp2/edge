#include "input_controller.h"

#include <cassert>
#include <iostream>

#include "interrupt_controller.hpp"
#include "SDL.h"

const uint16_t P0_ADDRESS = 0xFF00;
const uint8_t P0_P14 = 0x10;
const uint8_t P0_P15 = 0x20;
const uint8_t P0_RESET = 0x30;

const int POLL_EVENT_CYCLES = 10000;

const int TRIGGER_DURATION_4MHZ_CYCLES = 2 * 2 * 2 * 2;
const int KEY_HOLD_TRIGGER_CYCLES = TRIGGER_DURATION_4MHZ_CYCLES;

int KeyIndex(SDL_Scancode scancode) {
	switch (scancode) {
		// P14 selected.
		case SDL_SCANCODE_D: // Right
			return 0;
		case SDL_SCANCODE_A: // Left
			return 1;
		case SDL_SCANCODE_W: // Up
			return 2;
		case SDL_SCANCODE_S: // Down
			return 3;
		// P15 selected.
		case SDL_SCANCODE_J: // A
			return 4;
		case SDL_SCANCODE_K: // B
			return 5;
		case SDL_SCANCODE_N: // Select
			return 6;
		case SDL_SCANCODE_M: // Start
			return 7;
	}
	return -1;
}

InputController::InputController() {
	
}

bool InputController::Advance(int cycles) {
	bool trigger_interrupt = false;
	for (int i = 0; i < 8; i++) {
		int key_cycles = cycles_held_[i];
		if (key_cycles > 0) {
			key_cycles += cycles * 4; // 4 MHZ.
			if (key_cycles > KEY_HOLD_TRIGGER_CYCLES) {
				triggered_[i] = true;
				trigger_interrupt = true;
			}
		}
		cycles_held_[i] = key_cycles;
	}

	if (trigger_interrupt) {
		interrupt_handler_->HandleInterrupt(Interrupt_Input);
	}


	cycles_since_poll_ += cycles;
	if (cycles_since_poll_ >= POLL_EVENT_CYCLES) {
		cycles_since_poll_ = 0;
		return true;
	} else {
		return false;
	}
}

void InputController::PollAndApplyEvents() {
	SDL_Event e;
	while (SDL_PollEvent(&e) != 0) {
		if (e.type == SDL_QUIT) {
			std::cout << "SDL_QUIT" << std::endl;
			exit(0);
		}
		else {
			HandleEvent(e);
		}
	}
}

void InputController::HandleEvent(SDL_Event event) {
	if (event.type != SDL_KEYDOWN && event.type != SDL_KEYUP) {
		return;
	}
	SDL_KeyboardEvent kevent = event.key;
	int index = KeyIndex(kevent.keysym.scancode);
	if (index == -1) {
		return;
	}
	if (event.type == SDL_KEYDOWN && !triggered_[index]) {
		cycles_held_[index] = 1;
	}
	else {
		cycles_held_[index] = 0;
		triggered_[index] = false;
	}
}

void InputController::SetInterruptHandler(InterruptHandler* handler) {
	interrupt_handler_ = handler;
}

void InputController::SetByteAt(uint16_t address, uint8_t byte) {
    assert(address == P0_ADDRESS);
    p0_select_ = byte;
}

uint8_t InputController::FourBitTriggering(int offset) {
	uint8_t ports = 0x00;
	assert(offset == 0 || offset == 4);
	for (int i = offset + 3; i >= offset; i--) {
		ports <<= 1;
		if (triggered_[i]) {
			ports |= 0x01;
		}
	}
	return ports;
}


uint8_t InputController::GetByteAt(uint16_t address) {
	assert(address == P0_ADDRESS);

	/// Lower 4 bits of input are built based on the output pin selected.
	uint8_t input_4bits = 0x00;
	uint8_t selection_bits = p0_select_ & P0_RESET;
	switch (selection_bits)
	{
	case P0_P14:
		// Right, left, up, down 0->4.
		input_4bits = FourBitTriggering(0);
		break;
	case P0_P15:
		// A, b, Select, Start, 0->4.
		input_4bits = FourBitTriggering(4);
		break;
	default:
		break;
	}
	return selection_bits | input_4bits;
}
