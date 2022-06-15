#include "atom.h"
#include "boundaryConditions.h"
#include "chai3d.h"
#include "globals.h"
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
#include "optimizer.h"
using namespace chai3d;
using namespace std;

void  optimize(Atom *current, double timeInterval, int curr_atom, cVector3d position, cLabel *potE_label, cLabel *num_anchored, bool flagHapticsEnabled,
		double global_minimum, bool global_min_known, cLabel *scope_upper, cLabel *scope_lower, cPrecisionClock clock){
  // compute forces for all spheres
  double potentialEnergy = 0;

  // This section of code (1108-1138) used to be a series of loops dependent on the calculator type
  vector<vector<double>> forcesVec = calculatorPtr->getFandU(spheres);
  potentialEnergy += forcesVec[spheres.size()][0];
  for (int i = 0; i < spheres.size(); i++) {
    cVector3d force = cVector3d(forcesVec[i][0], forcesVec[i][1], forcesVec[i][2]);
    current = spheres[i];
    cVector3d pos0 = current->getLocalPos();
    current->setForce(force);
    cVector3d sphereAcc = (force / (current->getMass() * SPHERE_MASS_SCALE_FACTOR));
    current->setVelocity(V_DAMPING * (current->getVelocity() + timeInterval * sphereAcc));
    // compute /position
    cVector3d spherePos_change = timeInterval * current->getVelocity() + cSqr(timeInterval) * sphereAcc;
    double magnitude = spherePos_change.length();
    cVector3d spherePos = current->getLocalPos() + spherePos_change;
    if (magnitude > 5) {
      cout << i << " velocity " << current->getVelocity().length() << endl;
      cout << i << " force " << force.length() << endl;
      cout << i << " acceleration " << sphereAcc.length() << endl;
      cout << i << " time " << timeInterval << endl;
      cout << i << " position of  " << timeInterval << endl;
      }

    // A is the current position, B is the postion to move to. Used for checking bounds
    cVector3d A = current->getLocalPos();
    cVector3d B = spherePos;

    // apply david boundary conditions
    applyDavidBoundaryConditions(A, B);

    // apply sean boundary conditions
    applySeanBoundaryConditions(A, B, spherePos, northPlanePos, northPlaneNorm, southPlanePos, southPlaneNorm,
                                eastPlanePos, eastPlaneNorm, westPlanePos, westPlaneNorm, forwardPlanePos,
                                forwardPlaneNorm, backPlanePos, backPlaneNorm, BOUNDARY_LIMIT);

    // set position to new position if not controlled or anchored
    if (!current->isCurrent()) {
      if (!current->isAnchor()) {
         current->setLocalPos(spherePos);
         }
      }
    }
  if (!checkBounds(current->getLocalPos(), BOUNDARY_LIMIT)) {
    cout << "ATOM OUT OF BOUNDS";
    }

  if (flagHapticsEnabled) {
    current = spheres[curr_atom];
    current->setLocalPos(position);
     }

  // rescale helpPanel
  helpPanel->setLocalPos(width - 550, height - 530);
  helpHeader->setLocalPos(width - 490, height - 70);

  // rescale hotkey labels
  for (int i = 0; i < hotkeyKeys.size(); i++) {
    cLabel *tempKeyLabel = hotkeyKeys[i];
    cLabel *tempFuncLabel = hotkeyFunctions[i];
    tempKeyLabel->setLocalPos(width - 540, height - 130 - i * 25);
    tempFuncLabel->setLocalPos(width - 350, height - 130 - i * 25);
    }

  // JD: moved this out of nested for loop so that test is set only when
  // fully calculated update haptic and graphic rate data
  potE_label->setText("Potential Energy: " + cStr(potentialEnergy, 5));
  // update position of label
  potE_label->setLocalPos(0, 15, 0);
  // count the number of anchored atoms
  auto anchored{0};
  for (auto i{0}; i < spheres.size(); i++) {
    if (spheres[i]->isAnchor()) {
       anchored++;
       }
    }
  num_anchored->setText(to_string(anchored) + " anchored / " + to_string(spheres.size()) + " total");
  auto num_anchored_width = (width - num_anchored->getWidth()) - 5;
  num_anchored->setLocalPos(num_anchored_width, 0);

  // Update scope
  double currentTime = clock.getCurrentTimeSeconds();

  // rounds current time to the nearest tenth
  double currentTimeRounded = double(int(currentTime * 10 + .5)) / 10;

  // The number fmod() is compared to is the threshold, this adjusts the
  // timescale
  if (fmod(currentTime, currentTimeRounded) <= .01) {
    scope->setSignalValues(potentialEnergy, global_minimum);
    }
  // scale the graph if the minimum isn't known
  if (!global_min_known) {
     if (potentialEnergy < global_minimum) {
        global_minimum = potentialEnergy;
        num_anchored->setText(to_string(anchored) + " anchored / " + to_string(spheres.size()) + " total");
        auto num_anchored_width = (width - num_anchored->getWidth()) - 5;
        num_anchored->setLocalPos(num_anchored_width, 0);

        // Update scope
        double currentTime = clock.getCurrentTimeSeconds();

        // rounds current time to the nearest tenth
        double currentTimeRounded = double(int(currentTime * 10 + .5)) / 10;

        // The number fmod() is compared to is the threshold, this adjusts the
       	// timescale
       	if (fmod(currentTime, currentTimeRounded) <= .01) {
           scope->setSignalValues(potentialEnergy, global_minimum);
           }
        // scale the graph if the minimum isn't known
        if (!global_min_known) {
           if (potentialEnergy < global_minimum) {
              global_minimum = potentialEnergy;
              }
           if (global_minimum < scope->getRangeMin()) {
             auto new_lower = scope->getRangeMin() - 25;
             auto new_upper = scope->getRangeMax() - 25;
             scope->setRange(new_lower, new_upper);
             scope_upper->setText(cStr(scope->getRangeMax()));
             scope_lower->setText(cStr(scope->getRangeMin()));
             }
        }
    }
  }
}
