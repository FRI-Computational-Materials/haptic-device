#include "atom.h"
#include "chai3d.h"
#include "math.h"
#include <unordered_map>
#include <tuple>
#include <GLFW/glfw3.h>

using namespace std;
using namespace chai3d;

// map of atom stringnames by atomic number
std::unordered_map<int, std::string> atomStringNames({
    {1, "H"},
    {2, "He"},
    {3, "Li"},
    {4, "Be"},
    {5, "B"},
    {6, "C"},
    {7, "N"},
    {8, "O"},
    {9, "F"},
    {10, "Ne"},
    {11, "Na"},
    {12, "Mg"},
    {13, "Al"},
    {14, "Si"},
    {15, "P"},
    {16, "S"},
    {17, "Cl"},
    {18, "Ar"},
    {19, "K"},
    {20, "Ca"},
    {21, "Sc"},
    {22, "Ti"},
    {23, "V"},
    {24, "Cr"},
    {25, "Mn"},
    {26, "Fe"},
    {27, "Co"},
    {28, "Ni"},
    {29, "Cu"},
    {30, "Zn"},
    {31, "Ga"},
    {32, "Ge"},
    {33, "As"},
    {34, "Se"},
    {35, "Br"},
    {36, "Kr"},
    {37, "Rb"},
    {38, "Sr"},
    {39, "Y"},
    {40, "Zr"},
    {41, "Nb"},
    {42, "Mo"},
    {43, "Tc"},
    {44, "Ru"},
    {45, "Rh"},
    {46, "Pd"},
    {47, "Ag"},
    {48, "Cd"},
    {49, "In"},
    {50, "Sn"},
    {51, "Sb"},
    {52, "Te"},
    {53, "I"},
    {54, "Xe"},
    {55, "Cs"},
    {56, "Ba"},
    {57, "La"},
    {58, "Ce"},
    {59, "Pr"},
    {60, "Nd"},
    {61, "Pm"},
    {62, "Sm"},
    {63, "Eu"},
    {64, "Gd"},
    {65, "Tb"},
    {66, "Dy"},
    {67, "Ho"},
    {68, "Er"},
    {69, "Tm"},
    {70, "Yb"},
    {71, "Lu"},
    {72, "Hf"},
    {73, "Ta"},
    {74, "W"},
    {75, "Re"},
    {76, "Os"},
    {77, "Ir"},
    {78, "Pt"},
    {79, "Au"},
    {80, "Hg"},
    {81, "Tl"},
    {82, "Pb"},
    {83, "Bi"},
    {84, "Po"},
    {85, "At"},
    {86, "Rn"},
    {87, "Fr"},
    {88, "Ra"},
    {89, "Ac"},
    {90, "Th"},
    {91, "Pa"},
    {92, "U"},
    {93, "Np"},
    {94, "Pu"},
    {95, "Am"},
    {96, "Cm"},
    {97, "Bk"},
    {98, "Cf"},
    {99, "Es"},
    {100, "Fm"},
    {101, "Md"},
    {102, "No"},
    {103, "Lr"},
    {104, "Rf"},
    {105, "Db"},
    {106, "Sg"},
    {107, "Bh"},
    {108, "Hs"},
    {109, "Mt"},
    {110, "Ds"}, 
    {111, "Rg"}, 
    {112, "Cn"}, 
    {113, "Nh"},
    {114, "Fl"},
    {115, "Mc"},
    {116, "Lv"},
    {117, "Ts"},
    {118, "Og"}
});


// map of atom weights by atomic number
std::unordered_map<int, double> atomWeights({
    {1, 1.007},
    {2, 4.002},
    {3, 6.941},
    {4, 9.012},
    {5, 10.811},
    {6, 12.011},
    {7, 14.007},
    {8, 15.999},
    {9, 18.998},
    {10, 20.18},
    {11, 22.99},
    {12, 24.305},
    {13, 26.982},
    {14, 28.086},
    {15, 30.974},
    {16, 32.065},
    {17, 35.453},
    {18, 39.948},
    {19, 39.098},
    {20, 40.078},
    {21, 44.956},
    {22, 47.867},
    {23, 50.942},
    {24, 51.996},
    {25, 54.938},
    {26, 55.845},
    {27, 58.933},
    {28, 58.693},
    {29, 63.546},
    {30, 65.38},
    {31, 69.723},
    {32, 72.64},
    {33, 74.922},
    {34, 78.96},
    {35, 79.904},
    {36, 83.798},
    {37, 85.468},
    {38, 87.62},
    {39, 88.906},
    {40, 91.224},
    {41, 92.906},
    {42, 95.96},
    {43, 98},
    {44, 101.07},
    {45, 102.906},
    {46, 106.42},
    {47, 107.868},
    {48, 112.411},
    {49, 114.818},
    {50, 118.71},
    {51, 121.76},
    {52, 127.6},
    {53, 126.904},
    {54, 131.293},
    {55, 132.905},
    {56, 137.327},
    {57, 138.905},
    {58, 140.116},
    {59, 140.908},
    {60, 144.242},
    {61, 145},
    {62, 150.36},
    {63, 151.964},
    {64, 157.25},
    {65, 158.925},
    {66, 162.5},
    {67, 164.93},
    {68, 167.259},
    {69, 168.934},
    {70, 173.054},
    {71, 174.967},
    {72, 178.49},
    {73, 180.948},
    {74, 183.84},
    {75, 186.207},
    {76, 190.23},
    {77, 192.217},
    {78, 195.084},
    {79, 196.967},
    {80, 200.59},
    {81, 204.383},
    {82, 207.2},
    {83, 208.98},
    {84, 210},
    {85, 210},
    {86, 222},
    {87, 223},
    {88, 226},
    {89, 227},
    {90, 232.038},
    {91, 231.036},
    {92, 238.029},
    {93, 237},
    {94, 244},
    {95, 243},
    {96, 247},
    {97, 247},
    {98, 251},
    {99, 252},
    {100, 257},
    {101, 258},
    {102, 259},
    {103, 262},
    {104, 261},
    {105, 262},
    {106, 266},
    {107, 264},
    {108, 267},
    {109, 268},
    {110, 271},
    {111, 272},
    {112, 285},
    {113, 284},
    {114, 289},
    {115, 288},
    {116, 292},
    {117, 295},
    {118, 294}
});

// map for colors
std::unordered_map<int, std::tuple<const GLfloat, const GLfloat, const GLfloat>> atomColors({
    {6, {200, 200, 200}},
    {8, {240, 0, 0}},
    {1, {255, 255, 255}},
    {7, {143, 143, 255}},
    {16, {255, 200, 50}},
    {15, {255, 165, 0}},
    {17, {0, 255, 0}},
    {35, {165, 42, 42}},
    {30, {165, 42, 42}},
    {11, {0, 0, 255}},
    {26, {255, 165, 0}},
    {25, {42, 128, 42}},
    {20, {128, 128, 128}},
    {0, {255, 20, 147}},
    {79, {255,  215, 0}},
    {78, {200, 200, 200}}
});

Atom::Atom(double radius, int atomicNumber, cColorf color)
: cShapeSphere(radius) {
    anchor = false;
    current = false;
    repeating = false;
    velVector = new cShapeLine(cVector3d(0, 0, 0), cVector3d(0, 0, 0));
    force.zero();
    this->atomicNumber = atomicNumber;
    // note - cColorf defaults to white, as such
    // the default for atoms is also white (see the header file)
    base_color = color;
    
    // set the color
    m_material->setColor(color);
}

Atom::Atom(double radius, int atomicNumber) : cShapeSphere(radius) {
    anchor = false;
    current = false;
    repeating = false;
    velVector = new cShapeLine(cVector3d(0, 0, 0), cVector3d(0, 0, 0));
    force.zero();
    this->atomicNumber = atomicNumber;

    // check if atomic number has a registered color. If not, use 0 (magenta)
    std::tuple<const GLfloat, const GLfloat, const GLfloat> col = 
        (atomColors.find(atomicNumber) == atomColors.end()) ? atomColors[0] : atomColors[atomicNumber];

    base_color = cColorf();
    base_color.set(get<0>(col)/255, get<1>(col)/255, get<2>(col)/255);
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

void Atom::setColor(cColorf color) { m_material->setColor(color); }

int Atom::getAtomicNumber() { return atomicNumber; }

void Atom::setAtomicNumber( int num ) { atomicNumber = num; }

string Atom::getElement() {
    return atomStringNames[atomicNumber];
}

double Atom::getMass() {
    return atomWeights[atomicNumber];
}
