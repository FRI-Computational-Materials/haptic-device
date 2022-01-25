#include "potentials.h"
#include <vector>
#include "atom.h"
#include <python3.8/Python.h>
#include "PyAMFF/PyAMFF.h"

///////////////////////////// MORSE ///////////////////////////////////////
// Force and Potential energy calculation function for the Morse calculator
vector<vector<double>> morseCalculator::getFandU(vector<Atom*>& spheres) {
  vector<vector<double>> returnVector;
  double potentialEnergy = 0;
  Atom* current;
  for (int i = 0; i < spheres.size(); i++) {
    // compute force on atom
    cVector3d force;
    current = spheres[i];
    cVector3d pos0 = current->getLocalPos();
    // check forces with all other spheres
    force.zero();

    // this loop is for finding all of atom i's neighbors
    for (int j = 0; j < spheres.size(); j++) {
      // Don't compute forces between an atom and itself
      if (i != j) {
        // get position of sphere
        cVector3d pos1 = spheres[j]->getLocalPos();

        // compute direction vector from sphere 0 to 1

        cVector3d dir01 = cNormalize(pos0 - pos1);

        // compute distance between both spheres
        double distance = cDistance(pos0, pos1) / distanceScale;
        potentialEnergy += getMorseEnergy(distance);
        double appliedForce = getMorseForce(distance);
        force.add(appliedForce * dir01);
      }
    }
    vector<double> pushBack = {force.x(), force.y(), force.z()};
    returnVector.push_back(pushBack);
  }
  // Potential energy -- Halve it because pairwise
  vector<double> potentE = {potentialEnergy / 2};
  returnVector.push_back(potentE);

  return returnVector;
}

// Pairwise energy calculation for morse
double morseCalculator::getMorseEnergy(double distance) {
  double expf = exp(rho0 * (1.0 - distance / r0));
  return epsilon * expf * (expf - 2);
}

// Pairwise force calculation for morseCalculator
double morseCalculator::getMorseForce(double distance) {
  double temp = -2 * rho0 * epsilon * exp(rho0 - (2 * rho0 * distance) / r0) * (exp((rho0 * distance)/r0) - exp(rho0));
  return temp / r0 * forceDamping;
}

//////////////////////////////// LJ ///////////////////////////////////////
// Force and Potential energy calculation function for the lj calculator
vector<vector<double>> ljCalculator::getFandU(vector<Atom*>& spheres) {
  vector<vector<double>> returnVector;
  double potentialEnergy = 0;
  Atom* current;
  for (int i = 0; i < spheres.size(); i++) {
    // compute force on atom
    cVector3d force;
    current = spheres[i];
    cVector3d pos0 = current->getLocalPos();
    // check forces with all other spheres
    force.zero();

    // this loop is for finding all of atom i's neighbors
    for (int j = 0; j < spheres.size(); j++) {
      // Don't compute forces between an atom and itself
      if (i != j) {
        // get position of sphere
        cVector3d pos1 = spheres[j]->getLocalPos();

        // compute direction vector from sphere 0 to 1

        cVector3d dir01 = cNormalize(pos0 - pos1);

        // compute distance between both spheres
        double distance = cDistance(pos0, pos1) / distanceScale;
        potentialEnergy += getLennardJonesEnergy(distance);
        double appliedForce = getLennardJonesForce(distance);
        force.add(appliedForce * dir01);
      }
    }
  vector<double> pushBack = {force.x(), force.y(), force.z()};
  returnVector.push_back(pushBack);
  }
  // Potential energy -- halve it because pairwise
  vector<double> potentE = {potentialEnergy / 2};
  returnVector.push_back(potentE);

  return returnVector;
}

// Pairwise energy calculation for lj
double ljCalculator::getLennardJonesEnergy(double distance) {
  return 4 * epsilon * (pow(sigma / distance, 12) - pow(sigma / distance, 6));
}

// Pairwise force calculation for lj
double ljCalculator::getLennardJonesForce(double distance) {
  return -4 * epsilon *
         ((-12 * pow(sigma / distance, 13)) - (-6 * pow(sigma / distance, 7)));
}

//////////////////////////////// pyamff ////////////////////////////////////
// Force and Potential energy calculation function for the pyamff calculator
vector<vector<double>> pyamffCalculator::getFandU(std::vector<Atom*>& spheres) {

  double pyamffF[pyamffN * 3];
  double pyamffU;
  double* pyamffUPtr = &pyamffU;
  // Prepare positions so they may be passed
  double atomArray [spheres.size() * 3];
  for (int i = 0; i < spheres.size() * 3; i+=3){
    cVector3d pos = spheres[i/3]->getLocalPos();
    atomArray[i] = pos.x()/.02 + 50;
    atomArray[i+1] = pos.y()/.02 + 50;
    atomArray[i+2] = pos.z()/.02 + 50;
  }

  // Make a copy of the vector
  int unique[num_elements];
  copy(unique_atomicNrs.begin(), unique_atomicNrs.end(), unique);
  calc_eon(&pyamffN, atomArray, box, atomicNumbers, pyamffF, pyamffUPtr, &num_elements, unique);

  // Convert to array for return
  vector<vector<double>> returnVector = {};
  for (int i = 0; i < spheres.size() * 3; i+=3) {
    vector<double> pushBack = {*(pyamffF + i), *(pyamffF + i + 1), *(pyamffF + i + 2)};
    returnVector.push_back(pushBack);
  }

  returnVector.push_back({*pyamffUPtr});
  return returnVector;
}

////////////////////////////////// ase //////////////////////////////////////
// ase calculator constructor
aseCalculator::aseCalculator(std::string& cName, int* atomicNrs, const double* b){
  calculatorString = cName;
  atomicNumbers = atomicNrs;
  box = b;

  // Start python instance and add current directory to path
  Py_Initialize();
  PyRun_SimpleString("import sys\n"
                    "import ase.calculators.lj\n"  // racking my brain. calculator.py has trouble finding this module on some systems
                    "sys.path.append('../../haptic-device/')");
}

// Force and Potential energy calculation function for the ase calculator
std::vector<std::vector<double>> aseCalculator::getFandU(std::vector<Atom*>& spheres){
  // Prepare positions so they may be passed to python
  double atomArray [spheres.size() * 3];
  for (int i = 0; i < spheres.size() * 3; i+=3){
    cVector3d pos = spheres[i/3]->getLocalPos();
    atomArray[i] = pos.x()/.02 + 50.0;
    atomArray[i+1] = pos.y()/.02 + 50.0;
    atomArray[i+2] = pos.z()/.02 + 50.0;
  }

  PyObject *pName, *pModule, *pFunc;
  PyObject *pValue, *pTuple, *pResult, *pFinal;

  pName = PyUnicode_FromString("calculator");

  pModule = PyImport_Import(pName);
  if (pModule == nullptr) {
    PyErr_Print();
    std::exit(1);
  }

  Py_DECREF(pName);

  if (pModule != NULL) {
    pFunc = PyObject_GetAttrString(pModule, "getValues");
    /* pFunc is a new reference */
    if (pFunc && PyCallable_Check(pFunc)) {
      pResult = PyTuple_New(spheres.size() * 3);
      for (int i = 0; i < spheres.size() * 3; ++i) {
        pValue = PyFloat_FromDouble(atomArray[i]);
        if (!pValue) {
          Py_DECREF(pResult);
          Py_DECREF(pModule);
          fprintf(stderr, "Cannot convert argument\n");
                //return 1;
        }
            // pValue reference stolen here:
        PyTuple_SetItem(pResult, i, pValue);
      }
      //Create tuple to put pArgs inside of -- Becaue we need to pass one object to python

      pTuple = PyTuple_New(1);
      PyTuple_SetItem(pTuple, 0, pResult);
      //pFinal = PyTuple_New(spheres.size() * 3);
      pFinal = PyObject_CallObject(pFunc, pTuple);
      if (pTuple != NULL){
        Py_DECREF(pTuple);
      }
      if (pFinal != NULL) {
        vector<vector<double>> forceArr;
        for (int j = 0; j < spheres.size()*3; j+=3){
          vector<double> temp;
          temp.push_back(PyFloat_AsDouble(PyList_GetItem(pFinal,j)));
          temp.push_back(PyFloat_AsDouble(PyList_GetItem(pFinal,j + 1)));
          temp.push_back(PyFloat_AsDouble(PyList_GetItem(pFinal,j + 2)));
          forceArr.push_back(temp);
        }
        // For the Potential Energy
        vector<double> temp;
        temp.push_back(PyFloat_AsDouble(PyList_GetItem(pFinal, spheres.size() * 3)));
        forceArr.push_back(temp);

        return forceArr;
        Py_DECREF(pValue);
        Py_DECREF(pResult);
        Py_DECREF(pFinal);
      }
      else {
        Py_DECREF(pFunc);
        Py_DECREF(pModule);
        PyErr_Print();
        fprintf(stderr,"Call failed\n");
        //return 1;
      }
    }
  }
}
