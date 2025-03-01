#include <fstream>
#include <iostream>

#include "system.h"

int main(int argc, char* argv[]) {
  (void)argc;
  (void)argv;
  std::cout << "Loading ROM " << argv[1] << std::endl;

  std::ofstream log("log.txt");
  std::cout.rdbuf(log.rdbuf());

  System *system = new System(argv[1]);
  system->Main();
  return 0;
}
