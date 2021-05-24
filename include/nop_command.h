#pragma once

#include "command.h"

class AbstractCommandFactory;

class NopCommand : public Command {
 public:
  NopCommand(uint8_t opcode);
  ~NopCommand();
  void Run(CPU *cpu);
};

void registerNopCommands(AbstractCommandFactory *factory);
