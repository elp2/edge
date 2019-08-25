#include "NopOpcode.hpp"

NopOpcode::NopOpcode(uint8_t opcode) {
    this->opcode = opcode;
    this->description = "NOP";
    this->cycles = 4;
}

NopOpcode::~NopOpcode() {

}

void NopOpcode::Run(CPU *cpu, MMU *mmu) {
    // Nop.
    (void)cpu;
    (void)mmu;
}