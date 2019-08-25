#pragma once

#include "CPU.hpp"

using namespace std;

// Holds Opcodes logic so we don't have to put them all in the CPU.
class Opcodes {

public:
    Opcodes(CPU *cpu);
    ~Opcodes();

    void RunOpcode(uint8_t opcode);

private:
    CPU *cpu;

};