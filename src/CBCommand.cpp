#include "CBCommand.hpp"

#include "CPU.hpp"
#include "MMU.hpp"
#include "Utils.hpp"

uint8_t bitForRowColumn(uint8_t startingRow, uint8_t row, uint8_t column) {
    uint8_t times = ((row - startingRow) * 16 + column) / 8;
    uint8_t testBit = 1;
    while (times--) {
        testBit = testBit << 1;
    }
    return testBit;
}

CBCommand::CBCommand(uint8_t opcode) {
    this->opcode = opcode;
    // for (int r = 4; r < 8; r++) {
    //     for (int column = 0; column < 16; column++) {
    //         int row = r;
    //         uint8_t testBit = bitForRowColumn(4, row, column);

    //         cout << hex << unsigned(testBit) << endl;
    //     }
    // }
    // assert(false);
}

CBCommand::~CBCommand() {
}

void CBCommand::TestBit(uint8_t row, uint8_t column, CPU *cpu) {
    this->description = "BIT "; // TODO sprintf the register, values.
    uint8_t testBit = bitForRowColumn(4, row, column);    
    cpu->flags.z = ((testBit & cpu->Get8Bit(destinationForColumn(column))) == 0);
    cpu->flags.n = false;
    cpu->flags.h = true;
}

void CBCommand::Run(CPU *cpu, MMU *mmu) {
    (void)mmu;

    // See http://www.pastraiser.com/cpu/gameboy/gameboy_opcodes.html.
    uint8_t row = NIBBLEA(opcode);
    uint8_t column = NIBBLEB(opcode);
    cycles = destinationForColumn(column) == Address_HL ? 16 : 8;

    if (row >= 0x4 && row <= 0x7) {
        return TestBit(row, column, cpu);
    } else {
        cout << "No CB for 0x" << hex << row << hex << column << endl;
        assert(false);
    }
}