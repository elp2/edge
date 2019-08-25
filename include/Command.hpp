#pragma once

#include <cstdint>
#include <iostream>
#include <string>

using namespace std;

class CPU;
class MMU;

class Command {
 public:
    virtual ~Command() = default;
 
    virtual void Run(CPU *cpu, MMU *mmu) {(void)cpu; (void)mmu;};

    int cycles;
    uint8_t opcode;
    string description;
};
