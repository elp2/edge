#pragma once

#include "Command.hpp"
#include "Destination.hpp"

class AbstractCommandFactory;

class MathCommand : public Command {

 public:
    MathCommand(uint8_t opcode);
    ~MathCommand();
    void Run(CPU *cpu, MMU *mmu);
 private:
    void Inc(CPU *cpu);
    void Dec(CPU *cpu);
    void Delta8(CPU *cpu, Destination n, bool add, bool carry);
    void AddSP(CPU *cpu);
};

void registerMathCommands(AbstractCommandFactory *factory);