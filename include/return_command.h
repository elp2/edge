#pragma once

#include "command.h"

class AbstractCommandFactory;

class ReturnCommand : public Command {
 public:
  ReturnCommand(uint8_t opcode, string description, int cycles);
  ~ReturnCommand();

  void Run(CPU *cpu);

 private:
  int base_cycles_ = 0;
};

void registerReturnCommands(AbstractCommandFactory *factory);