#pragma once

#include <cstdint>
#include <string>
#include <time.h>

#include "state.h"

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

  time_t ElapsedRTCTime() const;
  void LatchRTC(uint8_t byte);
  void SetRTCPreviousSessionDuration(time_t duration) { rtc_previous_session_duration_ = duration; }
  void SetRTCSessionStartTime(time_t start) { rtc_session_start_time_ = start; }
  void SetRTCTimeOverride(time_t override_time) {
    rtc_current_time_override_ = override_time;
    rtc_has_override_ = true;
  }
  void ClearRTCTimeOverride() { rtc_has_override_ = false; }

  time_t GetRTCPreviousSessionDuration() const { return rtc_previous_session_duration_; }
  time_t GetRTCSessionStartTime() const { return rtc_session_start_time_; }
  time_t GetRTCTimeOverride() const { return rtc_current_time_override_; }

  void SetState(const struct CartridgeSaveState &state);
  void GetState(struct CartridgeSaveState& state);
  void SaveRAMSnapshot(const std::string& target_dir);
  void LoadRAMSnapshot(const std::string& source_dir);

 private:
  uint8_t *rom_;
  bool HasRTC();
  bool HasBattery();

  uint8_t ram_rtc_enable_;
  uint8_t ram_bank_rtc_;

  uint8_t rtc_latch_register_;
  bool rtc_latched_;
  time_t rtc_latched_time_;

  uint8_t *ram_;
  uint8_t GetRAM(int address);
  void SetRAM(int address, uint8_t byte);
  int GetBankedRAMAddress(int address);

  uint8_t GetRTC();
  void SetRTC(uint8_t byte);

  bool IsMBC2();
  bool IsMBC3();

  time_t rtc_previous_session_duration_;
  time_t rtc_session_start_time_;
  time_t rtc_current_time_override_;
  bool rtc_has_override_;
  bool rtc_halted_;

  uint8_t GetRTCSeconds() const;
  uint8_t GetRTCMinutes() const;
  uint8_t GetRTCHours() const;
  uint16_t GetRTCDays() const;
  bool GetRTCHalted() const;
  bool GetRTCDayCarry() const;

  time_t GetCurrentRTCTime() const;
};
