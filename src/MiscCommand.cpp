#include "MiscCommand.hpp"

MiscCommand::MiscCommand(uint8_t opcode, string description, int cycles) {
    this->opcode = opcode;
    this->description = description;
    this->cycles = cycles;
}

MiscCommand::~MiscCommand() {

}

void MiscCommand::Run(CPU *cpu, MMU *mmu) {
    (void)mmu;
    uint8_t nextPCByte;
    switch (opcode)
    {
    case 0x27:
        // TODO DA.
        assert(false); // TODO!
        break;
    case 0x2F:
        cpu->a = ~cpu->a;
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
        cpu->haltRequested = true;
        break;
    case 0x10:
        cpu->stopRequested = true;
        nextPCByte = cpu->ReadOpcodeAtPC();
        assert(nextPCByte == 0x00);
        cpu->AdvancePC();
        break;
    case 0xF3:
        cpu->disableInterruptsRequested = true;
        break;
    case 0xFB:
        cpu->enableInterruptsRequested = true;
        break;
    
    
    default:
        cout << "Attempt to use MiscComman for " << hex << unsigned(opcode) << endl;
        assert(false);
        break;
    }
}

void registerMiscCommands(CPU *cpu) {
    cpu->RegisterOpcode(new MiscCommand(0x27, "DAA", 4));
    cpu->RegisterOpcode(new MiscCommand(0x2F, "CPL", 4));
    cpu->RegisterOpcode(new MiscCommand(0x3F, "CCF", 4));
    cpu->RegisterOpcode(new MiscCommand(0x37, "SCF", 4));
    cpu->RegisterOpcode(new MiscCommand(0x76, "HALT", 4));
    cpu->RegisterOpcode(new MiscCommand(0x10, "STOP", 4));
    cpu->RegisterOpcode(new MiscCommand(0xF3, "DI", 4));
    cpu->RegisterOpcode(new MiscCommand(0xFB, "EI", 4));
}