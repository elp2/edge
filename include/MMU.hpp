#pragma once

#include <string>
using namespace std;

enum CartridgeType {CartridgeType_ROM_MBC1, CartridgeType_Unsupported};
enum ROMSize {ROMSize_64k, ROMSize_Unsupported};

class MMU {

public:
    MMU();
    ~MMU();

    // Returns the max 16 character upper case game title.
    std::string GameTitle();

    // Loads a ROM file. Returns true if loaded successfully.
    bool LoadRom();

    CartridgeType CartridgeType();
    ROMSize ROMSize();

    void Validate();

private:
    char * rom;
};