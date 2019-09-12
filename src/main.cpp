#include "SDL.h"

#include "system.hpp"

int main(int argc, char* argv[]) {
    System *system = new System("../../gb-test-roms/cpu_instrs/cpu_instrs.gb");
    system->Main();
    return 0;
}
