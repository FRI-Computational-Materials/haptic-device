#ifndef ATOM_H
#define ATOM_H

#include "chai3d.h"

using namespace std;
using namespace chai3d;

class Atom : public cShapeSphere {
private:
    bool anchor;
    bool current;
    bool repeating;
    cVector3d velocity;
    cVector3d force;
    cShapeLine *velVector;
    int atomicNumber;
    cColorf base_color;


public:
    Atom(double radius, int atomicNumber, cColorf color);
    Atom(double radius, int atomicNumber);
    bool isAnchor();
    void setAnchor(bool newAnchor);
    bool isRepeating();
    void setRepeating(bool newRepeat);
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
    void setColor(cColorf color);
    int getAtomicNumber();
    void setAtomicNumber(int num);
    string getElement();
    double getMass();
};

#endif  // ATOM_H
