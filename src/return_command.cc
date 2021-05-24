#include "return_command.h"

#include <cassert>

#include "command_factory.h"
#include "cpu.h"
#include "mmu.h"

ReturnCommand::ReturnCommand(uint8_t opcode, string description, int cycles) {
  this->opcode = opcode;
  this->description = description;
  this->base_cycles_ = cycles;
}

ReturnCommand::~ReturnCommand() {}

void ReturnCommand::Run(CPU *cpu) {
  cycles = base_cycles_;
  switch (this->opcode) {
    case 0xc9:
      cpu->JumpAddress(cpu->Pop16Bit());
      break;
    case 0xc0:
      if (!cpu->flags.z) {
        cycles = 20;
        cpu->JumpAddress(cpu->Pop16Bit());
      }
      break;
    case 0xc8:
      if (cpu->flags.z) {
        cycles = 20;
        cpu->JumpAddress(cpu->Pop16Bit());
      }
      break;
    case 0xd0:
      if (!cpu->flags.c) {
        cycles = 20;
        cpu->JumpAddress(cpu->Pop16Bit());
      }
      break;
    case 0xd8:
      if (cpu->flags.c) {
        cycles = 20;
        cpu->JumpAddress(cpu->Pop16Bit());
      }
      break;
    case 0xd9:
      cpu->JumpAddress(cpu->Pop16Bit());
      cpu->EnableInterrupts();
      break;

    default:
      cout << "un-implemented Restart: " << hex << unsigned(opcode) << endl;
      assert(false);
      break;
  }
}

void registerReturnCommands(AbstractCommandFactory *factory) {
  factory->RegisterCommand(new ReturnCommand(0xc9, "RET", 16));

  factory->RegisterCommand(new ReturnCommand(0xc0, "RET NZ", 8));
  factory->RegisterCommand(new ReturnCommand(0xc8, "RET Z", 8));
  factory->RegisterCommand(new ReturnCommand(0xd0, "RET CZ", 8));
  factory->RegisterCommand(new ReturnCommand(0xd8, "RET C", 8));

  factory->RegisterCommand(new ReturnCommand(0xd9, "RETI", 16));
}