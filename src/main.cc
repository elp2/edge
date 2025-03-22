#include <fstream>
#include <iostream>

#include "system.h"

int main(int argc, char* argv[]) {
  std::string state_dir = "";
  if (argc < 2) {
    std::cout << "Usage: rom.gb [States/statename]" << std::endl;
    return 0;
  } else if (argc == 2) {
    state_dir = "";
  } else {
    state_dir = argv[2];
  }

  std::string rom_file = argv[1];
  std::cout << "Loading ROM " << rom_file << std::endl;

  std::ofstream log("log.txt");
  std::cout.rdbuf(log.rdbuf());

  System *system = new System(rom_file, state_dir);
  system->Main();
  return 0;
}
