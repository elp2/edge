#include "CPU.hpp"
#include "MMU.hpp"

#include <iostream>

class Command {
 public:
    int cycles;
    uint8_t opcode;
    string description;

    virtual void Run(CPU *cpu, MMU *mmu);

};

class PushCommand : public Command {
 public:
    PushCommand(uint8_t opcode, string description, Destination from, int cycles) {
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


