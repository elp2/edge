#pragma once

#include "command.h"
#include "destination.h"

class AbstractCommandFactory;

class MathCommand : public Command {
 public:
  MathCommand(uint8_t opcode);
  ~MathCommand();
  void Run(CPU *cpu);

 private:
  void Inc(CPU *cpu);
  void Dec(CPU *cpu);
  void Delta8(CPU *cpu, Destination n, bool add, bool carry);
  void AddHL(CPU *cpu, Destination n);
};

uint16_t AddSP(CPU *cpu);

// Adds or subtracts, and sets Z, N, H, C flags on CPU appropriately.
uint8_t aluAdd8(CPU *cpu, bool add, bool carry, uint8_t orig, uint8_t delta);

void registerMathCommands(AbstractCommandFactory *factory);