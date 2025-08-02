#pragma once

#include <string>
#include <vector>
#include <memory>

class State;
class CPU;
class MMU;
class Cartridge;
class AddressRouter;
class InterruptController;
class PPU;
class Screen;

class StateController {
public:
    StateController(const std::string& game_state_dir, CPU* cpu, MMU* mmu, Cartridge* cartridge, 
                   AddressRouter* router, InterruptController* interrupt_controller, PPU* ppu, Screen* screen);
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
        
    // Get the game state directory
    std::string GetGameStateDir() const { return game_state_dir_; }

    // Get the rotating slot with a state, or -1 if none exist
    int GetRotatingSlot() const;

    int GetMainSlot() const { return 0; }
    int GetNextRotatingSlot() const;

    // Load the latest slot if it exists.
    bool MaybeLoadLatestSlot();

    void SaveState(int slot);
    void SaveRotatingSlot();
    bool LoadStateSlot(int slot);
    bool LoadState(const struct SaveState& save_state);

    struct SaveState GetSaveState();

    void FinishedFrame(int frame_count);

    // Returns the frame number of the state that was loaded.
    long GoBackInMemory();

private:
    std::string game_state_dir_;
    static constexpr int MAX_SLOTS = 10;
    int latest_rotating_slot_ = -1;
    
    CPU* cpu_;
    MMU* mmu_;
    Cartridge* cartridge_;
    AddressRouter* router_;
    InterruptController* interrupt_controller_;
    PPU* ppu_;
    Screen* screen_;
    
    std::string GetStateDir(int slot) const;
    std::string GetStateFile(int slot) const;

    static constexpr int SAVE_INTERVAL_FRAMES = 1 * 60;
    static constexpr int MEMORY_SAVES_COUNT = 60;
    long memory_start_frame_ = 0;
    long memory_current_frame_ = 0;
    long memory_end_frame_ = 0;
    std::vector<struct SaveState> memory_states_;
}; 