#pragma once

#include "Opcode.hpp"

class NopOpcode : public Opcode {

public:
    NopOpcode(uint8_t opcode);
    ~NopOpcode();
    void Run(CPU *cpu, MMU *mmu);
};