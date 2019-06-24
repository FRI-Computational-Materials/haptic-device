#include "utility.h"
#include <cctype>
#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>
#include <iostream>

// check if file already exists in directory
bool fileExists(const std::string &name) {
  struct stat buffer;
  return (stat(name.c_str(), &buffer) == 0);
}

//read in global minimum by cluster size
double getGlobalMinima(int cluster_size) {
  std::string file_path = "../resources/data/";
  std::string file_name = "global_minima.txt";
  std::ifstream infile(file_path + file_name);
  if (!infile) {
    std::cerr << "Could not open \"" + file_name + "\" for reading" << std::endl;
    std::cerr << "Did you move it to \"" + file_path + "\"?" << std::endl;
    exit(1);
  } else if ((cluster_size < 2) || (cluster_size > 150)) {
    std::cout << "WARNING: \"" + file_name +
                "\" doesn't have data for clusters of this size yet."
         << std::endl;
    std::cout << "The graph may not be accurate." << std::endl;
  }

  int cluster_size_file;
  double minimum;
  while (infile >> cluster_size_file >> minimum) {
    if (cluster_size_file == cluster_size) {
      break;
    }
  }
  std::cout << "Global minimum:" << minimum << std::endl;
  return minimum;
}

// check if char array represents a number
bool isNumber(char number[]) {
  for (int i = 0; number[i] != 0; i++) {
    if (!isdigit(number[i])) return false;
  }
  return true;
}
