#pragma once

#include <cstdint>
#include <string>

using namespace std;

enum CartridgeType {CartridgeType_ROM_only, CartridgeType_ROM_MBC1, CartridgeType_Unsupported};
enum ROMSizeType {ROMSize_32k, ROMSize_64k, ROMSize_Unsupported};

class ROM {
 public:
   ROM();
   ~ROM();

   bool LoadFile(string filename);
   uint8_t GetByteAt(uint16_t address);

   CartridgeType GetCartridgeType();
   ROMSizeType GetROMSizeType();

   streampos Size() { return romSize; }

   string GameTitle();

 private:
    uint8_t *rom;
    streampos romSize;
};