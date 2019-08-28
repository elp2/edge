#include "MathCommand.hpp"

#include <sstream>

#include "CommandFactory.hpp"
#include "CPU.hpp"
#include "MMU.hpp"
#include "Utils.hpp"

MathCommand::MathCommand(uint8_t opcode) {
    this->opcode = opcode;
}

MathCommand::~MathCommand() {

}

void MathCommand::Inc(CPU *cpu) {
    Destination d;
    switch (opcode)
    {
    case 0x3c:
        d = Register_A;
        break;
    case 0x04:
        d = Register_B;
        break;
    case 0x0c:
        d = Register_C;
        break;
    case 0x14:
        d = Register_D;
        break;
    case 0x1c:
        d = Register_E;
        break;
    case 0x24:
        d = Register_H;
        break;
    case 0x2c:
        d = Register_L;
        break;
    case 0x34:
        d = Address_HL;
        break;
    case 0x03:
        d = Register_BC;
        break;
    case 0x13:
        d = Register_DE;
        break;
    case 0x23:
        d = Register_HL;
        break;
    case 0x33:
        d = Register_SP;
        break;
    
    default:
        cout << "Unknown inc command: 0x" << hex << unsigned(opcode) << endl;
        assert(false);
        break;
    }

    if (d == Address_HL) {
        cycles = 12;
    } else {
        cycles = cpu->Requires16Bits(d) ? 8 : 4;
    }
    stringstream stream;
    stream << "INC " << destinationToString(d);
    description = stream.str();

    if (cpu->Requires16Bits(d)) {
        uint16_t orig = cpu->Get16Bit(d);
        uint16_t inc = orig + 1;
        cpu->Set16Bit(d, inc);
        // TODO: Test overflow.
    } else {
        uint8_t orig = cpu->Get8Bit(d);
        uint8_t inc = orig + 1;
        // TODO: Test overflow2.
        cpu->flags.z = (inc == 0);
        cpu->flags.n = false;
        cpu->flags.h = NIBBLELOW(orig) == 0xf; // TODO test set if carry2.
        // c not affected.
        cpu->Set8Bit(d, inc);
    }
}

void MathCommand::Run(CPU *cpu, MMU *mmu) {
    // Nop.
    (void)mmu;

    switch (opcode)
    {
    case 0x3c:
    case 0x04:
    case 0x0c:
    case 0x14:
    case 0x1c:
    case 0x24:
    case 0x2c:
    case 0x34:
    case 0x03:
    case 0x13:
    case 0x23:
    case 0x33:  
        Inc(cpu);
        break;
    
    default:
        cout << "Unknown math command: 0x" << hex << unsigned(opcode) << endl;
        assert(false);
        break;
    }
}

void registerMathCommands(AbstractCommandFactory *factory) {
    // INC A->(HL).
    factory->RegisterCommand(new MathCommand(0x3c));
    factory->RegisterCommand(new MathCommand(0x04));
    factory->RegisterCommand(new MathCommand(0x0c));
    factory->RegisterCommand(new MathCommand(0x14));
    factory->RegisterCommand(new MathCommand(0x1c));
    factory->RegisterCommand(new MathCommand(0x24));
    factory->RegisterCommand(new MathCommand(0x2c));
    factory->RegisterCommand(new MathCommand(0x34));

    // 16 bit INC.
    factory->RegisterCommand(new MathCommand(0x03));
    factory->RegisterCommand(new MathCommand(0x13));
    factory->RegisterCommand(new MathCommand(0x23));
    factory->RegisterCommand(new MathCommand(0x33));

}