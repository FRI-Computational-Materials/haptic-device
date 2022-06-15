#include "atom.h"
#include "chai3d.h"
#include "globals.h"
#include "inputHandling.h"
#include "potentials.h"
#include "utility.h"
#include "PyAMFF/PyAMFF.h"
#include "boundaryConditions.h"
#include "optimizer.h"
#include "defaultArg.h"
#include "python_instance.h"
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

void button_control(bool button1, bool button2, bool button1_changed, bool button2_changed,
                    int curr_camera, Atom *current, int curr_atom, Atom *previous, cVector3d position){
    // Changes the camera when button2 is pressed
    if (button2) {
      if (!button2_changed) {
        // rotates camera around in square
        switch (curr_camera) {
          case 1:
            camera->setSphericalPolarRad(0);
            camera->setSphericalAzimuthRad(0);
            updateCameraLabel(camera_pos, camera);
            break;
          case 2:
            camera->setSphericalPolarRad(0);
            camera->setSphericalAzimuthRad(M_PI);
            updateCameraLabel(camera_pos, camera);
            ;
            break;
          case 3:
            camera->setSphericalPolarRad(M_PI);
            camera->setSphericalAzimuthRad(M_PI);
            updateCameraLabel(camera_pos, camera);
            break;
          case 4:
            curr_camera = 0;
            camera->setSphericalPolarRad(M_PI);
            camera->setSphericalAzimuthRad(0);
            updateCameraLabel(camera_pos, camera);
            break;
        }
        curr_camera++;
        button2_changed = true;
      }
    } else {
      button2_changed = false;
      }
    if (button1) {
      if (!button1_changed) {
        // computes current atom by taking the remainder of the curr_atom +1 and
        // number of spheres
        int previous_curr_atom = curr_atom;
        curr_atom = remainder(curr_atom + 1, spheres.size());
        if (curr_atom < 0) {
          curr_atom = spheres.size() + curr_atom;
        }

        // Skip anchored atoms; will eventually terminate at previous_curr_atom
        while (spheres[curr_atom]->isAnchor()) {
          curr_atom = remainder(curr_atom + 1, spheres.size());
          if (curr_atom < 0) {
            curr_atom = spheres.size() + curr_atom;
          }
        }
        current = spheres[curr_atom];
        previous = spheres[previous_curr_atom];
        cVector3d A = current->getLocalPos();

        // Change attributes of previous current and new current
        previous->setCurrent(false);
        previous->setLocalPos(A);
        current->setCurrent(true);
        current->setLocalPos(position);
        cVector3d translate =
            (previous->getLocalPos()) - (current->getLocalPos());

        Atom *traverser;
        for (int i = 0; i < spheres.size(); i++) {
          traverser = spheres[i];
          if (i != curr_atom) {
            if (i == (previous_curr_atom)) {
              traverser->setLocalPos(previous->getLocalPos() -
                                     (2.0 * translate));
              cVector3d positions = traverser->getLocalPos();
            } else {
              traverser->setLocalPos(traverser->getLocalPos() - (translate));
              cVector3d positions = traverser->getLocalPos();
            }
          }
        }
        button1_changed = true;
      }
    } else {
      button1_changed = false;
    }
}
