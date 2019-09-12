#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "interrupt_controller.hpp"

using testing::_;

class MockInterruptExecutor : public InterruptExecutor {
 public:
    MockInterruptExecutor() {};
    MOCK_METHOD(void, InterruptToPC, (uint8_t pc), (override));
};

class InterruptRouterTest : public ::testing::Test {
 protected:
    InterruptRouterTest() {};
    ~InterruptRouterTest() {};
    void SetUp() override {
        controller_ = new InterruptController();
        controller_->set_executor(&mock_executor_);
    }

    MockInterruptExecutor mock_executor_;
    InterruptController *controller_;
};

TEST_F(InterruptRouterTest, InterruptsDisabled) {
    EXPECT_CALL(mock_executor_, InterruptToPC(_)).Times(0);
    controller_->GenerateInterrupt(Interrupt_VBlank);
}

TEST_F(InterruptRouterTest, InterruptVblankNotEnabled) {
    EXPECT_CALL(mock_executor_, InterruptToPC(_)).Times(0);

    controller_->set_interrupts_enabled(true);
    controller_->GenerateInterrupt(Interrupt_VBlank);
    EXPECT_TRUE(controller_->interrupts_enabled());
}

TEST_F(InterruptRouterTest, InterruptVblankEnabled) {
    EXPECT_CALL(mock_executor_, InterruptToPC(0x40));
    controller_->set_interrupts_enabled(true);
    controller_->set_interrupt_enabled_flags(Interrupt_VBlank);
    controller_->GenerateInterrupt(Interrupt_VBlank);

    EXPECT_FALSE(controller_->interrupts_enabled());
}

TEST_F(InterruptRouterTest, InterruptVblankDoesntTriggerForLCDC) {
    EXPECT_CALL(mock_executor_, InterruptToPC(_)).Times(0);

    controller_->set_interrupts_enabled(true);
    controller_->set_interrupt_enabled_flags(Interrupt_LCDC);
    controller_->GenerateInterrupt(Interrupt_VBlank);

    EXPECT_TRUE(controller_->interrupts_enabled());
}
