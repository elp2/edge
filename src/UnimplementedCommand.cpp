#include "UnimplementedCommand.hpp"

#include "CommandFactory.hpp"
#include "CPU.hpp"
#include "MMU.hpp"

UnimplementedCommand::UnimplementedCommand(uint8_t opcode) {
    this->opcode = opcode;
    this->description = "Unimplemented CPU Opcode!";
    this->cycles = 0;
}

UnimplementedCommand::~UnimplementedCommand() {

}

void UnimplementedCommand::Run(CPU *cpu, MMU *mmu) {
    // Nop.
    (void)cpu;
    (void)mmu;

    cout << "Unimplemented CPU opcode: " << opcode << endl;
    assert(false);
}

void registerUnimplementedCommands(AbstractCommandFactory *factory) {
    factory->RegisterCommand(new UnimplementedCommand(0xD3));
    factory->RegisterCommand(new UnimplementedCommand(0xDB));
    factory->RegisterCommand(new UnimplementedCommand(0xDD));
    factory->RegisterCommand(new UnimplementedCommand(0xE3));
    factory->RegisterCommand(new UnimplementedCommand(0xE4));
    factory->RegisterCommand(new UnimplementedCommand(0xEB));
    factory->RegisterCommand(new UnimplementedCommand(0xEC));
    factory->RegisterCommand(new UnimplementedCommand(0xED));
    factory->RegisterCommand(new UnimplementedCommand(0xF4));
    factory->RegisterCommand(new UnimplementedCommand(0xFC));
    factory->RegisterCommand(new UnimplementedCommand(0xFD));
}