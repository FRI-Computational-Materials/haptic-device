#ifndef POTENTIALS_H
#define POTENTIALS_H
#include <vector>
#include "atom.h"
#include "PyAMFF/PyAMFF.h"

////// Base class for all calculators
class Calculator {
  protected:
    int* atomicNumbers;
    const double* box;
  public:
    // Note: any other values used in this function should be private variables
    // For good design, anything that is expected to change frequently should be included in the function call. In our case, that is only positions.
    // You could get a nasty unreported error by relying too much on these private parameters
    virtual std::vector<std::vector<double>> getFandU(std::vector<Atom*>& spheres) = 0;
};

////// Calculator for the morse potential
class morseCalculator:public Calculator {
  double rho0;
  double r0;
  double epsilon;
  double forceDamping;
  double distanceScale;
  public:
    morseCalculator(double rho=6.0, double r=1.0, double e=1.0, double fd=1.0, double ds=.02) {
      rho0 = rho;
      r0 = r;
      epsilon = e;
      forceDamping = fd;
      distanceScale = ds;
    }
    vector<vector<double>> getFandU(vector<Atom*>& spheres);
    double getMorseEnergy(double distance);
    double getMorseForce(double distance);
};

////// Calculator for the LJ potential
class ljCalculator:public Calculator {
  double sigma;
  double epsilon;
  double distanceScale;
  public:
    ljCalculator(double s=1.0, double e=1.0, double ds=.02) {
      sigma = s;
      epsilon = e;
      distanceScale = ds;
    }
    vector<vector<double>> getFandU(vector<Atom*>& spheres);
    double getLennardJonesEnergy(double distance);
    double getLennardJonesForce(double distance);
};

////// Calculator for pyamff
class pyamffCalculator:public Calculator {
  PyAMFF PyAMFFCalculatorInstance;
  long pyamffN;
  int num_elements;
  int max_fps = 100;
  vector<int> unique_atomicNrs;
  // Some values that we'll need
  const int atomicNumbers[5] = {1, 1, 46, 46, 46};
  const double box[3] = {100, 100, 100};
  public:
    pyamffCalculator(long nAtoms){
      pyamffN = nAtoms;

      // initialize pyamff
      // stealing this from the PyAMFF.cpp with instruction from Ryan Ciufo
      // Make a vector of the unique atomic numbers present
      for (int i = 0; i < pyamffN; i++) {
        int j;
        for (j = 0; j < i; j++) {
          if (atomicNumbers[i] == atomicNumbers[j]) {
            break;
          }
        }
        if (i == j) {
          unique_atomicNrs.push_back(atomicNumbers[i]);
        }
      }

      // get the number of elements
      num_elements = unique_atomicNrs.size();
      std::cout << num_elements << endl;
      // make a copy of unique_atomicNrs (This is what the original code did. Maybe calc_eon pops the unique atomic nrs)
      int unique[num_elements];
      copy(unique_atomicNrs.begin(), unique_atomicNrs.end(), unique);
      // prepare the fingerprints and neural network
      read_mlffParas(&pyamffN, &num_elements, &max_fps, atomicNumbers, unique);
      prepfNN(&pyamffN, &num_elements, &max_fps, atomicNumbers, unique);
    }
    vector<vector<double>> getFandU(std::vector<Atom*>& spheres);
};

////// Calculator for ASE
class aseCalculator:public Calculator {
  std::string importString; // This will be evaluated to import the proper calculator module in calculator.py
  std::string calculatorString; // This will be evaluated for the calc = calcname(parameters) line in calculator.py
  public:
    aseCalculator(std::string& cName, int* atomicNrs, const double* b); // in cpp because of Python.h dependency
    // Get forces and potential energy. Does not support anything other than positions right now
    std::vector<std::vector<double>> getFandU(std::vector<Atom*>& spheres);
};

#endif
