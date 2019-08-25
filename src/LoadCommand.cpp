#include "LoadCommand.hpp"

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
    bool use16Bit = cpu->Requires16Bits(from);
    if (use16Bit) {
        cpu->Set16Bit(to, cpu->Read16Bit(from));
    } else {
        cpu->Set8Bit(to, cpu->Read8Bit(from));
    }
}

void registerLoadCommands(CPU *cpu) {
cpu->RegisterCommand(new LoadCommand(0x7F, "LD A,A", Register_A, Register_A, 4));
cpu->RegisterCommand(new LoadCommand(0x78, "LD A,B", Register_A, Register_B, 4));
cpu->RegisterCommand(new LoadCommand(0x79, "LD A,C", Register_A, Register_C, 4));
cpu->RegisterCommand(new LoadCommand(0x7A, "LD A,D", Register_A, Register_D, 4));
cpu->RegisterCommand(new LoadCommand(0x7B, "LD A,E", Register_A, Register_E, 4));
cpu->RegisterCommand(new LoadCommand(0x7C, "LD A,H", Register_A, Register_H, 4));
cpu->RegisterCommand(new LoadCommand(0x7D, "LD A,L", Register_A, Register_L, 4));
cpu->RegisterCommand(new LoadCommand(0x7E, "LD A,(HL)", Register_A, Register_HL, 8));
cpu->RegisterCommand(new LoadCommand(0x40, "LD B,B", Register_B, Register_B, 4));
cpu->RegisterCommand(new LoadCommand(0x41, "LD B,C", Register_B, Register_C, 4));
cpu->RegisterCommand(new LoadCommand(0x42, "LD B,D", Register_B, Register_D, 4));
cpu->RegisterCommand(new LoadCommand(0x43, "LD B,E", Register_B, Register_E, 4));
cpu->RegisterCommand(new LoadCommand(0x44, "LD B,H", Register_B, Register_H, 4));
cpu->RegisterCommand(new LoadCommand(0x45, "LD B,L", Register_B, Register_L, 4));
cpu->RegisterCommand(new LoadCommand(0x46, "LD B,(HL)", Register_B, Register_HL, 8));
cpu->RegisterCommand(new LoadCommand(0x48, "LD C,B", Register_C, Register_B, 4));
cpu->RegisterCommand(new LoadCommand(0x49, "LD C,C", Register_C, Register_C, 4));
cpu->RegisterCommand(new LoadCommand(0x4A, "LD C,D", Register_C, Register_D, 4));
cpu->RegisterCommand(new LoadCommand(0x4B, "LD C,E", Register_C, Register_E, 4));
cpu->RegisterCommand(new LoadCommand(0x4C, "LD C,H", Register_C, Register_H, 4));
cpu->RegisterCommand(new LoadCommand(0x4D, "LD C,L", Register_C, Register_L, 4));
cpu->RegisterCommand(new LoadCommand(0x4E, "LD C,(HL)", Register_C, Register_HL, 8));
cpu->RegisterCommand(new LoadCommand(0x50, "LD D,B", Register_D, Register_B, 4));
cpu->RegisterCommand(new LoadCommand(0x51, "LD D,C", Register_D, Register_C, 4));
cpu->RegisterCommand(new LoadCommand(0x52, "LD D,D", Register_D, Register_D, 4));
cpu->RegisterCommand(new LoadCommand(0x53, "LD D,E", Register_D, Register_E, 4));
cpu->RegisterCommand(new LoadCommand(0x54, "LD D,H", Register_D, Register_H, 4));
cpu->RegisterCommand(new LoadCommand(0x55, "LD D,L", Register_D, Register_L, 4));
cpu->RegisterCommand(new LoadCommand(0x56, "LD D,(HL)", Register_D, Register_HL, 8));
cpu->RegisterCommand(new LoadCommand(0x58, "LD E,B", Register_E, Register_B, 4));
cpu->RegisterCommand(new LoadCommand(0x59, "LD E,C", Register_E, Register_C, 4));
cpu->RegisterCommand(new LoadCommand(0x5A, "LD E,D", Register_E, Register_D, 4));
cpu->RegisterCommand(new LoadCommand(0x5B, "LD E,E", Register_E, Register_E, 4));
cpu->RegisterCommand(new LoadCommand(0x5C, "LD E,H", Register_E, Register_H, 4));
cpu->RegisterCommand(new LoadCommand(0x5D, "LD E,L", Register_E, Register_L, 4));
cpu->RegisterCommand(new LoadCommand(0x5E, "LD E,(HL)", Register_E, Register_HL, 8));
cpu->RegisterCommand(new LoadCommand(0x60, "LD H,B", Register_H, Register_B, 4));
cpu->RegisterCommand(new LoadCommand(0x61, "LD H,C", Register_H, Register_C, 4));
cpu->RegisterCommand(new LoadCommand(0x62, "LD H,D", Register_H, Register_D, 4));
cpu->RegisterCommand(new LoadCommand(0x63, "LD H,E", Register_H, Register_E, 4));
cpu->RegisterCommand(new LoadCommand(0x64, "LD H,H", Register_H, Register_H, 4));
cpu->RegisterCommand(new LoadCommand(0x65, "LD H,L", Register_H, Register_L, 4));
cpu->RegisterCommand(new LoadCommand(0x66, "LD H,(HL)", Register_H, Register_HL, 8));
cpu->RegisterCommand(new LoadCommand(0x68, "LD L,B", Register_L, Register_B, 4));
cpu->RegisterCommand(new LoadCommand(0x69, "LD L,C", Register_L, Register_C, 4));
cpu->RegisterCommand(new LoadCommand(0x6A, "LD L,D", Register_L, Register_D, 4));
cpu->RegisterCommand(new LoadCommand(0x6B, "LD L,E", Register_L, Register_E, 4));
cpu->RegisterCommand(new LoadCommand(0x6C, "LD L,H", Register_L, Register_H, 4));
cpu->RegisterCommand(new LoadCommand(0x6D, "LD L,L", Register_L, Register_L, 4));
cpu->RegisterCommand(new LoadCommand(0x6E, "LD L,(HL)", Register_L, Register_HL, 8));
cpu->RegisterCommand(new LoadCommand(0x70, "LD (HL),B", Register_HL, Register_B, 8));
cpu->RegisterCommand(new LoadCommand(0x71, "LD (HL),C", Register_HL, Register_C, 8));
cpu->RegisterCommand(new LoadCommand(0x72, "LD (HL),D", Register_HL, Register_D, 8));
cpu->RegisterCommand(new LoadCommand(0x73, "LD (HL),E", Register_HL, Register_E, 8));
cpu->RegisterCommand(new LoadCommand(0x74, "LD (HL),H", Register_HL, Register_H, 8));
cpu->RegisterCommand(new LoadCommand(0x75, "LD (HL),L", Register_HL, Register_L, 8));
cpu->RegisterCommand(new LoadCommand(0x36, "LD (HL),n", Register_HL, Eat_PC_Byte, 12));
cpu->RegisterCommand(new LoadCommand(0x06, "LD B,n", Register_B, Eat_PC_Byte, 8));
cpu->RegisterCommand(new LoadCommand(0x0E, "LD C,n", Register_C, Eat_PC_Byte, 8));
cpu->RegisterCommand(new LoadCommand(0x16, "LD D,n", Register_D, Eat_PC_Byte, 8));
cpu->RegisterCommand(new LoadCommand(0x1E, "LD E,n", Register_E, Eat_PC_Byte, 8));
cpu->RegisterCommand(new LoadCommand(0x26, "LD H,n", Register_H, Eat_PC_Byte, 8));
cpu->RegisterCommand(new LoadCommand(0x2E, "LD L,n", Register_L, Eat_PC_Byte, 8));
cpu->RegisterCommand(new LoadCommand(0x02, "LD (BC),A", Register_BC, Register_A, 8));
cpu->RegisterCommand(new LoadCommand(0x12, "LD (DE),A", Register_DE, Register_A, 8));
cpu->RegisterCommand(new LoadCommand(0x77, "LD (HL),A", Register_HL, Register_A, 8));
cpu->RegisterCommand(new LoadCommand(0xEA, "LD (nn),A", Eat_PC_Word, Register_A, 16));
cpu->RegisterCommand(new LoadCommand(0x01, "LD BC,nn", Register_BC, Eat_PC_Word, 12));
cpu->RegisterCommand(new LoadCommand(0x11, "LD DE,nn", Register_DE, Eat_PC_Word, 12));
cpu->RegisterCommand(new LoadCommand(0x21, "LD HL,nn", Register_HL, Eat_PC_Word, 12));
cpu->RegisterCommand(new LoadCommand(0x31, "LD SP,nn", Register_SP, Eat_PC_Word, 12));
cpu->RegisterCommand(new LoadCommand(0x47, "LD B,A", Register_B, Register_A, 4));
cpu->RegisterCommand(new LoadCommand(0x4F, "LD C,A", Register_C, Register_A, 4));
cpu->RegisterCommand(new LoadCommand(0x57, "LD D,A", Register_D, Register_A, 4));
cpu->RegisterCommand(new LoadCommand(0x5F, "LD E,A", Register_E, Register_A, 4));
cpu->RegisterCommand(new LoadCommand(0x67, "LD H,A", Register_H, Register_A, 4));
cpu->RegisterCommand(new LoadCommand(0x6F, "LD L,A", Register_L, Register_A, 4));
}