#include <iostream>

#include "CommandFactory.hpp"
#include "CPU.hpp"
#include "MMU.hpp"
#include "Utils.hpp"

CPU *getCPU() {
    ROM *bootROM = new ROM();
    assert(bootROM->LoadFile("../boot.gb"));
    ROM *cartridgeROM = new ROM();
    assert(cartridgeROM->LoadFile("../gb-test-roms/cpu_instrs/cpu_instrs.gb"));
    cout << "Title: " << cartridgeROM->GameTitle() << endl;

    MMU mmu = MMU();
    mmu.SetROMs(bootROM, cartridgeROM);

    return new CPU(mmu);
}

void testRegisters() {
    CPU *cpu = getCPU();
    cpu->Set8Bit(Register_A, 0xed);
    cpu->Set8Bit(Register_B, 0x12);
    cpu->Set8Bit(Register_C, 0x34);
    uint16_t bc = cpu->Get16Bit(Register_BC);

    assert(cpu->Get8Bit(Register_B) == 0x12);
    assert(cpu->Get8Bit(Register_C) == 0x34);
    assert(bc == 0x1234);

    cpu->Set8Bit(Register_A, cpu->Get8Bit(Register_B));
    assert(cpu->Get8Bit(Register_A) == 0x12);
    cpu->Set8Bit(Register_A, cpu->Get8Bit(Register_C));
    assert(cpu->Get8Bit(Register_A) == 0x34);

    uint8_t x = 0x12;
    uint16_t y = 0x0012;
    assert(x==y);

    cout << hex << unsigned(bc) << endl;
    assert(bc == 0x1234);
    cpu->Set16Bit(Register_BC, bc);
    assert(bc == 0x1234);

    cpu->Set16Bit(Register_HL, 0x134);
    cpu->Set8Bit(Register_A, cpu->Get8Bit(Register_L));
    cout << hex << unsigned(cpu->Get16Bit(Register_HL)) << " ";
    cout << " H: " << hex << unsigned(cpu->Get8Bit(Register_H)) << " ";
    cout << " L: " << hex << unsigned(cpu->Get8Bit(Register_L)) << " ";
    cout << " A: " << hex << unsigned(cpu->Get8Bit(Register_A)) << " ";
    assert(cpu->Get8Bit(Register_A) == 0x34);
}

void testStack() {
    CPU *cpu = getCPU();
    cpu->Set16Bit(Register_HL, 0x1234);
    cpu->Push16Bit(cpu->Get16Bit(Register_HL));
    uint16_t popped = cpu->Pop16Bit();
    assert(popped == 0x1234);
    assert(popped == cpu->Get16Bit(Register_HL));
}

int main() {
    testStack();
    testRegisters();

    CPU *cpu = getCPU();
    cpu->JumpAddress(0xe0);

    int LIMIT = 10000;
    for (int i = 0; i < LIMIT; i++) {
        if (cpu->Get16Bit(Register_PC) == 0xfe) {
            cout << "# Instructions = " << i << endl;
            cout << "CPU Cycles " << cpu->Cycles() << endl;
            exit(0);
        }
        cpu->Step();
    }
    cout << "REACHED LIMIT OF " << LIMIT << endl;
}