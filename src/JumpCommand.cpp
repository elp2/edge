#include "JumpCommand.hpp"

#include "CPU.hpp"
#include "MMU.hpp"

JumpCommand::JumpCommand(uint8_t opcode, string description, int cycles) {
    this->opcode = opcode;
    this->description = description;
    this->cycles = cycles;
}

JumpCommand::~JumpCommand() {

}

void jumpCondition(CPU *cpu, bool jump, uint16_t address) {
    if (jump) {
        cpu->JumpAddress(address);
    }
}

void jumpConditionRelative(CPU *cpu, bool jump, uint8_t relative) {
    if (jump) {
        cpu->JumpRelative(relative);
    }
}

void JumpCommand::Run(CPU *cpu, MMU *mmu) {
    (void)mmu; // TODO remove - temporarily silencing warning.
    switch (this->opcode)
    {
    case 0xC3:
        jumpCondition(cpu, true, cpu->Read16Bit(Eat_PC_Word));
        return;
    case 0xC2:
        jumpCondition(cpu, !cpu->flags.z, cpu->Read16Bit(Eat_PC_Word));
        return;
    case 0xCA:
        jumpCondition(cpu, cpu->flags.z, cpu->Read16Bit(Eat_PC_Word));
        return;
    case 0xD2:
        jumpCondition(cpu, !cpu->flags.c, cpu->Read16Bit(Eat_PC_Word));
        return;
    case 0xDA:
        jumpCondition(cpu, cpu->flags.c, cpu->Read16Bit(Eat_PC_Word));
        return;
    case 0xE9:
        jumpCondition(cpu, true, cpu->Read16Bit(Register_HL));
        return;
    case 0x18:
        jumpConditionRelative(cpu, true, cpu->Read8Bit(Eat_PC_Byte));
        return;
    case 0x20:
        jumpConditionRelative(cpu, !cpu->flags.z, cpu->Read8Bit(Eat_PC_Byte));
        //  JR NZ,* 20 8
        return;
    case 0x28:
        jumpConditionRelative(cpu, cpu->flags.z, cpu->Read8Bit(Eat_PC_Byte));
        //  JR Z,* 28 8
        return;
    case 0x30:
        jumpConditionRelative(cpu, !cpu->flags.c, cpu->Read8Bit(Eat_PC_Byte));
        //  JR NC,* 30 8
        return;
    case 0x38:
        jumpConditionRelative(cpu, cpu->flags.c, cpu->Read8Bit(Eat_PC_Byte));
        //  JR C,* 38 8
        return;
    default:
        cout << "un-implemented Jump: " << hex << unsigned(opcode) << endl;
        assert(false);
        break;
    }
    // TODO - nothing should hit here.
    assert(false);
}

void registerJumpCommands(CPU *cpu) {
     cpu->RegisterCommand(new JumpCommand(0xC3, "JP nn", 12));

     cpu->RegisterCommand(new JumpCommand(0xC2, "JP NZ,nn", 12));
     cpu->RegisterCommand(new JumpCommand(0xCA, "JP Z,nn", 12));
     cpu->RegisterCommand(new JumpCommand(0xD2, "JP NC,nn", 12));
     cpu->RegisterCommand(new JumpCommand(0xDA, "JP C,nn", 12));

     cpu->RegisterCommand(new JumpCommand(0xE9, "JP (HL)", 4));

     cpu->RegisterCommand(new JumpCommand(0x18, "JR n", 8));

     cpu->RegisterCommand(new JumpCommand(0x20, "JR NZ,*", 8));
     cpu->RegisterCommand(new JumpCommand(0x28, "JR Z,*", 8));
     cpu->RegisterCommand(new JumpCommand(0x30, "JR NC,*", 8));
     cpu->RegisterCommand(new JumpCommand(0x38, "JR C,*", 8));
}