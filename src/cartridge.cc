#include "cartridge.h"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>
#include <fcntl.h>
#include <fstream>
#include <unistd.h>
#include <sys/mman.h>
#include <filesystem>
#include <system_error>
#include <time.h>

#include "constants.h"
#include "utils.h"

const int RTC_SECONDS_REGISTER = 0x08;
const int RTC_MINUTES_REGISTER = 0x09;
const int RTC_HOURS_REGISTER = 0x0A;
const int RTC_DAYS_LOW8_REGISTER = 0x0B;
const int RTC_DAYS_HIGH_CARRY_HALT_REGISTER = 0x0C;
const std::string CARTRIDGE_RAM_FILENAME = "cartridge_ram.bin";

uint8_t *UnsignedCartridgeBytes(string filename) {
  ifstream file(filename, ios::in | ios::binary);
  if (!file.is_open()) {
    cout << "Could not open file!: " << filename << endl;
    return NULL;
  }

  file.seekg(0, ios::end);
  streamoff rom_size = file.tellg();
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

Cartridge::Cartridge(string filename)
  : rtc_previous_session_duration_(0),
    rtc_session_start_time_(time(nullptr)),
    rtc_current_time_override_(0),
    rtc_has_override_(false) {
  rom_ = UnsignedCartridgeBytes(filename);
  ram_ = new uint8_t[RAMSize()];
  // TODO: MBC1 uses a special addressing for large ROMS.
  assert(ROMSize() <= 524288 || GetCartridgeType() == CartridgeType_ROM_MBC3_RAM_BATT);
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
   std::cout << "HasBattery: " << HasBattery() << std::endl;
}

Cartridge::~Cartridge() {
  free(ram_);
  free(rom_);
}

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
    case 0x11:
      return CartridgeType_ROM_MBC3;
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
  if (IsMBC2()) {
    // No RAM banks, just 512 half bytes of built-RAM.
    return 512 * 4;
  }
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
  if (!HasRTC()) {
    return 0xFF;
  }
  switch (ram_bank_rtc_) {
    case RTC_SECONDS_REGISTER:
      return GetRTCSeconds();
    case RTC_MINUTES_REGISTER:
      return GetRTCMinutes();
    case RTC_HOURS_REGISTER:
      return GetRTCHours();
    case RTC_DAYS_LOW8_REGISTER:
      return GetRTCDays() & 0xFF;
    case RTC_DAYS_HIGH_CARRY_HALT_REGISTER:
      return ((GetRTCDays() >> 8) & 0x1) |
             (GetRTCHalted() ? 0x40 : 0) |
             (GetRTCDayCarry() ? 0x80 : 0);
    default:
      std::cout << "Unknown RTC Register: " << std::hex << int(ram_bank_rtc_) << std::endl;
      assert(false);
      return 0;
  }
}

void Cartridge::SetRTC(uint8_t byte) {
  if (ram_bank_rtc_ == RTC_DAYS_HIGH_CARRY_HALT_REGISTER) {
    bool was_halted = GetRTCHalted();
    rtc_halted_ = (byte & 0x40) != 0;
    if (!was_halted && GetRTCHalted()) {
      // Collapse all time into the previous session.
      rtc_previous_session_duration_ = ElapsedRTCTime();
      rtc_session_start_time_ = 0;
      rtc_current_time_override_ = 0;
      rtc_has_override_ = true;
    } else if (was_halted && !GetRTCHalted()) {
      rtc_session_start_time_ = time(nullptr);
      rtc_has_override_ = false;
    }
  }

  // std::cout << "PSD : " << int(rtc_previous_session_duration_) << " byte: " << std::dec << int(byte) << std::endl;
  std::cout << int(GetRTCSeconds()) << ":" << int(GetRTCMinutes()) << ":" << int(GetRTCHours()) << ": " << int(GetRTCDays()) << std::endl;
  std::cout << std::dec << int(byte) << std::endl;  


  switch (ram_bank_rtc_) {
    case RTC_SECONDS_REGISTER:
      rtc_previous_session_duration_ += std::min(byte & 0x3F, 60) - GetRTCSeconds();
      break;
    case RTC_MINUTES_REGISTER:
      rtc_previous_session_duration_ += 60 * (std::min(byte & 0x3F, 60) - GetRTCMinutes());
      break;
    case RTC_HOURS_REGISTER:
      rtc_previous_session_duration_ += 3600 * (std::min(byte & 0x1F, 24) - GetRTCHours());
      break;
    case RTC_DAYS_LOW8_REGISTER:
      rtc_previous_session_duration_ += 86400 * (byte - GetRTCDays() % 256);
      break;
    case RTC_DAYS_HIGH_CARRY_HALT_REGISTER:
      rtc_previous_session_duration_ += 86400 * (bit_set(byte, 0) ? 256 : 0 - (GetRTCDays() & 0x100));
      break;
  }
  std::cout << " After " << int(rtc_previous_session_duration_) << std::endl;
  std::cout << int(GetRTCSeconds()) << ":" << int(GetRTCMinutes()) << ":" << int(GetRTCHours()) << ": " << int(GetRTCDays()) << std::endl;

  if (!GetRTCHalted()) {
    // There is a race condition here, but it's OK since they should have set the halting.
    rtc_session_start_time_ = time(nullptr);
  }
}

uint8_t Cartridge::GetRAM(int address) {
  int banked_address = GetBankedRAMAddress(address);
  if (banked_address >= RAMSize()) {
    // Ignore reads from out of bounds RAM (restoring, cartridge errors).
    return 0xFF;
  }
  uint8_t byte = ram_[banked_address];
  if (IsMBC2()) {
    // Half bytes of RAM - upper 4 bits are unreliable.
    byte &= 0x0F;
  }
  return byte;
}

void Cartridge::SetRAM(int address, uint8_t byte) {
  if (RAMSize() == 0) {
    // Ignore all writes to RAM when there is none. For example, Mario clears all memory inclding non-existent RAM.
    return;
  }
  int banked_address = GetBankedRAMAddress(address);
  if (banked_address >= RAMSize()) {
    // Ignore writes to out of bounds RAM (restoring, cartridge errors).
    return;
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

void Cartridge::SetRAMRTCEnable(uint8_t byte) {
  if (byte > 0x0A) {
    // Zelda sets 0xFF when it wants to disable.
    byte = 0;
  }
  ram_rtc_enable_ = byte;
}

void Cartridge::SetRAMBankRTC(uint8_t byte) {
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
  if (!HasRTC()) {
    return;
  }
  bool will_latch = (byte == 0x01 && rtc_latch_register_ == 0x00);
  if (will_latch) {
    rtc_latched_time_ = GetCurrentRTCTime();
  }
  rtc_latch_register_ = byte;
  rtc_latched_ = will_latch;
}

bool Cartridge::HasRTC() {
  return GetCartridgeType() == CartridgeType_ROM_MBC3 ||
         GetCartridgeType() == CartridgeType_ROM_MBC3_RAM ||
         GetCartridgeType() == CartridgeType_ROM_MBC3_RAM_BATT;
}

bool Cartridge::IsMBC2() {
  return GetCartridgeType() == CartridgeType_ROM_MBC2 || 
         GetCartridgeType() == CartridgeType_ROM_MBC2_BATT;
}

bool Cartridge::IsMBC3() {
  return GetCartridgeType() == CartridgeType_ROM_MBC3 ||
         GetCartridgeType() == CartridgeType_ROM_MBC3_RAM || 
         GetCartridgeType() == CartridgeType_ROM_MBC3_RAM_BATT;
}

bool Cartridge::HasBattery() {
  return GetCartridgeType() == CartridgeType_ROM_MBC1_RAM_BATT ||
      GetCartridgeType() == CartridgeType_ROM_MBC2_BATT ||
      GetCartridgeType() == CartridgeType_ROM_MBC3_RAM_BATT;
}

time_t Cartridge::GetCurrentRTCTime() const {
  if (rtc_latched_) {
    return rtc_latched_time_;
  } else if (rtc_has_override_) {
    return rtc_current_time_override_;
  } else {
    return time(nullptr);
  }
}

time_t Cartridge::ElapsedRTCTime() const {
  return rtc_previous_session_duration_ + (GetCurrentRTCTime() - rtc_session_start_time_);
}

uint8_t Cartridge::GetRTCSeconds() const {
  return ElapsedRTCTime() % 60;
}

uint8_t Cartridge::GetRTCMinutes() const {
  return (ElapsedRTCTime() / 60) % 60;
}

uint8_t Cartridge::GetRTCHours() const {
  return (ElapsedRTCTime() / 3600) % 24;
}

uint16_t Cartridge::GetRTCDays() const {
  return (ElapsedRTCTime() / 86400) % 512;
}

bool Cartridge::GetRTCHalted() const {
  return rtc_halted_;
}

bool Cartridge::GetRTCDayCarry() const {
  return (ElapsedRTCTime() / 86400) >= 512;
}

void Cartridge::SetState(const struct CartridgeSaveState &state) {
  rtc_previous_session_duration_ = state.rtc_previous_session_duration;
  rtc_session_start_time_ = state.rtc_session_start_time;
  rtc_current_time_override_ = state.rtc_current_time_override;
  rtc_has_override_ = state.rtc_has_override;
  rtc_latch_register_ = state.rtc_latch_register;
  rtc_latched_ = state.rtc_latched;
  rtc_latched_time_ = state.rtc_latched_time;
  rtc_halted_ = state.rtc_halted;
  
  if (state.ram && state.ram_size > 0) {
    memcpy(ram_, state.ram, std::min<uint32_t>(state.ram_size, RAMSize()));
  }
}

void Cartridge::GetState(struct CartridgeSaveState& state) {
  state.rtc_previous_session_duration = rtc_previous_session_duration_;
  state.rtc_session_start_time = rtc_session_start_time_;
  state.rtc_current_time_override = rtc_current_time_override_;
  state.rtc_has_override = rtc_has_override_;
  state.rtc_latch_register = rtc_latch_register_;
  state.rtc_latched = rtc_latched_;
  state.rtc_latched_time = rtc_latched_time_;
  state.rtc_halted = rtc_halted_;
  state.ram_size = RAMSize();
  state.ram = ram_;
}

