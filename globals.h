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

// offset between the position of the mmouse click on the object and the object
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
