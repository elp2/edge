#include "LoadCommand.hpp"

#include "CommandFactory.hpp"
#include "CPU.hpp"
#include "MMU.hpp"
LoadCommand::LoadCommand(uint8_t opcode, string description, Destination to, Destination from, int cycles) {
        this->opcode = opcode;
        this->description = description;
        this->cycles = cycles;
        this->from = from;
        this->to = to;
}

LoadCommand::~LoadCommand() {
}

void LoadCommand::Run(CPU *cpu, MMU *mmu) {
    (void)mmu;
    bool from16Bit = cpu->Requires16Bits(from);
    bool to16Bit = cpu->Requires16Bits(to);
    if (from16Bit) {
        if (to16Bit) {
            cpu->Set16Bit(to, cpu->Read16Bit(from));
        } else {
            cpu->Set8Bit(to, cpu->Read16Bit(from));
        }
    } else {
        if (to16Bit) {
            cpu->Set16Bit(to, cpu->Get8Bit(from));
        } else {
            cpu->Set8Bit(to, cpu->Get8Bit(from));
        }
        
    }
}

// Special commands which didn't fit nicely into the existing model.
class SpecialLoadCommand : public Command {
 public:
    SpecialLoadCommand(uint8_t opcode, string description, int cycles) {
        this->opcode = opcode;
        this->description = description;
        this->cycles = cycles;
    }

    ~SpecialLoadCommand() {}

    void Run(CPU *cpu, MMU *mmu) {
        if (opcode == 0xF8) {
            uint8_t offset = cpu->Get8Bit(Eat_PC_Byte);
            cpu->Set16Bit(Register_SP, 0xff00 + offset);
            cpu->flags.z = false;
            cpu->flags.n = false;
            // H/z depending on result.
            // TODO: FLAGS!!!
            assert(false);
            return;
        }

        uint16_t address;
        uint8_t value;
        switch (opcode)
        {
        case 0xf2:
            address = 0xff00 + cpu->Get8Bit(Register_C);
            value = mmu->GetByteAt(address);
            cpu->Set8Bit(Register_A, value);
            break;
        case 0xf0:
            // LD A,($FF00+n) F0 12
            address = 0xff00 + cpu->Get8Bit(Eat_PC_Byte);
            value = mmu->GetByteAt(address);
            cpu->Set8Bit(Register_A, value);
            break;
        case 0xe2:
            address = 0xff00 + cpu->Get8Bit(Register_C);
            mmu->SetByteAt(address, cpu->Get8Bit(Register_A));
            break;
        case 0xe0:
            address = 0xff00 + cpu->Get8Bit(Eat_PC_Byte);
            mmu->SetByteAt(address, cpu->Get8Bit(Register_A));
            break;

        case 0x3a:
            address = cpu->Read16Bit(Register_HL);
            cpu->Set8Bit(Register_A, mmu->GetByteAt(address));
            cpu->Set16Bit(Register_HL, address - 1);
            break;
        case 0x2a:
            address = cpu->Read16Bit(Register_HL);
            cpu->Set8Bit(Register_A, mmu->GetByteAt(address));
            cpu->Set16Bit(Register_HL, address + 1);
            break;
        case 0x32:
            mmu->SetByteAt(cpu->Read16Bit(Register_HL), cpu->Get8Bit(Register_A));
            cpu->Set16Bit(Register_HL, cpu->Read16Bit(Register_HL) - 1);
            break;
        case 0x22:
            mmu->SetByteAt(cpu->Read16Bit(Register_HL), cpu->Get8Bit(Register_A));
            cpu->Set16Bit(Register_HL, cpu->Read16Bit(Register_HL) + 1);
            break;

        default:
            cout << "Unexpected opcode for Special Load: 0x" << hex << opcode;
            assert(1);
        }
    }
};

void registerSpecialLoadCommands(AbstractCommandFactory *factory) {
    factory->RegisterCommand(new SpecialLoadCommand(0xf2, "LD A,(C)", 8));
    factory->RegisterCommand(new SpecialLoadCommand(0xe2, "LD ($FF00+C),A", 8));

    factory->RegisterCommand(new SpecialLoadCommand(0x3a, "LD A,(HL-)", 8));
    factory->RegisterCommand(new SpecialLoadCommand(0x32, "LD (HL-),A", 8));

    factory->RegisterCommand(new SpecialLoadCommand(0x2a, "LD A,(HL+)", 8));
    factory->RegisterCommand(new SpecialLoadCommand(0x22, "LD (HL+),A", 8));

    factory->RegisterCommand(new SpecialLoadCommand(0xe0, "LD ($FF00+n),A", 12));
    factory->RegisterCommand(new SpecialLoadCommand(0xf0, "LD A,($FF00+n)", 12));

    factory->RegisterCommand(new SpecialLoadCommand(0xf8, "LDHL SP,n", 12));
}

void register16BitLoadCommands(AbstractCommandFactory *factory) {
    // TODO: Clean these up.
    // factory->RegisterCommand(new LoadCommand(0x01, "LD BC,nn", Register_BC, Eat_PC_Word, 12));
    // factory->RegisterCommand(new LoadCommand(0x11, "LD DE,nn", Register_DE, Eat_PC_Word, 12));
    // factory->RegisterCommand(new LoadCommand(0x21, "LD HL,nn", Register_HL, Eat_PC_Word, 12));
    // factory->RegisterCommand(new LoadCommand(0x31, "LD SP,nn", Register_SP, Eat_PC_Word, 12));
    factory->RegisterCommand(new LoadCommand(0xF9, "LD SP,HL", Register_SP, Register_HL, 8));
    factory->RegisterCommand(new LoadCommand(0x08, "LD (nn),SP", Address_nn, Register_SP, 20));    
}

void registerLoadCommands(AbstractCommandFactory *factory) {
    register16BitLoadCommands(factory);
    registerSpecialLoadCommands(factory);

    factory->RegisterCommand(new LoadCommand(0x06, "LD B,n", Register_B, Eat_PC_Byte, 8));
    factory->RegisterCommand(new LoadCommand(0x0E, "LD C,n", Register_C, Eat_PC_Byte, 8));
    factory->RegisterCommand(new LoadCommand(0x16, "LD D,n", Register_D, Eat_PC_Byte, 8));
    factory->RegisterCommand(new LoadCommand(0x1E, "LD E,n", Register_E, Eat_PC_Byte, 8));
    factory->RegisterCommand(new LoadCommand(0x26, "LD H,n", Register_H, Eat_PC_Byte, 8));
    factory->RegisterCommand(new LoadCommand(0x2E, "LD L,n", Register_L, Eat_PC_Byte, 8));
    factory->RegisterCommand(new LoadCommand(0x02, "LD (BC),A", Address_BC, Register_A, 8));
    factory->RegisterCommand(new LoadCommand(0x12, "LD (DE),A", Address_DE, Register_A, 8));
    factory->RegisterCommand(new LoadCommand(0x77, "LD (HL),A", Address_HL, Register_A, 8));
    factory->RegisterCommand(new LoadCommand(0xEA, "LD (nn),A", Address_nn, Register_A, 16));
    factory->RegisterCommand(new LoadCommand(0x01, "LD BC,nn", Register_BC, Eat_PC_Word, 12));
    factory->RegisterCommand(new LoadCommand(0x11, "LD DE,nn", Register_DE, Eat_PC_Word, 12));
    factory->RegisterCommand(new LoadCommand(0x21, "LD HL,nn", Register_HL, Eat_PC_Word, 12));
    factory->RegisterCommand(new LoadCommand(0x31, "LD SP,nn", Register_SP, Eat_PC_Word, 12));
    factory->RegisterCommand(new LoadCommand(0x7F, "LD A,A", Register_A, Register_A, 4));
    factory->RegisterCommand(new LoadCommand(0x78, "LD A,B", Register_A, Register_B, 4));
    factory->RegisterCommand(new LoadCommand(0x79, "LD A,C", Register_A, Register_C, 4));
    factory->RegisterCommand(new LoadCommand(0x7A, "LD A,D", Register_A, Register_D, 4));
    factory->RegisterCommand(new LoadCommand(0x7B, "LD A,E", Register_A, Register_E, 4));
    factory->RegisterCommand(new LoadCommand(0x7C, "LD A,H", Register_A, Register_H, 4));
    factory->RegisterCommand(new LoadCommand(0x7D, "LD A,L", Register_A, Register_L, 4));
    factory->RegisterCommand(new LoadCommand(0x7E, "LD A,(HL)", Register_A, Address_HL, 8));
    factory->RegisterCommand(new LoadCommand(0x40, "LD B,B", Register_B, Register_B, 4));
    factory->RegisterCommand(new LoadCommand(0x41, "LD B,C", Register_B, Register_C, 4));
    factory->RegisterCommand(new LoadCommand(0x42, "LD B,D", Register_B, Register_D, 4));
    factory->RegisterCommand(new LoadCommand(0x43, "LD B,E", Register_B, Register_E, 4));
    factory->RegisterCommand(new LoadCommand(0x44, "LD B,H", Register_B, Register_H, 4));
    factory->RegisterCommand(new LoadCommand(0x45, "LD B,L", Register_B, Register_L, 4));
    factory->RegisterCommand(new LoadCommand(0x46, "LD B,(HL)", Register_B, Address_HL, 8));
    factory->RegisterCommand(new LoadCommand(0x48, "LD C,B", Register_C, Register_B, 4));
    factory->RegisterCommand(new LoadCommand(0x49, "LD C,C", Register_C, Register_C, 4));
    factory->RegisterCommand(new LoadCommand(0x4A, "LD C,D", Register_C, Register_D, 4));
    factory->RegisterCommand(new LoadCommand(0x4B, "LD C,E", Register_C, Register_E, 4));
    factory->RegisterCommand(new LoadCommand(0x4C, "LD C,H", Register_C, Register_H, 4));
    factory->RegisterCommand(new LoadCommand(0x4D, "LD C,L", Register_C, Register_L, 4));
    factory->RegisterCommand(new LoadCommand(0x4E, "LD C,(HL)", Register_C, Address_HL, 8));
    factory->RegisterCommand(new LoadCommand(0x50, "LD D,B", Register_D, Register_B, 4));
    factory->RegisterCommand(new LoadCommand(0x51, "LD D,C", Register_D, Register_C, 4));
    factory->RegisterCommand(new LoadCommand(0x52, "LD D,D", Register_D, Register_D, 4));
    factory->RegisterCommand(new LoadCommand(0x53, "LD D,E", Register_D, Register_E, 4));
    factory->RegisterCommand(new LoadCommand(0x54, "LD D,H", Register_D, Register_H, 4));
    factory->RegisterCommand(new LoadCommand(0x55, "LD D,L", Register_D, Register_L, 4));
    factory->RegisterCommand(new LoadCommand(0x56, "LD D,(HL)", Register_D, Address_HL, 8));
    factory->RegisterCommand(new LoadCommand(0x58, "LD E,B", Register_E, Register_B, 4));
    factory->RegisterCommand(new LoadCommand(0x59, "LD E,C", Register_E, Register_C, 4));
    factory->RegisterCommand(new LoadCommand(0x5A, "LD E,D", Register_E, Register_D, 4));
    factory->RegisterCommand(new LoadCommand(0x5B, "LD E,E", Register_E, Register_E, 4));
    factory->RegisterCommand(new LoadCommand(0x5C, "LD E,H", Register_E, Register_H, 4));
    factory->RegisterCommand(new LoadCommand(0x5D, "LD E,L", Register_E, Register_L, 4));
    factory->RegisterCommand(new LoadCommand(0x5E, "LD E,(HL)", Register_E, Address_HL, 8));
    factory->RegisterCommand(new LoadCommand(0x60, "LD H,B", Register_H, Register_B, 4));
    factory->RegisterCommand(new LoadCommand(0x61, "LD H,C", Register_H, Register_C, 4));
    factory->RegisterCommand(new LoadCommand(0x62, "LD H,D", Register_H, Register_D, 4));
    factory->RegisterCommand(new LoadCommand(0x63, "LD H,E", Register_H, Register_E, 4));
    factory->RegisterCommand(new LoadCommand(0x64, "LD H,H", Register_H, Register_H, 4));
    factory->RegisterCommand(new LoadCommand(0x65, "LD H,L", Register_H, Register_L, 4));
    factory->RegisterCommand(new LoadCommand(0x66, "LD H,(HL)", Register_H, Address_HL, 8));
    factory->RegisterCommand(new LoadCommand(0x68, "LD L,B", Register_L, Register_B, 4));
    factory->RegisterCommand(new LoadCommand(0x69, "LD L,C", Register_L, Register_C, 4));
    factory->RegisterCommand(new LoadCommand(0x6A, "LD L,D", Register_L, Register_D, 4));
    factory->RegisterCommand(new LoadCommand(0x6B, "LD L,E", Register_L, Register_E, 4));
    factory->RegisterCommand(new LoadCommand(0x6C, "LD L,H", Register_L, Register_H, 4));
    factory->RegisterCommand(new LoadCommand(0x6D, "LD L,L", Register_L, Register_L, 4));
    factory->RegisterCommand(new LoadCommand(0x6E, "LD L,(HL)", Register_L, Address_HL, 8));
    factory->RegisterCommand(new LoadCommand(0x70, "LD (HL),B", Address_HL, Register_B, 8));
    factory->RegisterCommand(new LoadCommand(0x71, "LD (HL),C", Address_HL, Register_C, 8));
    factory->RegisterCommand(new LoadCommand(0x72, "LD (HL),D", Address_HL, Register_D, 8));
    factory->RegisterCommand(new LoadCommand(0x73, "LD (HL),E", Address_HL, Register_E, 8));
    factory->RegisterCommand(new LoadCommand(0x74, "LD (HL),H", Address_HL, Register_H, 8));
    factory->RegisterCommand(new LoadCommand(0x75, "LD (HL),L", Address_HL, Register_L, 8));
    factory->RegisterCommand(new LoadCommand(0x36, "LD (HL),n", Address_HL, Eat_PC_Byte, 12));
    factory->RegisterCommand(new LoadCommand(0x0A, "LD A,(BC)", Register_A, Address_BC, 8));
    factory->RegisterCommand(new LoadCommand(0x1A, "LD A,(DE)", Register_A, Address_DE, 8));
    factory->RegisterCommand(new LoadCommand(0xFA, "LD A,(nn)", Register_A, Address_nn, 16));
    factory->RegisterCommand(new LoadCommand(0x3E, "LD A,#", Register_A, Eat_PC_Byte, 8));
    factory->RegisterCommand(new LoadCommand(0x47, "LD B,A", Register_B, Register_A, 4));
    factory->RegisterCommand(new LoadCommand(0x4F, "LD C,A", Register_C, Register_A, 4));
    factory->RegisterCommand(new LoadCommand(0x57, "LD D,A", Register_D, Register_A, 4));
    factory->RegisterCommand(new LoadCommand(0x5F, "LD E,A", Register_E, Register_A, 4));
    factory->RegisterCommand(new LoadCommand(0x67, "LD H,A", Register_H, Register_A, 4));
    factory->RegisterCommand(new LoadCommand(0x6F, "LD L,A", Register_L, Register_A, 4));
}
