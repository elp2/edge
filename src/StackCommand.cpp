#include "StackCommand.hpp"

#include <sstream>

#include "CommandFactory.hpp"
#include "CPU.hpp"
#include "MMU.hpp"
#include "Utils.hpp"

Destination stackDestinationForRow(uint8_t row) {
    switch (row)
    {
    case 0xc:
        return Register_BC;
    case 0xd:
        return Register_DE;
    case 0xe:
        return Register_HL;
    case 0xf:
        return Register_AF;
    default:
        cout << "Unknown row: " << hex << unsigned(row) << endl;
        assert(false);
        break;
    }
}

StackCommand::StackCommand(uint8_t opcode) {
    this->opcode = opcode;
}

StackCommand::~StackCommand() {}

void StackCommand::Push(CPU *cpu, Destination d) {
    cycles = 16;

    stringstream stream;
    stream << "PUSH " << destinationToString(d);
    description = stream.str();

    cpu->Push16Bit(cpu->Get16Bit(d));
}

void StackCommand::Pop(CPU *cpu, Destination d) {
    cycles = 12;

    stringstream stream;
    stream << "POP " << destinationToString(d);
    description = stream.str();

    cpu->Set16Bit(d, cpu->Pop16Bit());
}

void StackCommand::Run(CPU *cpu, MMU *mmu) {
    (void)mmu;

    uint8_t row = NIBBLEHIGH(opcode);
    uint8_t col = NIBBLELOW(opcode);
    Destination d = stackDestinationForRow(row);
    if (col == 0x5) {
        return Push(cpu, d);
    } else {
        return Pop(cpu, d);
    }


}

void registerStackCommands(AbstractCommandFactory *factory) {
    // PUSH.
    factory->RegisterCommand(new StackCommand(0xc5));
    factory->RegisterCommand(new StackCommand(0xd5));
    factory->RegisterCommand(new StackCommand(0xe5));
    factory->RegisterCommand(new StackCommand(0xf5));

    // POP.
    factory->RegisterCommand(new StackCommand(0xc1));
    factory->RegisterCommand(new StackCommand(0xd1));
    factory->RegisterCommand(new StackCommand(0xe1));
    factory->RegisterCommand(new StackCommand(0xf1));
}