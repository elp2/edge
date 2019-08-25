#include "NopCommand.hpp"

#include "CPU.hpp"
#include "MMU.hpp"

NopCommand::NopCommand(uint8_t opcode) {
    this->opcode = opcode;
    this->description = "NOP";
    this->cycles = 4;
}

NopCommand::~NopCommand() {

}

void NopCommand::Run(CPU *cpu, MMU *mmu) {
    // Nop.
    (void)cpu;
    (void)mmu;
}

void registerNopCommands(CPU *cpu) {
    cpu->RegisterCommand(new NopCommand(0x00));
}