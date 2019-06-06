#include "atom.h"
#include "chai3d.h"

using namespace std;
using namespace chai3d;

Atom::Atom(double radius) : cShapeSphere(double radius){
        anchor = false;
        velVector = new cShapeLine(cVector3d(0, 0, 0), cVector3d(0, 0, 0));
        force.zero();
}
