#pragma once

#include <cstdint>
#include <string>

using namespace std;

enum CartridgeType {
  CartridgeType_ROM_only = 0x00,
  CartridgeType_ROM_MBC1 = 0x01,
  CartridgeType_ROM_MBC1_RAM = 0x02,
  CartridgeType_ROM_MBC1_RAM_BATT = 0x03,
  CartridgeType_ROM_MBC2 = 0x05,
  CartridgeType_ROM_MBC2_BATT = 0x06,
  CartridgeType_ROM_MBC3 = 0x11,
  CartridgeType_ROM_MBC3_RAM = 0x12,
  CartridgeType_ROM_MBC3_RAM_BATT = 0x13,
  CartridgeType_Unsupported = 0x15,
};

enum ROMSizeType {
  ROMSize_32k,
  ROMSize_64k,
  ROMSize_128k,
  ROMSize_256k,
  ROMSize_512k,
  ROMSize_1M,
  ROMSize_2M,
  ROMSize_Unsupported
};

enum RAMSizeType {
  RAMSize_0k,
  RAMSize_Unknown,
  RAMSize_8k,
  RAMSize_32k,
  RAMSize_128k,
  RAMSize_64k,
  RAMSize_Unsupported
};

uint8_t *UnsignedCartridgeBytes(string filename);

class Cartridge {
 public:
  Cartridge(string filename, const string& state_dir);
  ~Cartridge();

  bool LoadFile(string filename);
  void PrintDebugInfo();
  void SyncRAM();

  uint8_t GetROMByteAt(int address);

  CartridgeType GetCartridgeType();

  ROMSizeType GetROMSizeType();
  int ROMSize();
  int ROMBankCount();

  int RAMSize();
  RAMSizeType GetRAMSizeType();

  uint8_t GetRAMorRTC(uint16_t address);
  void SetRAMorRTC(uint16_t address, uint8_t byte);

  string GameTitle();

  void SetRAMRTCEnable(uint8_t byte);
  void SetRAMBankRTC(uint8_t byte);
  void LatchRTC(uint8_t byte);

 private:
  uint8_t *rom_;
  bool HasRTC();
  bool HasBattery();

  uint8_t ram_rtc_enable_;
  uint8_t ram_bank_rtc_;

  uint8_t rtc_latch_register_;
  bool rtc_latched_;
  uint8_t rtc_latched_value_;

  string state_dir_;
  int ram_fd_;
  void InitializeRAMFile(const std::string& state_dir);
  string GetRAMPath() const;

  uint8_t *ram_;
  uint8_t GetRAM(int address);
  void SetRAM(int address, uint8_t byte);
  int GetBankedRAMAddress(int address);

  uint8_t GetRTC();
  void SetRTC(uint8_t byte);

  bool IsMBC2();
  bool IsMBC3();
};
