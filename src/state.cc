#include "state.h"

#include <cassert>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <system_error>


State::State(const std::string& game_state_dir, int slot) : game_state_dir_(game_state_dir) {
  slot_ = -1;
  if (slot < 0) {
    for (int i = 0; i < MAX_SLOTS; i++) {
      std::cout << i << std::endl;
      if (!HasState(i)) {
        slot_ = i;
        std::filesystem::create_directories(GetStateDir(slot_));
        break;
      }
    }
  } else {
    assert(HasState(slot));
    slot_ = slot;
  }
  assert(slot_ != -1);
}

State::~State() {}

std::string State::GetStateDir() const {
  assert(slot_ >= 0);
  return GetStateDir(slot_);
}

std::string State::GetStateDir(int slot) const {
  return game_state_dir_ + "/" + std::to_string(slot);
}

std::string State::GetStateFile(int slot) const {
  return GetStateDir() + "/state.bin";
}

void State::SaveState(const struct SaveState& state) {
  SaveState(slot_, state);
}

void State::SaveState(int slot, const struct SaveState& state) {
  if (slot < 0 || slot >= MAX_SLOTS) return;
  WriteState(GetStateFile(slot), state);
}

bool State::LoadState(int slot, struct SaveState& state) {
  if (slot < 0 || slot >= MAX_SLOTS) return false;
  if (!HasState(slot)) return false;
  return ReadState(GetStateFile(slot), state);
}

bool State::HasState(int slot) const {
  if (slot < 0 || slot >= MAX_SLOTS) return false;
  return std::filesystem::exists(GetStateDir(slot));
}

void State::DeleteState(int slot) {
  if (slot < 0 || slot >= MAX_SLOTS) return;
  std::filesystem::remove_all(GetStateDir(slot));
}

std::vector<int> State::GetSaveSlots() const {
  std::vector<int> slots;
  for (int i = 0; i < MAX_SLOTS; i++) {
    if (HasState(i)) {
      slots.push_back(i);
    }
  }
  return slots;
}

void State::WriteState(const std::string& path, const struct SaveState& state) {
  std::ofstream file(path, std::ios::binary);

  file.write(reinterpret_cast<const char*>(&SaveState::MAGIC), sizeof(state.magic));
  file.write(reinterpret_cast<const char*>(&SaveState::VERSION), sizeof(state.version));
  file.write(reinterpret_cast<const char*>(&state.cpu), sizeof(state.cpu));
  file.write(reinterpret_cast<const char*>(&state.ppu), sizeof(state.ppu));
  file.write(reinterpret_cast<const char*>(&state.timer), sizeof(state.timer));
  file.write(reinterpret_cast<const char*>(&state.cartridge), sizeof(state.cartridge));
}

bool State::ReadState(const std::string& path, struct SaveState& state) {
  std::ifstream file(path, std::ios::binary);
  if (!file) return false;
  
  file.read(reinterpret_cast<char*>(&state.magic), sizeof(state.magic));
  if (state.magic != SaveState::MAGIC) return false;
  
  file.read(reinterpret_cast<char*>(&state.version), sizeof(state.version));
  if (state.version != SaveState::VERSION) return false;
  
  file.read(reinterpret_cast<char*>(&state.cpu), sizeof(state.cpu));
  file.read(reinterpret_cast<char*>(&state.ppu), sizeof(state.ppu));
  file.read(reinterpret_cast<char*>(&state.timer), sizeof(state.timer));
  file.read(reinterpret_cast<char*>(&state.cartridge), sizeof(state.cartridge));
  
  return true;
}
