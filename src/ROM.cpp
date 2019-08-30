#include "ROM.hpp"

#include <fstream>
#include <iostream>

ROM::ROM() {

}

bool ROM::LoadFile(string filename) {
    ifstream file (filename, ios::in | ios::binary);
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
    return true;
}

ROM::~ROM() {}

uint32_t ROM::Size() {
    return romSize;
}

uint8_t ROM::GetByteAt(uint16_t address) {
    assert(address < romSize);

    return rom[address];
}

CartridgeType ROM::CartridgeType() {
    uint16_t ctbyte = rom[0x147];
    switch (ctbyte)
    {
        case 0x01:
            return CartridgeType_ROM_MBC1;
        
        default:
            return CartridgeType_Unsupported;
    }
}

ROMSizeType ROM::ROMSizeType() {
    uint16_t ctbyte = rom[0x148];
    switch (ctbyte)
    {
        case 0x01:
            return ROMSize_64k;
        
        default:
            return ROMSize_Unsupported;
    }
}

std::string ROM::GameTitle() {
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
