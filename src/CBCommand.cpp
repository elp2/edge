#include "CBCommand.hpp"

#include <bitset>

#include <sstream>

#include "CPU.hpp"
#include "MMU.hpp"
#include "Utils.hpp"

uint8_t bitForRowColumn(uint8_t startingRow, uint8_t row, uint8_t column) {
    uint8_t times = ((row - startingRow) * 16 + column) / 8;
    assert(times < 8);
    uint8_t testBit = 1;
    while (times--) {
        testBit = testBit << 1;
    }
    return testBit;
}

CBCommand::CBCommand(uint8_t opcode) {
    this->opcode = opcode;
}

CBCommand::~CBCommand() {
}

string commandName(string base, uint8_t startingRow, uint8_t row, uint8_t column) {
    ostringstream stream;
    Destination d = destinationForColumn(column);
    uint8_t bitNum = ((row - startingRow) * 16 + column) / 8;
    stream << base << " " << unsigned(bitNum) << "," << destinationToString(d);
    return stream.str();
}

void CBCommand::TestBit(uint8_t row, uint8_t column, CPU *cpu) {
    this->description = commandName("BIT", 4, row, column);
    uint8_t testBit = bitForRowColumn(4, row, column);
    cpu->flags.z = ((testBit & cpu->Get8Bit(destinationForColumn(column))) == 0);
    cpu->flags.n = false;
    cpu->flags.h = true;
    // c unchanged.
}

void CBCommand::ResetBit(uint8_t row, uint8_t column, CPU *cpu) {
    this->description = commandName("RES", 8, row, column);
    uint8_t resetBit = bitForRowColumn(8, row, column);
    resetBit = ~resetBit;

    cout << "ResetBit: " << resetBit << endl;

    Destination d = destinationForColumn(column);
    cpu->Set8Bit(d, resetBit & cpu->Get8Bit(d));
}

void CBCommand::SetBit(uint8_t row, uint8_t column, CPU *cpu) {
    this->description = commandName("SET", 0xc, row, column);
    uint8_t setBit = bitForRowColumn(0xc, row, column);
    setBit = ~setBit;

    cout << "setBit: " << setBit << endl;

    Destination d = destinationForColumn(column);
    cpu->Set8Bit(d, setBit | cpu->Get8Bit(d));
}

void CBCommand::Swap(uint8_t column, CPU *cpu) {
    assert(column <= 7);
    Destination d = destinationForColumn(column);

    this->description = "SWAP TODO";
    uint8_t value = cpu->Get8Bit(d);
    uint8_t swapped = (NIBBLELOW(value) << 4) | NIBBLEHIGH(value);

    assert(true); // TODO Untested
    cpu->Set8Bit(d, swapped);
    cpu->flags.z = (swapped == 0);
    cpu->flags.n = false;
    cpu->flags.h = false;
    cpu->flags.c = false;
}

void CBCommand::Run(CPU *cpu, MMU *mmu) {
    (void)mmu;

    // See http://www.pastraiser.com/cpu/gameboy/gameboy_opcodes.html.
    uint8_t row = NIBBLEHIGH(opcode);
    uint8_t column = NIBBLELOW(opcode);

    cycles = destinationForColumn(column) == Address_HL ? 16 : 8;

    switch (row)
    {
    case 0x0:
        if (column <= 0x7) {
            return Swap(column, cpu);
        }
    case 0x1:
    case 0x2:
    case 0x3:
    case 0x4:
    case 0x5:
    case 0x6:
        cout << "Unhandled CB Opcode " << hex << unsigned(opcode) << endl;
        assert(false);
        break;
    case 0x7:
        return TestBit(row, column, cpu);
    case 0x8:
    case 0x9:
    case 0xa:
    case 0xb:
        return ResetBit(row, column, cpu);
    case 0xc:
    case 0xd:
    case 0xe:
    case 0xef:
        return SetBit(row, column, cpu);
    default:
        cout << "No CB for 0x" << hex << unsigned(row) << hex << unsigned(column) << endl;
        assert(false);
    }
}