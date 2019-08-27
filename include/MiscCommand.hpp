#pragma once

#include "Command.hpp"

class AbstractCommandFactory;

class MiscCommand : public Command {

public:
    MiscCommand(uint8_t opcode, string description, int cycles);
    ~MiscCommand();

    void Run(CPU *cpu, MMU *mmu);
};

void registerMiscCommands(AbstractCommandFactory *factory);