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

  virtual void Run(CPU *cpu) { (void)cpu; };

  int cycles = 0;
  uint8_t opcode = 0;
  string description;
};
