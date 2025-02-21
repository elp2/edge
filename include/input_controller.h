#pragma once

#include <cstdint>

class InterruptHandler;
union SDL_Event;

class InputController {
 public:
  InputController();
  ~InputController() = default;

  void SetInterruptHandler(InterruptHandler* handler);
  bool HandleEvent(SDL_Event e);
  void PollAndApplyEvents();

  void SetByteAt(uint16_t address, uint8_t byte);
  uint8_t GetByteAt(uint16_t);

 private:
  InterruptHandler* interrupt_handler_;
  uint8_t p0_select_ = 0x00;
  uint8_t dpad_nibble_ = 0x0f;
  uint8_t button_nibble_ = 0x0f;
  int cycles_since_poll_ = 0;

  uint8_t FourBitTriggering(int i);
};
