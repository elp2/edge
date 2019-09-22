#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "interrupt_controller.hpp"

const uint16_t IF_ADDRESS = 0xFF0F;
const uint16_t IE_ADDRESS = 0xFFFF;

using testing::_;

class MockInterruptExecutor : public InterruptExecutor {
 public:
    MockInterruptExecutor() {};
    MOCK_METHOD(void, InterruptToPC, (uint8_t pc), (override));
};

class InterruptControllerTest : public ::testing::Test {
 protected:
    InterruptControllerTest() {};
    ~InterruptControllerTest() {};
    void SetUp() override {
        controller_ = new InterruptController();
        controller_->set_executor(&mock_executor_);
    }

    MockInterruptExecutor mock_executor_;
    InterruptController *controller_;
};

TEST_F(InterruptControllerTest, InterruptsDisabled) {
    EXPECT_CALL(mock_executor_, InterruptToPC(_)).Times(0);
    controller_->HandleInterrupt(Interrupt_VBlank);
}

TEST_F(InterruptControllerTest, InterruptVblankNotEnabled) {
    EXPECT_CALL(mock_executor_, InterruptToPC(_)).Times(0);

    controller_->set_interrupts_enabled(true);
    controller_->HandleInterrupt(Interrupt_VBlank);
    EXPECT_TRUE(controller_->interrupts_enabled());
}

TEST_F(InterruptControllerTest, InterruptVblankEnabled) {
    EXPECT_CALL(mock_executor_, InterruptToPC(0x40));
    controller_->set_interrupts_enabled(true);
    controller_->set_interrupt_enabled_flags(Interrupt_VBlank);
    controller_->HandleInterrupt(Interrupt_VBlank);

    EXPECT_FALSE(controller_->interrupts_enabled());
}

TEST_F(InterruptControllerTest, InterruptVblankDoesntTriggerForLCDC) {
    EXPECT_CALL(mock_executor_, InterruptToPC(_)).Times(0);

    controller_->set_interrupts_enabled(true);
    controller_->set_interrupt_enabled_flags(Interrupt_LCDC);
    controller_->HandleInterrupt(Interrupt_VBlank);

    EXPECT_TRUE(controller_->interrupts_enabled());
}

TEST_F(InterruptControllerTest, DisablesInterruptsAfterCycles) {
    controller_->set_interrupts_enabled(true);

    controller_->DisableInterrupts();
    EXPECT_TRUE(controller_->interrupts_enabled());
    controller_->Advance(10);
    EXPECT_TRUE(controller_->interrupts_enabled());
    controller_->Advance(10);
    EXPECT_FALSE(controller_->interrupts_enabled());
}

TEST_F(InterruptControllerTest, EnablesInterruptsAfterCycles) {
    controller_->set_interrupts_enabled(false);

    controller_->EnableInterrupts();
    EXPECT_FALSE(controller_->interrupts_enabled());
    controller_->Advance(10);
    EXPECT_FALSE(controller_->interrupts_enabled());
    controller_->Advance(10);
    EXPECT_TRUE(controller_->interrupts_enabled());
}

TEST_F(InterruptControllerTest, InterruptTimer) {
	EXPECT_CALL(mock_executor_, InterruptToPC(0x50));
	controller_->set_interrupts_enabled(true);
	controller_->set_interrupt_enabled_flags(Interrupt_TimerOverflow);
	controller_->HandleInterrupt(Interrupt_TimerOverflow);

	ASSERT_EQ(controller_->GetByteAt(IF_ADDRESS), 0b100);

	EXPECT_FALSE(controller_->interrupts_enabled());
}