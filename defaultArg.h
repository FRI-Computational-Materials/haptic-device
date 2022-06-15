#ifndef DEFAULTARG_H
#define DEFAULTARG_H

using namespace std;
using namespace chai3d;

#include "atom.h"
#include "chai3d.h"
//#include "globals.h"
#include "inputHandling.h"
#include "potentials.h"
#include "utility.h"
//#include "PyAMFF/PyAMFF.h"
//------------------------------------------------------------------------------
#include <GLFW/glfw3.h>
#include <python3.8/Python.h>
#include <math.h>
#include <unistd.h>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
//------------------------------------------------------------------------------


void  defaultArg(int argc, char *argv[], cWorld *world, cTexture2dPtr texture);

#endif
