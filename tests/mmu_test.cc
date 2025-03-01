#include "mmu.h"

#include "gtest/gtest.h"
#include "utils.h"

class MMUTest : public ::testing::Test {
 protected:
  MMUTest(){};
  ~MMUTest(){};
};

TEST(MMUTest, SetBankRange) {
  MMU *mmu = getTestingMMU();

  mmu->SetByteAt(0x2000, 0x2);
  ASSERT_EQ(mmu->rom_bank(), 2);
  mmu->SetByteAt(0x2500, 0x1);
  ASSERT_EQ(mmu->rom_bank(), 1);
  mmu->SetByteAt(0x3500, 0x2);
  ASSERT_EQ(mmu->rom_bank(), 2);
  mmu->SetByteAt(0x3FFF, 0x1);
  ASSERT_EQ(mmu->rom_bank(), 1);
}

TEST(MMUTest, ReadBank) {
  const uint16_t bank1_address = 0x45C0;
  MMU *mmu = getTestingMMU();
  // Disable ROM overlay.
  mmu->SetByteAt(0xFF50, 0x1);

  ASSERT_EQ(mmu->rom_bank(), 1);
  ASSERT_EQ(mmu->GetByteAt(bank1_address), 0x00);

  mmu->SetByteAt(0x2000, 0x2);
  ASSERT_EQ(mmu->GetByteAt(bank1_address), 0x7A);
  mmu->SetByteAt(0x2000, 0x1);
  ASSERT_EQ(mmu->GetByteAt(bank1_address), 0x00);

  mmu->SetByteAt(0x2000, 0x3);
  ASSERT_EQ(mmu->GetByteAt(bank1_address), 0x7A);
}

TEST(MMUTest, EchoWrites) {
  MMU *mmu = getTestingMMU();
  uint8_t value = 0x00;
  uint16_t delta = 0xE000 - 0xC000;
  for (uint16_t address = 0xE000; address < 0xFE00; address++) {
    mmu->SetByteAt(address, value++);
    ASSERT_EQ(mmu->GetByteAt(address), mmu->GetByteAt(address - delta));
  }

  for (uint16_t address = 0xC000; address < 0xDE00; address++) {
    mmu->SetByteAt(address, value++);
    ASSERT_EQ(mmu->GetByteAt(address), mmu->GetByteAt(address + delta));
  }
}

TEST(MMUTest, OverlayBootROM) {
  MMU *mmu = getTestingMMU();

  ASSERT_EQ(mmu->GetByteAt(0x00), 0x31);
  // Disable ROM overlay.
  mmu->SetByteAt(0xFF50, 0x1);
  ASSERT_EQ(mmu->GetByteAt(0x00), 0x3C);
}

TEST(MMUTest, RAMBankFullReadWrite) {
  MMU *mmu = getTestingMMURAM();

  // Enable RAM Bank.
  mmu->SetByteAt(0x0000, 0xA);
  for (int bank = 1; bank < 4; bank++) {
    mmu->SetByteAt(0x4000, bank);
    for (int i = 0; i < 0x2000; i++) {
      mmu->SetByteAt(0xA000 + i, bank + 10 + i % 128);
    }
  }

  for (int bank = 1; bank < 4; bank++) {
    mmu->SetByteAt(0x4000, bank);
    for (int i = 0; i < 0x2000; i++) {
      ASSERT_EQ(mmu->GetByteAt(0xA000 + i), bank + 10 + i % 128);
    }
  }
}
