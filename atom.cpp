#include "atom.h"
#include "chai3d.h"
#include "math.h"

using namespace std;
using namespace chai3d;

Atom::Atom(double radius) : cShapeSphere(radius){
  anchor = false;
  current = false;
  velVector = new cShapeLine(cVector3d(0, 0, 0), cVector3d(0, 0, 0));
  force.zero();
  m_material->setWhite();
}

bool Atom::isAnchor(){
  return anchor;
}

void Atom::setAnchor(bool newAnchor){
    if(newAnchor){
      //setting atom to be an anchor, so change color to blue
      m_material->setBlue();
      current = false;
    }else{
      //removing atom as anchor, so change color to white
      m_material->setWhite();
    }
    anchor = newAnchor;
}

bool Atom::isCurrent(){
  return current;
}

void Atom::setCurrent(bool newCurrent){
  if(newCurrent){
    //setting atom to be current, so change color to red
    m_material->setRed();
    anchor = false;  //cannot be both anchor and current
  }else if(anchor){
    m_material->setBlue();
  }else{
    //toggling current off, so set to white
    m_material->setWhite();
  }
  current = newCurrent;
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
  force = newForce;				// Add exception for if controlled atom is in the same location as the anchored atom

}

cShapeLine* Atom::getVelVector(){
  return velVector;
}

void Atom::setVelVector(cShapeLine *newVelVector){
  velVector = newVelVector;
}

void Atom::setInitialPosition(){
  double phi = rand() / double(RAND_MAX) * 2 * M_PI;
  double costheta = rand() / double(RAND_MAX) * 2 - 1;
  double u = rand() / double(RAND_MAX);
  double theta = acos(costheta);
  double r = 0.1 * cbrt(u);
  setLocalPos(r * sin(theta) * cos(phi), r * sin(theta) * sin(phi), r * cos(theta));
  cout << " x " << r * sin(theta) * cos(phi) << " y " << r * sin(theta) * sin(phi) << " z " << r * cos(theta) << endl;
}
