#include <cassert>
#include <iostream>

#include "AddressRouter.hpp"
#include "CommandFactory.hpp"
#include "CPU.hpp"
#include "MMU.hpp"
#include "PPU.hpp"
#include "Utils.hpp"
#include "Sprite.hpp"

CPU *getCPU() {
    ROM *bootROM = new ROM();
    assert(bootROM->LoadFile("../../boot.gb"));
    ROM *cartridgeROM = new ROM();
    assert(cartridgeROM->LoadFile("../../gb-test-roms/cpu_instrs/cpu_instrs.gb"));
    cout << "Title: " << cartridgeROM->GameTitle() << endl;

    MMU mmu = MMU();
    mmu.SetROMs(bootROM, cartridgeROM);

    return new CPU(mmu);
}

void testSprite() {
    cout << "----TESTING SPRITE----" << endl;
    Sprite *sprite = new Sprite(0x12, 0x34, 0x34, 0xa0);

    assert(sprite->Priority());
    assert(!sprite->XFlip());
    assert(sprite->YFlip());
    assert(sprite->Palette() == SpritePalette0);
    cout << endl;
}

void testRegisters() {
    cout << "----TESTING REGISTERS----" << endl;
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
    cout << " A: " << hex << unsigned(cpu->Get8Bit(Register_A)) << " " << endl;
    assert(cpu->Get8Bit(Register_A) == 0x34);
    cout << endl;
}

void testStack() {
    cout << "----TESTING REGISTERS----" << endl;
    CPU *cpu = getCPU();
    cpu->Set16Bit(Register_HL, 0x1234);
    cpu->Push16Bit(cpu->Get16Bit(Register_HL));
    uint16_t popped = cpu->Pop16Bit();
    assert(popped == 0x1234);
    assert(popped == cpu->Get16Bit(Register_HL));
    cout << endl;
}

void testPPU() {
    PPU *ppu = new PPU();

    cout << "----TESTING PPU----" << endl;

    ppu->SetByteAt(0xFF40, 0x05);
    uint8_t lcdc = ppu->GetByteAt(0xFF40);
    assert(ppu->GetByteAt(0xFF40) == 0x05);

    ppu->SetByteAt(0x8000, 0x56);
    assert(ppu->GetByteAt(0x8000) == 0x56);

    ppu->Advance(17556); // One full frame.
    cout << "and1: " << endl;
    ppu->Advance(1);

    cout << endl;
}

void testAddressRouter() {
    cout << "----TESTING ADDRESS ROUTER----" << endl;
    PPU *ppu = new PPU();

    ROM *bootROM = new ROM();
    assert(bootROM->LoadFile("../../boot.gb"));
    ROM *cartridgeROM = new ROM();
    assert(cartridgeROM->LoadFile("../../gb-test-roms/cpu_instrs/cpu_instrs.gb"));
    cout << "Title: " << cartridgeROM->GameTitle() << endl;

    MMU *mmu = new MMU();
    mmu->SetROMs(bootROM, cartridgeROM);

    AddressRouter *addressRouter = new AddressRouter(mmu, ppu);

    addressRouter->SetByteAt(0x8000, 0xed);
    assert(ppu->GetByteAt(0x8000) == 0xed);

    addressRouter->SetByteAt(0xFF40, 0x91);
    assert(ppu->GetByteAt(0xFF40) == 0x91);
    assert(ppu->GetByteAt(0xFF40) == addressRouter->GetByteAt(0xFF40));

    mmu->SetByteAt(0xFF00, 0x35);
    assert(mmu->GetByteAt(0xFF00) == addressRouter->GetByteAt(0xFF00));
    assert(mmu->GetByteAt(0xFF00) == 0x35);

    mmu->SetWordAt(0xC000, 0x1234);
    cout << "GWA: " << hex << unsigned(addressRouter->GetWordAt(0xC000)) << endl;
    assert(addressRouter->GetWordAt(0xC000) == 0x1234);

    addressRouter->SetWordAt(0xC123, 0x8877);
    assert(addressRouter->GetWordAt(0xC123) == mmu->GetWordAt(0xC123));
    assert(addressRouter->GetWordAt(0xC123) == 0x8877);

    cout << endl;
}

int main() {
    testStack();
    testRegisters();
    testSprite();
    testPPU();
    testAddressRouter();

    CPU *cpu = getCPU();
    cpu->JumpAddress(0xe0);

    int LIMIT = 10000;
    for (int i = 0; i < LIMIT; i++) {
        if (cpu->Get16Bit(Register_PC) == 0xfe) {
            cout << "# Instructions = " << i << endl;
            cout << "CPU Cycles " << cpu->Cycles() << endl;
            break;
        }
        cpu->Step();
    }
}