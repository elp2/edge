#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct CPUSaveState {
  uint8_t a;
  uint8_t f;
  uint8_t b;
  uint8_t c;
  uint8_t d;
  uint8_t e;
  uint8_t h;
  uint8_t l;
  uint16_t sp;
  uint16_t pc;
  bool flag_z;
  bool flag_h;
  bool flag_n;
  bool flag_c;
};

struct PPUSaveState {
  uint8_t lcdc;
  uint8_t stat;
  uint8_t scy;
  uint8_t scx;
  uint8_t ly;
  uint8_t lyc;
  uint8_t bgp;
  uint8_t obp0;
  uint8_t obp1;
  uint8_t wy;
  uint8_t wx;
};

struct TimerSaveState {
  uint8_t div;
  uint8_t tima;
  uint8_t tma;
  uint8_t tac;
};

struct CartridgeSaveState {
  time_t rtc_previous_session_duration;
  time_t rtc_session_start_time;
  bool rtc_has_override;
  time_t rtc_current_time_override;
};

struct MemorySaveState {
  uint8_t ram[0x8000];
};

struct MMUSaveState {
  bool overlay_boot_rom;
  uint8_t rom_bank;
  uint8_t ram_bank;
  uint8_t switchable_ram_bank_active;
  bool switchable_ram_bank_enabled;
  uint8_t register_2000_3fff;
};

struct SaveState {
  static constexpr uint32_t MAGIC = 0x45444745;  // "EDGE"
  static constexpr uint32_t VERSION = 1;
  
  uint32_t magic;
  uint32_t version;

  CPUSaveState cpu;
  CartridgeSaveState cartridge;
  MemorySaveState memory;
  MMUSaveState mmu;
};

class State {
 public:
  State(const std::string& game_state_dir, int slot);
  ~State();

  void SaveState(const struct SaveState& state);
  bool LoadState(struct SaveState& state);

  void DeleteState(int slot);
  std::vector<int> GetSaveSlots() const;
  std::string GetStateDir() const;
  void AdvanceSlot();
  int GetLatestSlot() const;

 private:
  std::string game_state_dir_;
  int slot_;
  static constexpr int MAX_SLOTS = 10;

  bool HasState(int slot) const;

  std::string GetStateDir(int slot) const;
  std::string GetStateFile(int slot) const;
  void WriteState(const std::string& path, const struct SaveState& state);
  bool ReadState(const std::string& path, struct SaveState& state);

  void CreateNewSlot();
};
