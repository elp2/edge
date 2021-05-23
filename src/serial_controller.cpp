#include "serial_controller.h"

#include <cassert>
#include <iostream>

using namespace std;

SerialController::SerialController() {

}

uint8_t SerialController::GetByteAt(uint16_t address) {
    switch (address)
    {
    case 0xFF01:
        return sb();
    case 0xFF02:
        return sc();
    default:
        assert(false);
        return 0xFF;
    }
}

void SerialController::SetByteAt(uint16_t address, uint8_t byte) {
    switch (address)
    {
    case 0xFF01:
        set_sb(byte);
        break;
    case 0xFF02:
        set_sc(byte);
        break;
    default:
        assert(false);
        break;
    }
}

void SerialController::set_sc(uint8_t byte) {
    sc_ = byte;

    if (byte == 0x81) {
        line->push_back((char)sb());
        cout << "**** SERIAL: [" << *line << "]" << endl;
    } else {
        cout << "Unexpected serial sc: " << hex << unsigned(byte) << endl;
    }
}