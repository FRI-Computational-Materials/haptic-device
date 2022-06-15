#ifndef POTENTIALS_H
#define POTENTIALS_H
#include <vector>
#include "atom.h"
#include "PyAMFF/PyAMFF.h"
#include <unordered_map>
//#include <algorithm>

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

//////// Calculator for ljCalculator
class ljCalculator:public Calculator {
  double distanceScale;
  unordered_map<string, pair<double, double>> epsandsig;

  public:
    ljCalculator() {
      distanceScale = .02;
      // format for new epsilon/sigma values:
      // epsandsig.insert(make_pair("ELEMENT PAIR IN ALPHABETICAL ORDER", make_pair(EPSILON, SIGMA)));
      /*
      epsandsig.insert(make_pair("AuAu", make_pair(0.4414, 2.637)));
      epsandsig.insert(make_pair("AuPt", make_pair(0.5485, 2.590))); // Graham's Sketchy Values
      epsandsig.insert(make_pair("PtPt", make_pair(0.6816, 2.542)));
      epsandsig.insert(make_pair("CH", make_pair(7.6950, 0.37607)));
      epsandsig.insert(make_pair("CC", make_pair(7.6950, 0.37607))); // Sydney's Sketchy Values
      epsandsig.insert(make_pair("HH", make_pair(7.6950, 0.37607)));
      */
      }
    vector<vector<double>> getFandU(vector<Atom*>& spheres);
    tuple<double, double> getEpsandSig(Atom* a1, Atom* a2);
};

////// Calculator for pyamff
class pyamffCalculator:public Calculator {
  PyAMFF PyAMFFCalculatorInstance;
  long pyamffN;
  int num_elements;
  int max_fps = 100;
  vector<int> unique_atomicNrs;
  // Some values that we'll need
  const double box[9] = {100, 0, 0, 0, 100, 0, 0, 0, 100};
  int atomicNumbers[500]; // I had to put a value to initialize the array; it doesn't matter what it is as long as greater than number of atoms
  public:
    pyamffCalculator(vector<Atom *> spheres){
      pyamffN = spheres.size();

      // initialize pyamff
      // stealing this from the PyAMFF.cpp with instruction from Ryan Ciufo
      for (int i = 0; i < pyamffN; i++) {
	atomicNumbers[i] = spheres[i]->getAtomicNumber();
	}
      sort(atomicNumbers, atomicNumbers+pyamffN);

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
      // make a copy of unique_atomicNrs (This is what the original code did. Maybe calc_eon pops the unique atomic nrs)
      int unique[num_elements];
      copy(unique_atomicNrs.begin(), unique_atomicNrs.end(), unique);
      // prepare the fingerprints and neural network
      read_mlffParas(&pyamffN, &num_elements, &max_fps, atomicNumbers, unique);
      prepfNN(&pyamffN, &num_elements, atomicNumbers, unique);
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
