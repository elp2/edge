#include "MMU.hpp"

#include <iostream>

#include "Utils.hpp"

using namespace std;

MMU::MMU() {
    ram = new uint8_t[0x8000];
    // TODO Initialize memory to random values.
}

void MMU::SetROMs(ROM *bootROM, ROM *cartridgeROM) {
    overlayBootROM = true;
    this->bootROM = bootROM;
    this->cartridgeROM = cartridgeROM;
}

string MMU::AddressRegion(uint16_t address) {
    if (UseBootROMForAddress(address)) {
        return "BOOT ROM";
    } else if (address < 0x4000) {
        return "ROM Bank 0 (16kb)";
    } else if (address < 0x8000) {
        // TODO - detect + prevent writes to banks.
        return "ROM Bank 1 (switchable)";
    } else if (address < 0xa000) {
        return "Video RAM";
    } else if (address < 0xc000) {
        return "Switchable RAM";
    } else if (address < 0xe000) {
        return "Internal RAM";
    } else if (address < 0xfe00) {
        return "Echo of 8k Internal RAM";
    } else if (address < 0xfea0) {
        return "OAM";
    } else if (address < 0xff00) {
        return "Empty i/o";
    } else if (address < 0xff4c) {
        return "i/o ports";
    } else if (address < 0xff80) {
        return "Empty i/o (2)";
    } else if (address < 0xffff) {
        return "Internal RAM";
    } else {
        return "Interrupt Enable Register";
    }
}

void MMU::SetDisassemblerMode(bool disasemblerMode) {
    this->disasemblerMode = disasemblerMode;
}

bool MMU::UseBootROMForAddress(uint16_t address) {
    return overlayBootROM && address < bootROM->Size();
}

uint8_t MMU::GetByteAt(uint16_t address) {
    if (disasemblerMode) {
        return 0xed;
    }

    uint8_t byte;
    if (UseBootROMForAddress(address)) {
        byte = bootROM->GetByteAt(address);
    } else {
        byte = cartridgeROM->GetByteAt(address);
    }

    cout << AddressRegion(address) << "[0x" << hex << unsigned(address) << "]";
    cout << " 0x" << hex << unsigned(byte) << " (GET) " << endl;

    return byte;
}

uint16_t MMU::GetWordAt(uint16_t address) {
    if (disasemblerMode) {
        return 0xeded;
    }
    uint8_t lsb = GetByteAt(address);
    uint16_t result = (GetByteAt(address+1) << 8)|lsb;
    return result;
}

void MMU::SetByteAt(uint16_t address, uint8_t byte) {
    if (disasemblerMode) {
        return;
    }
    if (address < 0x8000) {
        cout << "Can't Write to: ";
        cout << AddressRegion(address) << "[0x" << hex << unsigned(address) << "]";
        cout << " = 0x" << hex << unsigned(byte) << " (SET)" << endl;

        assert(false);
    }
    cout << AddressRegion(address) << "[0x" << hex << unsigned(address) << "]";
    cout << " = 0x" << hex << unsigned(byte) << " (SET)" << endl;

    // TODO: Test general setting.
    // TODO: Probably shouldn't be setting the ROM, how does RAM work?
    ram[address - 0x8000] = byte;
}

void MMU::SetWordAt(uint16_t address, uint16_t word) {
    if (disasemblerMode) {
        return;
    }

    // TODO: Test.
    SetByteAt(address, HIGHER8(word));
    SetByteAt(address + 1, LOWER8(word));
}

MMU::~MMU() {

}