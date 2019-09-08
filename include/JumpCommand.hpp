#pragma once

#include "Command.hpp"
#include "Destination.hpp"

class AbstractCommandFactory;

class JumpCommand : public Command {
 private:
    void JumpCondition(CPU *cpu, bool jump, Destination destination);
    void JumpConditionRelative(CPU *cpu, bool jump, Destination destination);

 public:
    JumpCommand(uint8_t opcode, string description, int cycles);
    ~JumpCommand();

    void Run(CPU *cpu);
};

void registerJumpCommands(AbstractCommandFactory *factory);
