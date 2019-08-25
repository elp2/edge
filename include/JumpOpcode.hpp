#pragma once

#include "Opcode.hpp"

class JumpOpcode : public Opcode {

public:
    JumpOpcode(uint8_t opcode, string description, int cycles);
    ~JumpOpcode();

    void Run(CPU *cpu, MMU *mmu);    
};