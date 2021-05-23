#include "SDL.h"

#include <fstream>
#include <iostream>

#include "system.h"


int main(int argc, char* argv[]) {
	(void)argc;
	(void)argv;
	std::ofstream log("log.txt");
	std::cout.rdbuf(log.rdbuf());

	// System *system = new System("../../dmg-acid2.gb");
	// System *system = new System("../../cgb_sound.gb");

	// System *system = new System("../../gb-test-roms/instr_timing/instr_timing.gb");
	// System *system = new System("../../gb-test-roms/cpu_instrs/cpu_instrs.gb"); // All OK, #3 failed.
	// System *system = new System("../../gb-test-roms/instr_timing/instr_timing.gb"); // Passes.
	// System *system = new System("../../gb-test-roms/interrupt_time/interrupt_time.gb");  // Fails. CGB Only?
	// System *system = new System("../../gb-test-roms/mem_timing/mem_timing.gb");  // Fails 1, 2, 3.

	System *system = new System("../../gb-test-roms/cpu_instrs/individual/03-op sp,hl.gb"); // E8 F8 Failed (Add, ld sp).
	system->Main();
    return 0;
}
