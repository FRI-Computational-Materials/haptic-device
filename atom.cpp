#include "atom.h"
#include "chai3d.h"
#include "math.h"

using namespace std;
using namespace chai3d;

Atom::Atom(double radius, double sphere_mass, cColorf color)
: cShapeSphere(radius) {
    anchor = false;
    current = false;
    repeating = false;
    notCalculated = false;
    copynumber = -1;
    xPos = 0;
    yPos = 0;
    zPos = 0;
    velVector = new cShapeLine(cVector3d(0, 0, 0), cVector3d(0, 0, 0));
    force.zero();
    this->sphere_mass = sphere_mass;
    // note - cColorf defaults to white, as such
    // the default for atoms is also white (see the header file)
    base_color = color;
    m_material->setColor(base_color);
}


bool Atom::isAnchor() { return anchor; }

void Atom::setAnchor(bool newAnchor) {
    if (newAnchor) {
        // setting atom to be an anchor, so change color to blue
        m_material->setBlue();
        current = false;
    } else {
        // removing atom as anchor, so change color to white
        m_material->setColor(base_color);
    }
    anchor = newAnchor;
}

bool Atom::isCurrent() { return current; }

void Atom::setCurrent(bool newCurrent) {
    if (newCurrent) {
        // setting atom to be current, so change color to red
        m_material->setRed();
        anchor = false;  // cannot be both anchor and current
    } else if (anchor) {
        m_material->setBlue();
    } else {
        // toggling current off, so set to white
        m_material->setColor(base_color);
    }
    current = newCurrent;
}

bool Atom::isRepeating() { return repeating; }

void Atom::setRepeating(bool newRepeat) {
    if (newRepeat) {
        // setting atom to be repeating, changing color to black
        m_material->setBlack();
        anchor = false;
    } else {
        // toggling off, so set to white
        m_material->setColor(base_color);
    }
    repeating = newRepeat;
}

bool Atom::isNotCalculated() { return notCalculated; }

void Atom::setNotCalculated(bool newNotCalculated) {
  if (newNotCalculated) {
    notCalculated = true;
  }
  else {
    notCalculated = false;
  }
}

void Atom::setLatticePosition(int xPosition, int yPosition, int zPosition){
  xPos = xPosition;
  yPos = yPosition;
  zPos = zPosition;
}

double Atom::getLatticeX(){return xPos;}
double Atom::getLatticeY(){return yPos;}
double Atom::getLatticeZ(){return zPos;}

void Atom::setCopyNumber(int newNum){
  copynumber = newNum;
}

int Atom::getCopyNumber(){
  return copynumber;
}

cVector3d Atom::getVelocity() { return velocity; }

void Atom::setVelocity(cVector3d newVel) { velocity = newVel; }

cVector3d Atom::getForce() { return force; }

void Atom::setForce(cVector3d newForce) {
    force = newForce;  // Add exception for if controlled atom is in the same
    // location as the anchored atom
}

cShapeLine* Atom::getVelVector() { return velVector; }

void Atom::setVelVector(cShapeLine* newVelVector) { velVector = newVelVector; }

void Atom::updateVelVector() {
    // Create a line representing the forces felt on the atom
    cVector3d newPointNormalized = cAdd(this->getLocalPos(), this->getForce());
    this->getForce().normalizer(newPointNormalized);
    this->velVector->m_pointA =
    cAdd(this->getLocalPos(), newPointNormalized * this->getRadius());
    this->velVector->m_pointB =
    cAdd(this->getVelVector()->m_pointA, this->getForce() * .005);
    this->velVector->setLineWidth(5);

    // Update the color based on the current status of the atom
    if (current) {
        this->velVector->m_colorPointA.setRed();
        this->velVector->m_colorPointB.setRed();
    } else {
        this->velVector->m_colorPointA.setBlack();
        this->velVector->m_colorPointB.setBlack();
    }
}

void Atom::setInitialPosition(double spawn_dist) {
    double phi = rand() / double(RAND_MAX) * 2 * M_PI;
    double costheta = rand() / double(RAND_MAX) * 2 - 1;
    double u = rand() / double(RAND_MAX);
    double theta = acos(costheta);
    double r = spawn_dist * cbrt(u);
    setLocalPos(r * sin(theta) * cos(phi), r * sin(theta) * sin(phi),
                r * cos(theta));
}

double Atom::getMass() { return sphere_mass; }

void Atom::setColor(cColorf color) { m_material->setColor(color); }
