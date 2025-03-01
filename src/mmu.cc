#include "mmu.h"

#include <cassert>
#include <iostream>

#include "constants.h"
#include "utils.h"

using namespace std;

MMU::MMU() {
  ram_ = new uint8_t[0x8000];
  boot_rom_ = NULL;
  cartridge_ = NULL;
  overlay_boot_rom_ = false;
  high_memory_ = new uint8_t[HIGH_RAM_END - HIGH_RAM_START + 1];
}

void MMU::SetBootROM(uint8_t *bytes) {
  boot_rom_ = bytes;
  overlay_boot_rom_ = true;
}

void MMU::SetCartridge(Cartridge *cartridge) {
  cartridge_ = cartridge;
  assert(cartridge_->GetCartridgeType() != CartridgeType_Unsupported);
  assert(cartridge_->GetROMSizeType() != ROMSize_Unsupported);
  rom_bank_ = 1;
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
  return overlay_boot_rom_ && address < 256;
}

uint8_t MMU::GetByteAt(uint16_t address) {
  if (disasembler_mode_) {
    return 0xED;
  }

  if (overlay_boot_rom_ && address < 0x8000) {
    if (UseBootROMForAddress(address)) {
      return boot_rom_[address];
    } else if (address > 0x14E) {
      cout << "ROM access above logo while overlaid at 0x" << hex
           << unsigned(address) << endl;
      assert(false);
    } else {
      assert(false);
    }
  }

  if (address >= ROM_BANK_0_START && address <= ROM_BANK_0_END) {
    return cartridge_->GetROMByteAt(address);
  } else if (address >= ROM_BANK_1_START && address <= ROM_BANK_1_END) {
    // TODO: Handle switchable ROM banks.
    return cartridge_->GetROMByteAt(address - ROM_BANK_1_START + rom_bank_ * 0x4000);
  } else if (address >= VIDEO_RAM_START && address <= VIDEO_RAM_END) {
    // PPU should handle this.
    assert(false);
  } else if (address >= EXTERNAL_RAM_START && address <= EXTERNAL_RAM_END) {
    return cartridge_->GetRAMorRTC(address - EXTERNAL_RAM_START);
  } else if (address >= WORK_RAM_START && address <= WORK_RAM_END) {
    return GetRAM(address - WORK_RAM_START);
  } else if (address >= ECHO_RAM_START && address <= ECHO_RAM_END) {
    return GetRAM(address - ECHO_RAM_START);
  } else if (address >= OAM_RAM_START && address <= OAM_RAM_END) {
    // PPU should handle this.
    assert(false);
  } else if (address >= FORBIDDEN_RAM_START && address <= FORBIDDEN_RAM_END) {
    assert(false);
  } else if (address >= IO_RAM_START && address <= IO_RAM_END) {
    // Hanled Elsewhere
    assert(false);
  } else if (address >= HIGH_RAM_START && address <= HIGH_RAM_END) {
    return high_memory_[address - HIGH_RAM_START];
  } 

  std::cout << "Unknown access to 0x" << hex << unsigned(address) << endl;
  assert(false);
  return 0;
}

uint16_t MMU::GetWordAt(uint16_t address) {
  if (disasembler_mode_) {
    return 0xed02;
  }
  uint8_t lsb = GetByteAt(address);
  uint16_t result = (GetByteAt(address + 1) << 8) | lsb;
  return result;
}

void MMU::SetByteAt(uint16_t address, uint8_t byte) {
  if (disasembler_mode_) {
    return;
  }
  // if (overlay_boot_rom_ && address < 0x8000) {
  //   assert(false);
  // }
  
  if (address >= RAM_RTC_ENABLE_REGISTER_START && address <= RAM_RTC_ENABLE_REGISTER_END) {
    std::cout << "SetRAMRTCEnable: " << std::hex << (int)address << " = 0x" << (int)byte << std::endl;
    cartridge_->SetRAMRTCEnable(byte);
    return;
  } else if (address >= ROM_BANK_SELECT_REGISTER_START && address <= ROM_BANK_SELECT_REGISTER_END) {
    register_2000_3fff_ = byte;
    UpdateROMBank();
    return;
  } else if (address >= RAM_BANK_RTC_SELECT_REGISTER_START && address <= RAM_BANK_RTC_SELECT_REGISTER_END) {
    cartridge_->SetRAMBankRTC(byte);
    return;
  } else if (address >= LATCH_RTC_REGISTER_START && address <= LATCH_RTC_REGISTER_END) {
    cartridge_->LatchRTC(byte);
    return;
  } else if (address >= VIDEO_RAM_START && address <= VIDEO_RAM_END) {
    // PPU should handle this.
    assert(false);
  } else if (address >= EXTERNAL_RAM_START && address <= EXTERNAL_RAM_END) {
    cartridge_->SetRAMorRTC(address - EXTERNAL_RAM_START, byte);
  } else if (address >= WORK_RAM_START && address <= WORK_RAM_END) {
    SetRAM(address - WORK_RAM_START, byte);
  } else if (address >= ECHO_RAM_START && address <= ECHO_RAM_END) {
    SetRAM(address - ECHO_RAM_START, byte);
  } else if (address >= OAM_RAM_START && address <= OAM_RAM_END) {
    // PPU should handle this.
    assert(false);
  } else if (address >= FORBIDDEN_RAM_START && address <= FORBIDDEN_RAM_END) {
    cout << "FORBIDDEN RAM: ";
    cout << AddressRegion(address) << "[0x" << hex << unsigned(address)
          << "]";
    cout << " = 0x" << hex << unsigned(byte) << " (SET)" << endl;
    // assert(false);
  } else if (address >= HIGH_RAM_START && address <= HIGH_RAM_END) {
    high_memory_[address - HIGH_RAM_START] = byte;
    return;
  } else if (address == 0xFF50) {
    overlay_boot_rom_ = false;
    cout << "**** REMOVED OVERLAY BOOT ROM ***" << endl;
  } else if (address >= IO_RAM_START && address <= IO_RAM_END) {
    cout << "IO RAM: ";
    cout << AddressRegion(address) << "[0x" << hex << unsigned(address)
          << "]";
    cout << " = 0x" << hex << unsigned(byte) << " (SET)" << endl;
  } else {
    cout << "Can't Write to: ";
    cout << AddressRegion(address) << "[0x" << hex << unsigned(address)
          << "]";
    cout << " = 0x" << hex << unsigned(byte) << " (SET)" << endl;

    assert(false);
  }
}

uint8_t MMU::GetRAM(uint16_t address) {
  assert(address <= WORK_RAM_END - WORK_RAM_START);
  return ram_[address];
}

void MMU::SetRAM(uint16_t address, uint8_t byte) {
  // TODO: For CBB, the second half is switchable.
  assert(address <= WORK_RAM_END - WORK_RAM_START);
  ram_[address] = byte;
}

void MMU::UpdateROMBank() {
  uint8_t rom_bank = register_2000_3fff_ & 0x7;
  rom_bank_ = rom_bank;
  if (rom_bank == 0) {
    cout << "ROM bank 0 is not allowed. Switching to bank 1." << endl;
    rom_bank_ = 1;
  }
  cout << "Switched to ROM bank: 0x" << hex << unsigned(rom_bank_) << endl;

  assert(rom_bank_ < cartridge_->ROMBankCount());
}

void MMU::SetWordAt(uint16_t address, uint16_t word) {
  if (disasembler_mode_) {
    return;
  }

  SetByteAt(address, LOWER8(word));
  SetByteAt(address + 1, HIGHER8(word));
}

MMU::~MMU() {}
