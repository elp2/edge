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