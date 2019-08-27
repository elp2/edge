#include <cstdint>

// Bit utility functions.
#define HIGHER8(word) ((word >> 8) & 0xff)
#define LOWER8(word) (word & 0xff)

uint16_t build16(uint8_t lsb, uint8_t msb);