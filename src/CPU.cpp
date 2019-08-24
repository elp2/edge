#include "CPU.hpp"

#include <ios>
#include <iostream>

CPU::CPU(MMU mmu) {
    this->mmu = mmu;
    Reset();
}

CPU::~CPU() {

}

// step
// read instruction code at pc
// look up instruction command based on that (might be 2 step for cb's)
// advance the number of steps that the instruction demanded

// ??? interrupts.


void CPU::Reset() {
    pc = 0x100;
    // Initialized on start, but most programs will move it themselves anyway.
    sp = 0xfffe;
    flags.z = 0;
    flags.h = 0;
    flags.n = 0;
    flags.c = 0;
}


void CPU::Debugger() {
    cout << "A: " << hex << unsigned(a) << endl;
    cout << "SP: " << hex << sp << endl;
    cout << "PC: " << hex << pc << "[" << hex << unsigned(mmu.ByteAt(pc)) << "]" << endl;
    cout << "Flags:" << endl;
    cout << "   Z: " << hex << flags.z << endl;
    cout << "   C: " << hex << flags.c << endl;
    cout << "   H: " << hex << flags.h << endl;
    cout << "   N: " << hex << flags.n << endl;
}