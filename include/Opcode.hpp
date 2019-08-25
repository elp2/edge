#pragma once

#include <cstdint>
#include <iostream>

#include "CPU.hpp"
#include "MMU.hpp"

class Opcode {
 public:
    virtual ~Opcode() = default;
 
    virtual void Run(CPU *cpu, MMU *mmu) {(void)cpu; (void)mmu;};

    int cycles;
    uint8_t opcode;
    string description;
};
