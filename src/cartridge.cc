#include "cartridge.h"

#include <cassert>
#include <fstream>
#include <iostream>

#include "constants.h"

const int RTC_SECONDS_REGISTER = 0x08;
const int RTC_MINUTES_REGISTER = 0x09;
const int RTC_HOURS_REGISTER = 0x0A;
const int RTC_DAYS_LOW8_REGISTER = 0x0B;
const int RTC_DAYS_HIGH_CARRY_HALT_REGISTER = 0x0C;

uint8_t *UnsignedCartridgeBytes(string filename) {
  ifstream file(filename, ios::in | ios::binary);
  if (!file.is_open()) {
    cout << "Could not open file!: " << filename << endl;
    return NULL;
  }

  file.seekg(0, ios::end);
  uint32_t rom_size = file.tellg();
  file.seekg(0, ios::beg);
  uint8_t *rom = new uint8_t[rom_size];
  file.seekg(0, ios::beg);
  char *unsigned_contents = new char[rom_size];
  file.read(unsigned_contents, rom_size);
  for (uint32_t i = 0; i < rom_size; i++) {
    rom[i] = unsigned_contents[i];
  }
  file.close();

  return rom;
}

Cartridge::Cartridge(string filename) {
   rom_ = UnsignedCartridgeBytes(filename);
   ram_ = new uint8_t[RAMSize()];
   // TODO: MBC1 uses a special addressing for large ROMS.
   assert(ROMSize() <= 524288 || GetCartridgeType() == CartridgeType_ROM_MBC3_RAM_BATT);

  if (SUPER_DEBUG) {
    PrintDebugInfo();
  }
}

void Cartridge::PrintDebugInfo() {
   std::cout << "Game Title: " << GameTitle() << std::endl;
   std::cout << "Cartridge Type: 0x" << hex << GetCartridgeType() << std::endl;
   std::cout << "ROMSizeType: 0x" << hex << GetROMSizeType() << std::endl;
   std::cout << "ROM Bank Count: 0x" << hex << ROMBankCount() << std::endl;
   std::cout << "ROM Size: 0x" << hex << ROMSize() << std::endl;
   std::cout << "RAMSizeType: 0x" << hex << GetRAMSizeType() << std::endl;
   std::cout << "RAM Size: 0x" << hex << RAMSize() << std::endl;
   std::cout << "HasRTC: " << HasRTC() << std::endl;
}

Cartridge::~Cartridge() {}

uint8_t Cartridge::GetROMByteAt(int address) {
  assert(rom_);
  assert(address < ROMSize());

  return rom_[address];
}

CartridgeType Cartridge::GetCartridgeType() {
  uint16_t ctbyte = rom_[0x147];
  switch (ctbyte) {
    case 0x00:
      return CartridgeType_ROM_only;
    case 0x01:
      return CartridgeType_ROM_MBC1;
    case 0x02:
      return CartridgeType_ROM_MBC1_RAM;
    case 0x03:
      return CartridgeType_ROM_MBC1_RAM_BATT;
    case 0x05:
      return CartridgeType_ROM_MBC2;
    case 0x06:
      return CartridgeType_ROM_MBC2_BATT;
    case 0x10:
      return CartridgeType_ROM_MBC3;
    case 0x11:
      return CartridgeType_ROM_MBC3_BATT;
    case 0x12:
      return CartridgeType_ROM_MBC3_RAM;
    case 0x13:
      return CartridgeType_ROM_MBC3_RAM_BATT;
    default:
      std::cout << "Unsupported cartridge type: " << std::hex << (int)ctbyte << std::endl;
      return CartridgeType_Unsupported;
  }
}

ROMSizeType Cartridge::GetROMSizeType() {
  uint16_t ctbyte = rom_[0x148];
  switch (ctbyte) {
    case 0x00:
      return ROMSize_32k;
    case 0x01:
      return ROMSize_64k;
    case 0x02:
      return ROMSize_128k;
    case 0x03:
      return ROMSize_256k;
    case 0x04:
      return ROMSize_512k;
    case 0x05:
      return ROMSize_1M;
    case 0x06:
      return ROMSize_2M;
    default:
      return ROMSize_Unsupported;
  }
}

int Cartridge::ROMSize() {
  return 0x4000 * ROMBankCount();
}

RAMSizeType Cartridge::GetRAMSizeType() {
  uint16_t ctbyte = rom_[0x149];
  switch (ctbyte) {
    case 0x00:
      return RAMSize_0k;
    case 0x01:
      std::cout << "Impossible RAM size: " << std::hex << (int)ctbyte << std::endl;
      assert(false);
      return RAMSize_Unknown;
    case 0x02:
      return RAMSize_8k;
    case 0x03:
      return RAMSize_32k;
    case 0x04:
      return RAMSize_128k;
    case 0x05:
      return RAMSize_64k;
    default:
      std::cout << "Unsupported RAM size: " << std::hex << (int)ctbyte << std::endl;
      assert(false);
      return RAMSize_Unsupported;
  }
}

int Cartridge::RAMSize() {
  switch (GetRAMSizeType()) {
    case RAMSize_0k:
      return 0;
    case RAMSize_8k:
      return 8192;
    case RAMSize_32k:
      return 32768;
    case RAMSize_128k:
      return 131072;
    case RAMSize_64k:
      return 65536;
    default:
      assert(false);
      return 0;
  }
}

std::string Cartridge::GameTitle() {
  assert(rom_);
  int TITLE_START = 0x134;
  int TITLE_MAX_LENGTH = 16;

  string gameTitle = std::string();
  for (int i = TITLE_START; i < TITLE_START + TITLE_MAX_LENGTH; i++) {
    if (rom_[i] == 0x00) {
      break;
    }
    gameTitle.push_back(rom_[i]);
  }
  return gameTitle;
}

uint8_t Cartridge::GetRAMorRTC(uint16_t address) {
  if (ram_bank_rtc_ >= RTC_SECONDS_REGISTER) {
    return GetRTC();
  }
  return GetRAM(address);
}

void Cartridge::SetRAMorRTC(uint16_t address, uint8_t byte) {
  if (ram_bank_rtc_ >= RTC_SECONDS_REGISTER) {
    SetRTC(byte);
  } else {
    SetRAM(address, byte);
  }
}

uint8_t Cartridge::GetRTC() {
  assert(HasRTC());
  if (rtc_latched_) {
    return rtc_latched_value_;
  }
  std::cout << "TODO: GetRTC. Temporary return 7." << std::endl;
  return 7;
}

void Cartridge::SetRTC(uint8_t byte) {
  assert(false);
}

uint8_t Cartridge::GetRAM(int address) {
  int banked_address = GetBankedRAMAddress(address);
  if (banked_address >= RAMSize()) {
    std::cout << "GetRAM: " << std::hex << address << " size: " << std::hex << RAMSize() << std::endl;
    assert(false);
  }

  return ram_[banked_address];
}

void Cartridge::SetRAM(int address, uint8_t byte) {
  int banked_address = GetBankedRAMAddress(address);
  if (banked_address >= RAMSize()) {
    std::cout << "SetRAM: " << std::hex << address << " size: " << std::hex << RAMSize() << std::endl;
    assert(false);
  }

  ram_[banked_address] = byte;
}

int Cartridge::GetBankedRAMAddress(int address) {
  assert(ram_bank_rtc_ < RTC_SECONDS_REGISTER);
  return address + (int(ram_bank_rtc_) * 0x2000);
}

int Cartridge::ROMBankCount() {
  switch (GetROMSizeType()) {
    case ROMSize_32k:
      return 2;
    case ROMSize_64k:
      return 4;
    case ROMSize_128k:
      return 8;
    case ROMSize_256k:
      return 16;
    case ROMSize_512k:
      return 32;
    case ROMSize_1M:
      return 64;
    case ROMSize_2M:
      return 128;
    default:
      assert(false);
      return 0;
  }
}

bool Cartridge::HasRAM() {
  return GetCartridgeType() == CartridgeType_ROM_MBC1_RAM ||
         GetCartridgeType() == CartridgeType_ROM_MBC1_RAM_BATT ||
         GetCartridgeType() == CartridgeType_ROM_MBC3_RAM ||
         GetCartridgeType() == CartridgeType_ROM_MBC3_RAM_BATT;
}

void Cartridge::SetRAMRTCEnable(uint8_t byte) {
  if (byte > 0x0A) {
    std::cout << "Unexpected too high value for SetRAMRTCEnable - will treat as disable: " << std::hex << (int)byte << std::endl;
    byte = 0;
  }
  ram_rtc_enable_ = byte;
}

void Cartridge::SetRAMBankRTC(uint8_t byte) {
  assert(HasRAM());
  if (byte > RTC_DAYS_HIGH_CARRY_HALT_REGISTER) {
    std::cout << "Unexpected too high value for SetRAMBankRTC: " << std::hex << (int)byte << std::endl;
    assert(false);
  }
  if (byte >= RTC_SECONDS_REGISTER) {
    assert(HasRTC());
  }
  ram_bank_rtc_ = byte;
}

void Cartridge::LatchRTC(uint8_t byte) {
  assert(HasRTC());
  rtc_latched_ = (byte == 0x01 && rtc_latch_register_ == 0x00);
  rtc_latch_register_ = byte;
  rtc_latched_value_ = GetRTC();
}

bool Cartridge::HasRTC() {
  return GetCartridgeType() == CartridgeType_ROM_MBC3 ||
         GetCartridgeType() == CartridgeType_ROM_MBC3_BATT ||
         GetCartridgeType() == CartridgeType_ROM_MBC3_RAM ||
         GetCartridgeType() == CartridgeType_ROM_MBC3_RAM_BATT;
}
