#include "CPU.hpp"

#include <cassert>
#include <ios>
#include <iostream>

#include "AddressRouter.hpp"
#include "Command.hpp"
#include "Utils.hpp"
#include "PPU.hpp"

CPU::CPU(MMU *mmu, PPU *ppu) {
    // TODO: Create AR in the System and pass in here, not MMU and PPU.
    addressRouter_ = new AddressRouter(mmu, ppu);
    commandFactory_ = new CommandFactory();
    cbCommandFactory_ = new CBCommandFactory();
    debugPrint_ = false;

    Reset();
}

CPU::~CPU() {

}

Command *CPU::CommandForOpcode(uint8_t opcode) {
    if (opcode == 0xCB) {
        opcode = Get8Bit(Eat_PC_Byte);
        return cbCommandFactory_->CommandForOpcode(opcode);
    } else {
        return commandFactory_->CommandForOpcode(opcode);
    }
}

int CPU::Step() {
    // Take actions requested in previous cycle.
    // TODO these actually need a countdown since they happen 2 instructions later.
    if (disableInterruptsNextLoop_) {
        interruptsEnabled_ = false;
        disableInterruptsNextLoop_ = false;
    } else if (enableInterruptsNextLoop_) {
        interruptsEnabled_ = true;
        enableInterruptsNextLoop_ = false;
    } else if (haltNextLoop_) {
        cout << "TODO: Halt!";
        haltNextLoop_ = false;
        assert(false); // TODO!
    } else if (stopNextLoop_) {
        cout << "TODO: Stop!";
        stopNextLoop_ = false;
        assert(false); // TODO!
    }

    uint16_t commandPC = pc_;
    uint8_t opcode = ReadOpcodeAtPC();
    AdvancePC();

    Command *command = CommandForOpcode(opcode);
    command->Run(this);
    int stepped = command->cycles;
    cycles_ += stepped;

    if (debugPrint_) {
        cout << command->description << " ; PC=" << commandPC << endl;
        Debugger();
    }

    assert(stepped < 33 && stepped > 0);

    return stepped;
}

bool CPU::Requires16Bits(Destination destination) {
    switch (destination)
    {
        case Register_A:
        case Register_B:
        case Register_C:
        case Register_D:
        case Register_E:
        case Register_F:
        case Register_H:
        case Register_L:
        case Address_BC:
        case Address_DE:
        case Address_HL:
        case Address_nn:
        case Address_SP:
        case Eat_PC_Byte:
            return false;
        default:
            return true;
    }
}

uint8_t CPU::Get8Bit(Destination destination) {
    uint8_t pcByte;
	uint16_t word;
	switch (destination)
	{
	case Register_A:
		return a_;
	case Register_B:
		return b_;
	case Register_C:
		return c_;
	case Register_D:
		return d_;
	case Register_E:
		return e_;
	case Register_F:
		return (flags.z ? 0x80 : 0) | (flags.n ? 0x40 : 0) | (flags.h ? 0x20 : 0) | (flags.c ? 0x10 : 0);
    case Register_H:
        return h_;
    case Register_L:
        return l_;
    case Address_0xFF00_Byte:
        return addressRouter_->GetByteAt(0xff00 + Get8Bit(Eat_PC_Byte));
    case Address_0xFF00_Register_C:
        return addressRouter_->GetByteAt(0xff00 + Get8Bit(Register_C));        
    case Eat_PC_Byte: {
        pcByte = addressRouter_->GetByteAt(pc_);
        AdvancePC();
        return pcByte;
    case Address_BC:
        return addressRouter_->GetByteAt(Get16Bit(Register_BC));
        break;
    case Address_DE:
        return addressRouter_->GetByteAt(Get16Bit(Register_DE));
        break;
    case Address_HL:
        return addressRouter_->GetByteAt(Get16Bit(Register_HL));
        break;
    case Address_nn:
        word = addressRouter_->GetWordAt(pc_);
        AdvancePC();
        AdvancePC();
        return addressRouter_->GetByteAt(word);
    case Address_SP:
        return addressRouter_->GetByteAt(Get16Bit(Register_SP));
        break;
    }
    default:
        cout << "Unknown 8 bit destination: 0x" << hex << unsigned(destination) << endl;
        assert(false);
        return 0xED;
    }
}

uint16_t CPU::Get16Bit(Destination destination) {
    uint16_t word;
    switch (destination)
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
    case Address_BC:
    case Address_DE:
    case Address_HL:
    case Address_SP:
    case Address_nn:
    case Address_0xFF00_Byte:
    case Address_0xFF00_Register_C:
        cout << "Tried reading 0x" << hex << unsigned(destination) << " as a 16 bit value." << endl;
        assert(false);
    case Register_AF:   
        return buildMsbLsb16(a_, Get8Bit(Register_F));
    case Register_BC:
        return buildMsbLsb16(b_, c_);
    case Register_DE:
        return buildMsbLsb16(d_, e_);
    case Register_HL:
        return buildMsbLsb16(h_, l_);
    case Register_SP:
        return sp_;
    case Register_PC:
        return pc_;
    case Eat_PC_Word:
        word = addressRouter_->GetWordAt(pc_);
        AdvancePC();
        AdvancePC();
        return word;
    default:
        cout << "Unknown desination for get: 0x" << hex << unsigned(destination) << endl;
        assert(false);
    }
    return 0xDEAD;
}

void CPU::Set8Bit(Destination destination, uint8_t value) {
    switch (destination)
    {
    case Register_A:
        a_ = value;
        break;
    case Register_B:
        b_ = value;
        break;
    case Register_C:
        c_ = value;
        break;
    case Register_D:
        d_ = value;
        break;
    case Register_E:
        e_ = value;
        break;
    case Register_F:
        flags.z = 0x80 & value;
        flags.n = 0x40 & value;
        flags.h = 0x20 & value;
        flags.c = 0x10 & value;
        break;
    case Register_H:
        h_ = value;
        break;
    case Register_L:
        l_ = value;
        break;
    case Address_BC:
        addressRouter_->SetByteAt(Get16Bit(Register_BC), value);
        break;
    case Address_DE:
        addressRouter_->SetByteAt(Get16Bit(Register_DE), value);
        break;
    case Address_HL:
        addressRouter_->SetByteAt(Get16Bit(Register_HL), value);
        break;
    case Address_SP:
        cout << "Consider moving the SP on set? How is it happening elsewhere?" << endl;
        assert(false);
        addressRouter_->SetByteAt(Get16Bit(Register_SP), value);
        break;
    case Address_nn:
        addressRouter_->SetByteAt(Get16Bit(Eat_PC_Word), value);
        break;
    case Address_0xFF00_Byte:
        addressRouter_->SetByteAt(0xff00 + Get8Bit(Eat_PC_Byte), value);
        break;
    case Address_0xFF00_Register_C:
        addressRouter_->SetByteAt(0xff00 + Get8Bit(Register_C), value);
        break;
    case Eat_PC_Byte:
        // TODO: Is this even valid/necessary?
        assert(false);
        break;
    default:
        cout << "Unknown 8 bit destination: 0x" << hex << unsigned(destination) << endl;
        assert(false);
    }
}

void CPU::Set16Bit(Destination destination, uint16_t value) {
    uint16_t address;
    switch (destination)
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
    case Address_0xFF00_Byte:
    case Address_0xFF00_Register_C:
        cout << "0x" << hex << unsigned(destination) << " is not 16 bits." << endl;
        assert(false);
    case Register_AF:
        a_ = HIGHER8(value);
        Set8Bit(Register_F, LOWER8(value));
        break;
    case Register_BC:
        b_ = HIGHER8(value);
        c_ = LOWER8(value);
        break;
    case Register_DE:
        d_ = HIGHER8(value);
        e_ = LOWER8(value);
        break;
    case Register_HL:
        h_ = HIGHER8(value);
        l_ = LOWER8(value);
        break;
    case Register_PC:
        pc_ = value;
        break;
    case Register_SP:
        sp_ = value;
        break;
    case Eat_PC_Word:
        addressRouter_->SetWordAt(pc_, value);
        assert(false);
        break;
    case Address_BC:
    case Address_DE:
    case Address_HL:
    case Address_SP:
    case Address_nn:
        address = Get16Bit(destination);
        addressRouter_->SetWordAt(address, value);
        break;
    default:
        cout << "Unknown 16 bit set: " << hex << unsigned(destination) << endl;
        assert(false);
    }
}

uint8_t CPU::ReadOpcodeAtPC() {
    if (disasemblerMode_) {
        addressRouter_->EnableDisassemblerMode(false);
        uint8_t opcode = addressRouter_->GetByteAt(pc_);
        addressRouter_->EnableDisassemblerMode(true);
        return opcode;
    }
    return addressRouter_->GetByteAt(pc_);
}

void CPU::Push8Bit(uint8_t byte) {
    // Stack pointer grows down before anything is pushed.
    sp_ -= 1;
    addressRouter_->SetByteAt(sp_, byte);
}

void CPU::Push16Bit(uint16_t word) {
    // TODO is this the right order???
    // Test.
    Push8Bit(LOWER8(word));
    Push8Bit(HIGHER8(word));
}

uint8_t CPU::Pop8Bit() {
    uint16_t oldSp = sp_;
    uint8_t byte = addressRouter_->GetByteAt(sp_);
    sp_ += 1;
    assert(sp_ > oldSp);
    return byte;
}

uint16_t CPU::Pop16Bit() {
    uint8_t msb = Pop8Bit();
    uint8_t lsb = Pop8Bit();
    return buildMsbLsb16(msb, lsb);
}

void CPU::EnableDisassemblerMode() {
    disasemblerMode_ = true;
    addressRouter_->EnableDisassemblerMode(true);
}

void CPU::JumpAddress(uint16_t address) {
    if (disasemblerMode_) {
        return;
    }

    // cout << "Jumping to 0x" << hex << unsigned(address) << endl;
    Set16Bit(Register_PC, address);
}

void CPU::JumpRelative(uint8_t relative) {
    if (disasemblerMode_) {
        return;
    }

    uint16_t originalAddress = Get16Bit(Register_PC);
    int8_t signedRelative = relative;
    uint16_t newAddress = originalAddress + signedRelative;
    // cout << "Jumping 0x" << hex << signed(signedRelative);
    // cout << " relative to 0x" << hex << unsigned(originalAddress);
    // cout << " to 0x" << hex << unsigned(newAddress) << endl;
    Set16Bit(Register_PC, newAddress);    
}

void CPU::AdvancePC() {
    pc_++;
}

void CPU::Reset() {
    pc_ = 0;
    // Initialized on start, but most programs will move it themselves anyway.
    sp_ = 0xfffe;
    flags.z = false;
    flags.h = false;
    flags.n = false;
    flags.c = false;

    interruptsEnabled_ = true;

    haltNextLoop_ = false;
    stopNextLoop_ = false;
    disableInterruptsNextLoop_ = false;
    enableInterruptsNextLoop_ = false;

    cycles_ = 0;
    a_ = b_ = c_ = d_ = e_ = h_ = l_ = 0;
}

void CPU::Debugger() {
    cout << "AF: " << hex << unsigned(Get8Bit(Register_A)) << "," << hex << unsigned(Get8Bit(Register_F));
    cout << " BC: " << hex << unsigned(Get8Bit(Register_B)) << "," << hex << unsigned(Get8Bit(Register_C));
    cout << " DE: " << hex << unsigned(Get8Bit(Register_D)) << "," << hex << unsigned(Get8Bit(Register_E));
    cout << " HL: " << hex << unsigned(Get8Bit(Register_H)) << "," << hex << unsigned(Get8Bit(Register_L));
    cout << " SP: " << hex << unsigned(sp_);
    cout << " PC: " << hex << unsigned(pc_);
    cout << " Flags:";
    cout << " Z: " << hex << flags.z;
    cout << " C: " << hex << flags.c;
    cout << " H: " << hex << flags.h;
    cout << " N: " << hex << flags.n << endl << endl;
}