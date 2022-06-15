extern int just_unanchored;
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// GENERAL SETTINGS
//------------------------------------------------------------------------------
// Stereo Mode
/*
 C_STEREO_DISABLED:            Stereo is disabled
 C_STEREO_ACTIVE:              Active stereo for OpenGL NVIDIA QUADRO
 cards C_STEREO_PASSIVE_LEFT_RIGHT:  Passive stereo where L/R images are
 rendered next to each other C_STEREO_PASSIVE_TOP_BOTTOM:  Passive stereo
 where L/R images are rendered above each other
 */

cStereoMode stereoMode = C_STEREO_DISABLED;

// Fullscreen mode
bool fullscreen = false;

//------------------------------------------------------------------------------
// DECLARED VARIABLES
//------------------------------------------------------------------------------
// calculator
Calculator* calculatorPtr;

// radius of the camera
double rho = .35;

// a world that contains all objects of the virtual environment
cWorld *world;

// a camera to render the world in the window display
cCamera *camera;

// a light source to illuminate the objects in the world
cSpotLight *light;

// a haptic device handler
cHapticDeviceHandler *handler;

// a pointer to the current haptic device
cGenericHapticDevicePtr hapticDevice;

// highest stiffness the current haptic device can render
double hapticDeviceMaxStiffness;

// sphere objects
vector<Atom *> spheres;

// a colored background
cBackground *background;

// a label to display the rate [Hz] at which the simulation is running
cLabel *labelRates;

// a label to show the potential energy
cLabel *potE_label;

// label showing the # anchored
cLabel *num_anchored;

// a label to display the total energy of the system
cLabel *total_energy;

// a label to show whether or not the atoms are frozen
cLabel *isFrozen;

// a label to display the camera position
cLabel *camera_pos;

// a label to identify the potential energy surface
cLabel *potentialLabel;

// labels for the scope
cLabel *scope_upper;
cLabel *scope_lower;

// a flag that indicates if the haptic simulation is currently running
bool simulationRunning = false;

// a flag that indicates if the haptic simulation has terminated
bool simulationFinished = true;

// mouse state
MouseState mouseState = MOUSE_IDLE;

// a frequency counter to measure the simulation graphic rate
cFrequencyCounter freqCounterGraphics;

// a frequency counter to measure the simulation haptic rate
cFrequencyCounter freqCounterHaptics;

// haptic thread
cThread *hapticsThread;

// a handle to window display context
GLFWwindow *window = NULL;

// current width of window
int width = 0;

// current height of window
int height = 0;

// swap interval for the display context (vertical synchronization)
int swapInterval = 1;

// root resource path
string resourceRoot;

// a scope to monitor the potential energy
cScope *scope;

// global minimum for the given cluster size
double global_minimum;

// a pointer to the selected object
Atom *selectedAtom = NULL;

// offset between the position of the mouse click on the object and the object
// reference frame location.
cVector3d selectedAtomOffset;

// position of mouse click.
cVector3d selectedPoint;

// determine if atoms should be frozen
bool freezeAtoms = false;

// save coordinates of central atom
double centerCoords[3] = {50.0, 50.0, 50.0};

// default potential is Lennard Jones
LocalPotential energySurface = LENNARD_JONES; //Changed from lennard jones but eventually this will become lennard jones

// check if able to read in the global min
bool global_min_known = true;

// panel that displays hotkeys
cPanel *helpPanel;

// help panel header
cLabel *helpHeader;

// vector holding hotkey key labels
vector<cLabel *> hotkeyKeys;

// vector holding function key labels (must be separate for formatting)
vector<cLabel *> hotkeyFunctions;

// keep track of how long screenshot label has been displayed
int screenshotCounter = -2;

// keep track of how long write to con label has been displayed
int writeConCounter = -2;

// screenshot notification label
cLabel *screenshotLabel;

// write to con notification label
cLabel *writeConLabel;

// callback when the window display is resized
void windowSizeCallback(GLFWwindow *a_window, int a_width, int a_height);

// callback when an error GLFW occurs
void errorCallback(int error, const char *a_description);

// this function updates the draw positions
void updateGraphics(void);

// this function contains the main haptics simulation loop
void updateHaptics(void);

// this function closes the application
void close(void);

// add a label to the world with default black text
void addLabel(cLabel *&label);

// Update camera text
void updateCameraLabel(cLabel *&camera_pos, cCamera *&camera);

// save configuration in .con file
void writeToCon(string fileName);

//------------------------------------------------------------------------------
// DECLARED MACROS
//------------------------------------------------------------------------------
// Convert to resource path
#define RESOURCE_PATH(p) (char *)((resourceRoot + string(p)).c_str())
//==============================================================================
/*
 main.cpp
 This program simulates clusters of varying sizes using modified
 sphere primitives (atom.cpp). All dynamics and collisions are computed in the
 haptics thread.
 */
//==============================================================================
// current camera
int curr_camera = 1;