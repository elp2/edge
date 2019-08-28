#include <iostream>

#include "CommandFactory.hpp"
#include "CPU.hpp"
#include "MMU.hpp"
int main() {
    cout << "Loading MMU";
    MMU mmu = MMU();
    if (mmu.LoadRom()) {
        cout << "... Loaded";
    } else {
        cout << "... Error!!!";
        exit(0);
    }
    cout << endl;

    cout << "Title: " << mmu.GameTitle() << endl;

    CPU cpu = CPU(mmu);

    cpu.JumpAddress(0x00);
    cpu.PreventJumpsDisassembler(true);

    cpu.Debugger();
    for (int i = 0; i < 500; i++) {
        cpu.Step();
    }
}