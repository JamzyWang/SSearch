#include "Algorithm.h"

#include <iostream>

int main(int argc, char *argv[]) {
  DefaultAlgorithm algorithm;



  if (argv[1] == std::string("build") && argc == 4) {
    algorithm.update_database(argv[2]);
    algorithm.save_database(argv[3]);
    return 0;
  }

  if (argv[1] == std::string("update") && argc == 4) {
    algorithm.load_database(argv[3]);
    algorithm.update_database(argv[2]);
    algorithm.save_database(argv[3]);
  }
}
