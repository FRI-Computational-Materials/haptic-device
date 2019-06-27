#include <string>

//checks if given file exists
bool fileExists(const std::string &name);

// Reads in global minimum from global_minima.txt
double getGlobalMinima(int cluster_size);

// checks if char array represents a number
bool isNumber(char number[]);
