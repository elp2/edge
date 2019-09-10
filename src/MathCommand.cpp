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

	uint8_t row = NIBBLEHIGH(opcode);
	uint8_t col = NIBBLELOW(opcode);

	if (opcode == 0xE8) {
		cycles = 16;
		description = "ADD SP, #";
		return;
	}

	if ((col == 0x3 || col == 0xb) && row < 0x4) {
		switch (opcode)
		{
			case 0x03:
				description = "INC BC";
				break;
			case 0x23:
				description = "INC DE";
				break;
			case 0x33:
				description = "INC HL";
				break;
			case 0x43:
				description = "INC SP";
				break;
			case 0x0B:
				description = "DEC BC";
				break;
			case 0x1B:
				description = "DEC DE";
				break;
			case 0x2B:
				description = "DEC HL";
				break;
			case 0x3B:
				description = "DEC SP";
				break;
			default:
				break;
		}
		cycles = 8;
		return;
	}

	if ((col == 0x4 && row <= 0x3) || (col == 0xc && row <= 0x3)) {
		stringstream stream;
		stream << "DEC " << destinationToString(destinationForColumn(col));
		description = stream.str();
		return;
	}

	if ((col == 0x5 && row <= 0x3) || (col == 0xd && row <= 0x3)) {
		stringstream stream;
		stream << "INC " << destinationToString(destinationForColumn(col));
		description = stream.str();
		return;
	}

	if (col == 0x9 && (row < 4)) {
		switch (opcode)
		{
		case 0x09:
			description = "ADD HL, BC";
			break;
		case 0x19:
			description = "ADD HL, DE";
			break;
		case 0x29:
			description = "ADD HL, HL";
			break;
		case 0x39:
			description = "ADD HL, SP";
			break;
		default:
			break;
		}
		return;
	}

	bool carry;
	bool add;
	Destination d;
	if (row == 0x8 || opcode == ADDAd8 || opcode == ADCAd8) {
		// ADD, ADC.
		carry = col > 7;
		add = true;
		d = (opcode == ADDAd8 || opcode == ADCAd8) ? Eat_PC_Byte : destinationForColumn(col);
	}

	if (row == 0x9 || opcode == SUBAd8 || opcode == SBCAd8) {
		// SUB, SUBC.
		carry = col > 7;
		add = false;
		d = (opcode == SUBAd8 || opcode == SBCAd8) ? Eat_PC_Byte : destinationForColumn(col);
	}


	stringstream stream;
	if (add) {
		if (carry) {
			stream << "ADC";
		}
		else {
			stream << "ADD";
		}
	}
	else {
		if (carry) {
			stream << "SBC";
		}
		else {
			stream << "SUB";
		}
	}
	stream << " A," << destinationToString(d);
	description = stream.str();
}

MathCommand::~MathCommand() {

}

uint8_t aluAdd(uint8_t a, uint8_t b, bool carry_in, bool *carry_out) {
    assert(a < 0x10);
    assert(b < 0x10);

    uint8_t ret = a + b + (carry_in ? 1 : 0);

    *carry_out = (a + b + (carry_in ? 1 : 0) > 0xf) ? true : false;
    ret = ret & 0xF;
    return ret;
}

uint8_t aluSub(uint8_t a, uint8_t b, bool carry_in, bool *carry_out) {
    assert(a < 0x10);
    assert(b < 0x10);
    uint8_t delta = (b + (carry_in ? 1 : 0));
    uint8_t ret = a - delta;

    *carry_out = (a < delta) ? true : false;
    ret = ret & 0xF;

    return ret;
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

uint8_t aluAdd8(CPU *cpu, bool add, bool carry, uint8_t orig, uint8_t delta) {
    uint8_t result = 0;
    uint8_t orig_nib;
    uint8_t delta_nib;

    bool math_carry = (carry ? cpu->flags.c : false);
    if (add) {
        orig_nib = NIBBLELOW(orig);
        delta_nib = NIBBLELOW(delta);
        result |= aluAdd(orig_nib, delta_nib, math_carry, &math_carry);
        cpu->flags.h = math_carry;

        orig_nib = NIBBLEHIGH(orig);
        delta_nib = NIBBLEHIGH(delta);
        result |= (aluAdd(orig_nib, delta_nib, math_carry, &math_carry) << 4);
        cpu->flags.c = math_carry;
    } else {
        orig_nib = NIBBLELOW(orig);
        delta_nib = NIBBLELOW(delta);
        result |= aluSub(orig_nib, delta_nib, math_carry, &math_carry);
        cpu->flags.h = math_carry;

        orig_nib = NIBBLEHIGH(orig);
        delta_nib = NIBBLEHIGH(delta);
        result |= (aluSub(orig_nib, delta_nib, math_carry, &math_carry) << 4);
        cpu->flags.c = math_carry;
    }
    return result;
}

void MathCommand::Delta8(CPU *cpu, Destination n, bool add, bool carry) {


    uint8_t orig = cpu->Get8Bit(Register_A);
    uint8_t delta = cpu->Get8Bit(n);

    uint8_t result = aluAdd8(cpu, add, carry, orig, delta);
    cpu->Set8Bit(Register_A, result);

    cycles = (n == Eat_PC_Byte || n == Address_HL) ? 8 : 4;

    cpu->flags.z = (result == 0x00);
    cpu->flags.n = !add;
}

void MathCommand::AddHL(CPU *cpu, Destination n) {
    uint16_t hl = cpu->Get16Bit(Register_HL);
    uint16_t other = cpu->Get16Bit(n);

    uint16_t result = 0;
    uint8_t hlnib = NIBBLELOW(LOWER8(hl));
    uint8_t otnib = NIBBLELOW(LOWER8(other));

    bool carry = false;
    result |= aluAdd(hlnib, otnib, false, &carry);

    hlnib = NIBBLEHIGH(LOWER8(hl));
    otnib = NIBBLEHIGH(LOWER8(other));
    result |= (aluAdd(hlnib, otnib, carry, &carry) << 4);

    hlnib = NIBBLELOW(HIGHER8(hl));
    otnib = NIBBLELOW(HIGHER8(other));
    result |= (aluAdd(hlnib, otnib, carry, &carry) << 8);
    cpu->flags.h = carry;

    hlnib = NIBBLEHIGH(HIGHER8(hl));
    otnib = NIBBLEHIGH(HIGHER8(other));
    result |= (aluAdd(hlnib, otnib, carry, &carry) << 12);
    cpu->flags.c = carry;

    stringstream stream;
    stream << "ADD HL," << destinationToString(n);
    description = stream.str();

    cycles = 8;

    cpu->flags.n = false;

    cpu->Set16Bit(Register_HL, result);
}

uint16_t AddSP(CPU *cpu) {
    uint8_t unsigned_byte = cpu->Get8Bit(Eat_PC_Byte);
    int8_t signed_byte = unsigned_byte;
    uint16_t sp = cpu->Get16Bit(Register_SP);
    

    uint8_t delta;
    bool add;

    if (signed_byte > 0) {
        add = true;
        delta = unsigned_byte;
    } else {
        add = false;
        delta = -1 * signed_byte;
    }
    uint8_t lower8 = aluAdd8(cpu, add, false, LOWER8(sp), delta);
    uint16_t sp_after = (HIGHER8(sp) << 8) | lower8;
    if (cpu->flags.c) {
        sp_after += 0x0100; // TODO negative?
    }

    if (sp_after < 0x1000) {
        cout << "SP too low after adding: " << hex << unsigned(sp_after) << endl;
        assert(false);
    }
    return sp_after;
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
        case 0xE8:
            cpu->Set16Bit(Register_SP, AddSP(cpu));
            return;
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

    case 0x09:
        AddHL(cpu, Register_BC);
        break;
    case 0x19:
        AddHL(cpu, Register_DE);
        break;
    case 0x29:
        AddHL(cpu, Register_HL);
        break;
    case 0x39:
        AddHL(cpu, Register_SP);
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

    // 16 bit adds.
    factory->RegisterCommand(new MathCommand(0x09));
    factory->RegisterCommand(new MathCommand(0x19));
    factory->RegisterCommand(new MathCommand(0x29));
    factory->RegisterCommand(new MathCommand(0x39));

    // ADD SP, n;
    factory->RegisterCommand(new MathCommand(0xE8));
}