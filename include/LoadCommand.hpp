#pragma once

#include "Command.hpp"
#include "Destination.hpp"

class AbstractCommandFactory;

class LoadCommand : public Command {

 public:
    LoadCommand(uint8_t opcode, string description, Destination to, Destination from, int cycles);
    ~LoadCommand();

    void Run(CPU *cpu, MMU *mmu);    

 private:
    Destination from;
    Destination to;
};

void registerLoadCommands(AbstractCommandFactory *factory);