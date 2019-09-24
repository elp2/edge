#include "SDL.h"

#include <fstream>
#include <iostream>

#include "system.hpp"


int main(int argc, char* argv[]) {
	std::ofstream log("log.txt");
	std::cout.rdbuf(log.rdbuf());

	System* system = new System("../../d.gb");

	// System* system = new System("../../gb-test-roms/instr_timing/instr_timing.gb");

	// System* system = new System("../../gb-test-roms/cpu_instrs/individual/01-special.gb"); // PASS
	// System* system = new System("../../gb-test-roms/cpu_instrs/individual/03-op sp,hl.gb"); // E8 E8 F8 F8 Failed (Add, ld sp).
	// System *system = new System("../../gb-test-roms/cpu_instrs/individual/04-op r,imm.gb"); // PASS
	// System *system = new System("../../gb-test-roms/cpu_instrs/individual/05-op rp.gb"); // PASS
	// System* system = new System("../../gb-test-roms/cpu_instrs/individual/06-ld r,r.gb"); // PASS
	// System* system = new System("../../gb-test-roms/cpu_instrs/individual/07-jr,jp,call,ret,rst.gb"); // EVERYTHING failed.
	// System* system = new System("../../gb-test-roms/cpu_instrs/individual/08-misc instrs.gb"); // F5 C5 D5 E5 F1 C1 D1 E1 Failed
	// System *system = new System("../../gb-test-roms/cpu_instrs/individual/09-op r,r.gb"); PASS.
	// System* system = new System("../../gb-test-roms/cpu_instrs/individual/10-bit ops.gb"); // PASS
	// System* system = new System("../../gb-test-roms/cpu_instrs/individual/11-op a,(hl).gb"); // PASS
	system->Main();
    return 0;
}
