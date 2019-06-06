#include "chai3d.h"

using namespace std;
using namespace chai3d;

class Atom: public cShapeSphere{
  private:
    bool anchor;
    cVector3d velocity;
    cVector3d force;
    cShapeLine *velVector;

  public:
    Atom();

};
