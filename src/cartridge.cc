#include "cartridge.h"

#include <cassert>
#include <fstream>
#include <iostream>

uint8_t *UnsignedCartridgeBytes(string filename) {
  ifstream file(filename, ios::in | ios::binary);
  if (!file.is_open()) {
    cout << "Could not open file!: " << filename << endl;
    return NULL;
  }

  file.seekg(0, ios::end);
  uint32_t rom_size = file.tellg();
  file.seekg(0, ios::beg);
  uint8_t *rom = new uint8_t[rom_size];
  file.seekg(0, ios::beg);
  char *unsigned_contents = new char[rom_size];
  file.read(unsigned_contents, rom_size);
  for (uint32_t i = 0; i < rom_size; i++) {
    rom[i] = unsigned_contents[i];
  }
  file.close();

  return rom;
}

Cartridge::Cartridge(string filename) {
   rom_ = UnsignedCartridgeBytes(filename); 
}

Cartridge::~Cartridge() {}

uint8_t Cartridge::GetByteAt(uint16_t address) {
  assert(rom_);
  assert(address < ROMSize());

  return rom_[address];
}

CartridgeType Cartridge::GetCartridgeType() {
  uint16_t ctbyte = rom_[0x147];
  switch (ctbyte) {
    case 0x00:
      return CartridgeType_ROM_only;
    case 0x01:
      return CartridgeType_ROM_MBC1;
    case 0x02:
    case 0x03:  // With Battery Backup - ignore.
      return CartridgeType_ROM_MBC1_RAM;

    default:
      std::cout << "Unsupported cartridge type: " << std::hex << (int)ctbyte << std::endl;
      return CartridgeType_Unsupported;
  }
}

ROMSizeType Cartridge::GetROMSizeType() {
  uint16_t ctbyte = rom_[0x148];
  switch (ctbyte) {
    case 0x00:
      return ROMSize_32k;
    case 0x01:
      return ROMSize_64k;
    case 0x02:
      return ROMSize_128k;
    case 0x03:
      return ROMSize_256k;
    case 0x04:
      return ROMSize_512k;
    default:
      return ROMSize_Unsupported;
  }
}

int Cartridge::ROMSize() {
  switch (GetROMSizeType()) {
    case ROMSize_32k:
      return 32768;
    case ROMSize_64k:
      return 65536;
    case ROMSize_128k:
      return 131072;
    case ROMSize_256k:
      return 262144;
    case ROMSize_512k:
      return 524288;
    default:
      assert(false);
      return 0;
  }
}

std::string Cartridge::GameTitle() {
  assert(rom_);
  int TITLE_START = 0x134;
  int TITLE_MAX_LENGTH = 16;

  string gameTitle = std::string();
  for (int i = TITLE_START; i < TITLE_START + TITLE_MAX_LENGTH; i++) {
    if (rom_[i] == 0x00) {
      break;
    }
    gameTitle.push_back(rom_[i]);
  }
  return gameTitle;
}
