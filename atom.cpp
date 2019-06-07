#include "atom.h"
#include "chai3d.h"

using namespace std;
using namespace chai3d;

Atom::Atom(double radius) : cShapeSphere(radius){
  anchor = false;
  velVector = new cShapeLine(cVector3d(0, 0, 0), cVector3d(0, 0, 0));
  force.zero();
}

bool Atom::isAnchor(){
  return anchor;
}

void Atom::setAnchor(bool newAnchor){
  anchor = newAnchor;
}

cVector3d Atom::getVelocity(){
  return velocity;
}

void Atom::setVelocity(cVector3d newVel){
  velocity = newVel;
}

cVector3d Atom::getForce(){
  return force;
}

void Atom::setForce(cVector3d newForce){
  force = newForce;
}

cShapeLine* Atom::getVelVector(){
  return velVector;
}

void Atom::setVelVector(cShapeLine *newVelVector){
  velVector = newVelVector;
}
