#include "RestartCommand.hpp"

#include "CommandFactory.hpp"
#include "CPU.hpp"
#include "MMU.hpp"
RestartCommand::RestartCommand(uint8_t opcode, string description, int cycles) {
    this->opcode = opcode;
    this->description = description;
    this->cycles = cycles;
}

RestartCommand::~RestartCommand() {

}

void RestartCommand::Run(CPU *cpu, MMU *mmu) {
    (void)cpu;
    (void)mmu; // TODO remove - temporarily silencing warning.
    switch (this->opcode)
    {
    case 0xC3:
  
    default:
        cout << "un-implemented Restart: " << hex << unsigned(opcode) << endl;
        assert(false);
        break;
    }
}

void registerRestartCommands(AbstractCommandFactory *factory) {
    (void)factory;
}