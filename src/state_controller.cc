#include "state_controller.h"

#include "address_router.h"
#include "cartridge.h"
#include "cpu.h"
#include "mmu.h"
#include "interrupt_controller.h"
#include "ppu.h"
#include "screen.h"
#include "state.h"

#include <algorithm>
#include <cassert>
#include <filesystem>
#include <iostream>
#include <fstream>

StateController::StateController(const std::string& game_state_dir, CPU* cpu, MMU* mmu, Cartridge* cartridge, 
                               AddressRouter* router, InterruptController* interrupt_controller, PPU* ppu, Screen* screen)
    : game_state_dir_(game_state_dir), cpu_(cpu), mmu_(mmu), cartridge_(cartridge), router_(router), 
      interrupt_controller_(interrupt_controller), ppu_(ppu), screen_(screen) {
    memory_states_.resize(MEMORY_SAVES_COUNT);
}

std::vector<std::unique_ptr<State>> StateController::GetSaveStates() const {
    std::vector<std::unique_ptr<State>> states;
    
    for (int i = 0; i < MAX_SLOTS; i++) {
        if (HasState(i)) {
            try {
                states.push_back(std::make_unique<State>(game_state_dir_, i));
            } catch (const std::exception& e) {
                std::cout << "Warning: Could not load state " << i << ": " << e.what() << std::endl;
                continue;
            }
        }
    }
    
    return states;
}

std::vector<int> StateController::GetSaveSlots() const {
    std::vector<int> slots;
    
    for (int i = 0; i < MAX_SLOTS; i++) {
        if (HasState(i)) {
            slots.push_back(i);
        }
    }
    
    return slots;
}

bool StateController::HasState(int slot) const {
    if (slot < 0 || slot >= MAX_SLOTS) return false;
    
    std::string state_file = GetStateFile(slot);
    std::ifstream file(state_file);
    return file.good();
}

std::unique_ptr<State> StateController::CreateState(int slot) {
    if (slot < 0 || slot >= MAX_SLOTS) {
        throw std::invalid_argument("Invalid slot number");
    }
    
    return std::make_unique<State>(game_state_dir_, slot);
}

std::unique_ptr<State> StateController::LoadState(int slot) {
    if (!HasState(slot)) {
        throw std::runtime_error("State does not exist for slot " + std::to_string(slot));
    }
    
    return std::make_unique<State>(game_state_dir_, slot);
}

void StateController::DeleteState(int slot) {
    if (slot < 0 || slot >= MAX_SLOTS) return;
    
    std::string state_dir = GetStateDir(slot);
    // Note: This would need filesystem support to actually delete
    // For now, we'll just mark it as not having a state
    std::cout << "Would delete state directory: " << state_dir << std::endl;
}

std::string StateController::GetStateDir(int slot) const {
    return game_state_dir_ + "/" + std::to_string(slot);
}

std::string StateController::GetStateFile(int slot) const {
    return GetStateDir(slot) + "/state.bin";
} 

int StateController::GetRotatingSlot() const {
    int oldest_slot = -1;
    std::filesystem::file_time_type oldest_time;
    
    for (int i = 1; i < MAX_SLOTS; i++) { // Skip slot 0 (main slot)
        if (HasState(i)) {
            try {
                auto state_path = GetStateFile(i);
                auto mod_time = std::filesystem::last_write_time(state_path);
                
                if (oldest_slot == -1 || mod_time > oldest_time) {
                    oldest_time = mod_time;
                    oldest_slot = i;
                }
            } catch (const std::filesystem::filesystem_error& e) {
                std::cout << "Warning: Could not get modification time for " << GetStateFile(i) << ": " << e.what() << std::endl;
                continue;
            }
        }
    }
    
    return oldest_slot;
}

int StateController::GetNextRotatingSlot() const {
    int oldest_slot = GetRotatingSlot();
    int next_slot;
    if (oldest_slot == -1) {
        next_slot = 1; // Start at slot 1 if no states exist
    } else {
        next_slot = (oldest_slot + 1) % MAX_SLOTS;
        if (next_slot == GetMainSlot()) next_slot = 1;
    }
    return next_slot;
}

void StateController::SaveRotatingSlot() {
    int slot = GetNextRotatingSlot();
    SaveState(slot);
}

void StateController::SaveState(int slot) {
    std::cout << "Saving state to slot " << slot << "..." << std::endl;
    
    auto new_state = CreateState(slot);

    struct SaveState save_state = GetSaveState();

    new_state->SaveState(save_state);
    
    std::cout << "Taking state screenshot..." << std::endl;
    screen_->SaveScreenshotToPath(new_state->GetScreenshotPath());
    if (slot != GetMainSlot()) {
        latest_rotating_slot_ = slot;
    }
    std::cout << "Saved state to slot " << slot << std::endl;
}

bool StateController::LoadStateSlot(int slot) {
    if (!HasState(slot)) {
        std::cout << "State does not exist for slot " << slot << std::endl;
        return false;
    }
    
    std::cout << "Loading state from slot " << slot << std::endl;
    
    auto state = std::make_unique<State>(game_state_dir_, slot);
    
    struct SaveState save_state = {};
    if (state->LoadState(save_state)) {
        if (slot != GetMainSlot()) {
            latest_rotating_slot_ = slot;
        }
        return LoadState(save_state);
    } else {
        std::cout << "Failed to load state from slot " << slot << std::endl;
        return false;
    }
}

struct SaveState StateController::GetSaveState() {
    struct SaveState save_state = {};
    cpu_->GetState(save_state.cpu);
    std::cout << "CPU state saved at PC: " << std::hex << int(save_state.cpu.pc) << std::endl;
    mmu_->GetState(save_state.mmu);
    cartridge_->GetState(save_state.cartridge);
    router_->SaveState(save_state.memory);
    interrupt_controller_->GetState(save_state.interrupt_controller);
    ppu_->GetState(save_state.ppu);
    return save_state;
}

bool StateController::LoadState(const struct SaveState& save_state) {
    cpu_->SetState(save_state.cpu);
    router_->LoadState(save_state.memory);
    mmu_->SetState(save_state.mmu);
    cartridge_->SetState(save_state.cartridge);
    interrupt_controller_->SetState(save_state.interrupt_controller);
    ppu_->SetState(save_state.ppu);
    
    std::cout << "Loaded state successfully" << std::endl;
    return true;
} 

bool StateController::MaybeLoadLatestSlot() {
    if (latest_rotating_slot_ == -1) {
        return false;
    }
    return LoadStateSlot(latest_rotating_slot_);
}

void StateController::FinishedFrame(int frame_count) {
    if (frame_count % SAVE_INTERVAL_FRAMES != 0) {
        return;
    }
    SaveState(GetMainSlot());

    memory_current_frame_ = frame_count;

    if (memory_current_frame_ > memory_end_frame_) {
        memory_end_frame_ = memory_current_frame_;
    }

    int buffer_pos = (memory_current_frame_ / SAVE_INTERVAL_FRAMES) % MEMORY_SAVES_COUNT;
    std::cout << "Saving memory state at frame " << frame_count << " to buffer pos " << buffer_pos << std::endl;
    memory_states_[buffer_pos] = GetSaveState();

    memory_start_frame_ = std::max(0L, memory_end_frame_ - (MEMORY_SAVES_COUNT - 1) * SAVE_INTERVAL_FRAMES);
}

long StateController::GoBackInMemory() {
    static constexpr int BACK_FRAMES = 2 * 60;
    long target_frame = std::max(memory_start_frame_, memory_current_frame_ - BACK_FRAMES);

    int buffer_pos = (target_frame / SAVE_INTERVAL_FRAMES) % MEMORY_SAVES_COUNT;
    std::cout << "Trying to load from frame " << target_frame << " buffer pos " << buffer_pos << std::endl;
    std::cout << "Memory range: [" << memory_start_frame_ << ", " << memory_end_frame_ << "]" << std::endl;
    
    assert(buffer_pos >= 0 && buffer_pos < MEMORY_SAVES_COUNT);

    std::cout << "Loading state from buffer pos " << buffer_pos << std::endl;
    LoadState(memory_states_[buffer_pos]);
    memory_current_frame_ = target_frame;
    return memory_current_frame_;
}
