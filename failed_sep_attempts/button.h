#ifndef BUTTON_H
#define BUTTON_H

#include "chai3d.h"

using namespace std;
using namespace chai3d;

void  button_control(bool button1, bool button2, bool button1_changed, bool button2_changed, int curr_camera,
                    Atom *current, int curr_atom, Atom *previous, cVector3d position);
#endif
