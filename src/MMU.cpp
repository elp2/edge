#include "MMU.hpp"

#include <fstream>
#include <iostream>

#include "Utils.hpp"

using namespace std;

MMU::MMU() {
    // Initialize memory to random values.
}

string AddressRegion(uint16_t address) {
    if (address < 0x4000) {
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

uint8_t MMU::GetByteAt(uint16_t address) {
    if (disasemblerMode) {
        return 0xed;
    }
    uint8_t byte = (uint8_t)rom[address];

    cout << AddressRegion(address) << "[0x" << hex << unsigned(address) << "]";
    cout << " 0x" << hex << unsigned(byte) << " (GET) " << endl;

    assert(address < romSize);

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
    cout << AddressRegion(address) << "[0x" << hex << unsigned(address) << "]";
    cout << " = 0x" << hex << unsigned(byte) << " (SET)" << endl;

    // TODO: Test general setting.
    // TODO: Probably shouldn't be setting the ROM, how does RAM work?
    rom[address] = byte;
}

void MMU::SetWordAt(uint16_t address, uint16_t word) {
    if (disasemblerMode) {
        return;
    }

    // TODO: Test.
    SetByteAt(address, HIGHER8(word));
    SetByteAt(address + 1, LOWER8(word));
}

std::string MMU::GameTitle() {
    int TITLE_START = 0x134;
    int TITLE_MAX_LENGTH = 16;
    
    string gameTitle = std::string();
    for (int i = TITLE_START; i < TITLE_START + TITLE_MAX_LENGTH; i++) {
        if (rom[i] == 0x00) {
            break;
        }
        gameTitle.push_back(rom[i]);
    }
    return gameTitle;
}

bool MMU::LoadRom() {
    ifstream file ("/Users/edwardpalmer/Desktop/gb-test-roms/oam_bug/oam_bug.gb",
                   ios::in | ios::binary);
    if (!file.is_open()) {
        cout << "Could not open file!";
        return false;
    }

    file.seekg(0, ios::end);
    romSize = file.tellg();
    file.seekg(0, ios::beg);
    rom = new uint8_t[romSize];
    file.seekg(0, ios::beg);
    char *unsignedContents = new char[romSize];
    file.read(unsignedContents, romSize);
    for (uint32_t i=0; i < romSize; i++) {
        rom[i] = unsignedContents[i];    
    }
    file.close();

    Validate();
    return true;
}

void MMU::Validate() {
    // TODO: Validate Cartridge and ROMs.
    // assert(CartridgeType() != CartridgeType_Unsupported);
    // assert(ROMSize() != ROMSize_Unsupported);
}

CartridgeType MMU::CartridgeType() {
    uint16_t ctbyte = rom[0x147];
    switch (ctbyte)
    {
        case 0x01:
            return CartridgeType_ROM_MBC1;
        
        default:
            return CartridgeType_Unsupported;
    }
}

ROMSize MMU::ROMSize() {
    uint16_t ctbyte = rom[0x148];
    switch (ctbyte)
    {
        case 0x01:
            return ROMSize_64k;
        
        default:
            return ROMSize_Unsupported;
    }
}

MMU::~MMU() {

}