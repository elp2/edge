#include "Utils.hpp"

#include <cassert>
#include <iostream>
#include <sstream>

#include "CPU.hpp"
#include "MMU.hpp"
#include "ROM.hpp"
#include "PPU.hpp"

using namespace std;

uint16_t buildMsbLsb16(uint8_t msb, uint8_t lsb) {
    uint16_t word = msb;
    word = word << 8;
    word |= lsb;
    assert(HIGHER8(word) == msb);
    assert(LOWER8(word) == lsb);

    return word;
}

Destination destinationForColumn(uint8_t column) {
    switch (column)
    {
    case 0x00:
    case 0x08:
        return Register_B;
    case 0x01:
    case 0x09:
        return Register_C;
    case 0x02:
    case 0x0a:
        return Register_D;
    case 0x03:
    case 0x0b:
        return Register_E;
    case 0x04:
    case 0x0c:
        return Register_H;
    case 0x05:
    case 0x0d:
        return Register_L;
    case 0x06:
    case 0x0e:
        return Address_HL;
    case 0x07:
    case 0x0f:
        return Register_A;
    
    default:
        cout << "Unexpected destination for column: 0x" << hex << column << endl;
        assert(false);
        return Destination_Unknown;
    }
}

string destinationToString(Destination d) {
    switch (d)
    {
    case Register_A:
        return "A";
    case Register_B:
        return "B";
    case Register_C:
        return "C";
    case Register_D:
        return "D";
    case Register_E:
        return "E";
    case Register_H:
        return "H";
    case Register_L:
        return "L";
    case Register_AF:
        return "AF";
    case Register_BC:
        return "BC";
    case Register_DE:
        return "DE";
    case Register_HL:
        return "HL";
	case Register_SP:
		return "SP";
    case Address_SP:
        return "(SP)";
    case Address_BC:
        return "(BC)";
    case Address_DE:
        return "(DE)";
    case Address_HL:
        return "(HL)";
    case Address_nn:
        return "(nn)";
    case Eat_PC_Byte:
        return "n";
    case Eat_PC_Word:
        return "nn";    
    default:
        cout << "Unknown d 0x" << hex << unsigned(d) << endl;
        assert(false);        
        break;
    }
    return "?";
}

std::string descriptionforPixel(Pixel p) {
    stringstream stream;
    stream << hex << unsigned(p.two_bit_color_) << " P:" << p.palette_;
    return stream.str();
}

MMU *getTestingMMU() {
    ROM *bootROM = new ROM();
    assert(bootROM->LoadFile("../../boot.gb"));
    ROM *cartridgeROM = new ROM();
    assert(cartridgeROM->LoadFile("../../gb-test-roms/cpu_instrs/cpu_instrs.gb"));

    MMU *mmu = new MMU();
    mmu->SetROMs(bootROM, cartridgeROM);
    return mmu;
}

CPU *getTestingCPU() {
    MMU *mmu = getTestingMMU();
    PPU *ppu = new PPU();
    return new CPU(mmu, ppu);
}

CPU *getTestingCPUWithInstructions(std::vector<uint8_t> instructions) {
    MMU *mmu = getTestingMMU();
    PPU *ppu = new PPU();
    CPU *cpu = new CPU(mmu, ppu);

    uint16_t ram_start = 0xC000;
    cpu->JumpAddress(ram_start);
    cpu->Set16Bit(Register_SP, 0xFFFE);
    for (std::vector<uint8_t>::iterator i = instructions.begin(); i != instructions.end(); i++) {
        mmu->SetByteAt(ram_start, *i);
        ram_start++;
    }
    return cpu;
}
