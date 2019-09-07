#include <cassert>
#include <iostream>

#include "AddressRouter.hpp"
#include "BitCommand.hpp"
#include "CommandFactory.hpp"
#include "CPU.hpp"
#include "MMU.hpp"
#include "PPU.hpp"
#include "Utils.hpp"

int main() {
    CPU *cpu = getTestingCPU();
    cpu->JumpAddress(0x00);

    long LIMIT = 10000000;
	// cpu->SetDebugPrint(true);
    for (long i = 0; i < LIMIT; i++) {
        if (cpu->Get16Bit(Register_PC) == 0x89) {
            // cpu->SetDebugPrint(true);
        }
        if (cpu->Get16Bit(Register_PC) == 0x00fe) {
            cpu->Step();
            cout << "# Instructions = " << i << endl;
            cout << "CPU Cycles " << cpu->Cycles() << endl;
            // break;
        }
        cpu->Step();
    }

    cout << "Finished at PC: 0x" << hex << unsigned(cpu->Get16Bit(Register_PC)) << endl;
}