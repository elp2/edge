#pragma once

#include <cstdint>

using namespace std;

class MMU;
class PPU;

enum AddressOwner {
    AdressOwner_Unknown = 0,
    AddressOwner_MMU,
    AddressOwner_PPU,
};

// Redirects reads and writes to the MMU, PPU, Sound, and potentially other controllers.
class AddressRouter {
 public:
    AddressRouter(MMU *mmu, PPU *ppu);
    ~AddressRouter() = default;

    uint8_t GetByteAt(uint16_t address);
    void SetByteAt(uint16_t address, uint8_t byte);

    uint16_t GetWordAt(uint16_t address);
    void SetWordAt(uint16_t address, uint16_t word);

 private:
    MMU *mmu_;
    PPU *ppu_;

    uint8_t GetByteAtAddressFromOwner(AddressOwner owner, uint16_t address);
    void SetByteAtAddressInOwner(AddressOwner owner, uint16_t address, uint8_t byte);
};
