#pragma once

#include "Command.hpp"

class AbstractCommandFactory;

class JumpCommand : public Command {

public:
    JumpCommand(uint8_t opcode, string description, int cycles);
    ~JumpCommand();

    void Run(CPU *cpu, MMU *mmu);    
};

void registerJumpCommands(AbstractCommandFactory *factory);