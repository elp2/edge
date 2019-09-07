#include "MiscCommand.hpp"

#include <cassert>

#include "CommandFactory.hpp"
#include "CPU.hpp"
#include "MMU.hpp"

MiscCommand::MiscCommand(uint8_t opcode, string description, int cycles) {
    this->opcode = opcode;
    this->description = description;
    this->cycles = cycles;
}

MiscCommand::~MiscCommand() {

}

void MiscCommand::Run(CPU *cpu) {
    uint8_t nextPCByte;
    switch (opcode)
    {
    case 0x27:
        // TODO DA.
        assert(false); // TODO!
        break;
    case 0x2F:
        cpu->Set8Bit(Register_A, ~cpu->Get8Bit(Register_A));
        // TODO: n, h
        assert(false); // TODO!
        break;
    case 0x3F:
        // z unchanged.
        cpu->flags.n = false;
        cpu->flags.h = false;
        cpu->flags.c = !cpu->flags.c;
        break;
    case 0x37:
        // z unchanged.
        cpu->flags.n = false;
        cpu->flags.h = false;
        cpu->flags.c = true;
        break;
    case 0x76:
        cpu->HaltNextLoop();
        break;
    case 0x10:
        cpu->StopNextLoop();
        nextPCByte = cpu->Get8Bit(Eat_PC_Byte);
        assert(nextPCByte == 0x00);
        cpu->AdvancePC();
        break;
    case 0xF3:
        cpu->DisableInterruptsNextLoop();
        break;
    case 0xFB:
        cpu->EnableInterruptsNextLoop();
        break;
    
    default:
        cout << "Attempt to use MiscComman for " << hex << unsigned(opcode) << endl;
        assert(false);
        break;
    }
}

void registerMiscCommands(AbstractCommandFactory *factory) {
    factory->RegisterCommand(new MiscCommand(0x27, "DAA", 4));
    factory->RegisterCommand(new MiscCommand(0x2F, "CPL", 4));
    factory->RegisterCommand(new MiscCommand(0x3F, "CCF", 4));
    factory->RegisterCommand(new MiscCommand(0x37, "SCF", 4));
    factory->RegisterCommand(new MiscCommand(0x76, "HALT", 4));
    factory->RegisterCommand(new MiscCommand(0x10, "STOP", 4));
    factory->RegisterCommand(new MiscCommand(0xF3, "DI", 4));
    factory->RegisterCommand(new MiscCommand(0xFB, "EI", 4));
}