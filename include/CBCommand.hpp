#pragma once

#include "Command.hpp"

class CBCommand : public Command {

public:
    CBCommand(uint8_t opcode);
    ~CBCommand();
    void Run(CPU *cpu, MMU *mmu);
 private:
    void TestBit(uint8_t row, uint8_t column, CPU *cpu);
};