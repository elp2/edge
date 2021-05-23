#pragma once

#include "command.h"
#include "destination.h"

class AbstractCommandFactory;

class StackCommand : public Command {

 public:
    StackCommand(uint8_t opcode);
    ~StackCommand();
    void Run(CPU *cpu);

 private:
    void Push(CPU *cpu, Destination d);
    void Pop(CPU *cpu, Destination d);
};

void registerStackCommands(AbstractCommandFactory *factory);