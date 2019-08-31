#pragma once

#include "Command.hpp"

class AbstractCommandFactory;

class UnimplementedCommand : public Command {

public:
    UnimplementedCommand(uint8_t opcode);
    ~UnimplementedCommand();
    void Run(CPU *cpu, MMU *mmu);
};

void registerUnimplementedCommands(AbstractCommandFactory *factory);