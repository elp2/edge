#include "MathCommand.hpp"

#include <cassert>
#include <sstream>

#include "CommandFactory.hpp"
#include "CPU.hpp"
#include "MMU.hpp"
#include "Utils.hpp"

#define ADDAd8 0xc6
#define SUBAd8 0xd6
#define ADCAd8 0xce
#define SBCAd8 0xde

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

void MathCommand::Dec(CPU *cpu) {
    Destination d;
    switch (opcode)
    {
    case 0x3d:
        d = Register_A;
        break;
    case 0x05:
        d = Register_B;
        break;
    case 0x0d:
        d = Register_C;
        break;
    case 0x15:
        d = Register_D;
        break;
    case 0x1d:
        d = Register_E;
        break;
    case 0x25:
        d = Register_H;
        break;
    case 0x2d:
        d = Register_L;
        break;
    case 0x35:
        d = Address_HL;
        break;
    case 0x0b:
        d = Register_BC;
        break;
    case 0x1b:
        d = Register_DE;
        break;
    case 0x2b:
        d = Register_HL;
        break;
    case 0x3b:
        d = Register_SP;
        break;
    
    default:
        cout << "Unknown dec command: 0x" << hex << unsigned(opcode) << endl;
        assert(false);
        break;
    }

    if (d == Address_HL) {
        cycles = 12;
    } else {
        cycles = cpu->Requires16Bits(d) ? 8 : 4;
    }
    stringstream stream;
    stream << "DEC " << destinationToString(d);
    description = stream.str();

    if (cpu->Requires16Bits(d)) {
        uint16_t orig = cpu->Get16Bit(d);
        uint16_t dec = orig - 1;
        cpu->Set16Bit(d, dec);
        // TODO: Test overflow.
    } else {
        uint8_t orig = cpu->Get8Bit(d);
        uint8_t dec = orig - 1;
        // TODO: Test overflow2.
        cpu->flags.z = (dec == 0);
        cpu->flags.n = false;
        cpu->flags.h = NIBBLELOW(orig) == 0x00; // TODO test set if carry2.
        // c not affected.
        cpu->Set8Bit(d, dec);
    }
}

void MathCommand::Delta8(CPU *cpu, Destination n, bool add, bool carry) {
    uint8_t orig = cpu->Get8Bit(Register_A);
    uint8_t delta = cpu->Get8Bit(n) + (carry ? cpu->flags.c : 0);

    uint8_t after;
    if (add) {
        after = orig + delta;
    } else {
        after = orig - delta;
    }

    stringstream stream;
    if (add) {
        if (carry) {
            stream << "ADC";
        } else {
            stream << "ADD";
        }
    } else {
        if (carry) {
            stream << "SBC";
        } else {
            stream << "SUB";
        }
    }
    stream << " A," << destinationToString(n);
    description = stream.str();

    cycles = (n == Eat_PC_Byte || n == Address_HL) ? 8 : 4;

    cpu->flags.z = (after == 0x00);
    cpu->flags.n = !add;
    // TODO - half carry. Test.
    // if a + b < max, then this would be LOWER(orig) > LOWER(new), e.g. we've pushed stuff out.
    // cpu->flags.h = add ? LOWER(orig);
    cpu->flags.c = add ? (after < orig) : (orig > after); // TODO test. Maybe whether switched sign?
    cpu->Set8Bit(Register_A, after);
}

void MathCommand::AddHL(CPU *cpu, Destination n) {
    uint16_t orig = cpu->Get16Bit(Register_HL);
    uint16_t after = orig + cpu->Get16Bit(n);

    stringstream stream;
    stream << "ADD HL," << destinationToString(n);
    description = stream.str();

    cycles = 8;

    // Not affected: cpu->flags.z;
    cpu->flags.n = false;
    // TODO: Test.
    assert(false);
    cpu->flags.h = orig < 0x0fff && after > 0xff;
    cpu->flags.c = orig < after;

    cpu->Set16Bit(Register_HL, after);
}

void MathCommand::AddSP(CPU *cpu) {
    uint8_t unsignedByte = cpu->Get8Bit(Eat_PC_Byte);
    int8_t signedByte = unsignedByte;
    uint16_t sp = cpu->Get16Bit(Register_SP);
    uint16_t spAfter = sp + signedByte;
    cpu->Set16Bit(Register_SP, spAfter);

    // TODO Test.
    cpu->flags.z = false;
    cpu->flags.n = false;
    cpu->flags.h = spAfter > 0xff && sp <= 0xff;
    cpu->flags.c = spAfter < sp && signedByte > 0;

    cycles = 16;
    description = "ADD SP, #";
}

void MathCommand::Run(CPU *cpu) {
    uint8_t row = NIBBLEHIGH(opcode);
    uint8_t col = NIBBLELOW(opcode);
    
    if ((col == 0x4 && row <= 0x3) || (col == 0xc && row <= 0x3)) {
        Inc(cpu);
        return;
    }

    if ((col == 0x5 && row <= 0x3) || (col == 0xd && row <= 0x3)) {
        Dec(cpu);
        return;
    }

    if (col == 0x9 && (row >= 0 && row < 4)) {
        
    }

    if (row == 0x8 || opcode == ADDAd8 || opcode == ADCAd8) {
        // ADD, ADC.
        bool carry = col > 7;
        bool add = true;
        Destination d = (opcode == ADDAd8 || opcode == ADCAd8) ? Eat_PC_Byte : destinationForColumn(col);
        Delta8(cpu, d, add, carry);
        return;
    }

    if (row == 0x9 || opcode == SUBAd8 || opcode == SBCAd8) {
        // SUB, SUBC.
        bool carry = col > 7;
        bool add = false;
        Destination d = (opcode == SUBAd8 || opcode == SBCAd8) ? Eat_PC_Byte : destinationForColumn(col);
        Delta8(cpu, d, add, carry);
        return;
    }

    switch (opcode)
    {
        case 0xe8:
            return AddSP(cpu);
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

    case 0x3d:
    case 0x05:
    case 0x0d:
    case 0x15:
    case 0x1d:
    case 0x25:
    case 0x2d:
    case 0x35:
    case 0x0b:
    case 0x1b:
    case 0x2b:
    case 0x3b:  
        Dec(cpu);
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

    // DEC A->(HL).
    factory->RegisterCommand(new MathCommand(0x3d));
    factory->RegisterCommand(new MathCommand(0x05));
    factory->RegisterCommand(new MathCommand(0x0d));
    factory->RegisterCommand(new MathCommand(0x15));
    factory->RegisterCommand(new MathCommand(0x1d));
    factory->RegisterCommand(new MathCommand(0x25));
    factory->RegisterCommand(new MathCommand(0x2d));
    factory->RegisterCommand(new MathCommand(0x35));

    // 16 bit DEC.
    factory->RegisterCommand(new MathCommand(0x0b));
    factory->RegisterCommand(new MathCommand(0x1b));
    factory->RegisterCommand(new MathCommand(0x2b));
    factory->RegisterCommand(new MathCommand(0x3b));

    // ADD, ADC.
    for (uint8_t i = 0x80; i < 0x90; i++) {
        factory->RegisterCommand(new MathCommand(i));
    }
    factory->RegisterCommand(new MathCommand(ADDAd8));
    factory->RegisterCommand(new MathCommand(ADCAd8));

    // SUB, SBC.
    for (uint8_t i = 0x90; i < 0xa0; i++) {
        factory->RegisterCommand(new MathCommand(i));
    }
    factory->RegisterCommand(new MathCommand(SUBAd8));
    factory->RegisterCommand(new MathCommand(SBCAd8));

    factory->RegisterCommand(new MathCommand(0x09));
    factory->RegisterCommand(new MathCommand(0x19));
    factory->RegisterCommand(new MathCommand(0x29));
    factory->RegisterCommand(new MathCommand(0x39));

    // ADD SP, n;
    factory->RegisterCommand(new MathCommand(0xE8));
}