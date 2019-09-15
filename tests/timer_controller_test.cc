#include "timer_controller.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "interrupt_controller.hpp"

const uint16_t DIV_ADDRESS = 0xFF04;

using testing::_;

class MockInterruptHandler : public InterruptHandler {
 public:
    MockInterruptHandler() {};
    MOCK_METHOD(void, GenerateInterrupt, (Interrupt interrupt), (override));
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
    EXPECT_CALL(mock_handler_, GenerateInterrupt(_)).Times(0);
    uint8_t tima = 0x57;
    uint8_t tma = 0xF0;
    uint8_t tac = 0x06;

    controller_->SetByteAt(0xFF05, tima);
    controller_->SetByteAt(0xFF06, tma);
    controller_->SetByteAt(0xFF07, tac);
}

TEST_F(TimerControllerTest, SettingDIVResetsIt) {
    controller_->Advance(10000);
    ASSERT_NE(controller_->GetByteAt(DIV_ADDRESS), 0x00);
    controller_->SetByteAt(DIV_ADDRESS, 0x10);
    ASSERT_EQ(controller_->GetByteAt(DIV_ADDRESS), 0x00);
}

TEST_F(TimerControllerTest, DIVAdvancing) {
    ASSERT_EQ(controller_->GetByteAt(DIV_ADDRESS), 0x00);

    controller_->Advance(100);
    ASSERT_EQ(controller_->GetByteAt(DIV_ADDRESS), 0x00);

    controller_->Advance(1947);
    ASSERT_EQ(controller_->GetByteAt(DIV_ADDRESS), 0x00);

    controller_->Advance(1);
    ASSERT_EQ(controller_->GetByteAt(DIV_ADDRESS), 0x01);

    controller_->Advance(1);
    ASSERT_EQ(controller_->GetByteAt(DIV_ADDRESS), 0x01);

    controller_->Advance(2048 - 2);
    ASSERT_EQ(controller_->GetByteAt(DIV_ADDRESS), 0x01);

    controller_->Advance(1);
    ASSERT_EQ(controller_->GetByteAt(DIV_ADDRESS), 0x02);
}
