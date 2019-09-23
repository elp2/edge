#include "input_controller.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "SDL.h"

#include "interrupt_controller.hpp"

using testing::_;

const uint16_t P0_ADDRESS = 0xFF00;
const uint8_t P0_P14 = 0x10;
const uint8_t P0_P15 = 0x20;
const uint8_t P0_RESET = 0x30;

class MockInterruptHandler : public InterruptHandler {
public:
	MockInterruptHandler() {};
	MOCK_METHOD(void, RequestInterrupt, (Interrupt interrupt), (override));
};

class InputControllerTest : public ::testing::Test {
 protected:
	 InputControllerTest() {};
	~InputControllerTest() {};
	void SetUp() override {
		controller_ = new InputController();
		controller_->SetInterruptHandler(&mock_handler_);
	}

	SDL_Event Button(bool key_down, SDL_Scancode sc) {
		SDL_Event button = SDL_Event();
		button.type = key_down ? SDL_KEYDOWN : SDL_KEYUP;
		button.key.keysym.scancode = sc;
		return button;
	}

	MockInterruptHandler mock_handler_;
	InputController* controller_;
};

TEST_F(InputControllerTest, NoAdvanceNoVisibleOrInterrupt) {
	EXPECT_CALL(mock_handler_, RequestInterrupt(_)).Times(0);
	controller_->SetByteAt(P0_ADDRESS, P0_P15);
	controller_->HandleEvent(Button(true, SDL_SCANCODE_J));
}

TEST_F(InputControllerTest, InterruptsForKeydown) {
	EXPECT_CALL(mock_handler_, RequestInterrupt(Interrupt_Input)).Times(1);
	controller_->SetByteAt(P0_ADDRESS, P0_P15);
	controller_->HandleEvent(Button(true, SDL_SCANCODE_J));
	controller_->Advance(1001);
}

TEST_F(InputControllerTest, ReadsKeyDown) {
	EXPECT_CALL(mock_handler_, RequestInterrupt(Interrupt_Input)).Times(1);
	controller_->SetByteAt(P0_ADDRESS, P0_P15);
	controller_->HandleEvent(Button(true, SDL_SCANCODE_J));
	controller_->Advance(1001);
	EXPECT_EQ(controller_->GetByteAt(P0_ADDRESS), 0x21);
}


TEST_F(InputControllerTest, Reads2KeyDown) {
	EXPECT_CALL(mock_handler_, RequestInterrupt(Interrupt_Input)).Times(1);
	controller_->SetByteAt(P0_ADDRESS, P0_P14);
	controller_->HandleEvent(Button(true, SDL_SCANCODE_A));
	controller_->HandleEvent(Button(true, SDL_SCANCODE_S));
	controller_->Advance(1001);
	EXPECT_EQ(controller_->GetByteAt(P0_ADDRESS), 0x1A);
}

TEST_F(InputControllerTest, KeyUps) {
	EXPECT_CALL(mock_handler_, RequestInterrupt(Interrupt_Input)).Times(1);
	controller_->SetByteAt(P0_ADDRESS, P0_P14);
	controller_->HandleEvent(Button(true, SDL_SCANCODE_A));
	controller_->HandleEvent(Button(true, SDL_SCANCODE_S));
	controller_->Advance(1001);
	EXPECT_EQ(controller_->GetByteAt(P0_ADDRESS), 0x1A);
	controller_->HandleEvent(Button(false, SDL_SCANCODE_A));
	controller_->HandleEvent(Button(false, SDL_SCANCODE_S));
	EXPECT_EQ(controller_->GetByteAt(P0_ADDRESS), 0x10);
}

TEST_F(InputControllerTest, IgnoresUnknonwKeys) {
	EXPECT_CALL(mock_handler_, RequestInterrupt(Interrupt_Input)).Times(0);
	controller_->SetByteAt(P0_ADDRESS, P0_P14);
	controller_->HandleEvent(Button(true, SDL_SCANCODE_Z));
	controller_->Advance(1001);
	EXPECT_EQ(controller_->GetByteAt(P0_ADDRESS), 0x10);
	controller_->SetByteAt(P0_ADDRESS, P0_P15);
	EXPECT_EQ(controller_->GetByteAt(P0_ADDRESS), 0x20);
}

TEST_F(InputControllerTest, InterruptsMultipleTimes) {
	EXPECT_CALL(mock_handler_, RequestInterrupt(Interrupt_Input)).Times(2);
	controller_->SetByteAt(P0_ADDRESS, P0_P14);
	controller_->HandleEvent(Button(true, SDL_SCANCODE_A));
	controller_->Advance(1001);
	controller_->HandleEvent(Button(false, SDL_SCANCODE_A));
	controller_->Advance(1001);

	controller_->HandleEvent(Button(true, SDL_SCANCODE_A));
	controller_->Advance(1001);
	controller_->HandleEvent(Button(false, SDL_SCANCODE_A));
	controller_->Advance(1001);
}
