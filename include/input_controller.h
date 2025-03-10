#pragma once

#include <cstdint>

class InterruptHandler;
struct SDL_KeyboardEvent;
union SDL_Event;

class InputController {
 public:
  InputController();
  ~InputController() = default;

  void SetInterruptHandler(InterruptHandler* handler);

  void SetButtons(bool dpadUp, bool dpadDown, bool dpadLeft, bool dpadRight, bool buttonA, bool buttonB, bool buttonSelect, bool buttonStart);

  bool HandleKeyboardEvent(const SDL_KeyboardEvent& event, bool pressed);
  void HandleEvent(const SDL_Event& e);
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
