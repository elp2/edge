#include "Utils.hpp"

#include <cassert>
#include <iostream>

using namespace std;

uint16_t build16(uint8_t lsb, uint8_t msb) {
    uint16_t word = msb;
    word = word << 8;
    word |= lsb;
    assert(HIGHER8(word) == msb);
    assert(LOWER8(word) == lsb);

    return word;
}

Destination destinationForColumn(uint8_t column) {
    switch (column)
    {
    case 0x00:
    case 0x08:
        return Register_B;
    case 0x01:
    case 0x09:
        return Register_C;
    case 0x02:
    case 0x0a:
        return Register_D;
    case 0x03:
    case 0x0b:
        return Register_E;
    case 0x04:
    case 0x0c:
        return Register_H;
    case 0x05:
    case 0x0d:
        return Register_L;
    case 0x06:
    case 0x0e:
        return Address_HL;
    case 0x07:
    case 0x0f:
        return Register_A;
    
    default:
        cout << "Unexpected destination for column: 0x" << hex << column << endl;
        assert(false);
        break;
    }
}

string destinationToString(Destination d) {
    switch (d)
    {
    case Register_A:
        return "A";
    case Register_B:
        return "B";
    case Register_C:
        return "C";
    case Register_D:
        return "D";
    case Register_E:
        return "E";
    case Register_H:
        return "H";
    case Register_L:
        return "L";
    case Register_BC:
        return "BC";
    case Register_DE:
        return "DE";
    case Register_HL:
        return "HL";
    case Address_SP:
        return "(SP)";
    case Address_BC:
        return "(BC)";
    case Address_DE:
        return "(DE)";
    case Address_HL:
        return "(HL)";
    case Address_nn:
        return "(nn)";
    case Eat_PC_Byte:
        return "n";
    case Eat_PC_Word:
        return "nn";    
    default:
        cout << "Unknown d 0x" << hex << unsigned(d) << endl;
        assert(false);        
        break;
    }
    return "?";
}