#include "chai3d.h"
#include "atom.h"
#include <vector>

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
