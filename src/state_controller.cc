#include "state_controller.h"

#include "address_router.h"
#include "cartridge.h"
#include "cpu.h"
#include "mmu.h"
#include "interrupt_controller.h"
#include "ppu.h"
#include "screen.h"
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
    int rotating_slot = -1;
    for (int i = 0; i < MAX_SLOTS; i++) {
        if (HasState(i)) {
            rotating_slot = i;
        }
    }
    return rotating_slot;
}

int StateController::GetNextRotatingSlot() const {
    int rotating_slot = GetRotatingSlot();
    int next_slot;
    if (rotating_slot == -1) {
        next_slot = 1; // Start at slot 1 if no states exist
    } else {
        next_slot = (rotating_slot + 1) % MAX_SLOTS;
        if (next_slot == 0) next_slot = 1; // Skip slot 0
    }
    return next_slot;
} 

std::unique_ptr<State> StateController::SaveRotatingSlot() {
    int rotating_slot = GetRotatingSlot();
    int next_slot;
    if (rotating_slot == -1) {
        next_slot = 1; // Start at slot 1 if no states exist
    } else {
        next_slot = (rotating_slot + 1) % MAX_SLOTS;
        if (next_slot == 0) next_slot = 1; // Skip slot 0
    }
    return CreateState(next_slot);
} 

void StateController::SaveState(int slot, CPU* cpu, MMU* mmu, Cartridge* cartridge, AddressRouter* router, 
                               InterruptController* interrupt_controller, PPU* ppu, Screen* screen) {
    std::cout << "Saving state to slot " << slot << "..." << std::endl;
    
    // Create a new state for the specified slot
    auto new_state = CreateState(slot);

    // Consolidate all component states
    struct SaveState save_state = {};
    cpu->GetState(save_state.cpu);
    std::cout << "CPU state saved at PC: " << std::hex << int(save_state.cpu.pc) << std::endl;
    mmu->GetState(save_state.mmu);
    cartridge->GetState(save_state.cartridge);
    router->SaveState(save_state.memory);
    interrupt_controller->GetState(save_state.interrupt_controller);
    ppu->GetState(save_state.ppu);

    // Save the consolidated state
    new_state->SaveState(save_state);
    
    // Take screenshot for the state
    std::cout << "Taking state screenshot..." << std::endl;
    screen->SaveScreenshotToPath(new_state->GetScreenshotPath());
    if (slot != GetMainSlot()) {
        latest_rotating_slot_ = slot;
    }
    std::cout << "Saved state to slot " << slot << std::endl;
}

bool StateController::LoadState(int slot, CPU* cpu, MMU* mmu, Cartridge* cartridge, AddressRouter* router,
                               InterruptController* interrupt_controller, PPU* ppu) {
    if (!HasState(slot)) {
        std::cout << "State does not exist for slot " << slot << std::endl;
        return false;
    }
    
    std::cout << "Loading state from slot " << slot << std::endl;
    
    // Load the state for the specific slot
    auto state = std::make_unique<State>(game_state_dir_, slot);
    
    struct SaveState save_state = {};
    if (state->LoadState(save_state)) {
        // Restore all component states
        cpu->SetState(save_state.cpu);
        router->LoadState(save_state.memory);
        mmu->SetState(save_state.mmu);
        cartridge->SetState(save_state.cartridge);
        interrupt_controller->SetState(save_state.interrupt_controller);
        ppu->SetState(save_state.ppu);
        
        std::cout << "Loaded state from slot " << slot << std::endl;
        if (slot != GetMainSlot()) {
            latest_rotating_slot_ = slot;
        }    

        return true;
    } else {
        std::cout << "Failed to load state from slot " << slot << std::endl;
        return false;
    }
} 

bool StateController::MaybeLoadLatestSlot(CPU* cpu, MMU* mmu, Cartridge* cartridge, AddressRouter* router,
                                          InterruptController* interrupt_controller, PPU* ppu) {
    if (latest_rotating_slot_ == -1) {
        return false;
    }
    return LoadState(latest_rotating_slot_, cpu, mmu, cartridge, router, interrupt_controller, ppu);
}
