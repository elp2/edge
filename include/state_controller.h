#pragma once

#include <string>
#include <vector>
#include <memory>

class State;

class StateController {
public:
    StateController(const std::string& game_state_dir);
    ~StateController() = default;

    // Get all available save states for this game
    std::vector<std::unique_ptr<State>> GetSaveStates() const;
    
    // Get save state slots (just the numbers)
    std::vector<int> GetSaveSlots() const;
    
    // Check if a specific slot has a state
    bool HasState(int slot) const;
    
    // Create a new state for a specific slot
    std::unique_ptr<State> CreateState(int slot);
    
    // Load an existing state for a specific slot
    std::unique_ptr<State> LoadState(int slot);
    
    // Delete a state
    void DeleteState(int slot);
    
    // Create a new state in the next rotating slot
    std::unique_ptr<State> SaveRotatingSlot();
    
    // Get the game state directory
    std::string GetGameStateDir() const { return game_state_dir_; }

    // Get the rotating slot with a state, or -1 if none exist
    int GetRotatingSlot() const;

private:
    std::string game_state_dir_;
    static constexpr int MAX_SLOTS = 10;
    
    std::string GetStateDir(int slot) const;
    std::string GetStateFile(int slot) const;
}; 