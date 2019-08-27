#include "BitCommand.hpp"

#include "CommandFactory.hpp"
#include "CPU.hpp"
#include "MMU.hpp"

void andAWithDestination(CPU *cpu, Destination d) {
    uint8_t anded = cpu->Get8Bit(Register_A) & cpu->Get8Bit(d);
    cpu->Set8Bit(Register_A, anded);

    cpu->flags.z = (anded == 0);
    cpu->flags.n = false;
    cpu->flags.h = true;
    cpu->flags.c = false;
}

void xorAWithDestination(CPU *cpu, Destination d) {
    uint8_t xored = cpu->Get8Bit(Register_A) ^ cpu->Get8Bit(d);
    cpu->Set8Bit(Register_A, xored);

    cpu->flags.z = (xored == 0);
    cpu->flags.n = false;
    cpu->flags.h = false;
    cpu->flags.c = false;
}

BitCommand::BitCommand(uint8_t opcode, string description, int cycles) {
    this->opcode = opcode;
    this->description = description;
    this->cycles = cycles;
}

BitCommand::~BitCommand() {

}

void BitCommand::Run(CPU *cpu, MMU *mmu) {
    (void)mmu;

    switch (opcode)
    {
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
}