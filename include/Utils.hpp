#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "Destination.hpp"
#include "Pixel.hpp"

// Bit utility functions.
#define HIGHER8(word) ((word >> 8) & 0xff)
#define LOWER8(word) (word & 0xff)

#define NIBBLEHIGH(byte) ((byte  >> 4 ) & 0xf)
#define NIBBLELOW(byte) (byte & 0xf)

uint16_t buildMsbLsb16(uint8_t msb, uint8_t lsb);

// See http://www.pastraiser.com/cpu/gameboy/gameboy_opcodes.html.
Destination destinationForColumn(uint8_t column);

std::string destinationToString(Destination d);

std::string descriptionforPixel(Pixel p);

class CPU;
CPU *getTestingCPU();
CPU *getTestingCPUWithInstructions(std::vector<uint8_t> instructions);

class MMU;
MMU *getTestingMMU();
