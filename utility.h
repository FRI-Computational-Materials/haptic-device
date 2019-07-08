#include <string>
#include "chai3d.h"

//checks if given file exists
bool fileExists(const std::string &name);

// Reads in global minimum from global_minima.txt
double getGlobalMinima(int cluster_size);

// checks if char array represents a number
bool isNumber(char number[]);

//add hotkey help labels
void addHotkeyLabel(std::string key, std::string text);

// add a label to the world with default black text
void addLabel(chai3d::cLabel *&label);

// Update camera text
void updateCameraLabel(chai3d::cLabel *&camera_pos, chai3d::cCamera *&camera);

// save configuration in .con file
void writeToCon(std::string fileName);
