#include "call_command.h"

#include <cassert>

#include "command_factory.h"
#include "cpu.h"
#include "MMU.hpp"

class CallCommand : public Command {
 public:
    CallCommand(uint8_t opcode, string description, int cycles) {
        this->opcode = opcode;
        this->description = description;
        this->no_jump_cycles_ = cycles;
    }

    ~CallCommand() {}

    void DoCall(CPU *cpu, uint16_t callAddress, bool condition) {
        if (!condition) {
            cycles = no_jump_cycles_;
            return;
        }
        if (no_jump_cycles_ == 12) {
            // Conditional calls take 2x as many cycles if they jump.
            cycles = 24;
        } else {
            cycles = no_jump_cycles_;
        }
        //  Where we should return is where we are currently pointing after the call.
        uint16_t returnAddress = cpu->Get16Bit(Register_PC);
        cpu->Push16Bit(returnAddress);
        cpu->JumpAddress(callAddress);
    }

    void Run(CPU *cpu) {
        switch (opcode)
        {
        case 0xCD:
            return DoCall(cpu, cpu->Get16Bit(Eat_PC_Word), true);
        case 0xC4:
            return DoCall(cpu, cpu->Get16Bit(Eat_PC_Word), !cpu->flags.z);
        case 0xCC:
            return DoCall(cpu, cpu->Get16Bit(Eat_PC_Word), cpu->flags.z);
        case 0xD4:
            return DoCall(cpu, cpu->Get16Bit(Eat_PC_Word), !cpu->flags.c);
        case 0xDC:
            return DoCall(cpu, cpu->Get16Bit(Eat_PC_Word), cpu->flags.c);
        case 0xC7:
            return DoCall(cpu, 0x00, true);
        case 0xCF:
            return DoCall(cpu, 0x08, true);
        case 0xD7:
            return DoCall(cpu, 0x10, true);
        case 0xDF:
            return DoCall(cpu, 0x18, true);
        case 0xE7:
            return DoCall(cpu, 0x20, true);
        case 0xEF:
            return DoCall(cpu, 0x28, true);
        case 0xF7:
            return DoCall(cpu, 0x30, true);
        case 0xFF:
            return DoCall(cpu, 0x38, true);
        default:
            cout << "No Call opcode for 0x" << hex << unsigned(opcode) << endl;
            assert(false);
            break;
        }
    }

 private:
    int no_jump_cycles_ = 0;
};

void registerCallCommands(AbstractCommandFactory *factory) {
    // CALL.
    factory->RegisterCommand(new CallCommand(0xCD, "CALL nn", 24));
    factory->RegisterCommand(new CallCommand(0xC4, "CALL NZ,nn", 12));
    factory->RegisterCommand(new CallCommand(0xCC, "CALL Z,nn", 12));
    factory->RegisterCommand(new CallCommand(0xD4, "CALL NC,nn", 12));
    factory->RegisterCommand(new CallCommand(0xDC, "CALL C,nn", 12));

    // RST.
    factory->RegisterCommand(new CallCommand(0xC7, "RST 00H", 16));
    factory->RegisterCommand(new CallCommand(0xCF, "RST 08H", 16));
    factory->RegisterCommand(new CallCommand(0xD7, "RST 10H", 16));
    factory->RegisterCommand(new CallCommand(0xDF, "RST 18H", 16));
    factory->RegisterCommand(new CallCommand(0xE7, "RST 20H", 16));
    factory->RegisterCommand(new CallCommand(0xEF, "RST 28H", 16));
    factory->RegisterCommand(new CallCommand(0xF7, "RST 30H", 16));
    factory->RegisterCommand(new CallCommand(0xFF, "RST 38H", 16));
}