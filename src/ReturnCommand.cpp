#include "ReturnCommand.hpp"

#include <cassert>

#include "CommandFactory.hpp"
#include "CPU.hpp"
#include "MMU.hpp"

ReturnCommand::ReturnCommand(uint8_t opcode, string description, int cycles) {
    this->opcode = opcode;
    this->description = description;
    this->cycles = cycles;
}

ReturnCommand::~ReturnCommand() {

}

void ReturnCommand::Run(CPU *cpu) {
    // TODO - when we take a branch it eats 12 more cycles?

    switch (this->opcode)
    {  
        case 0xc9:
            cpu->JumpAddress(cpu->Pop16Bit());
            break;
        case 0xc0:
            if (!cpu->flags.z) {
                cpu->JumpAddress(cpu->Pop16Bit());
            }
            break;
        case 0xc8:
            if (cpu->flags.z) {
                cpu->JumpAddress(cpu->Pop16Bit());
            }
            break;
        case 0xd0:
            if (!cpu->flags.c) {
                cpu->JumpAddress(cpu->Pop16Bit());
            }
            break;
        case 0xd8:
            if (cpu->flags.c) {
                cpu->JumpAddress(cpu->Pop16Bit());
            }
            break;
        case 0xd9:
            cpu->JumpAddress(cpu->Pop16Bit());
            cpu->EnableInterruptsNextLoop();
            break;

    default:
        cout << "un-implemented Restart: " << hex << unsigned(opcode) << endl;
        assert(false);
        break;
    }
}

void registerReturnCommands(AbstractCommandFactory *factory) {
    factory->RegisterCommand(new ReturnCommand(0xc9, "RET", 8));

    factory->RegisterCommand(new ReturnCommand(0xc0, "RET NZ", 8));
    factory->RegisterCommand(new ReturnCommand(0xc8, "RET Z", 8));
    factory->RegisterCommand(new ReturnCommand(0xd0, "RET CZ", 8));
    factory->RegisterCommand(new ReturnCommand(0xd8, "RET C", 8));

    factory->RegisterCommand(new ReturnCommand(0xd9, "RETI", 8));
}