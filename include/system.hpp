#pragma once

#include <cstdint>
#include <string>

class AddressRouter;
class CPU;
class MMU;
class PPU;
class SerialController;

struct SDL_Renderer;
struct SDL_Texture;
struct SDL_Window;

using namespace std;
class System {
 public:
    System(string rom_filename);
    ~System() = default;

    void Main();

 private:
    MMU *mmu_;
    CPU *cpu_;
    PPU *ppu_;
    AddressRouter *router_;
    SerialController *serial_controller_;

    // TODO: These should be in the screen.
    SDL_Renderer *renderer_;
    SDL_Texture *texture_;  
    SDL_Window *window_;
    int cycles_ = 0;
    uint32_t *pixels_;
    // End TODO

    MMU *GetMMU(string rom_filename);
    void InitSDL();
    void Advance(int stepped);
    void FrameEnded();
};
