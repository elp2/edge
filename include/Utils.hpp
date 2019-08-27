#include <cstdint>

#include "Destination.hpp"

// Bit utility functions.
#define HIGHER8(word) ((word >> 8) & 0xff)
#define LOWER8(word) (word & 0xff)

#define NIBBLEA(byte) ((byte & 0xf) >> 1)
#define NIBBLEB(byte) (byte & 0xf)

uint16_t build16(uint8_t lsb, uint8_t msb);

// See http://www.pastraiser.com/cpu/gameboy/gameboy_opcodes.html.
Destination destinationForColumn(uint8_t column);