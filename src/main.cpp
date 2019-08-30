#include <iostream>

#include "CommandFactory.hpp"
#include "CPU.hpp"
#include "MMU.hpp"
int main() {
    cout << "Loading MMU";

    ROM *bootROM = new ROM();
    assert(bootROM->LoadFile("../boot.gb"));
    ROM *cartridgeROM = new ROM();
    assert(cartridgeROM->LoadFile("../gb-test-roms/cpu_instrs/cpu_instrs.gb"));
    cout << "Title: " << cartridgeROM->GameTitle() << endl;

    MMU mmu = MMU();
    mmu.SetROMs(bootROM, cartridgeROM);

    CPU cpu = CPU(mmu);
    cpu.JumpAddress(0x00E0);

    int LIMIT = 500;
    for (int i = 0; i < LIMIT; i++) {
        cpu.Step();
    }
    cout << "REACHED LIMIT OF " << LIMIT << endl;
}