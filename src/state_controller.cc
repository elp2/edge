#include "state_controller.h"
#include "state.h"
#include <iostream>
#include <fstream>

StateController::StateController(const std::string& game_state_dir) 
    : game_state_dir_(game_state_dir) {
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
    int latest_slot = -1;
    for (int i = 0; i < MAX_SLOTS; i++) {
        if (HasState(i)) {
            latest_slot = i;
        }
    }
    return latest_slot;
} 

std::unique_ptr<State> StateController::SaveRotatingSlot() {
    int latest_slot = GetRotatingSlot();
    int next_slot = (latest_slot + 1) % MAX_SLOTS;
    return CreateState(next_slot);
} 