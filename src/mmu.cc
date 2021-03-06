#include "mmu.h"

#include <cassert>
#include <iostream>

#include "utils.h"

using namespace std;

MMU::MMU() {
  ram = new uint8_t[0x8000];
  // Create a bank big enough for the 32KB RAM if necessary.
  switchable_ram_bank_ = new uint8_t[0x10000];
  bootROM = NULL;
  cartridgeROM = NULL;
  overlayBootROM = true;
}

void MMU::SetROMs(ROM *bootROM, ROM *cartridgeROM) {
  overlayBootROM = true;
  this->bootROM = bootROM;
  this->cartridgeROM = cartridgeROM;

  assert(this->cartridgeROM->GetCartridgeType() != CartridgeType_Unsupported);
  assert(this->cartridgeROM->GetROMSizeType() != ROMSize_Unsupported);
  bank_ = 1;
}

string MMU::AddressRegion(uint16_t address) {
  if (UseBootROMForAddress(address)) {
    return "BOOT ROM";
  } else if (address < 0x4000) {
    return "ROM Bank 0 (16kb)";
  } else if (address < 0x8000) {
    return "ROM Bank 1 (switchable)";
  } else if (address < 0xa000) {
    return "Video RAM";
  } else if (address < 0xC000) {
    return "Switchable RAM";
  } else if (address < 0xE000) {
    return "Internal RAM";
  } else if (address < 0xFE00) {
    return "Echo of 8k Internal RAM";
  } else if (address < 0xFEA0) {
    return "OAM";
  } else if (address < 0xFF00) {
    return "Empty i/o";
  } else if (address < 0xFF4C) {
    return "i/o ports";
  } else if (address < 0xFF80) {
    return "Empty i/o (2)";
  } else if (address < 0xFFFF) {
    return "Internal RAM";
  } else {
    return "Interrupt Enable Register";
  }
}

bool MMU::UseBootROMForAddress(uint16_t address) {
  return overlayBootROM && address < bootROM->Size();
}

uint8_t MMU::GetByteAt(uint16_t address) {
  if (disasemblerMode_) {
    return 0xed;
  }

  uint8_t byte;
  if (UseBootROMForAddress(address)) {
    byte = bootROM->GetByteAt(address);
  } else if (address < 0x8000) {
    if (overlayBootROM && address > 0x14E) {
      cout << "ROM access above logo while overlaid at 0x" << hex
           << unsigned(address) << endl;
      assert(false);
    }
    if (address < 0x4000) {
      byte = cartridgeROM->GetByteAt(address);
    } else {
      uint16_t bank_address = address + (bank() - 1) * 0x4000;
      byte = cartridgeROM->GetByteAt(bank_address);
    }
  } else if (address >= 0xA000 && address < 0xC000) {
    assert(cartridgeROM->GetCartridgeType() == CartridgeType_ROM_MBC1_RAM);
    return switchable_ram_bank_[address - 0xA000 +
                                switchable_ram_bank_active_ * 0x2000];
  } else {
    byte = ram[address - 0x8000];
  }

  // cout << AddressRegion(address) << "[0x" << hex << unsigned(address) << "]";
  // cout << " 0x" << hex << unsigned(byte) << " (GET) " << endl;

  return byte;
}

uint16_t MMU::GetWordAt(uint16_t address) {
  if (disasemblerMode_) {
    return 0xed02;
  }
  uint8_t lsb = GetByteAt(address);
  uint16_t result = (GetByteAt(address + 1) << 8) | lsb;
  return result;
}

void MMU::SetByteAt(uint16_t address, uint8_t byte) {
  if (disasemblerMode_) {
    return;
  }

  if (address >= 0x2000 && address <= 0x3FFF) {
    UpdateROMBank(byte);
    return;
  } else if (address < 0x8000) {
    if (cartridgeROM->GetCartridgeType() == CartridgeType_ROM_MBC1_RAM) {
      UpdateRAMBank(address, byte);
    } else {
      cout << "Can't Write to: ";
      cout << AddressRegion(address) << "[0x" << hex << unsigned(address)
           << "]";
      cout << " = 0x" << hex << unsigned(byte) << " (SET)" << endl;

      assert(false);
    }
    return;
  } else if (address >= 0xA000 && address < 0xC000) {
    assert(switchable_ram_bank_enabled_);
    switchable_ram_bank_[address - 0xA000 +
                         switchable_ram_bank_active_ * 0x2000] = byte;
    return;
  }
  // cout << AddressRegion(address) << "[0x" << hex << unsigned(address) << "]";
  // cout << " = 0x" << hex << unsigned(byte) << " (SET)" << endl;

  if (address == 0xFF50) {
    overlayBootROM = false;
    cout << "**** REMOVED OVERLAY BOOT ROM ***" << endl;
  }

  // TODO: Test general setting.
  // TODO: Probably shouldn't be setting the ROM, how does RAM work?
  ram[address - 0x8000] = byte;

  // Echo of Internal 8 Bit RAM.
  if (address >= 0xE000 && address < 0xFE00) {
    uint16_t echoAddress = 0xC000 + (address - 0xE000);
    ram[echoAddress - 0x8000] = byte;
  }

  if (address >= 0xC000 && address < 0xDE00) {
    uint16_t echoAddress = 0xE000 + (address - 0xC000);
    ram[echoAddress - 0x8000] = byte;
  }
}

void MMU::UpdateROMBank(uint8_t byte) {
  if (byte == 0x00) {
    // Bank 0 is always mapped low.
    byte = 0x01;
  }
  bank_ = byte;
  cout << "Switched to ROM bank: 0x" << hex << unsigned(bank_) << endl;
  int max_bank;
  switch (cartridgeROM->GetROMSizeType()) {
    case ROMSize_32k:
      max_bank = 2;
      break;
    case ROMSize_64k:
      max_bank = 4;
      break;
    case ROMSize_128k:
      max_bank = 8;
      break;
    case ROMSize_256k:
      max_bank = 16;
      break;
    case ROMSize_512k:
      max_bank = 32;
      break;
    default:
      assert(false);
      break;
  }
  assert(bank_ < max_bank);
}

void MMU::UpdateRAMBank(uint16_t address, uint8_t byte) {
  if (address <= 0x1FFF) {
    switchable_ram_bank_enabled_ = byte & 0xA;
  } else if (address >= 0x4000 && address <= 0x5FFF) {
    uint8_t bank_num = byte & 0b11;
    assert(bank_num < switchable_ram_bank_count_);
    switchable_ram_bank_active_ = bank_num;
    switchable_ram_bank_enabled_ =
        false;  // TODO - does this need to be per switchable bank?
  } else if (address >= 0x6000 && address <= 0x7FFF) {
    if (byte & 0b1) {
      switchable_ram_bank_count_ = 4;
    } else {
      switchable_ram_bank_count_ = 1;
    }
  } else {
    assert(false);
  }
}

void MMU::SetWordAt(uint16_t address, uint16_t word) {
  if (disasemblerMode_) {
    return;
  }

  SetByteAt(address, LOWER8(word));
  SetByteAt(address + 1, HIGHER8(word));
}

MMU::~MMU() {}