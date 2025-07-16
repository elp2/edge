#include "state.h"

#include <cassert>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <system_error>


State::State(const std::string& game_state_dir, int slot) : game_state_dir_(game_state_dir) {
  assert(slot >= -1 && slot < MAX_SLOTS);

  if (slot == -1) {
    slot_ = GetLatestSlot();
    if (slot_ == -1) {
      CreateNewSlot();
    }
  } else {
    assert(HasState(slot));
    slot_ = slot;
  }
  assert(slot_ != -1);
}

void State::CreateNewSlot() {
  bool created = false;
  for (int i = 0; i < MAX_SLOTS; i++) {
    if (!HasState(i)) {
      slot_ = i;
      std::filesystem::create_directories(GetStateDir(slot_));
      created = true;
      break;
    }
  }
  if (!created) {
    std::cout << "No free slots found. Please delete an existing state." << std::endl;
    assert(false);
  }
}

int State::GetLatestSlot() const {
  int latest_slot = -1;
  std::filesystem::file_time_type latest_time;
  
  for (int i = 0; i < MAX_SLOTS; i++) {
    if (HasState(i)) {
      auto state_path = GetStateFile(i);
      auto mod_time = std::filesystem::last_write_time(state_path);
      
      if (latest_slot == -1 || mod_time > latest_time) {
        latest_time = mod_time;
        latest_slot = i;
      }
    }
  }
  
  return latest_slot;
}

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
  assert(HasState(slot_));
  std::cout << "Saving state to " << GetStateFile(slot_) << std::endl;
  WriteState(GetStateFile(slot_), state);
}

bool State::LoadState(struct SaveState& state) {
  std::cout << "Loading state from " << GetStateFile(slot_) << std::endl;
  return ReadState(GetStateFile(slot_), state);
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

void State::AdvanceSlot() {
  slot_ += 1;
  if (slot_ >= MAX_SLOTS) {
    slot_ = 0;
  }
  if (!HasState(slot_)) {
    CreateNewSlot();
  }
}

void State::WriteState(const std::string& path, const struct SaveState& state) {
  std::cout << "START: Saving state to " << path << std::endl;

  std::ofstream file(path, std::ios::binary);

  file.write(reinterpret_cast<const char*>(&SaveState::MAGIC), sizeof(state.magic));
  file.write(reinterpret_cast<const char*>(&SaveState::VERSION), sizeof(state.version));
  file.write(reinterpret_cast<const char*>(&state.cpu), sizeof(state.cpu));
  file.write(reinterpret_cast<const char*>(&state.memory), sizeof(state.memory));
  file.write(reinterpret_cast<const char*>(&state.cartridge), sizeof(state.cartridge));
  file.write(reinterpret_cast<const char*>(&state.mmu), sizeof(state.mmu));

  std::cout << "END: Saved state to " << path << std::endl;
}

bool State::ReadState(const std::string& path, struct SaveState& state) {
  std::ifstream file(path, std::ios::binary);
  if (!file) return false;

  std::cout << "START: Loading state from " << path << std::endl;

  file.read(reinterpret_cast<char*>(&state.magic), sizeof(state.magic));
  if (state.magic != SaveState::MAGIC) return false;
  
  file.read(reinterpret_cast<char*>(&state.version), sizeof(state.version));
  if (state.version != SaveState::VERSION) return false;
  
  file.read(reinterpret_cast<char*>(&state.cpu), sizeof(state.cpu));
  file.read(reinterpret_cast<char*>(&state.memory), sizeof(state.memory));
  file.read(reinterpret_cast<char*>(&state.cartridge), sizeof(state.cartridge));
  file.read(reinterpret_cast<char*>(&state.mmu), sizeof(state.mmu));

  std::cout << "END: Loaded state from " << path << std::endl;

  return true;
}
