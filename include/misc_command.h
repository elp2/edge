#pragma once

#include "command.h"

class AbstractCommandFactory;

class MiscCommand : public Command {

public:
    MiscCommand(uint8_t opcode, string description, int cycles);
    ~MiscCommand();

    void Run(CPU *cpu);
};

void registerMiscCommands(AbstractCommandFactory *factory);