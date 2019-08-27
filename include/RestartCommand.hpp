#pragma once

#include "Command.hpp"

class AbstractCommandFactory;

class RestartCommand : public Command {

public:
    RestartCommand(uint8_t opcode, string description, int cycles);
    ~RestartCommand();

    void Run(CPU *cpu, MMU *mmu);    
};

void registerRestartCommands(AbstractCommandFactory *factory);