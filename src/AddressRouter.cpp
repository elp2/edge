#include "AddressRouter.hpp"

#include <cassert>
#include <iostream>

#include "MMU.hpp"
#include "PPU.hpp"
#include "Utils.hpp"

using namespace std;

AddressRouter::AddressRouter(MMU *mmu, PPU *ppu) {
    mmu_ = mmu;
    ppu_ = ppu;
}

AddressOwner ownerForIOAddress(uint16_t address) {
    switch (address)
    {
    case 0xFF00:
        // P! Joy Pad
        return AddressOwner_MMU;
    case 0xFF01:
    case 0xFF02:
        // Serials.
        return AddressOwner_MMU;
    case 0xFF04:
        // Divider.
        return AddressOwner_MMU;
    case 0xFF05:
        // Timer Counter TIMA.
        return AddressOwner_MMU;
    case 0xFF06:
        // TMA Timer Modulo.
        return AddressOwner_MMU;
    case 0xFF07:
        // TAC Timer Control.
        return AddressOwner_MMU;
    case 0xFF0F:
        // Return IF Interrupt flag.
        return AddressOwner_MMU;
    case 0xFF10:
    case 0xFF11:
    case 0xFF12:
    case 0xFF13:
    case 0xFF14:
        // FF15?
    case 0xFF16:
    case 0xFF17:
    case 0xFF18:
    case 0xFF19:
    case 0xFF1A:
    case 0xFF1B:
    case 0xFF1C:
    case 0xFF1D:
    case 0xFF1E:
        // FF1F?
    case 0xFF20:
    case 0xFF21:
    case 0xFF22:
    case 0xFF23:
    case 0xFF24:
    case 0xFF25:
    case 0xFF26:
        //nothng until FF30.
        // Sound!
        cout << "Bleep bloop " << hex << unsigned(address) << endl;
        return AddressOwner_MMU;
    case 0xFF30:
    case 0xFF31:
    case 0xFF32:
    case 0xFF33:
    case 0xFF34:
    case 0xFF35:
    case 0xFF36:
    case 0xFF37:
    case 0xFF38:
    case 0xFF39:
    case 0xFF3A:
    case 0xFF3B:
    case 0xFF3C:
    case 0xFF3D:
    case 0xFF3E:
        cout << "WaveForm! " << hex << unsigned(address) << endl;
        return AddressOwner_MMU;
    case 0xFF40:
    case 0xFF41:
    case 0xFF42:
    case 0xFF43:
    case 0xFF44:
    case 0xFF45:
    case 0xFF46:
    case 0xFF47:
    case 0xFF48:
    case 0xFF49:
    case 0xFF4A:
    case 0xFF4B:
        return AddressOwner_PPU;
        // ???
    case 0xFFFF:
    // Interrupt Enable.
    return AddressOwner_MMU;
    default:
        cout << "Unknown i/o address: 0x" << hex << unsigned(address) << endl;
        return AdressOwner_Unknown;
    }
}

AddressOwner ownerForAddress(uint16_t address) {
    if (address < 0x8000) {
        return AddressOwner_MMU;
    } else if (address < 0xa000) {
        return AddressOwner_PPU;
    } else if (address < 0xc000) {
        return AddressOwner_MMU;
    } else if (address < 0xe000) {
        return AddressOwner_MMU;
    } else if (address < 0xfe00) {
        return AddressOwner_MMU;
    } else if (address < 0xfea0) {
        return AddressOwner_PPU; // OAM.
    } else if (address < 0xff00) {
        return AddressOwner_MMU; // Empty i/o.
    } else if (address < 0xff4c) {
        return ownerForIOAddress(address);
    } else if (address < 0xff80) {
        return AddressOwner_MMU; // Empty i/o (2)
    } else if (address < 0xffff) {
        return AddressOwner_MMU; //Internal RAM
    } else {
        return AddressOwner_MMU; // Interrupt Enable Register.
    }    
}

uint8_t AddressRouter::GetByteAtAddressFromOwner(AddressOwner owner, uint16_t address) {
    switch (owner)
    {
    case AddressOwner_MMU:
        return mmu_->GetByteAt(address);
    case AddressOwner_PPU:
        return ppu_->GetByteAt(address);
    default:
		assert(false);
		return 0x00;
    }
}

void AddressRouter::EnableDisassemblerMode(bool disassemblerMode) {
    disassemblerMode_ = disassemblerMode;
    mmu_->EnableDisassemblerMode(disassemblerMode);
}

void AddressRouter::SetByteAtAddressInOwner(AddressOwner owner, uint16_t address, uint8_t byte) {
    switch (owner)
    {
    case AddressOwner_MMU:
        return mmu_->SetByteAt(address, byte);
    case AddressOwner_PPU:
        return ppu_->SetByteAt(address, byte);
    
    default:
        break;
    }
}

uint8_t AddressRouter::GetByteAt(uint16_t address) {
    AddressOwner owner = ownerForAddress(address);
    return GetByteAtAddressFromOwner(owner, address);
}

void AddressRouter::SetByteAt(uint16_t address, uint8_t byte) {
    AddressOwner owner = ownerForAddress(address);
    SetByteAtAddressInOwner(owner, address, byte);
}

uint16_t AddressRouter::GetWordAt(uint16_t address) {
    AddressOwner owner_lsb = ownerForAddress(address);
    AddressOwner owner_msb = ownerForAddress(address + 1);
    if (owner_lsb != owner_msb) {
        cout << "Unexpected cross owner get at 0x" << hex << unsigned(address) << " +1" << endl;
        assert(false);
    }

    uint8_t lsb = GetByteAtAddressFromOwner(owner_lsb, address);
    uint8_t msb = GetByteAtAddressFromOwner(owner_msb, address + 1);

    uint16_t result = (msb << 8) | lsb;
    return result;
}

void AddressRouter::SetWordAt(uint16_t address, uint16_t word) {
    AddressOwner owner_lsb = ownerForAddress(address);
    AddressOwner owner_msb = ownerForAddress(address + 1);
    if (owner_lsb != owner_msb) {
        cout << "Unexpected cross owner set at 0x" << hex << unsigned(address) << " +1" << endl;
        assert(false);
    }

    SetByteAtAddressInOwner(owner_lsb, address, LOWER8(word));
    SetByteAtAddressInOwner(owner_msb, address + 1, HIGHER8(word));
}