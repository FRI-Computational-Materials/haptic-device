#include "atom.h"
#include "boundaryConditions.h"
#include "chai3d.h"
#include "globals.h"
#include "inputHandling.h"
#include "potentials.h"
#include "utility.h"
#include "python_instance.h"
#include "PyAMFF/PyAMFF.h"
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
using namespace chai3d;
using namespace std;

void  pepperoni(char *argv[], cWorld *world, cTexture2dPtr texture){
  // Begin python instance and set path
  Py_Initialize();
  PyRun_SimpleString("import sys\nsys.path.append('../../haptic-device/')\n");

  // Create variables for the function and module name, return tuple object, and
  PyObject *pName, *pModule, *pFunc, *pFileName, *pCallTuple;
  PyObject *pResult;
  std::vector<std::vector<float>> positions;
  std::vector<int> startingAtomicNrs;
  int nAtoms;

  // Import python module
  pName = PyUnicode_FromString("ase_file_io");
  pModule = PyImport_Import(pName);

  // return an error if PyImport_Import can't find the pModule
  if (pModule != NULL) {
    // get function
    pFunc = PyObject_GetAttrString(pModule, "get_state_information");

    if (pFunc && PyCallable_Check(pFunc)) {
      // Send filename to function. pResult is a python tuple that will be unpacked to get out values that we want
      pFileName = PyUnicode_FromString(argv[1]);
      pCallTuple = PyTuple_New(1); // this is because CallObject requires that you pass in a tuple containing the data you want
      PyTuple_SetItem(pCallTuple, 0, pFileName);
      pResult = PyObject_CallObject(pFunc, pCallTuple);

      // get number of atoms
      nAtoms = (int)PyLong_AsLong(PyList_GetItem(pResult, 0));

      // Unpack Positions and extract values
      for (int i = 0; i < nAtoms; i++) {
        // unpack positions
        positions.push_back(std::vector<float>({
          PyFloat_AsDouble(PyList_GetItem(pResult, 3 * i + 1)),
          PyFloat_AsDouble(PyList_GetItem(pResult, 3 * i + 2)),
          PyFloat_AsDouble(PyList_GetItem(pResult, 3 * i + 3))
        }));
        // unpack atomic numbers
        startingAtomicNrs.push_back((int)PyLong_AsLong(PyList_GetItem(pResult, 3*nAtoms + 1 + i)));
      }

      // decref everything (this breaks our python instance. Will try to implement correctly later)
      Py_DECREF(pName);
      Py_DECREF(pModule);
      Py_DECREF(pResult);
      Py_DECREF(pFunc);
      Py_DECREF(pFileName);
      Py_DECREF(pCallTuple);

      // end python instance (doing this caused a segfault)
      Py_FinalizeEx(); //As of 05/26/2022, the decrefs and ending python instance are not causing any obervable issues - Sydney :)
    }
   //}
  } else {
    std::cout << "Error: module not found" << std::endl;
  }

  // create atoms objects, put them in spheres, world
  for (int i = 0; i < nAtoms; i++) {
    // Create atom pointer
    Atom* newAtom = new Atom(SPHERE_RADIUS, startingAtomicNrs[i]);

    // store pointer to sphere primitive
    spheres.push_back(newAtom);

    // add sphere primitive to world
    world->addChild(newAtom);

    // add line to world
    world->addChild(newAtom->getVelVector());

    // set graphic properties of sphere
    newAtom->setTexture(texture);
    newAtom->m_texture->setSphericalMappingEnabled(true);
    newAtom->setUseTexture(true);

    // Set the positions of all atoms
    if (i == 0) {
      // make very first atom the current atom
      newAtom->setCurrent(true);
      // get coordinates from PositionTriplet
      for (int j = 0; j < 3; j++) {
        centerCoords[j] = positions[0][j];
      }
      // set first atom at center of view
      newAtom->setLocalPos(0.0, 0.0, 0.0);
    } else {
      // Anchor by default
      newAtom->setAnchor(true);
      // scale coordinates and insert
      newAtom->setLocalPos(
        0.02 * (positions[i][0] - centerCoords[0]), // position offset -- should probably disappear once we get boxes working
        0.02 * (positions[i][1] - centerCoords[1]),
        0.02 * (positions[i][2] - centerCoords[2])
      );
    }    
  }
}
