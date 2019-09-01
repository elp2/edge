#include "NopCommand.hpp"

#include "CommandFactory.hpp"
#include "CPU.hpp"
#include "MMU.hpp"

NopCommand::NopCommand(uint8_t opcode) {
    this->opcode = opcode;
    this->description = "NOP";
    this->cycles = 4;
}

NopCommand::~NopCommand() {

}

void NopCommand::Run(CPU *cpu) {
    // Nop.
    (void)cpu;
}

void registerNopCommands(AbstractCommandFactory *factory) {
    factory->RegisterCommand(new NopCommand(0x00));
}