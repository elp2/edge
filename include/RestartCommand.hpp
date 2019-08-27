#pragma once

#include "Command.hpp"

class RestartCommand : public Command {

public:
    RestartCommand(uint8_t opcode, string description, int cycles);
    ~RestartCommand();

    void Run(CPU *cpu, MMU *mmu);    
};

void registerRestartCommands(CPU *cpu);