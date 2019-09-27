#include "timer_controller.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "interrupt_controller.hpp"

const int CYCLES_PER_SECOND = 4194304;
const uint16_t DIV_ADDRESS = 0xFF04;
const uint16_t TIMA_ADDRESS = 0xFF05;
const uint16_t TMA_ADDRESS = 0xFF06;
const uint16_t TAC_ADDRESS = 0xFF07;

using testing::_;

class MockInterruptHandler : public InterruptHandler {
 public:
    MockInterruptHandler() {};
    MOCK_METHOD(void, RequestInterrupt, (Interrupt interrupt), (override));
};

class TimerControllerTest : public ::testing::Test {
 protected:
    TimerControllerTest() {};
    ~TimerControllerTest() {};
    void SetUp() override {
        controller_ = new TimerController();
        controller_->SetInterruptHandler(&mock_handler_);
    }

    MockInterruptHandler mock_handler_;
    TimerController *controller_;
};

TEST_F(TimerControllerTest, GetAndSetValues) {
    EXPECT_CALL(mock_handler_, RequestInterrupt(_)).Times(0);
    uint8_t tima = 0x57;
    uint8_t tma = 0xF0;
    uint8_t tac = 0x06;

    controller_->SetByteAt(0xFF05, tima);
    controller_->SetByteAt(0xFF06, tma);
    controller_->SetByteAt(0xFF07, tac);
}

TEST_F(TimerControllerTest, SettingDIVResetsIt) {
    EXPECT_CALL(mock_handler_, RequestInterrupt(_)).Times(0);    
    controller_->Advance(10000);
    ASSERT_NE(controller_->GetByteAt(DIV_ADDRESS), 0x00);
    controller_->SetByteAt(DIV_ADDRESS, 0x10);
    ASSERT_EQ(controller_->GetByteAt(DIV_ADDRESS), 0x00);
}

TEST_F(TimerControllerTest, DivTimer16khz) {
    EXPECT_CALL(mock_handler_, RequestInterrupt(Interrupt_TimerOverflow)).Times(1);
	controller_->SetByteAt(TAC_ADDRESS, 0b111);
    ASSERT_EQ(controller_->GetByteAt(DIV_ADDRESS), 0x00);
	ASSERT_EQ(controller_->GetByteAt(TIMA_ADDRESS), 0x00);

	int cycles_per_advance = CYCLES_PER_SECOND / 16384;
	for (int i = 0; i < 256; i++) {
		ASSERT_EQ(controller_->GetByteAt(DIV_ADDRESS), controller_->GetByteAt(TIMA_ADDRESS));
		controller_->Advance(cycles_per_advance);
	}
	ASSERT_EQ(controller_->GetByteAt(DIV_ADDRESS), 0);
	ASSERT_EQ(controller_->GetByteAt(TIMA_ADDRESS), 0);
}
TEST_F(TimerControllerTest, DIVLoops) {
	EXPECT_CALL(mock_handler_, RequestInterrupt(_)).Times(0);
	ASSERT_EQ(controller_->GetByteAt(DIV_ADDRESS), 0x00);
	int cycles_per_advance = CYCLES_PER_SECOND / 16384;
	for (int i = 0; i < 256; i++) {
		ASSERT_EQ(controller_->GetByteAt(DIV_ADDRESS), i);
		controller_->Advance(cycles_per_advance);
	}
	ASSERT_EQ(controller_->GetByteAt(DIV_ADDRESS), 0);
}

TEST_F(TimerControllerTest, TimerStartsAtZero) {
    EXPECT_CALL(mock_handler_, RequestInterrupt(_)).Times(0);    
    uint8_t timer = controller_->GetByteAt(TIMA_ADDRESS);
    ASSERT_EQ(0, timer);
}

TEST_F(TimerControllerTest, TimerDoesntAdvanceWhenInactive) {
    EXPECT_CALL(mock_handler_, RequestInterrupt(_)).Times(0);    
    uint8_t timer = controller_->GetByteAt(TIMA_ADDRESS);
    controller_->SetByteAt(TAC_ADDRESS, 0x0);
    controller_->Advance(1000);
    ASSERT_EQ(0, timer);
}

TEST_F(TimerControllerTest, TimerDoesntInterruptWithoutOverflow) {
    EXPECT_CALL(mock_handler_, RequestInterrupt(_)).Times(0);    
    controller_->SetByteAt(TAC_ADDRESS, 0x4);
    controller_->Advance(1);
    ASSERT_EQ(controller_->GetByteAt(TIMA_ADDRESS), 0x0);
    // Shouldn't interrupt at all.
}

TEST_F(TimerControllerTest, TimerInterruptsWhenOverflows) {
    EXPECT_CALL(mock_handler_, RequestInterrupt(Interrupt_TimerOverflow)).Times(1);
    controller_->SetByteAt(TAC_ADDRESS, 0x4);
    controller_->Advance(256 * CYCLES_PER_SECOND / 4096);
    ASSERT_EQ(controller_->GetByteAt(TIMA_ADDRESS), 0x0);
    ASSERT_EQ(controller_->GetByteAt(TAC_ADDRESS), 0x4);
}

TEST_F(TimerControllerTest, ModuloAfterOverflow) {
    EXPECT_CALL(mock_handler_, RequestInterrupt(Interrupt_TimerOverflow)).Times(1);
    controller_->SetByteAt(TAC_ADDRESS, 0x4);
    uint8_t TMA = 0xED;
    controller_->SetByteAt(TMA_ADDRESS, TMA);
    controller_->Advance(256 * CYCLES_PER_SECOND / 4096);
    EXPECT_EQ(controller_->GetByteAt(TIMA_ADDRESS), TMA);
}

TEST_F(TimerControllerTest, FourThousandHZ) {
    EXPECT_CALL(mock_handler_, RequestInterrupt(Interrupt_TimerOverflow)).Times(4096/256);
    controller_->SetByteAt(TAC_ADDRESS, 0x4);
    controller_->Advance(CYCLES_PER_SECOND);
}

TEST_F(TimerControllerTest, TwoSixTwoKHZ) {
    EXPECT_CALL(mock_handler_, RequestInterrupt(Interrupt_TimerOverflow)).Times(262144/256);
    controller_->SetByteAt(TAC_ADDRESS, 0x5);
    controller_->Advance(CYCLES_PER_SECOND);
}

TEST_F(TimerControllerTest, SwitchFrequencies) {
    EXPECT_CALL(mock_handler_, RequestInterrupt(Interrupt_TimerOverflow)).Times(1);
    controller_->SetByteAt(TAC_ADDRESS, 0x5);
    controller_->Advance(128 * CYCLES_PER_SECOND / 262144);
    EXPECT_EQ(controller_->GetByteAt(TIMA_ADDRESS), 0x80);
    controller_->SetByteAt(TAC_ADDRESS, 0x4);
    controller_->Advance(128 * CYCLES_PER_SECOND / 4096);
    EXPECT_EQ(controller_->GetByteAt(TIMA_ADDRESS), 0x0);
}

TEST_F(TimerControllerTest, FourThousandHZOrganicBig) {
	EXPECT_CALL(mock_handler_, RequestInterrupt(Interrupt_TimerOverflow)).Times(1);
	controller_->SetByteAt(TAC_ADDRESS, 0x4);
	EXPECT_EQ(controller_->GetByteAt(TIMA_ADDRESS), 0x0);
	int advance = 256 * (CYCLES_PER_SECOND / 4096) + 30;
	while (advance > 0) {
		int step = advance > 20 ? 20 : advance;
		controller_->Advance(step);
		advance -= step;
	}
}

TEST_F(TimerControllerTest, TwoSixTwoOneLoop) {
	EXPECT_CALL(mock_handler_, RequestInterrupt(Interrupt_TimerOverflow)).Times(1);
	controller_->SetByteAt(TAC_ADDRESS, 0x5);
	EXPECT_EQ(controller_->GetByteAt(TIMA_ADDRESS), 0x0);
	int advance = 256 * (CYCLES_PER_SECOND / 262144) + 2;
	while (advance > 0) {
		int step = advance > 20 ? 20 : advance;
		controller_->Advance(step);
		advance -= step;
	}
}
