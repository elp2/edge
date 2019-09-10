#include "MiscCommand.hpp"

#include <cassert>

#include "CommandFactory.hpp"
#include "CPU.hpp"
#include "MMU.hpp"
#include "Utils.hpp"

MiscCommand::MiscCommand(uint8_t opcode, string description, int cycles) {
    this->opcode = opcode;
    this->description = description;
    this->cycles = cycles;
}

MiscCommand::~MiscCommand() {

}

void DAA(CPU* cpu) {
	uint8_t a = cpu->Get8Bit(Register_A);
	bool carry = cpu->flags.c;
	int low_dec = NIBBLELOW(a);
	int high_dec = NIBBLEHIGH(a);

	if (cpu->flags.n) {
		// Subtraction.
	}
	else {
		if (cpu->flags.h) {
			low_dec += 16;
			// Higher decimal got a free increase since low nibble add spilled over.
			high_dec--;
		}
		if (low_dec >= 10) {
			low_dec %= 10;
			high_dec++;
		}
		if (carry) {
			high_dec += 16;
		}
		if (high_dec >= 10) {
			high_dec %= 10;
			carry = true;
		}
		assert(high_dec < 10 && high_dec >= 0);
		assert(low_dec < 10 && low_dec >= 0);
		uint8_t h = high_dec;
		uint8_t l = low_dec;
		a = (h << 4) | l;
	}

	cpu->flags.z = a == 0;
	// cpu->flags.n not affected.
	cpu->flags.h = false; // Half carry only makes sense for Decimal Adjustment.
	cpu->flags.c = carry;
	cpu->Set8Bit(Register_A, a);
}

void MiscCommand::Run(CPU *cpu) {
    uint8_t nextPCByte;

    switch (opcode)
    {
    case 0x27:
		DAA(cpu);
        break;
    case 0x2F:
        cpu->Set8Bit(Register_A, ~cpu->Get8Bit(Register_A));
        // Flags Z and C not affected.
        cpu->flags.n = cpu->flags.h = true;
        break;
    case 0x3F:
        // z unchanged.
        cpu->flags.n = false;
        cpu->flags.h = false;
        cpu->flags.c = !cpu->flags.c;
        break;
    case 0x37:
        // z unchanged.
        cpu->flags.n = false;
        cpu->flags.h = false;
        cpu->flags.c = true;
        break;
    case 0x76:
        cpu->HaltNextLoop();
        break;
    case 0x10:
        cpu->StopNextLoop();
        nextPCByte = cpu->Get8Bit(Eat_PC_Byte);
        assert(nextPCByte == 0x00);
        break;
    case 0xF3:
        cpu->DisableInterruptsNextLoop();
        break;
    case 0xFB:
        cpu->EnableInterruptsNextLoop();
        break;
    
    default:
        cout << "Attempt to use MiscComman for " << hex << unsigned(opcode) << endl;
        assert(false);
        break;
    }
}

void registerMiscCommands(AbstractCommandFactory *factory) {
    factory->RegisterCommand(new MiscCommand(0x27, "DAA", 4));
    factory->RegisterCommand(new MiscCommand(0x2F, "CPL", 4));
    factory->RegisterCommand(new MiscCommand(0x3F, "CCF", 4));
    factory->RegisterCommand(new MiscCommand(0x37, "SCF", 4));
    factory->RegisterCommand(new MiscCommand(0x76, "HALT", 4));
    factory->RegisterCommand(new MiscCommand(0x10, "STOP", 4));
    factory->RegisterCommand(new MiscCommand(0xF3, "DI", 4));
    factory->RegisterCommand(new MiscCommand(0xFB, "EI", 4));
}