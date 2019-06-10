#include "atom.h"
#include "chai3d.h"

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
    m_material->setRed();
    anchor = false;
  }else if(anchor){
    m_material->setBlue();
  }else{
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
  force = newForce;
}

cShapeLine* Atom::getVelVector(){
  return velVector;
}

void Atom::setVelVector(cShapeLine *newVelVector){
  velVector = newVelVector;
}
