#include "math.h"
#include "potentials.h"

const double SIGMA = 1.0;
const double EPSILON = 1.0;
const double FORCE_DAMPING = .75;
const double EQUILIBRIUM_BOND_LENGTH = 1.2;
const double WELL_DEPTH = -999;
const double ALPHA = -999;

double getLennardJonesEnergy(double distance) {
  return 4 * EPSILON * (pow(SIGMA / distance, 12) - pow(SIGMA / distance, 6));
}

double getLennardJonesForce(double distance) {
  return -4 * FORCE_DAMPING * EPSILON *
         ((-12 * pow(SIGMA / distance, 13)) - (-6 * pow(SIGMA / distance, 7)));
}

double getMorseEnergy(double distance){
  return WELL_DEPTH * pow(1 - exp(-1 * ALPHA * (distance - EQUILIBRIUM_BOND_LENGTH)), 2);
}

double getMorseForce(double distance){
  return 0.;
}
