#include "Opcodes.hpp"
#include "CPU.hpp"
#include "MMU.hpp"

#include <iostream>

class Opcode {
 public:
    int cycles;
    uint8_t opcode;
    string description;

    virtual void Run(CPU *cpu, MMU *mmu);

};

Opcodes::Opcodes(CPU *cpu) {
    this->cpu = cpu;
}

Opcodes::~Opcodes() {

}

void Opcodes::RunOpcode(uint8_t opcode) {
    switch (opcode)
    {
    case 0x00:
        
        break;
    case 0xCB:
        cout << "Can't handle CB opcode!" << endl;
    default:
        break;
    }
}

class PushOpcode : public Opcode {
 public:
    PushOpcode(uint8_t opcode, string description, Destination from, int cycles) {
        this->opcode = opcode;
        this->description = description;
        this->cycles = cycles;

        // TODO!!!
        (void)from;
        //this->from = from;
    }

    // void Run(CPU *cpu, MMU *mmu) {
    //     uint16_t value = cpu->Read16Bit(this->from);
    //     uint8_t lsb = value & 0xff;
    //     uint8_t msb = (value >>8) & 0xff;
    //     // set lsb
    //     // dec sp
    //     // set msb
    //     // dec sp

    // }

//  private:
//     Destination from;
};


