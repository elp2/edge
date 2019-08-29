#pragma once

#include "Command.hpp"

class AbstractCommandFactory;

class BitCommand : public Command {

 public:
    BitCommand(uint8_t opcode, string description, int cycles);
    ~BitCommand();
    void Run(CPU *cpu, MMU *mmu);
 private:
    void RL(CPU *cpu);
    void RR(CPU *cpu);
};

void registerBitCommands(AbstractCommandFactory *factory);