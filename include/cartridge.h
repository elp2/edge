#pragma once

#include <cstdint>
#include <string>

using namespace std;

enum CartridgeType {
  CartridgeType_ROM_only,
  CartridgeType_ROM_MBC1,
  CartridgeType_ROM_MBC1_RAM,
  CartridgeType_ROM_MBC1_RAM_BATT,
  CartridgeType_ROM_MBC2,
  CartridgeType_ROM_MBC2_BATT,
  CartridgeType_ROM_MBC3,
  CartridgeType_ROM_MBC3_BATT,
  CartridgeType_ROM_MBC3_RAM,
  CartridgeType_ROM_MBC3_RAM_BATT,
  CartridgeType_Unsupported
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
  Cartridge(string filename);
  ~Cartridge();

  bool LoadFile(string filename);
  void PrintDebugInfo();

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
  bool HasRAM();
  bool HasRTC();

  uint8_t ram_rtc_enable_;
  uint8_t ram_bank_rtc_;

  uint8_t rtc_latch_register_;
  bool rtc_latched_;
  uint8_t rtc_latched_value_;

  uint8_t *ram_;
  uint8_t GetRAM(int address);
  void SetRAM(int address, uint8_t byte);
  int GetBankedRAMAddress(int address);

  uint8_t GetRTC();
  void SetRTC(uint8_t byte);
};
