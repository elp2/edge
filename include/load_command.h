#pragma once

#include "command.h"
#include "destination.h"

class AbstractCommandFactory;

class LoadCommand : public Command {
 public:
  LoadCommand(uint8_t opcode, string description, Destination to,
              Destination from, int cycles);
  ~LoadCommand();

  void Run(CPU *cpu);

 private:
  Destination from;
  Destination to;
};

void registerLoadCommands(AbstractCommandFactory *factory);