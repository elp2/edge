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

void JumpCommand::Run(CPU *cpu, MMU *mmu) {
    (void)mmu; // TODO remove - temporarily silencing warning.
    bool jump = false;
    uint16_t jumpAddress;
    switch (this->opcode)
    {
    case 0xC3:
        jump = true;
        jumpAddress = cpu->Read16Bit(Eat_PC_Word);
        break;
    case 0xC2:
    case 0xCA:
    case 0xD2:
    case 0xDA:

    //case 
//     JP NZ,nn C2 12
//  JP Z,nn CA 12
//  JP NC,nn D2 12
//  JP C,nn DA 12
        break;

// JP (HL) E9 4

    // case // 
//     ction Parameters Command Cycles
//  JR n 18 8
    case 0x18:
        jump=true;
        jumpAddress = cpu->pc + cpu->Read8Bit(Eat_PC_Byte);
        break;

// uction Parameters Command Cycles
//  JR NZ,* 20 8
//  JR Z,* 28 8
//  JR NC,* 30 8
//  JR C,* 38 8
    default:
        cout << "un-implemented Jump: " << hex << unsigned(opcode) << endl;
        break;
    }

    if (!jump) {
        return;
    }

    cout << "Jumping to 0x" << hex << unsigned(jumpAddress) << endl;
    cpu->Set16Bit(Register_PC, jumpAddress);
}

void registerJumpCommands(CPU *cpu) {
     cpu->RegisterCommand(new JumpCommand(0xC3, "JP nn", 12));
     cpu->RegisterCommand(new JumpCommand(0x18, "JR n", 8));
}