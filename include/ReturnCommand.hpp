#pragma once

#include "Command.hpp"

class AbstractCommandFactory;

class ReturnCommand : public Command {

public:
    ReturnCommand(uint8_t opcode, string description, int cycles);
    ~ReturnCommand();

    void Run(CPU *cpu);    
};

void registerReturnCommands(AbstractCommandFactory *factory);