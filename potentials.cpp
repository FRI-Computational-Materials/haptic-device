#include "math.h"
#include "potentials.h"

const double SIGMA = 1.0;
const double EPSILON = 1.0;   // also known as the well depth
const double FORCE_DAMPING = .75;
const double EQUILIBRIUM_BOND_LENGTH = 1.0;
const double K_VALUE = 1.0;
const double ALPHA = 6.; //pow(K_VALUE / (2 * EPSILON), 1.0/2.0);

double getLennardJonesEnergy(double distance) {
  return 4 * EPSILON * (pow(SIGMA / distance, 12) - pow(SIGMA / distance, 6));
}

double getLennardJonesForce(double distance) {
  return -4 * FORCE_DAMPING * EPSILON *
         ((-12 * pow(SIGMA / distance, 13)) - (-6 * pow(SIGMA / distance, 7)));
}

double getMorseEnergy(double distance){
  return EPSILON * pow(1.0 - exp(-1. * ALPHA * (distance - EQUILIBRIUM_BOND_LENGTH)), 2) - EPSILON;
}

double getMorseForce(double distance){
  return -2 * FORCE_DAMPING * ALPHA * EPSILON * exp(ALPHA * (EQUILIBRIUM_BOND_LENGTH - distance)) * (1.0 - exp(-1 * ALPHA * (distance - EQUILIBRIUM_BOND_LENGTH)));
}
