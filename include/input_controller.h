#pragma once

#include <cstdint>

class InterruptHandler;
union SDL_Event;

class InputController {
 public:
	InputController();
	~InputController() = default;
	
	void SetInterruptHandler(InterruptHandler* handler);
	bool Advance(int cycles);
	void HandleEvent(SDL_Event e);
	void PollAndApplyEvents();

	void SetByteAt(uint16_t address, uint8_t byte);
	uint8_t GetByteAt(uint16_t);

 private:
	InterruptHandler* interrupt_handler_;
    uint8_t p0_select_ = 0x00;
	int cycles_held_[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	bool triggered_[8] = { false, false, false, false, false, false, false, false };
	int cycles_since_poll_ = 0;

	uint8_t FourBitTriggering(int i);
};
