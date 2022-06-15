#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include "chai3d.h"

using namespace std;
using namespace chai3d;

void  optimize(Atom *current,  double timeInterval, int curr_atom, cVector3d position, cLabel *potE_label, cLabel *num_anchored, bool flagHapticsEnabled,
                double global_minimum, bool global_min_known, cLabel *scope_upper, cLabel *scope_lower, cPrecisionClock clock);
#endif
