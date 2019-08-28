#pragma once

#include "Command.hpp"

class AbstractCommandFactory;

class MathCommand : public Command {

public:
    MathCommand(uint8_t opcode);
    ~MathCommand();
    void Run(CPU *cpu, MMU *mmu);
 private:
    void Inc(CPU *cpu);
};

void registerMathCommands(AbstractCommandFactory *factory);