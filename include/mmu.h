#pragma once

#include <string>

#include "ROM.hpp"

using namespace std;

class MMU {

public:
    MMU();
    ~MMU();

    uint8_t GetByteAt(uint16_t address);
    void SetByteAt(uint16_t address, uint8_t byte);
    // LS Byte First.
    uint16_t GetWordAt(uint16_t address);
    void SetWordAt(uint16_t address, uint16_t word);

    // Returns the max 16 character upper case game title.
    std::string GameTitle();

    void SetROMs(ROM *bootROM, ROM *cartridgeROM);

    // Hacks to simulate a disassembler.
    void EnableDisassemblerMode(bool disassemblerMode) { disasemblerMode_ = disassemblerMode; };

    uint8_t bank() { return bank_; };
private:
    bool UseBootROMForAddress(uint16_t address);
    string AddressRegion(uint16_t address);
    void UpdateROMBank(uint8_t byte);
    void UpdateRAMBank(uint16_t address, uint8_t byte);

    uint8_t *ram;

    bool disasemblerMode_ = false;
    bool overlayBootROM;
    ROM *bootROM;
    ROM *cartridgeROM;
    uint8_t bank_ = 0x1;

    uint8_t *switchable_ram_bank_;
    uint8_t switchable_ram_bank_active_ = 0x0;
    uint8_t switchable_ram_bank_count_ = 0x0;
    // Must be set explicitly before reading or writing RAM banks.
    bool switchable_ram_bank_enabled_ = false;
};