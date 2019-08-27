#pragma once

#include "Command.hpp"

class AbstractCommandFactory;

class BitCommand : public Command {

public:
    BitCommand(uint8_t opcode, string description, int cycles);
    ~BitCommand();
    void Run(CPU *cpu, MMU *mmu);
};

void registerBitCommands(AbstractCommandFactory *factory);