#pragma once

#include "command.h"
#include "destination.h"

class AbstractCommandFactory;

class JumpCommand : public Command {
 private:
  void JumpCondition(CPU *cpu, bool jump, Destination destination);
  void JumpConditionRelative(CPU *cpu, bool jump, Destination destination);
  int base_cycles_ = 0;

 public:
  JumpCommand(uint8_t opcode, string description, int cycles);
  ~JumpCommand();

  void Run(CPU *cpu);
};

void registerJumpCommands(AbstractCommandFactory *factory);
