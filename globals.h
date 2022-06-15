#include "chai3d.h"
#include "atom.h"
#include <vector>
#include <GLFW/glfw3.h>
#include "potentials.h"
#include <unordered_map>
#include <tuple>

//------------------------------------------------------------------------------
// STATES
//------------------------------------------------------------------------------
enum MouseState { MOUSE_IDLE, MOUSE_SELECTION };
enum LocalPotential { LENNARD_JONES, MORSE, PYAMFF, ASE };

// map of atom stringnames by atomic number
extern std::unordered_map<int, std::string> atomStringNames;

// map of atom weights by atomic number
extern std::unordered_map<int, double> atomWeights;

// map of atom colors by atomic number
extern std::unordered_map<int, std::tuple<const GLfloat, const GLfloat, const GLfloat>> atomColors;

// Calculator object for force and potential energy calculatorString
extern Calculator* calculatorPtr;

// vector holding hotkey key labels
extern std::vector<chai3d::cLabel *> hotkeyKeys;

//vector holding function key labels (must be separate for formatting)
extern std::vector<chai3d::cLabel *> hotkeyFunctions;

// a camera to render the world in the window display
extern chai3d::cCamera *camera;

//for updating camera position
extern double rho;

// sphere objects
extern std::vector<Atom *> spheres;

// coordinates of central atom
extern double centerCoords[3];

// a pointer to the selected object
extern Atom *selectedAtom;

// mouse state
extern MouseState mouseState;

// position of mouse click.
extern cVector3d selectedPoint;

// current width of window
extern int width;

// current height of window
extern int height;

// offset between the position of the mouse click on the object and the object
// reference frame location.
extern cVector3d selectedAtomOffset;

extern GLFWwindow *window;

extern bool fullscreen;

extern int swapInterval;

extern cScope *scope;

extern bool freezeAtoms;

extern cLabel *camera_pos;

extern cLabel *helpHeader;

extern cPanel *helpPanel;

extern int screenshotCounter;

extern int writeConCounter;

//------------------------------------------------------------------------------
// DECLARED CONSTANTS
//------------------------------------------------------------------------------
// Radius of each sphere
const double SPHERE_RADIUS = 0.008;
// number of cameras
const int NUM_CAM = 2;

// position of walls and ground
const double WALL_GROUND = 0.0 + SPHERE_RADIUS;
const double WALL_CEILING = 0.05;  // 0.2;
const double WALL_LEFT = -0.05;    //-0.1;
const double WALL_RIGHT = 0.05;    // 0.2;
const double WALL_FRONT = 0.05;    // 0.08;
const double WALL_BACK = -0.05;    //-0.08;
const double SPHERE_STIFFNESS = 500.0;
const double SPHERE_MASS_SCALE_FACTOR = 0.02;
const double F_DAMPING = 0.25;  // 0.25
const double V_DAMPING = 0.8;   // 0.1
const double A_DAMPING = 0.99;  // 0.5
const double K_MAGNET = 500.0;
const double HAPTIC_STIFFNESS = 1000.0;
const double SIGMA = 1.0;
const double EPSILON = 1.0;

// Scales the distance betweens atoms
const double DIST_SCALE = .02;

// boundary conditions
const double BOUNDARY_LIMIT = .5;
const cVector3d northPlanePos = cVector3d(0, BOUNDARY_LIMIT, 0);
const cVector3d northPlaneP1 = cVector3d(1, BOUNDARY_LIMIT, 0);
const cVector3d northPlaneP2 = cVector3d(1, BOUNDARY_LIMIT, 1);
const cVector3d northPlaneNorm =
    cComputeSurfaceNormal(northPlanePos, northPlaneP1, northPlaneP2);
const cVector3d southPlanePos = cVector3d(0, -BOUNDARY_LIMIT, 0);
const cVector3d southPlaneP1 = cVector3d(1, -BOUNDARY_LIMIT, 0);
const cVector3d southPlaneP2 = cVector3d(1, -BOUNDARY_LIMIT, 1);
const cVector3d southPlaneNorm =
    cComputeSurfaceNormal(southPlanePos, southPlaneP1, southPlaneP2);
const cVector3d eastPlanePos = cVector3d(BOUNDARY_LIMIT, 0, 0);
const cVector3d eastPlaneP1 = cVector3d(BOUNDARY_LIMIT, 1, 0);
const cVector3d eastPlaneP2 = cVector3d(BOUNDARY_LIMIT, 1, 1);
const cVector3d eastPlaneNorm =
    cComputeSurfaceNormal(eastPlanePos, eastPlaneP1, eastPlaneP2);
const cVector3d westPlanePos = cVector3d(-BOUNDARY_LIMIT, 0, 0);
const cVector3d westPlaneP1 = cVector3d(-BOUNDARY_LIMIT, 1, 0);
const cVector3d westPlaneP2 = cVector3d(-BOUNDARY_LIMIT, 1, 1);
const cVector3d westPlaneNorm =
    cComputeSurfaceNormal(westPlanePos, westPlaneP1, westPlaneP2);
const cVector3d forwardPlanePos = cVector3d(0, 0, BOUNDARY_LIMIT);
const cVector3d forwardPlaneP1 = cVector3d(0, 1, BOUNDARY_LIMIT);
const cVector3d forwardPlaneP2 = cVector3d(1, 1, BOUNDARY_LIMIT);
const cVector3d forwardPlaneNorm =
    cComputeSurfaceNormal(forwardPlanePos, forwardPlaneP1, forwardPlaneP2);
const cVector3d backPlanePos = cVector3d(0, 0, -BOUNDARY_LIMIT);
const cVector3d backPlaneP1 = cVector3d(0, 1, -BOUNDARY_LIMIT);
const cVector3d backPlaneP2 = cVector3d(1, 1, -BOUNDARY_LIMIT);
const cVector3d backPlaneNorm =
    cComputeSurfaceNormal(backPlanePos, backPlaneP1, backPlaneP2);

