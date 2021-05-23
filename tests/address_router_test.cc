#include "gtest/gtest.h"

#include "address_router.h"
#include "mmu.h"
#include "ROM.hpp"
#include "ppu.h"
#include "Utils.hpp"

class AddressRouterTest : public ::testing::Test {
 protected:
    AddressRouterTest() {};
    ~AddressRouterTest() {};
};

TEST(AddressRouterTest, PPU) {
    PPU *ppu = new PPU();
    MMU *mmu = getTestingMMU();
    AddressRouter *addressRouter = new AddressRouter(mmu, ppu, NULL, NULL, NULL, NULL, NULL);

    addressRouter->SetByteAt(0x8000, 0xed);
    ASSERT_EQ(ppu->GetByteAt(0x8000), 0xed);

    addressRouter->SetByteAt(0xFF40, 0x91);
    ASSERT_EQ(ppu->GetByteAt(0xFF40), 0x91);
    ASSERT_EQ(ppu->GetByteAt(0xFF40), addressRouter->GetByteAt(0xFF40));
}

TEST(AddressRouterTest, MMU) {
    PPU *ppu = new PPU();
    MMU *mmu = getTestingMMU();
    AddressRouter *addressRouter = new AddressRouter(mmu, ppu, NULL, NULL, NULL, NULL, NULL);

	const uint16_t INTERNAL_RAM_ADDRESS = 0xC000;
    mmu->SetByteAt(INTERNAL_RAM_ADDRESS, 0x35);
    ASSERT_EQ(mmu->GetByteAt(INTERNAL_RAM_ADDRESS), addressRouter->GetByteAt(INTERNAL_RAM_ADDRESS));
    ASSERT_EQ(mmu->GetByteAt(INTERNAL_RAM_ADDRESS), 0x35);

    mmu->SetWordAt(0xC000, 0x1234);
    ASSERT_EQ(addressRouter->GetWordAt(0xC000), 0x1234);

    addressRouter->SetWordAt(0xC123, 0x8877);
    ASSERT_EQ(addressRouter->GetWordAt(0xC123), mmu->GetWordAt(0xC123));
    ASSERT_EQ(addressRouter->GetWordAt(0xC123), 0x8877);
}
