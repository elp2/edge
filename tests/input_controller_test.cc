#include "input_controller.h"

#include <SDL3/SDL.h>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "interrupt_controller.h"

using testing::_;

const uint16_t P0_ADDRESS = 0xFF00;
const uint8_t P0_GET_DPAD = 0x20;  // P14 set low, P15 set high!
const uint8_t P0_GET_BTN = 0x10;  // P15 set low, p14 set high!
const uint8_t P0_RESET = 0x30;

class MockInterruptHandler : public InterruptHandler {
 public:
  MockInterruptHandler(){};
  MOCK_METHOD(void, RequestInterrupt, (Interrupt interrupt), (override));
};

class InputControllerTest : public ::testing::Test {
 protected:
  InputControllerTest(){};
  ~InputControllerTest(){};
  void SetUp() override {
    controller_ = new InputController();
    controller_->SetInterruptHandler(&mock_handler_);
  }

  SDL_Event Button(bool key_down, SDL_Scancode sc) {
    SDL_Event event = SDL_Event();
    event.key.type = key_down ? SDL_EVENT_KEY_DOWN : SDL_EVENT_KEY_UP;
    event.key.scancode = sc;

    return event;
  }

  MockInterruptHandler mock_handler_;
  InputController* controller_;
};

TEST_F(InputControllerTest, InitialStateGood) {
  EXPECT_CALL(mock_handler_, RequestInterrupt(_)).Times(0);

  // Nothing should be pressed.
  controller_->SetByteAt(P0_ADDRESS, P0_GET_DPAD);
  EXPECT_EQ(controller_->GetByteAt(P0_ADDRESS), P0_GET_DPAD | 0x0f);
  controller_->SetByteAt(P0_ADDRESS, P0_GET_BTN);
  EXPECT_EQ(controller_->GetByteAt(P0_ADDRESS), P0_GET_BTN | 0x0f);
  controller_->SetByteAt(P0_ADDRESS, P0_RESET);
  EXPECT_EQ(controller_->GetByteAt(P0_ADDRESS), P0_RESET | 0x0f);
}

TEST_F(InputControllerTest, InterruptsForKeydown) {
  EXPECT_CALL(mock_handler_, RequestInterrupt(Interrupt_Input)).Times(1);
  controller_->SetByteAt(P0_ADDRESS, P0_GET_BTN);
  controller_->HandleEvent(Button(true, SDL_SCANCODE_J));
}

TEST_F(InputControllerTest, InterruptsForKeydownWithReset) {
  // Regardless of what we're reading, a keydown should trigger an interrupt.
  EXPECT_CALL(mock_handler_, RequestInterrupt(Interrupt_Input)).Times(1);
  controller_->SetByteAt(P0_ADDRESS, P0_RESET);
  controller_->HandleEvent(Button(true, SDL_SCANCODE_J));
}

TEST_F(InputControllerTest, ReadsKeyDown) {
  EXPECT_CALL(mock_handler_, RequestInterrupt(Interrupt_Input)).Times(1);
  controller_->SetByteAt(P0_ADDRESS, P0_GET_BTN);
  controller_->HandleEvent(Button(true, SDL_SCANCODE_J));
  EXPECT_EQ(controller_->GetByteAt(P0_ADDRESS), P0_GET_BTN | 0b1110);
}

TEST_F(InputControllerTest, SelectsDpad) {
  EXPECT_CALL(mock_handler_, RequestInterrupt(Interrupt_Input)).Times(1);
  controller_->SetByteAt(P0_ADDRESS, P0_GET_DPAD);
  controller_->HandleEvent(Button(true, SDL_SCANCODE_J));
  EXPECT_EQ(controller_->GetByteAt(P0_ADDRESS), P0_GET_DPAD | 0b1111);
}

TEST_F(InputControllerTest, Reads2KeyDown) {
  EXPECT_CALL(mock_handler_, RequestInterrupt(Interrupt_Input)).Times(2);
  controller_->SetByteAt(P0_ADDRESS, P0_GET_DPAD);
  controller_->HandleEvent(Button(true, SDL_SCANCODE_A));
  controller_->HandleEvent(Button(true, SDL_SCANCODE_S));
  EXPECT_EQ(controller_->GetByteAt(P0_ADDRESS), P0_GET_DPAD | 0b0101);
}

TEST_F(InputControllerTest, KeyUps) {
  EXPECT_CALL(mock_handler_, RequestInterrupt(Interrupt_Input)).Times(2);
  controller_->SetByteAt(P0_ADDRESS, P0_GET_DPAD);
  controller_->HandleEvent(Button(true, SDL_SCANCODE_A));
  controller_->HandleEvent(Button(true, SDL_SCANCODE_S));

  controller_->HandleEvent(Button(false, SDL_SCANCODE_A));
  controller_->HandleEvent(Button(false, SDL_SCANCODE_S));
  EXPECT_EQ(controller_->GetByteAt(P0_ADDRESS), P0_GET_DPAD | 0b1111);
}

TEST_F(InputControllerTest, IgnoresUnknonwKeys) {
  EXPECT_CALL(mock_handler_, RequestInterrupt(Interrupt_Input)).Times(0);
  controller_->SetByteAt(P0_ADDRESS, P0_GET_DPAD);

  controller_->HandleEvent(Button(true, SDL_SCANCODE_Z));
  EXPECT_EQ(controller_->GetByteAt(P0_ADDRESS), P0_GET_DPAD | 0b1111);

  controller_->SetByteAt(P0_ADDRESS, P0_GET_BTN);
  EXPECT_EQ(controller_->GetByteAt(P0_ADDRESS), P0_GET_BTN | 0b1111);
}

TEST_F(InputControllerTest, InterruptsMultipleTimes) {
  EXPECT_CALL(mock_handler_, RequestInterrupt(Interrupt_Input)).Times(2);
  controller_->SetByteAt(P0_ADDRESS, P0_GET_DPAD);
  // First up/down should trigger an interrupt.
  controller_->HandleEvent(Button(true, SDL_SCANCODE_A));
  controller_->HandleEvent(Button(false, SDL_SCANCODE_A));
  // Second up/down should trigger an interrupt.
  controller_->HandleEvent(Button(true, SDL_SCANCODE_A));
  controller_->HandleEvent(Button(false, SDL_SCANCODE_A));
}

TEST_F(InputControllerTest, InterruptDoesntSaveButtons) {
  EXPECT_CALL(mock_handler_, RequestInterrupt(Interrupt_Input)).Times(1);
  controller_->SetByteAt(P0_ADDRESS, P0_GET_BTN);
  controller_->HandleEvent(Button(true, SDL_SCANCODE_J));
  controller_->HandleEvent(Button(false, SDL_SCANCODE_J));

  // The buttons should show the latest state (released) even if they triggered an interrupt.
  EXPECT_EQ(controller_->GetByteAt(P0_ADDRESS), P0_GET_BTN | 0b1111);
}
