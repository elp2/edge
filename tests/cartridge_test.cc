#include "cartridge.h"

#include "gtest/gtest.h"

const int RTC_SECONDS_REGISTER = 0x08;
const int RTC_MINUTES_REGISTER = 0x09;
const int RTC_HOURS_REGISTER = 0x0A;
const int RTC_DAYS_LOW8_REGISTER = 0x0B;
const int RTC_DAYS_HIGH_CARRY_HALT_REGISTER = 0x0C;

uint8_t GetRTCSeconds(Cartridge* cart) {
  cart->SetRAMBankRTC(RTC_SECONDS_REGISTER);
  return cart->GetRAMorRTC(0);
}

uint8_t GetRTCMinutes(Cartridge* cart) {
  cart->SetRAMBankRTC(RTC_MINUTES_REGISTER);
  return cart->GetRAMorRTC(0);
}

uint8_t GetRTCHours(Cartridge* cart) {
  cart->SetRAMBankRTC(RTC_HOURS_REGISTER);
  return cart->GetRAMorRTC(0);
}

uint8_t GetRTCDaysLow(Cartridge* cart) {
  cart->SetRAMBankRTC(RTC_DAYS_LOW8_REGISTER);
  return cart->GetRAMorRTC(0);
}

uint8_t GetRTCDaysHighAndFlags(Cartridge* cart) {
  cart->SetRAMBankRTC(RTC_DAYS_HIGH_CARRY_HALT_REGISTER);
  return cart->GetRAMorRTC(0);
}

void LatchCartrdige(Cartridge *cart) {
  cart->LatchRTC(0);
  cart->LatchRTC(1);
}

void UnlatchCartridge(Cartridge *cart) {
  cart->LatchRTC(0);
}

time_t DHMS(int days, int hours, int minutes, int seconds) {
  assert(days < 512);
  return seconds + 60 * minutes + 3600 * hours + 24 * 3600 * days;
}

class CartridgeTest : public ::testing::Test {
 protected:
  void SetUp() override {
    cartridge_ = new Cartridge("mbc3rtc.gb", "");
  }

  void TearDown() override {
    delete cartridge_;
  }

  Cartridge* cartridge_;
};

TEST_F(CartridgeTest, PreviousSession) {
  cartridge_->SetRTCPreviousSessionDuration(DHMS(1, 2, 3, 4));
  cartridge_->SetRTCSessionStartTime(0);
  cartridge_->SetRTCTimeOverride(0);

  EXPECT_EQ(GetRTCSeconds(cartridge_), 4);
  EXPECT_EQ(GetRTCMinutes(cartridge_), 3);
  EXPECT_EQ(GetRTCHours(cartridge_), 2);
  EXPECT_EQ(GetRTCDaysLow(cartridge_), 1);
  EXPECT_EQ(GetRTCDaysHighAndFlags(cartridge_), 0);
}

TEST_F(CartridgeTest, PreviousSessionIncluded) {
  cartridge_->SetRTCPreviousSessionDuration(DHMS(1, 2, 3, 4));
  cartridge_->SetRTCSessionStartTime(DHMS(1, 2, 3, 4));
  cartridge_->SetRTCTimeOverride(DHMS(2, 4, 6, 8));

  EXPECT_EQ(GetRTCSeconds(cartridge_), 8);
  EXPECT_EQ(GetRTCMinutes(cartridge_), 6);
  EXPECT_EQ(GetRTCHours(cartridge_), 4);
  EXPECT_EQ(GetRTCDaysLow(cartridge_), 2);
  EXPECT_EQ(GetRTCDaysHighAndFlags(cartridge_), 0);
}

TEST_F(CartridgeTest, DateHighBit) {
  cartridge_->SetRTCPreviousSessionDuration(DHMS(255, 0, 0, 0));
  cartridge_->SetRTCSessionStartTime(0);
  cartridge_->SetRTCTimeOverride(0);

  EXPECT_EQ(GetRTCDaysLow(cartridge_), 255);
  cartridge_->SetRTCTimeOverride(DHMS(1, 0, 0, 0));
  EXPECT_EQ(GetRTCDaysLow(cartridge_), 0);
  EXPECT_EQ(GetRTCDaysHighAndFlags(cartridge_), 0x01);
}

TEST_F(CartridgeTest, DateOverflow) {
  cartridge_->SetRTCPreviousSessionDuration(DHMS(511, 0, 0, 0));
  cartridge_->SetRTCSessionStartTime(0);
  cartridge_->SetRTCTimeOverride(DHMS(1, 0, 0, 0));

  EXPECT_EQ(GetRTCDaysLow(cartridge_), 0);
  EXPECT_EQ(GetRTCDaysHighAndFlags(cartridge_), 0x80);
}

TEST_F(CartridgeTest, LatchUnlatch) {
  cartridge_->SetRTCPreviousSessionDuration(DHMS(1, 2, 3, 4));
  cartridge_->SetRTCSessionStartTime(DHMS(1, 2, 3, 4));
  cartridge_->SetRTCTimeOverride(DHMS(2, 4, 6, 8));

  EXPECT_EQ(GetRTCSeconds(cartridge_), 8);
  LatchCartrdige(cartridge_);
  cartridge_->SetRTCTimeOverride(DHMS(2, 4, 6, 9));
  EXPECT_EQ(GetRTCSeconds(cartridge_), 8);

  UnlatchCartridge(cartridge_);
  EXPECT_EQ(GetRTCSeconds(cartridge_), 9);
}

TEST_F(CartridgeTest, RTCHaltCollapses) {
  cartridge_->SetRTCPreviousSessionDuration(DHMS(1, 2, 3, 4));
  cartridge_->SetRTCSessionStartTime(DHMS(1, 2, 3, 4));
  cartridge_->SetRTCTimeOverride(DHMS(2, 4, 6, 8));
  
  cartridge_->SetRAMBankRTC(RTC_DAYS_HIGH_CARRY_HALT_REGISTER);
  cartridge_->SetRAMorRTC(0, 0x40);
  
  EXPECT_EQ(GetRTCSeconds(cartridge_), 8);
  EXPECT_EQ(GetRTCMinutes(cartridge_), 6);
  EXPECT_EQ(GetRTCHours(cartridge_), 4);
  EXPECT_EQ(GetRTCDaysLow(cartridge_), 2);
  EXPECT_EQ(GetRTCDaysHighAndFlags(cartridge_), 0x40);
}

TEST_F(CartridgeTest, RTCHaltSets) {
  cartridge_->SetRTCPreviousSessionDuration(DHMS(2, 3, 6, 8));
  cartridge_->SetRTCTimeOverride(0);
  cartridge_->SetRTCSessionStartTime(0);
  
  cartridge_->SetRAMBankRTC(RTC_DAYS_HIGH_CARRY_HALT_REGISTER);
  cartridge_->SetRAMorRTC(0, 0x40);

  cartridge_->SetRAMBankRTC(RTC_SECONDS_REGISTER);
  cartridge_->SetRAMorRTC(0, 9);

  cartridge_->SetRAMBankRTC(RTC_MINUTES_REGISTER);
  cartridge_->SetRAMorRTC(0, 15);

  cartridge_->SetRAMBankRTC(RTC_HOURS_REGISTER);
  cartridge_->SetRAMorRTC(0, 23);

  cartridge_->SetRAMBankRTC(RTC_DAYS_LOW8_REGISTER);
  cartridge_->SetRAMorRTC(0, 245);


  EXPECT_EQ(GetRTCSeconds(cartridge_), 9);
  EXPECT_EQ(GetRTCMinutes(cartridge_), 15);
  EXPECT_EQ(GetRTCHours(cartridge_), 23);
  EXPECT_EQ(GetRTCDaysLow(cartridge_), 245);
}
