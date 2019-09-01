#pragma once

#include "Command.hpp"
#include "Destination.hpp"

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