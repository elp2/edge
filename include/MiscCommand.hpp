#pragma once

#include "Opcode.hpp"

class MiscCommand : public Opcode {

public:
    MiscCommand(uint8_t opcode, string description, int cycles);
    ~MiscCommand();

    void Run(CPU *cpu, MMU *mmu);
};

void registerMiscCommands(CPU *cpu);