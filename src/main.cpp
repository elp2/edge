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
    cpu.JumpAddress(0x00);

    for (int i = 0; i < 500; i++) {
        cpu.Step();
    }
}