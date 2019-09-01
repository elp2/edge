#pragma once

#include "Command.hpp"
#include "Destination.hpp"

class AbstractCommandFactory;
class CPU;

class BitCommand : public Command {

 public:
    BitCommand(uint8_t opcode, string description, int cycles);
    ~BitCommand();
    void Run(CPU *cpu);
};

void RL(CPU *cpu, Destination d, bool carry);
void RR(CPU *cpu, Destination d, bool carry);

void registerBitCommands(AbstractCommandFactory *factory);