#include "bit_command.h"

#include <cassert>
#include <sstream>
#include <string>

#include "command_factory.h"
#include "cpu.h"
#include "math_command.h"
#include "mmu.h"
#include "utils.h"

string detailedDescription(string base, Destination to, Destination from,
                           uint8_t a, uint8_t n) {
  stringstream stream;
  stream << base << destinationToString(to) << "[" << hex << unsigned(a)
         << "], ";
  stream << destinationToString(from) << "[" << hex << unsigned(n) << "]";

  return stream.str();
}

void andAWithDestination(CPU *cpu, Destination d) {
  uint8_t anded = cpu->Get8Bit(Register_A) & cpu->Get8Bit(d);
  cpu->Set8Bit(Register_A, anded);

  cpu->flags.z = (anded == 0);
  cpu->flags.n = false;
  cpu->flags.h = true;
  cpu->flags.c = false;
}

void BitCommand::CP(CPU *cpu, Destination d) {
  uint8_t a = cpu->Get8Bit(Register_A);
  uint8_t n = cpu->Get8Bit(d);

  aluAdd8(cpu, false, false, a, n);
}

void xorAWithDestination(CPU *cpu, Destination d) {
  uint8_t xored = cpu->Get8Bit(Register_A) ^ cpu->Get8Bit(d);
  cpu->Set8Bit(Register_A, xored);

  cpu->flags.z = (xored == 0);
  cpu->flags.n = false;
  cpu->flags.h = false;
  cpu->flags.c = false;
}

void orAWithDestination(CPU *cpu, Destination d) {
  uint8_t ored = cpu->Get8Bit(Register_A) | cpu->Get8Bit(d);
  cpu->Set8Bit(Register_A, ored);

  cpu->flags.z = (ored == 0);
  cpu->flags.n = false;
  cpu->flags.h = false;
  cpu->flags.c = false;
}

BitCommand::BitCommand(uint8_t opcode, string description, int cycles) {
  this->opcode = opcode;
  this->description = description;
  this->cycles = cycles;
}

BitCommand::~BitCommand() {}

void RR(CPU *cpu, Destination d, bool through_carry, bool cb) {
  uint8_t byte = cpu->Get8Bit(d);
  uint8_t bit7 = 0x0;
  uint8_t bit0 = byte & 0x1;

  if (through_carry) {
    bit7 = (cpu->flags.c) << 7;
    cpu->flags.c = bit0;
  } else {
    cpu->flags.c = bit0;
    bit7 = (bit0) << 7;
  }
  byte >>= 1;
  byte |= bit7;
  cpu->Set8Bit(d, byte);
  cpu->flags.z = cb && (byte == 0);
  cpu->flags.n = cpu->flags.h = false;
}

void RL(CPU *cpu, Destination d, bool through_carry, bool cb) {
  uint8_t byte = cpu->Get8Bit(d);
  uint8_t bit7 = byte & 0x80;
  uint8_t bit0 = 0x0;
  if (through_carry) {
    bit0 = cpu->flags.c;
    cpu->flags.c = bit7;
  } else {
    cpu->flags.c = bit7;
    bit0 = bit7 >> 7;
  }
  byte <<= 1;
  byte |= bit0;
  cpu->Set8Bit(d, byte);
  cpu->flags.z = cb && (byte == 0);
  cpu->flags.n = cpu->flags.h = false;
}

void BitCommand::Run(CPU *cpu) {
  uint8_t row = NIBBLEHIGH(opcode);
  uint8_t col = NIBBLELOW(opcode);
  if (row == 0xb && col < 0x8) {
    return orAWithDestination(cpu, destinationForColumn(col));
  }

  switch (opcode) {
    case 0xa7:
      andAWithDestination(cpu, Register_A);
      return;
    case 0xa0:
      andAWithDestination(cpu, Register_B);
      return;
    case 0xa1:
      andAWithDestination(cpu, Register_C);
      return;
    case 0xa2:
      andAWithDestination(cpu, Register_D);
      return;
    case 0xa3:
      andAWithDestination(cpu, Register_E);
      return;
    case 0xa4:
      andAWithDestination(cpu, Register_H);
      return;
    case 0xa5:
      andAWithDestination(cpu, Register_L);
      return;
    case 0xa6:
      andAWithDestination(cpu, Address_HL);
      return;
    case 0xe6:
      andAWithDestination(cpu, Eat_PC_Byte);
      return;

    // Xors
    case 0xaf:
      xorAWithDestination(cpu, Register_A);
      return;
    case 0xa8:
      xorAWithDestination(cpu, Register_B);
      return;
    case 0xa9:
      xorAWithDestination(cpu, Register_C);
      return;
    case 0xaa:
      xorAWithDestination(cpu, Register_D);
      return;
    case 0xab:
      xorAWithDestination(cpu, Register_E);
      return;
    case 0xac:
      xorAWithDestination(cpu, Register_H);
      return;
    case 0xad:
      xorAWithDestination(cpu, Register_L);
      return;
    case 0xae:
      xorAWithDestination(cpu, Address_HL);
      return;
    case 0xee:
      xorAWithDestination(cpu, Eat_PC_Byte);
      return;

    // ORs.
    case 0xb0:
      return orAWithDestination(cpu, destinationForColumn(0x0));
    case 0xb1:
      return orAWithDestination(cpu, destinationForColumn(0x1));
    case 0xb2:
      return orAWithDestination(cpu, destinationForColumn(0x2));
    case 0xb3:
      return orAWithDestination(cpu, destinationForColumn(0x3));
    case 0xb4:
      return orAWithDestination(cpu, destinationForColumn(0x4));
    case 0xb5:
      return orAWithDestination(cpu, destinationForColumn(0x5));
    case 0xb6:
      return orAWithDestination(cpu, destinationForColumn(0x6));
    case 0xb7:
      return orAWithDestination(cpu, destinationForColumn(0x7));
    case 0xF6:
      return orAWithDestination(cpu, Eat_PC_Byte);

    // CPs.
    case 0xbf:
      CP(cpu, Register_A);
      return;
    case 0xb8:
      CP(cpu, Register_B);
      return;
    case 0xb9:
      CP(cpu, Register_C);
      return;
    case 0xba:
      CP(cpu, Register_D);
      return;
    case 0xbb:
      CP(cpu, Register_E);
      return;
    case 0xbc:
      CP(cpu, Register_H);
      return;
    case 0xbd:
      CP(cpu, Register_L);
      return;
    case 0xbe:
      CP(cpu, Address_HL);
      return;
    case 0xfe:
      CP(cpu, Eat_PC_Byte);
      return;
    case 0x07:  // RLCA
    case 0x17:  // RLA
      RL(cpu, Register_A, opcode == 0x17, false);
      return;
    case 0x0F:  // RRCA
    case 0x1F:  // RRA
      RR(cpu, Register_A, opcode == 0x1F, false);
      return;
    default:
      cout << "Unknown bit command: 0x" << hex << unsigned(opcode);
      assert(false);
      break;
  }
}

void registerBitCommands(AbstractCommandFactory *factory) {
  factory->RegisterCommand(new BitCommand(0xa7, "AND A", 4));
  factory->RegisterCommand(new BitCommand(0xa0, "AND B", 4));
  factory->RegisterCommand(new BitCommand(0xa1, "AND C", 4));
  factory->RegisterCommand(new BitCommand(0xa2, "AND D", 4));
  factory->RegisterCommand(new BitCommand(0xa3, "AND E", 4));
  factory->RegisterCommand(new BitCommand(0xa4, "AND H", 4));
  factory->RegisterCommand(new BitCommand(0xa5, "AND L", 4));
  factory->RegisterCommand(new BitCommand(0xa6, "AND (HL)", 8));
  factory->RegisterCommand(new BitCommand(0xe6, "AND #", 8));

  factory->RegisterCommand(new BitCommand(0xaf, "XOR A", 4));
  factory->RegisterCommand(new BitCommand(0xa8, "XOR B", 4));
  factory->RegisterCommand(new BitCommand(0xa9, "XOR C", 4));
  factory->RegisterCommand(new BitCommand(0xaa, "XOR D", 4));
  factory->RegisterCommand(new BitCommand(0xab, "XOR E", 4));
  factory->RegisterCommand(new BitCommand(0xac, "XOR H", 4));
  factory->RegisterCommand(new BitCommand(0xad, "XOR L", 4));
  factory->RegisterCommand(new BitCommand(0xae, "XOR (HL)", 8));
  factory->RegisterCommand(new BitCommand(0xee, "XOR #", 8));

  factory->RegisterCommand(new BitCommand(0xb8, "CP B", 4));
  factory->RegisterCommand(new BitCommand(0xb9, "CP C", 4));
  factory->RegisterCommand(new BitCommand(0xba, "CP D", 4));
  factory->RegisterCommand(new BitCommand(0xbb, "CP E", 4));
  factory->RegisterCommand(new BitCommand(0xbc, "CP H", 4));
  factory->RegisterCommand(new BitCommand(0xbd, "CP L", 4));
  factory->RegisterCommand(new BitCommand(0xbe, "CP (HL)", 8));
  factory->RegisterCommand(new BitCommand(0xbf, "CP A", 4));
  factory->RegisterCommand(new BitCommand(0xfe, "CP #", 8));

  factory->RegisterCommand(new BitCommand(0x07, "RLCA", 4));
  factory->RegisterCommand(new BitCommand(0x17, "RLA", 4));
  factory->RegisterCommand(new BitCommand(0x0f, "RRCA", 4));
  factory->RegisterCommand(new BitCommand(0x1f, "RRA", 4));

  factory->RegisterCommand(new BitCommand(0xb0, "OR B", 4));
  factory->RegisterCommand(new BitCommand(0xb1, "OR C", 4));
  factory->RegisterCommand(new BitCommand(0xb2, "OR D", 4));
  factory->RegisterCommand(new BitCommand(0xb3, "OR E", 4));
  factory->RegisterCommand(new BitCommand(0xb4, "OR H", 4));
  factory->RegisterCommand(new BitCommand(0xb5, "OR L", 4));
  factory->RegisterCommand(new BitCommand(0xb6, "OR (HL)", 8));
  factory->RegisterCommand(new BitCommand(0xb7, "OR A", 4));
  factory->RegisterCommand(new BitCommand(0xF6, "OR #", 8));
}