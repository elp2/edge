#include "state.h"

#include <cassert>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <system_error>


State::State(const std::string& game_state_dir, int slot) : game_state_dir_(game_state_dir) {
  assert(slot >= 0 && slot < MAX_SLOTS);

  // If the slot doesn't exist, create it.
  if (!HasState(slot)) {
    slot_ = slot;
    std::filesystem::create_directories(GetStateDir(slot_));
  } else {
    slot_ = slot;
  }
  assert(slot_ != -1);
}

State::~State() {
  // Nothing to clean up - all members are automatic
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

std::string State::GetStateDir() const {
  assert(slot_ >= 0);
  return GetStateDir(slot_);
}

std::string State::GetScreenshotPath() const {
  assert(slot_ >= 0);
  return GetStateDir(slot_) + "/screenshot.bmp";
}

std::string State::GetStateDir(int slot) const {
  return game_state_dir_ + "/" + std::to_string(slot);
}

std::string State::GetStateFile(int slot) const {
  return GetStateDir(slot) + "/state.bin";
}

void State::SaveState(const struct SaveState& state) {
  std::cout << "Saving state to " << GetStateFile(slot_) << std::endl;
  WriteState(GetStateFile(slot_), state);
}

bool State::LoadState(struct SaveState& state) {
  std::cout << "Loading state from " << GetStateFile(slot_) << std::endl;
  return ReadState(GetStateFile(slot_), state);
}

bool State::HasState(int slot) const {
  if (slot < 0 || slot >= MAX_SLOTS) return false;
  try {
    return std::filesystem::exists(GetStateDir(slot));
  } catch (const std::filesystem::filesystem_error& e) {
    std::cout << "Warning: Could not check existence of " << GetStateDir(slot) << ": " << e.what() << std::endl;
    return false;
  }
}

void State::DeleteState(int slot) {
  if (slot < 0 || slot >= MAX_SLOTS) return;
  std::filesystem::remove_all(GetStateDir(slot));
}

void State::AdvanceSlot() {
  slot_ += 1;
  if (slot_ >= MAX_SLOTS) {
    slot_ = 1;
  }
  if (!HasState(slot_)) {
    CreateNewSlot();
  }
}

void State::SetSlot(int slot) {
  assert(slot >= 0 && slot < MAX_SLOTS);
  assert(HasState(slot));
  slot_ = slot;
}

void State::WriteState(const std::string& path, const struct SaveState& state) {
  std::cout << "START: Saving state to " << path << std::endl;

  std::ofstream file(path, std::ios::binary);

  file.write(reinterpret_cast<const char*>(&SaveState::MAGIC), sizeof(state.magic));
  file.write(reinterpret_cast<const char*>(&SaveState::VERSION), sizeof(state.version));
  file.write(reinterpret_cast<const char*>(&state.cpu), sizeof(state.cpu));
  file.write(reinterpret_cast<const char*>(&state.memory), sizeof(state.memory));
  file.write(reinterpret_cast<const char*>(&state.cartridge), sizeof(state.cartridge));
  file.write(reinterpret_cast<const char*>(state.cartridge.ram), state.cartridge.ram_size);
  file.write(reinterpret_cast<const char*>(&state.mmu), sizeof(state.mmu));
  file.write(reinterpret_cast<const char*>(&state.interrupt_controller), sizeof(state.interrupt_controller));
  file.write(reinterpret_cast<const char*>(&state.ppu), sizeof(state.ppu));
  std::cout << "GetState: " << std::hex << unsigned(state.interrupt_controller.interrupts_enabled) << std::endl;

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
  state.cartridge.ram = (uint8_t *)malloc(state.cartridge.ram_size);
  file.read(reinterpret_cast<char*>(state.cartridge.ram), state.cartridge.ram_size);
  file.read(reinterpret_cast<char*>(&state.mmu), sizeof(state.mmu));
  file.read(reinterpret_cast<char*>(&state.interrupt_controller), sizeof(state.interrupt_controller));
  file.read(reinterpret_cast<char*>(&state.ppu), sizeof(state.ppu));
  std::cout << "ReadState: " << std::hex << unsigned(state.interrupt_controller.interrupts_enabled) << std::endl;


  std::cout << "END: Loaded state from " << path << std::endl;

  return true;
}

time_t State::GetSaveTime() const {
  try {
    auto file_time = std::filesystem::last_write_time(GetStateFile(slot_));
    auto duration = file_time.time_since_epoch();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
    return seconds.count();
  } catch (const std::filesystem::filesystem_error& e) {
    std::cout << "Warning: Could not get modification time for " << GetStateFile(slot_) << ": " << e.what() << std::endl;
    return 0;
  }
}
