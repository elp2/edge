#pragma once

#include "command.h"

class AbstractCommandFactory;

class UnimplementedCommand : public Command {
 public:
  UnimplementedCommand(uint8_t opcode);
  ~UnimplementedCommand();
  void Run(CPU *cpu);
};

void registerUnimplementedCommands(AbstractCommandFactory *factory);