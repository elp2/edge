#pragma once

#include "Command.hpp"
#include "Destination.hpp"

class AbstractCommandFactory;
class CPU;

class BitCommand : public Command {
 private:
    void CP(CPU *cpu, Destination d);
 public:
    BitCommand(uint8_t opcode, string description, int cycles);
    ~BitCommand();
    void Run(CPU *cpu);
};

void RL(CPU *cpu, Destination d, bool through_carry, bool cb);
void RR(CPU *cpu, Destination d, bool through_carry, bool cb);

void registerBitCommands(AbstractCommandFactory *factory);