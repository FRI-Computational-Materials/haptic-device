#include "utility.h"
#include <cctype>
#include <sys/types.h>
#include <sys/stat.h> 

// check if char array represents a number
bool isNumber(char number[]) {
  for (int i = 0; number[i] != 0; i++) {
    if (!isdigit(number[i])) return false;
  }
  return true;
}

// check if file already exists in directory
bool fileExists(const std::string &name) {
  struct stat buffer;
  return (stat(name.c_str(), &buffer) == 0);
}
