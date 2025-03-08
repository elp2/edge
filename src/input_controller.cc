#include "input_controller.h"

#include <cassert>
#include <iostream>

#ifdef BUILD_IOS
#include <SDL3/SDL.h>
#else
#include "SDL.h"
#endif

#include "interrupt_controller.h"

const uint16_t P0_ADDRESS = 0xFF00;

int KeyNibble(SDL_Scancode scancode) {
  // Returns 0 if the key is in the DPAD nibble, 1 if it is in the BTN nibble, -1 if it is not a valid key.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch"
  switch (scancode) {
    // GET_DPAD selected.
    case SDL_SCANCODE_D:  // Right
    case SDL_SCANCODE_A:  // Left
    case SDL_SCANCODE_W:  // Up
    case SDL_SCANCODE_S:  // Down
      return 0;
    // GET_BTN selected.
    case SDL_SCANCODE_J:  // A
    case SDL_SCANCODE_K:  // B
    case SDL_SCANCODE_N:  // Select
    case SDL_SCANCODE_M:  // Start
      return 1;
    default:
      return -1;
  }
#pragma GCC diagnostic push
  return -1;
}

uint8_t ReleasedNibbleBit(SDL_Scancode scancode) {
  // Returns the bit position of the key in the nibble. 1 indicates the key is RELEASED.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch"
  switch (scancode) {
    // DPAD Nibble.
    case SDL_SCANCODE_D:  // Right
      return 1 << 0;
    case SDL_SCANCODE_A:  // Left
      return 1 << 1;
    case SDL_SCANCODE_W:  // Up
      return 1 << 2;
    case SDL_SCANCODE_S:  // Down
      return 1 << 3;
    // BTN Nibble.
    case SDL_SCANCODE_J:  // A
      return 1 << 0;
    case SDL_SCANCODE_K:  // B
      return 1 << 1;
    case SDL_SCANCODE_N:  // Select
      return 1 << 2;
    case SDL_SCANCODE_M:  // Start
      return 1 << 3;
  }
#pragma GCC diagnostic push
  return -1;
}

InputController::InputController() {}

void InputController::PollAndApplyEvents() {
#ifndef BUILD_IOS
  SDL_Event e;
  while (SDL_PollEvent(&e) != 0) {
    if (e.type == SDL_QUIT) {
      std::cout << "SDL_QUIT" << std::endl;
      exit(0);
    } else {
      HandleEvent(e);
    }
  }
#endif
  return;
}

bool InputController::HandleKeyboardEvent(const SDL_KeyboardEvent& event, bool pressed) {
    int nibble = KeyNibble(event.scancode);
    if (nibble == -1) {
        return false;
    }

    if (nibble == 0) {
        if (pressed) {
            dpad_nibble_ &= ~ReleasedNibbleBit(event.scancode);
        } else {
            dpad_nibble_ |= ReleasedNibbleBit(event.scancode);
        }
    } else if (nibble == 1) {
        if (pressed) {
            button_nibble_ &= ~ReleasedNibbleBit(event.scancode);
        } else {
            button_nibble_ |= ReleasedNibbleBit(event.scancode);
        }
    }

    if (pressed) {  // Trigger interrupt only on press
        interrupt_handler_->RequestInterrupt(Interrupt_Input);
    }

    return true;
}

void InputController::HandleEvent(const SDL_Event& e) {
    switch (e.type) {
        case SDL_EVENT_KEY_DOWN:
        case SDL_EVENT_KEY_UP:
            HandleKeyboardEvent(e.key, e.type == SDL_EVENT_KEY_DOWN);
            break;
        // Add other event types if needed
    }
}

void InputController::SetInterruptHandler(InterruptHandler* handler) {
  interrupt_handler_ = handler;
}

void InputController::SetByteAt(uint16_t address, uint8_t byte) {
  assert(address == P0_ADDRESS);
  p0_select_ = byte & 0xf0;
}

uint8_t InputController::GetByteAt(uint16_t address) {
  assert(address == P0_ADDRESS);

  uint8_t selected_nibble = 0x0f; // Default to all inputs being high.
  /// Lower 4 bits of input are built based on the output pin set low.
  if (!(p0_select_ & 1 << 4)) {
    selected_nibble = dpad_nibble_;
  }
  if (!(p0_select_ & 1 << 5)) {
    selected_nibble = button_nibble_;
  }
  uint8_t memory = p0_select_ | selected_nibble;
  return memory;
}
