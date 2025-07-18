#include <errno.h>
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>

#include "system.h"

int main(int argc, char* argv[]) {
  int state_number = -1;
  if (argc < 3 || argc > 4) {
    std::cout << "Usage: rom.gb StateDirectory (state_number)" << std::endl;
    return 1;
  } else if (argc == 4) { 
    state_number = std::stoi(argv[3]);
  }
  // Log to file.
  std::ofstream log("log.txt");
  std::cout.rdbuf(log.rdbuf());

  std::string rom_file = argv[1];
  std::cout << "Loading ROM " << rom_file << std::endl;

  std::string state_dir = argv[2];  
  // Check if state directory exists, create if necessary.
  struct stat st = {0};
  if (stat(state_dir.c_str(), &st) == -1) {
    std::cout << "Creating state directory: " << state_dir << std::endl;
    if (mkdir(state_dir.c_str(), 0700) == -1) {
      std::cerr << "Error creating directory: " << state_dir << " - " << strerror(errno) << std::endl;
      return 1;
    }
  } else {
    std::cout << "State directory exists: " << state_dir << std::endl;
  }

  size_t last_slash = rom_file.find_last_of("/\\");
  std::string rom_name = (last_slash != std::string::npos) ? rom_file.substr(last_slash + 1) : rom_file;
  std::string game_state_dir = state_dir + "/" + rom_name;
  std::cout << "Game state directory: " << game_state_dir << std::endl;
  
  System *system = new System(rom_file, game_state_dir);
  if (state_number >= 0) {
    std::cout << "Loading state " << state_number << std::endl;
    system->LoadStateSlot(state_number);
  } else {
    std::cout << "No state number provided, starting from beginning." << std::endl;
  }

  system->Main();
  return 0;
}
