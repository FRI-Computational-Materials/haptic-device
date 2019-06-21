#include "math.h"
#include "potentials.h"

const double SIGMA = 1.0;
const double EPSILON = 1.0;
const double FORCE_DAMPING = .75;

double getLennardJonesEnergy(double distance) {
  return 4 * EPSILON * (pow(SIGMA / distance, 12) - pow(SIGMA / distance, 6));
}

double getLennardJonesForce(double distance) {
  return -4 * FORCE_DAMPING * EPSILON *
         ((-12 * pow(SIGMA / distance, 13)) - (-6 * pow(SIGMA / distance, 7)));
}
