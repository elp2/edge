#include "MMU.hpp"

#include <fstream>
#include <iostream>

using namespace std;

MMU::MMU() {
    // Initialize memory to random values.
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
    ifstream file ("/Users/edwardpalmer/Desktop/gb-test-roms/cpu_instrs/cpu_instrs.gb",
                   ios::in | ios::binary);
    if (!file.is_open()) {
        cout << "Could not open file!";
        return false;
    }

    file.seekg(0, ios::end);
    streampos size = file.tellg();
    file.seekg(0, ios::beg);
    rom = new char[size];
    file.seekg(0, ios::beg);
    file.read(rom, size);
    file.close();

    Validate();
    return true;
}

void MMU::Validate() {
    assert(CartridgeType() != CartridgeType_Unsupported);
    assert(ROMSize() != ROMSize_Unsupported);
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