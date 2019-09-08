#include "JumpCommand.hpp"

#include <cassert>

#include "CommandFactory.hpp"
#include "CPU.hpp"
#include "MMU.hpp"

JumpCommand::JumpCommand(uint8_t opcode, string description, int cycles) {
    this->opcode = opcode;
    this->description = description;
    this->cycles = cycles;
}

JumpCommand::~JumpCommand() {

}

void JumpCommand::JumpCondition(CPU *cpu, bool jump, Destination destination) {
    uint16_t address = cpu->Get16Bit(destination);
    if (jump) {
        cycles = destination == Register_HL ? 4 : 16;
        cpu->JumpAddress(address);
    }
}

void JumpCommand::JumpConditionRelative(CPU *cpu, bool jump, Destination destination) {
    uint8_t relative = cpu->Get8Bit(destination);

    if (jump) {
        cycles = 12;
        cpu->JumpRelative(relative);
    }
}

void JumpCommand::Run(CPU *cpu) {
    switch (this->opcode)
    {
    case 0xC3:
        JumpCondition(cpu, true, Eat_PC_Word);
        return;
    case 0xC2:
        JumpCondition(cpu, !cpu->flags.z, Eat_PC_Word);
        return;
    case 0xCA:
        JumpCondition(cpu, cpu->flags.z, Eat_PC_Word);
        return;
    case 0xD2:
        JumpCondition(cpu, !cpu->flags.c, Eat_PC_Word);
        return;
    case 0xDA:
        JumpCondition(cpu, cpu->flags.c, Eat_PC_Word);
        return;
    case 0xE9:
        JumpCondition(cpu, true, Register_HL);
        return;
    case 0x18:
        JumpConditionRelative(cpu, true, Eat_PC_Byte);
        return;
    case 0x20:
        JumpConditionRelative(cpu, !cpu->flags.z, Eat_PC_Byte);
        //  JR NZ,* 20 8
        return;
    case 0x28:
        JumpConditionRelative(cpu, cpu->flags.z, Eat_PC_Byte);
        //  JR Z,* 28 8
        return;
    case 0x30:
        JumpConditionRelative(cpu, !cpu->flags.c, Eat_PC_Byte);
        //  JR NC,* 30 8
        return;
    case 0x38:
        JumpCommand::JumpConditionRelative(cpu, cpu->flags.c, Eat_PC_Byte);
        //  JR C,* 38 8
        return;
    default:
        cout << "un-implemented Jump: " << hex << unsigned(opcode) << endl;
        assert(false);
        break;
    }
    assert(false);
}

void registerJumpCommands(AbstractCommandFactory *factory) {
     factory->RegisterCommand(new JumpCommand(0xC3, "JP nn", 12));

     factory->RegisterCommand(new JumpCommand(0xC2, "JP NZ,nn", 12));
     factory->RegisterCommand(new JumpCommand(0xCA, "JP Z,nn", 12));
     factory->RegisterCommand(new JumpCommand(0xD2, "JP NC,nn", 12));
     factory->RegisterCommand(new JumpCommand(0xDA, "JP C,nn", 12));

     factory->RegisterCommand(new JumpCommand(0xE9, "JP (HL)", 4));

     factory->RegisterCommand(new JumpCommand(0x18, "JR n", 8));

     factory->RegisterCommand(new JumpCommand(0x20, "JR NZ,*", 8));
     factory->RegisterCommand(new JumpCommand(0x28, "JR Z,*", 8));
     factory->RegisterCommand(new JumpCommand(0x30, "JR NC,*", 8));
     factory->RegisterCommand(new JumpCommand(0x38, "JR C,*", 8));
}