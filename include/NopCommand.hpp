#pragma once

#include "Command.hpp"

class AbstractCommandFactory;

class NopCommand : public Command {

public:
    NopCommand(uint8_t opcode);
    ~NopCommand();
    void Run(CPU *cpu, MMU *mmu);
};

void registerNopCommands(AbstractCommandFactory *factory);