#ifndef ATOM_H
#define ATOM_H

#include "chai3d.h"

using namespace std;
using namespace chai3d;

class Atom : public cShapeSphere {
 private:
  bool anchor;
  bool current;
  cVector3d velocity;
  cVector3d force;
  cShapeLine *velVector;
  double sphere_mass;
  cColorf base_color;

 public:
  Atom(double radius, double sphere_mass, cColorf color = cColorf());
  bool isAnchor();
  void setAnchor(bool newAnchor);
  bool isCurrent();
  void setCurrent(bool newCurrent);
  cVector3d getVelocity();
  void setVelocity(cVector3d newVel);
  cVector3d getForce();
  void setForce(cVector3d newForce);
  cShapeLine *getVelVector();
  void setVelVector(cShapeLine *newVelVector);
  void updateVelVector();
  void setInitialPosition(double spawn_dist = .1);
  double getMass();
  void setColor(cColorf color);
};

#endif  // ATOM_H
