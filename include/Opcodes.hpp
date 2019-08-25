#pragma once

#include "CPU.hpp"

using namespace std;

// Holds Commands logic so we don't have to put them all in the CPU.
class Commands {

public:
    Commands(CPU *cpu);
    ~Commands();

    void RunCommand(uint8_t opcode);

private:
    CPU *cpu;

};