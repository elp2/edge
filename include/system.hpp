#pragma once

#include <cstdint>
#include <string>

class AddressRouter;
class CPU;
class InputController;
class InterruptController;
class MMU;
class PPU;
class SerialController;
class TimerController;

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
	InputController* input_controller_;
    InterruptController *interrupt_controller_;
    SerialController *serial_controller_;
	TimerController *timer_controller_;

    MMU *GetMMU(string rom_filename);
    void Advance(int stepped);
    void FrameEnded();
};
