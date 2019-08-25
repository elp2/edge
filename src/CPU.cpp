#include "CPU.hpp"

#include <ios>
#include <iostream>

#include "Opcode.hpp"
#include "JumpOpcode.hpp"
#include "MiscCommand.hpp"
#include "NopOpcode.hpp"

CPU::CPU(MMU mmu) {
    this->mmu = mmu;

    RegisterOpcodes();

    Reset();
}

CPU::~CPU() {

}

void CPU::RegisterOpcode(Opcode *command) {
    Opcode *existingCommand = opcodes[command->opcode];

    cout << unsigned(command->opcode) << endl;
    if (existingCommand != NULL) {
        cout << "Already have an opcode at 0x" << hex << unsigned(existingCommand->opcode) << ": " << existingCommand->description << " : New = " << command->description << endl;
        assert(false);
    }
    opcodes[command->opcode] = command;
    cout << "Registered 0x" << hex << unsigned(command->opcode) << " : " << command->description << endl;
}

void CPU::RegisterOpcodes() {
    
    RegisterOpcode(new NopOpcode(0x00));

    RegisterOpcode(new JumpOpcode(0xC3, "JP nn", 12));

    registerMiscCommands(this);
}

Opcode *CPU::CommandForOpcode(uint8_t opcode) {
    if (opcodes[opcode] == NULL) {
        cout << "No opcode for " << unsigned(opcode) << endl;
        assert(false);
    }
    return opcodes[opcode];
}

void CPU::Step() {
    // Take actions requested in previous cycle.
    if (disableInterruptsRequested) {
        interruptsEnabled = false;
        disableInterruptsRequested = false;
    } else if (enableInterruptsRequested) {
        interruptsEnabled = true;
        enableInterruptsRequested = false;
    } else if (haltRequested) {
        cout << "TODO: Halt!";
        assert(false); // TODO!
    } else if (stopRequested) {
        cout << "TODO: Stop!";
        assert(false); // TODO!
    }

    uint8_t opcode = ReadOpcodeAtPC();
    AdvancePC();

    Opcode *command = CommandForOpcode(opcode);
    cout << "Opcode: " << command->description << endl;
    command->Run(this, &mmu);
    // Time instruction takes
    // Directly updates - MMU, CPU


// read instruction code at pc
// look up instruction command based on that (might be 2 step for cb's)
// advance the number of steps that the instruction demanded

// ??? interrupts.

}



bool CPU::Requires16Bits(Destination d) {
    switch (d)
    {
    case Register_A:
    case Register_B:
    case Register_C:
    case Register_D:
    case Register_E:
    case Register_F:
    case Register_H:
    case Register_L:
        return false;
    
    default:
        return true;
    }
}
uint8_t CPU::Read8Bit(Destination d) {
    uint8_t pcByte;
    switch (d)
    {
    case Register_A:
        return a;
    case Register_B:
        return b;
    case Register_C:
        return c;
    case Register_D:
        return d;
    case Register_E:
        return e;
    case Register_F:
        return f;
    case Register_H:
        return h;
    case Register_L:
        return l;
    case Eat_PC_Byte: {
        pcByte = mmu.ByteAt(pc);
        AdvancePC();
        return pcByte;
    }
    default:
        assert(false);
    }
}

uint16_t CPU::Read16Bit(Destination d) {
    uint16_t word;
    switch (d)
    {
    case Register_A:
    case Register_B:
    case Register_C:
    case Register_D:
    case Register_E:
    case Register_F:
    case Register_H:
    case Register_L:
    case Eat_PC_Byte:
        assert(false);
    case Register_BC:
    case Register_DE:
    case Register_HL:
        // TODO: 16 bit registers.
        return 0xDEAD;
        break;
    case Register_SP:
        return sp;
    case Register_PC:
        return pc;
    case Eat_PC_Word:
        word = mmu.WordAt(pc);
        AdvancePC();
        AdvancePC();
        return word;
    default:
        assert(false);
    }
    return 0xDEAD;
}

void CPU::Set8Bit(Destination d, uint8_t value) {
    switch (d)
    {
    case Register_A:
        a = value;
        break;
    case Register_B:
        b = value;
        break;
    case Register_C:
        c = value;
        break;
    case Register_D:
        this->d = value;
        break;
    case Register_E:
        e = value;
        break;
    case Register_F:
        f = value;
        break;
    case Register_H:
        h = value;
        break;
    case Register_L:
        l = value;
        break;
    case Eat_PC_Byte:
        assert(false);
        //uint8_t pcByte = mmu.ByteAt(pc);
        //AdvancePC();
        // return pcByte;
        break;
    default:
        assert(false);
    }
}
void CPU::Set16Bit(Destination d, uint16_t value) {
    switch (d)
    {
    case Register_A:
    case Register_B:
    case Register_C:
    case Register_D:
    case Register_E:
    case Register_F:
    case Register_H:
    case Register_L:
    case Eat_PC_Byte:
        assert(false);
    case Register_BC:
    case Register_DE:
    case Register_HL:
        assert(false);
        break;
    case Register_PC:
        pc = value;
        break;
    case Register_SP:
        sp = value;
        break;
    default:
        assert(false);
    }
}



uint8_t CPU::ReadOpcodeAtPC() {
    return mmu.ByteAt(pc);
}

void CPU::StackDelta(int delta) {
    assert(delta == 1 || delta == -1);


}

void CPU::AdvancePC() {
    pc++;
}

void CPU::Reset() {
    pc = 0x100;
    // Initialized on start, but most programs will move it themselves anyway.
    sp = 0xfffe;
    flags.z = 0;
    flags.h = 0;
    flags.n = 0;
    flags.c = 0;
    interruptsEnabled = true;
}


void CPU::Debugger() {
    cout << "A: " << hex << unsigned(a) << endl;
    cout << "SP: " << hex << sp << endl;
    cout << "PC: " << hex << pc << "[" << hex << unsigned(mmu.ByteAt(pc)) << "]" << endl;
    cout << "Flags:" << endl;
    cout << "   Z: " << hex << flags.z << endl;
    cout << "   C: " << hex << flags.c << endl;
    cout << "   H: " << hex << flags.h << endl;
    cout << "   N: " << hex << flags.n << endl;
}