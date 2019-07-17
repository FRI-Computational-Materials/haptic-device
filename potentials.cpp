#include "math.h"
#include "potentials.h"

const double SIGMA = 1.0;
const double EPSILON = 1.0;   // also known as the well depth
const double FORCE_DAMPING = .75;
const double K_VALUE = 1.0;
const double RHO0 = 6.0;
const double R0 = 1.0;

double getLennardJonesEnergy(double distance) {
  return 4 * EPSILON * (pow(SIGMA / distance, 12) - pow(SIGMA / distance, 6));
}

double getLennardJonesForce(double distance) {
  return -4 * FORCE_DAMPING * EPSILON *
         ((-12 * pow(SIGMA / distance, 13)) - (-6 * pow(SIGMA / distance, 7)));
}

//based off off ASE Morse calculator parameters and format; see ASE documentation
double getMorseEnergy(double distance){
  double expf = exp(RHO0 * (1.0 - distance / R0));
  return EPSILON * expf * (expf - 2);
}

//negative derivative of energy with respect to distance
double getMorseForce(double distance){
  double temp = -2 * RHO0 * EPSILON * exp(RHO0 - (2 * RHO0 * distance) / R0) * (exp((RHO0 * distance)/R0) - exp(RHO0));
  return temp / R0 * FORCE_DAMPING;
}
