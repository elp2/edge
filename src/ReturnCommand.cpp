#include "ReturnCommand.hpp"

#include "CPU.hpp"
#include "MMU.hpp"

ReturnCommand::ReturnCommand(uint8_t opcode, string description, int cycles) {
    this->opcode = opcode;
    this->description = description;
    this->cycles = cycles;
}

ReturnCommand::~ReturnCommand() {

}

void ReturnCommand::Run(CPU *cpu, MMU *mmu) {
    (void)mmu;

    // TODO - when we take a branch it eats 12 more cycles?

    uint64_t address;
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
            cpu->enableInterruptsRequested = true;
            break;

    default:
        cout << "un-implemented Restart: " << hex << unsigned(opcode) << endl;
        assert(false);
        break;
    }
}

void registerReturnCommands(CPU *cpu) {
    cpu->RegisterCommand(new ReturnCommand(0xc9, "RET", 8));

    cpu->RegisterCommand(new ReturnCommand(0xc0, "RET NZ", 8));
    cpu->RegisterCommand(new ReturnCommand(0xc8, "RET Z", 8));
    cpu->RegisterCommand(new ReturnCommand(0xd0, "RET CZ", 8));
    cpu->RegisterCommand(new ReturnCommand(0xd8, "RET C", 8));

    cpu->RegisterCommand(new ReturnCommand(0xd9, "RETI", 8));
}