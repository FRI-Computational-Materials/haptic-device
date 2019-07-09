//==============================================================================
/*
 Software License Agreement (BSD License)
 Copyright (c) 2003-2016, CHAI3D.
 (www.chai3d.org)
 
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:
 
 * Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 
 * Redistributions in binary form must reproduce the above
 copyright notice, this list of conditions and the following
 disclaimer in the documentation and/or other materials provided
 with the distribution.
 
 * Neither the name of CHAI3D nor the names of its contributors may
 be used to endorse or promote products derived from this software
 without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
 \author    <http://www.chai3d.org>
 \author    Francois Conti
 \version   3.2.0 $Rev: 1922 $
 */
//==============================================================================

//------------------------------------------------------------------------------
#include "atom.h"
#include "chai3d.h"
#include "potentials.h"
#include "utility.h"
//------------------------------------------------------------------------------
#include <GLFW/glfw3.h>
#include <math.h>
#include <sys/stat.h>
#include <unistd.h>
#include <chrono>
#include <ctime>
#include <fstream>
#include <string>

//------------------------------------------------------------------------------
using namespace chai3d;
using namespace std;
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
// STATES
//------------------------------------------------------------------------------
enum MouseState { MOUSE_IDLE, MOUSE_SELECTION };

enum Potential { LENNARD_JONES, MORSE, MACHINE_LEARNING };

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
const double SPHERE_MASS = 0.02;
const double V_DAMPING = 0.0001;  // 0.996;
const double K_MAGNET = 500.0;
const double HAPTIC_STIFFNESS = 1000.0;
// Scales the distance betweens atoms
const double DIST_SCALE = .02;

//boundary conditions
const double BOUNDARY_LIMIT = 1;

const cVector3d northPlanePos = cVector3d(0,BOUNDARY_LIMIT,0);
const cVector3d northPlaneP1 = cVector3d(1,BOUNDARY_LIMIT,0);
const cVector3d northPlaneP2 = cVector3d(1,BOUNDARY_LIMIT,1);
const cVector3d northPlaneNorm = cComputeSurfaceNormal(northPlanePos,northPlaneP1,northPlaneP2);

const cVector3d southPlanePos = cVector3d(0,-BOUNDARY_LIMIT,0);
const cVector3d southPlaneP1 = cVector3d(1,-BOUNDARY_LIMIT,0);
const cVector3d southPlaneP2 = cVector3d(1,-BOUNDARY_LIMIT,1);
const cVector3d southPlaneNorm = cComputeSurfaceNormal(southPlanePos,southPlaneP1,southPlaneP2);

const cVector3d eastPlanePos = cVector3d(BOUNDARY_LIMIT,0,0);
const cVector3d eastPlaneP1 = cVector3d(BOUNDARY_LIMIT,1,0);
const cVector3d eastPlaneP2 = cVector3d(BOUNDARY_LIMIT,1,1);
const cVector3d eastPlaneNorm = cComputeSurfaceNormal(eastPlanePos,eastPlaneP1,eastPlaneP2);

const cVector3d westPlanePos = cVector3d(-BOUNDARY_LIMIT,0,0);
const cVector3d westPlaneP1 = cVector3d(-BOUNDARY_LIMIT,1,0);
const cVector3d westPlaneP2 = cVector3d(-BOUNDARY_LIMIT,1,1);
const cVector3d westPlaneNorm = cComputeSurfaceNormal(westPlanePos,westPlaneP1,westPlaneP2);

const cVector3d forwardPlanePos = cVector3d(0,0,BOUNDARY_LIMIT);
const cVector3d forwardPlaneP1 = cVector3d(0,1,BOUNDARY_LIMIT);
const cVector3d forwardPlaneP2 = cVector3d(1,1,BOUNDARY_LIMIT);
const cVector3d forwardPlaneNorm = cComputeSurfaceNormal(forwardPlanePos,forwardPlaneP1,forwardPlaneP2);

const cVector3d backPlanePos = cVector3d(0,0,-BOUNDARY_LIMIT);
const cVector3d backPlaneP1 = cVector3d(0,1,-BOUNDARY_LIMIT);
const cVector3d backPlaneP2 = cVector3d(1,1,-BOUNDARY_LIMIT);
const cVector3d backPlaneNorm = cComputeSurfaceNormal(backPlanePos,backPlaneP1,backPlaneP2);


//------------------------------------------------------------------------------
// DECLARED VARIABLES
//------------------------------------------------------------------------------

// radius of the camera
double rho = .35;

// a world that contains all objects of the virtual environment
cWorld *world;

// a camera to render the world in the window display
cCamera *camera;

// a light source to illuminate the objects in the world
cSpotLight *light;

// points to the repeating atoms vector
vector< vector<Atom *> > repeats;

// box manipulation
bool changeBox = false;
cShapeBox *box;

// for choosing bcc/fcc miller indices
bool checkIndices = false;
int millerIndices[3];
int millIndCount = 0;
bool bcc = false;
bool fcc = false;

// cToolCursor* tool;

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

// a font for rendering text
cFontPtr font = NEW_CFONTCALIBRI20();

// a label to display the rate [Hz] at which the simulation is running
cLabel *labelRates;

// a label to show the potential energy
cLabel *LJ_num;

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

// offset between the position of the mmouse click on the object and the object
// reference frame location.
cVector3d selectedAtomOffset;

// position of mouse click.
cVector3d selectedPoint;

// determine if atoms should be frozen
bool freezeAtoms = false;

// save coordinates of central atom
double centerCoords[3] = {50.0, 50.0, 50.0};

// default potential is Lennard Jones
Potential energySurface = LENNARD_JONES;

// check if able to read in the global min
bool global_min_known = true;

//------------------------------------------------------------------------------
// DECLARED MACROS
//------------------------------------------------------------------------------
// convert to resource path
#define RESOURCE_PATH(p) (char *)((resourceRoot + string(p)).c_str())

//------------------------------------------------------------------------------
// DECLARED FUNCTIONS
//------------------------------------------------------------------------------

// callback when the window display is resized
void windowSizeCallback(GLFWwindow *a_window, int a_width, int a_height);

// callback when an error GLFW occurs
void errorCallback(int error, const char *a_description);

// callback when a key is pressed
void keyCallback(GLFWwindow *a_window, int a_key, int a_scancode, int a_action,
                 int a_mods);

// callback to handle mouse click
void mouseButtonCallback(GLFWwindow *a_window, int a_button, int a_action,
                         int a_mods);

// callback to handle mouse motion
void mouseMotionCallback(GLFWwindow *a_window, double a_posX, double a_posY);

// this function renders the scene
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
 LJ.cpp
 This program simulates LJ clusters of varying sizes using modified
 sphere primitives (atom.cpp). All dynamics and collisions are computed in the
 haptics thread.
 */
//==============================================================================
int curr_camera = 1;
double theta = 0;
double x = .5;
double y = .5;

int main(int argc, char *argv[]) {
    //--------------------------------------------------------------------------
    // INITIALIZATION
    //--------------------------------------------------------------------------
    cout << endl;
    cout << "-----------------------------------" << endl;
    cout << "CHAI3D" << endl;
    cout << "LJ.cpp" << endl;
    
    cout << "-----------------------------------" << endl << endl << endl;
    cout << endl << endl;
    
    // seed random
    srand(time(NULL));
    
    //--------------------------------------------------------------------------
    // OPEN GL - WINDOW DISPLAY
    //--------------------------------------------------------------------------
    
    // initialize GLFW library
    if (!glfwInit()) {
        cout << "failed initialization" << endl;
        cSleepMs(1000);
        return 1;
    }
    
    // set error callback
    glfwSetErrorCallback(errorCallback);
    
    // compute desired size of window
    const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    //    const w *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    int w = 0.8 * mode->height;
    int h = 0.5 * mode->height;
    int x = 0.5 * (mode->width - w);
    int y = 0.5 * (mode->height - h);
    
    // set OpenGL version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    
    // set active stereo mode
    if (stereoMode == C_STEREO_ACTIVE) {
        glfwWindowHint(GLFW_STEREO, GL_TRUE);
    } else {
        glfwWindowHint(GLFW_STEREO, GL_FALSE);
    }
    // create display context
    window = glfwCreateWindow(w, h, "CHAI3D", NULL, NULL);
    if (!window) {
        cout << "failed to create window" << endl;
        cSleepMs(1000);
        glfwTerminate();
        return 1;
    }
    
    // get width and height of window
    glfwGetWindowSize(window, &width, &height);
    
    // set position of window
    glfwSetWindowPos(window, x, y);
    
    // set key callback
    glfwSetKeyCallback(window, keyCallback);
    
    // set mouse position callback
    glfwSetCursorPosCallback(window, mouseMotionCallback);
    
    // set mouse button callback
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    
    // set resize callback
    glfwSetWindowSizeCallback(window, windowSizeCallback);
    
    // set current display context
    glfwMakeContextCurrent(window);
    
    // sets the swap interval for the current display context
    glfwSwapInterval(swapInterval);
    
#ifdef GLEW_VERSION
    // initialize GLEW library
    if (glewInit() != GLEW_OK) {
        cout << "failed to initialize GLEW library" << endl;
        glfwTerminate();
        return 1;
    }
#endif
    
    //--------------------------------------------------------------------------
    // WORLD - CAMERA - LIGHTING
    //--------------------------------------------------------------------------
    // create a new world.
    world = new cWorld();
    
    // set the background color of the environment
    world->m_backgroundColor.setWhite();
    
    // create a camera and insert it into the virtual world
    camera = new cCamera(world);
    world->addChild(camera);
    
    // creates the radius, origin reference, along with the zenith and azimuth
    // direction vectors
    cVector3d origin(0.0, 0.0, 0.0);
    cVector3d zenith(0.0, 0.0, 1.0);
    cVector3d azimuth(1.0, 0.0, 0.0);
    
    // sets the camera's references of the origin, zenith, and azimuth
    camera->setSphericalReferences(origin, zenith, azimuth);
    
    // sets the camera's position to have a radius of .1, located at 0 radians
    // (vertically and horizontally)
    camera->setSphericalRad(rho, 0, 0);
    
    // set the near and far clipping planes of the camera
    // anything in front or behind these clipping planes will not be rendered
    camera->setClippingPlanes(0.01, 10.0);
    
    // set stereo mode
    camera->setStereoMode(stereoMode);
    
    // set stereo eye separation and focal length (applies only if stereo is
    // enabled)
    camera->setStereoEyeSeparation(0.03);
    camera->setStereoFocalLength(1.8);
    
    // set vertical mirrored display mode
    camera->setMirrorVertical(false);
    
    // create a light source
    light = new cSpotLight(world);
    
    // attach light to camera
    world->addChild(light);
    
    // enable light source
    light->setEnabled(true);
    
    // position the light source
    light->setLocalPos(0.0, 0.3, 0.4);
    
    // define the direction of the light beam
    light->setDir(0.0, -0.25, -0.4);
    
    // enable this light source to generate shadows
    light->setShadowMapEnabled(false);
    
    // set the resolution of the shadow map
    light->m_shadowMap->setQualityHigh();
    
    // set shadow factor
    world->setShadowIntensity(0.3);
    
    // set light cone half angle
    light->setCutOffAngleDeg(30);
    
    //--------------------------------------------------------------------------
    // HAPTIC DEVICES / TOOLS
    //--------------------------------------------------------------------------
    
    // create a haptic device handler
    handler = new cHapticDeviceHandler();
    
    // get access to the first available haptic device
    handler->getDevice(hapticDevice, 0);
    
    // retrieve information about the current haptic device
    cHapticDeviceInfo hapticDeviceInfo = hapticDevice->getSpecifications();
    
    // retrieve the highest stiffness this device can render
    hapticDeviceMaxStiffness = hapticDeviceInfo.m_maxLinearStiffness;
    
    // if the haptic devices carries a gripper, enable it to behave like a user
    // switch
    hapticDevice->setEnableGripperUserSwitch(true);
    
    //--------------------------------------------------------------------------
    // CREATE SPHERES
    //--------------------------------------------------------------------------
    
    // create texture
    cTexture2dPtr texture = cTexture2d::create();
    
    // load texture file
    bool fileload = texture->loadFromFile(
                                          RESOURCE_PATH("../resources/images/spheremap-3.jpg"));
    if (!fileload) {
#if defined(_MSVC)
        fileload = texture->loadFromFile("../resources/images/spheremap-3.jpg");
#endif
    }
    if (!fileload) {
        cout << "Error - Texture image failed to load correctly." << endl;
        close();
        return (-1);
    }
    // either no arguments were given or argument was an integer
    if (argc == 1 || isNumber(argv[1])) {
        // set numSpheres to input; if none or negative, default is five
        int numSpheres = argc > 1 ? atoi(argv[1]) : 5;
        for (int i = 0; i < numSpheres; i++) {
            // create a sphere and define its radius
            Atom *new_atom = new Atom(SPHERE_RADIUS, SPHERE_MASS);
            
            // store pointer to sphere primitive
            spheres.push_back(new_atom);
            
            // add sphere primitive to world
            world->addChild(new_atom);
            
            // add line to world
            world->addChild(new_atom->getVelVector());
            
            // set the position of the object at the center of the world
            
            bool inside_atom = true;
            if (i != 0) {
                bool collision_detected;
                auto iter{0};
                while (inside_atom) {
                    // Place atom at a random position
                    if (iter > 1000) {
                        // If there are too many failed attempts at placing the atom
                        // increase the radius in which it can spawn
                        new_atom->setInitialPosition(.115);
                    } else {
                        new_atom->setInitialPosition();
                    }
                    // Check that it doesn't collide with any others
                    collision_detected = false;
                    for (auto i{0}; i < spheres.size(); i++) {
                        auto dist_between =
                        cDistance(new_atom->getLocalPos(), spheres[i]->getLocalPos());
                        dist_between = dist_between / .02;
                        if (dist_between == 0) {
                            continue;
                        } else if (dist_between < 1.5) {
                            // The number dist between is being compared to
                            // is the threshold for collision
                            collision_detected = true;
                            iter++;
                            break;
                        }
                    }
                    if (!collision_detected) {
                        inside_atom = false;
                    }
                }
            }
            // set graphic properties of sphere
            new_atom->setTexture(texture);
            new_atom->m_texture->setSphericalMappingEnabled(true);
            new_atom->setUseTexture(true);
            // Set the first and second sphere (the one being controlled to red
            // initially and the anchor in blue)
            if (i == 0)  // sphere is current
            {
                new_atom->setCurrent(true);
            } else if (i == 1)  // sphere is anchor
            {
                new_atom->setAnchor(true);
            }
        }
    } else {  // read in specified file
        string file_path = "../resources/data/";
        string file_name = argv[1];
        ifstream readFile(file_path + file_name);
        // file not found, so terminate program
        if (!readFile.good()) {
            cout << "ERROR: Input file " << file_name << " not found" << endl;
            cout << "Input file should be in " << file_path << endl;
            exit(EXIT_FAILURE);
        }
        string line;
        for (int i = 0; i < 11; i++) {
            getline(readFile, line);
        }
        bool firstAtom = true;
        
        vector<double> inputCoords;  // Create vector to hold our coordinates
        
        while (true) {
            // read in next coordinates
            inputCoords.clear();
            getline(readFile, line);
            if (readFile.eof()) {
                break;
            }
            string buffer;          // Have a buffer string
            stringstream ss(line);  // Insert the string into a stream
            
            while (ss >> buffer) {
                inputCoords.push_back(stod(buffer));
            }
            
            // create a sphere and define its radius
            Atom *new_atom = new Atom(SPHERE_RADIUS, SPHERE_MASS);
            
            // store pointer to sphere primitive
            spheres.push_back(new_atom);
            
            // add sphere primitive to world
            world->addChild(new_atom);
            
            // add line to world
            world->addChild(new_atom->getVelVector());
            // set graphic properties of sphere
            new_atom->setTexture(texture);
            new_atom->m_texture->setSphericalMappingEnabled(true);
            new_atom->setUseTexture(true);
            
            // Set the first and second sphere (the one being controlled to red
            // initially and the anchor in blue)
            if (inputCoords[4] == 0) {  // sphere is current
                new_atom->setCurrent(true);
            } else if (inputCoords[4] == 1) {  // sphere is anchor
                new_atom->setAnchor(true);
            }
            if (firstAtom) {
                for (int i = 0; i < 3; i++) {
                    centerCoords[i] = inputCoords[i];
                }
                new_atom->setLocalPos(0.0, 0.0, 0.0);
                firstAtom = !firstAtom;
            } else {
                // scale coordinates
                for (int i = 0; i < 3; i++) {
                    inputCoords[i] = 0.02 * (inputCoords[i] - centerCoords[i]);
                }
                new_atom->setLocalPos(inputCoords[0], inputCoords[1], inputCoords[2]);
            }
        }
        readFile.close();
    }
    for (int i = 0; i < spheres.size(); i++) {
        spheres[i]->setVelocity(0);
    }
    // determine potential if specified
    if (argc > 2) {
        // convert to lowercase
        string arg = argv[2];
        for (char &c : arg) {
            c = tolower(c);
        }
        if (arg == "morse" || "m") {
            energySurface = MORSE;
        }
    }
    //--------------------------------------------------------------------------
    // WIDGETS
    //--------------------------------------------------------------------------
    
    // create a label to display the haptic and graphic rate of the simulation
    addLabel(labelRates);
    
    // potential energy label
    addLabel(LJ_num);
    
    // number anchored label
    addLabel(num_anchored);
    
    // total energy label
    addLabel(total_energy);
    
    // frozen state label
    addLabel(isFrozen);
    
    // camera position label
    addLabel(camera_pos);
    
    // energy surface label
    addLabel(potentialLabel);
    
    // Add labels to the graph
    addLabel(scope_upper);
    addLabel(scope_lower);
    
    // create a background
    background = new cBackground();
    camera->m_backLayer->addChild(background);
    
    // set aspect ration of background image a constant
    background->setFixedAspectRatio(true);
    
    // load background image
    fileload = background->loadFromFile(
                                        RESOURCE_PATH("../resources/images/background.png"));
    
    if (!fileload) {
#if defined(_MSVC)
        fileload = background->loadFromFile("../resources/images/background.png");
#endif
    }
    if (!fileload) {
        cout << "Error - Image failed to load correctly." << endl;
        close();
        return (-1);
    }
    
    // create a scope to plot potential energy
    
    scope = new cScope();
    scope->setLocalPos(0, 60);
    camera->m_frontLayer->addChild(scope);
    scope->setSignalEnabled(true, true, false, false);
    scope->setTransparencyLevel(.7);
    global_minimum = getGlobalMinima(spheres.size());
    double lower_bound, upper_bound;
    if (global_minimum != 0 && (energySurface == LENNARD_JONES)) {
        if (global_minimum > -50) {
            upper_bound = 0;
            lower_bound = global_minimum - .5;
        } else {
            upper_bound = 0 + (global_minimum * .2);
            lower_bound = global_minimum - 3;
        }
        global_min_known = true;
    } else {
        upper_bound = 0;
        lower_bound = static_cast<int>(spheres.size()) * -3;
        global_minimum = 0;
        global_min_known = false;
    }
    scope->setRange(lower_bound, upper_bound);
    scope_upper->setText(cStr(upper_bound));
    scope_lower->setText(cStr(lower_bound));
    // Height was guessed and added manually - there's probably a better way
    // To do this but the scope height is protected
    scope_upper->setLocalPos(cAdd(scope->getLocalPos(), cVector3d(0, 180, 0)));
    scope_lower->setLocalPos(scope->getLocalPos());
    // TODO - make more legible
    //scope_upper->m_fontColor.setRed();
    //scope_lower->m_fontColor.setRed();
    
    //--------------------------------------------------------------------------
    // START SIMULATION
    //--------------------------------------------------------------------------
    
    // create a thread which starts the main haptics rendering loop
    hapticsThread = new cThread();
    hapticsThread->start(updateHaptics, CTHREAD_PRIORITY_HAPTICS);
    
    // setup callback when application exits
    atexit(close);
    
    // sets the text for the camera position to appear on screen
    camera_pos->setLocalPos(0, 30, 0);
    updateCameraLabel(camera_pos, camera);
    
    // set energy surface label
    potentialLabel->setLocalPos(0, 0);
    string potentialName;
    if (energySurface == LENNARD_JONES) {
        potentialName = "Lennard Jones Potential";
    } else if (energySurface == MORSE) {
        potentialName = "Morse Potential";
    }
    potentialLabel->setText("Potential energy surface: " + potentialName);
    
    //--------------------------------------------------------------------------
    // MAIN GRAPHIC LOOP
    //--------------------------------------------------------------------------
    
    // call window size callback at initialization
    windowSizeCallback(window, width, height);
    
    // main graphic loop
    while (!glfwWindowShouldClose(window)) {
        // get width and height of window
        glfwGetWindowSize(window, &width, &height);
        
        // render graphics
        updateGraphics();
        
        // swap buffers
        glfwSwapBuffers(window);
        
        // process events
        glfwPollEvents();
        
        // signal frequency counter
        freqCounterGraphics.signal(1);
    }
    
    // close window
    glfwDestroyWindow(window);
    
    // terminate GLFW library
    glfwTerminate();
    
    // exit
    return 0;
}

//------------------------------------------------------------------------------

void windowSizeCallback(GLFWwindow *a_window, int a_width, int a_height) {
    // update window size
    width = a_width;
    height = a_height;
}

//------------------------------------------------------------------------------

void errorCallback(int a_error, const char *a_description) {
    cout << "Error: " << a_description << endl;
}

//------------------------------------------------------------------------------

bool checkBounds(cVector3d location, cVector3d boundCenter, double boundx, double boundy, double boundz){
    if(location.y() > (boundy + boundCenter.y()) || location.y() < (-boundy + boundCenter.y()) || location.x() > (boundx + boundCenter.x()) || location.x() < (-boundx + boundCenter.x()) || location.z() > (boundz + boundCenter.z()) || location.z() < (-boundz + boundCenter.z())){
        return false;
    }
    return true;
}

//------------------------------------------------------------------------------

void keyCallback(GLFWwindow *a_window, int a_key, int a_scancode, int a_action,
                 int a_mods) {
    // filter calls that only include a key press
    if ((a_action != GLFW_PRESS) && (a_action != GLFW_REPEAT)) {
        return;
    } else if ((a_key == GLFW_KEY_ESCAPE) || (a_key == GLFW_KEY_Q && !changeBox)) {
        // option - exit
        for(int i = 0; i < repeats.size(); i++){
            for(int j = 0; j < repeats.at(i).size(); j++){
                repeats.at(i).at(j)->removeFromGraph();
                repeats.at(i).at(j)->~Atom();
            }
        }
        glfwSetWindowShouldClose(a_window, GLFW_TRUE);
        
    } else if (a_key == GLFW_KEY_F && !changeBox) {  // option - toggle fullscreen
        // toggle state variable
        fullscreen = !fullscreen;
        
        // get handle to monitor
        GLFWmonitor *monitor = glfwGetPrimaryMonitor();
        
        // get information about monitor
        const GLFWvidmode *mode = glfwGetVideoMode(monitor);
        
        // set fullscreen or window mode
        if (fullscreen) {
            glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height,
                                 mode->refreshRate);
            glfwSwapInterval(swapInterval);
        } else {
            int w = 2. * mode->height;
            int h = 1.5 * mode->height;
            int x = 1.5 * (mode->width - w);
            int y = 1.5 * (mode->height - h);
            glfwSetWindowMonitor(window, NULL, x, y, w, h, mode->refreshRate);
            glfwSwapInterval(swapInterval);
        }
    } else if (a_key == GLFW_KEY_U) {
        // action - unanchor all key
        for (auto i{0}; i < spheres.size(); i++) {
            if (spheres[i]->isAnchor()) {
                spheres[i]->setAnchor(false);
            }
        }
    } else if (a_key == GLFW_KEY_S && !changeBox) {
        // option - save screenshot to file
        cImagePtr image = cImage::create();
        camera->m_frontLayer->removeChild(scope);
        camera->renderView(width, height);
        camera->copyImageBuffer(image);
        camera->m_frontLayer->addChild(scope);
        int index = 0;
        string filename_stem = "lj" + to_string(spheres.size()) + "_";
        while (fileExists(filename_stem + to_string(index) + ".png")) {
            index++;
        }
        image->saveToFile(filename_stem + to_string(index) + ".png");
    } else if (a_key == GLFW_KEY_SPACE) {  // freeze simulation
        freezeAtoms = !freezeAtoms;
    } else if (a_key == GLFW_KEY_C) {  // save atoms to con file
        ofstream writeFile;
        // prevent overwriting .con files
        int index = 0;
        while (fileExists("atoms" + to_string(index) + ".con")) {
            index++;
        }
        writeToCon("atoms" + to_string(index) + ".con");
    } else if (a_key == GLFW_KEY_A && !changeBox) {
        // anchor all atoms while maintaing control
        for (auto i{0}; i < spheres.size(); i++) {
            if (!spheres[i]->isAnchor() && !(spheres[i]->isCurrent()) && !(spheres[i]->isRepeating())) {
                spheres[i]->setAnchor(true);
            }
        }
    } else if (a_key == GLFW_KEY_UP || a_key == GLFW_KEY_DOWN) {
        int direction = (a_key == GLFW_KEY_UP) ? 1 : -1;
        camera->setSphericalPolarRad(camera->getSphericalPolarRad() +
                                     (M_PI / 50) * direction);
        // prevent overflow on camera position
        if (camera->getSphericalPolarRad() > 1000 * M_PI) {
            camera->setSphericalPolarRad(camera->getSphericalPolarRad() -
                                         1000 * M_PI);
        }
        if (camera->getSphericalPolarRad() < -1000 * M_PI) {
            camera->setSphericalPolarRad(camera->getSphericalPolarRad() +
                                         1000 * M_PI);
        }
        updateCameraLabel(camera_pos, camera);
        
    } else if (a_key == GLFW_KEY_RIGHT || a_key == GLFW_KEY_LEFT) {
        int direction = (a_key == GLFW_KEY_RIGHT) ? 1 : -1;
        camera->setSphericalAzimuthRad(camera->getSphericalAzimuthRad() +
                                       (M_PI / 50) * direction);
        
        // prevent overflow on camera position
        if (camera->getSphericalAzimuthRad() > 1000 * M_PI) {
            camera->setSphericalAzimuthRad(camera->getSphericalAzimuthRad() -
                                           1000 * M_PI);
        }
        if (camera->getSphericalAzimuthRad() < -1000 * M_PI) {
            camera->setSphericalAzimuthRad(camera->getSphericalAzimuthRad() +
                                           1000 * M_PI);
        }
        updateCameraLabel(camera_pos, camera);
        
    } else if ((a_key == GLFW_KEY_LEFT_BRACKET ||
                a_key == GLFW_KEY_RIGHT_BRACKET) && !changeBox) {
        int direction = (a_key == GLFW_KEY_RIGHT_BRACKET) ? 1 : -1;
        if ((direction == 1 && rho < 1) || (direction == -1 && rho > .15)) {
            camera->setSphericalRadius(camera->getSphericalRadius() +
                                       .01 * direction);
            rho = camera->getSphericalRadius();
            updateCameraLabel(camera_pos, camera);
        }
    } else if (a_key == GLFW_KEY_R) {
        // Reset the camera to its default pos
        camera->setSphericalPolarRad(0);
        camera->setSphericalAzimuthRad(0);
        camera->setSphericalRadius(.35);
        rho = .35;
        updateCameraLabel(camera_pos, camera);
    } else if (a_key == GLFW_KEY_1 && !changeBox && !checkIndices){
        //set a repeat flag on all anchored atoms
        
        
        cTexture2dPtr texture = cTexture2d::create();
        
        // load texture file
        bool fileload = texture->loadFromFile(
                                              RESOURCE_PATH("../resources/images/spheremap-3.jpg"));
        if (!fileload) {
#if defined(_MSVC)
            fileload = texture->loadFromFile("../resources/images/spheremap-3.jpg");
#endif
        }
        if (!fileload) {
            cout << "Error - Texture image failed to load correctly." << endl;
            close();
        }
        
        
        box = new cShapeBox(.1,.1,.1);
        box->s_boundaryBoxColor.setGreenTeal();
        box->setUseTexture(true);
        box->setTexture(texture);
        box->setLocalPos(cVector3d(0,0,0));
        box->setUseTransparency(true);
        box->setTransparencyLevel(.25);
        world->addChild(box);
        
        freezeAtoms = !freezeAtoms;
        
        changeBox = true;
        
        
    } else if (a_key == GLFW_KEY_2){
        // resets all repeating atoms
        
        for(int i = 0; i < spheres.size(); i++){
            if(spheres[i]->isRepeating()){
                spheres[i]->setRepeating(false);
            }
        }
        for(int i = 0; i < repeats.size(); i++){
            for(int j = 0; j < repeats.at(i).size(); j++){
                repeats.at(i).at(j)->removeFromGraph();
                repeats.at(i).at(j)->~Atom();
            }
        }
        
        // this will not work more than once without resizing
        repeats.resize(0);
        
    } else if (a_key == GLFW_KEY_Q){
        if(changeBox){
            box->setLocalPos(box->getLocalPos() + cVector3d(0,0,box->getSizeZ()/10));
        }
    } else if (a_key == GLFW_KEY_A){
        if(changeBox){
            box->setLocalPos(box->getLocalPos() - cVector3d(0,0,box->getSizeZ()/10));
        }
    } else if(a_key == GLFW_KEY_W){
        if(changeBox){
            box->setLocalPos(box->getLocalPos() + cVector3d(0,box->getSizeY()/10,0));
        }
    } else if (a_key == GLFW_KEY_S){
        if(changeBox){
            box->setLocalPos(box->getLocalPos() - cVector3d(0,box->getSizeY()/10,0));
        }
    } else if(a_key == GLFW_KEY_E){
        if(changeBox){
            box->setLocalPos(box->getLocalPos() + cVector3d(box->getSizeX()/10,0,0));
        }
    } else if (a_key == GLFW_KEY_D){
        if(changeBox){
            box->setLocalPos(box->getLocalPos() - cVector3d(box->getSizeX()/10,0,0));
        }
    } else if (a_key == GLFW_KEY_LEFT_BRACKET){
        if(changeBox && box->getSizeX() < BOUNDARY_LIMIT/3 && box->getSizeY() < BOUNDARY_LIMIT/3 && box->getSizeZ() < BOUNDARY_LIMIT/3){
            box->scale(1.1);
        }
    } else if (a_key == GLFW_KEY_RIGHT_BRACKET){
        if(changeBox && box->getSizeX() > .04 && box->getSizeY() > .04 && box->getSizeZ() > .04){
            box->scale(.9);
        }
    } else if (a_key == GLFW_KEY_ENTER){
        
        if(changeBox){
            changeBox = false;
            box->setTransparencyLevel(0);
            
            freezeAtoms = !freezeAtoms;
            
            cTexture2dPtr texture = cTexture2d::create();
            
            // load texture file
            bool fileload = texture->loadFromFile(
                                                  RESOURCE_PATH("../resources/images/spheremap-3.jpg"));
            if (!fileload) {
#if defined(_MSVC)
                fileload = texture->loadFromFile("../resources/images/spheremap-3.jpg");
#endif
            }
            if (!fileload) {
                cout << "Error - Texture image failed to load correctly." << endl;
                close();
            }
            
            double maxLen = BOUNDARY_LIMIT;
            
            if(box->getSizeX() < BOUNDARY_LIMIT/5 || box->getSizeY() < BOUNDARY_LIMIT/5 || box->getSizeZ() < BOUNDARY_LIMIT/5){
                maxLen = BOUNDARY_LIMIT/3;
            }
            
            int numLayers = maxLen/box->getSizeY() - 1;
            int numRepeatAtoms = pow(((2*numLayers)+1),2);
            int numZLayers = 3;
            numRepeatAtoms *= numZLayers;
            if (numLayers == 0){
                numRepeatAtoms = 0;
            }
            
            
            int repCount = 0;
            repeats.resize(numRepeatAtoms);
            for(int i = 0; i < spheres.size(); i++){
                if(checkBounds(spheres[i]->getLocalPos(), box->getLocalPos(), box->getSizeX(), box->getSizeY(), box->getSizeZ()) && !spheres[i]->isCurrent()){
                    
                    //spheres[i]->~Atom();
                    int xcount = -numLayers;
                    int ycount = -numLayers;
                    int zcount = 0;
                    for(int j = 0; j < numRepeatAtoms; j++){
                        //cout << "(" << xcount << ", " << ycount << ", " << zcount << ")\n";
                        Atom *temp = new Atom(SPHERE_RADIUS, SPHERE_MASS);
                        world->addChild(temp);
                        world->addChild(temp->getVelVector());
                        temp->setTexture(texture);
                        temp->m_texture->setSphericalMappingEnabled(true);
                        temp->setUseTexture(true);
                        
                        cVector3d repPos = cVector3d(spheres[i]->getLocalPos().x() + (box->getSizeX()*xcount), spheres[i]->getLocalPos().y() +  (box->getSizeY()*ycount), spheres[i]->getLocalPos().z() + (box->getSizeZ()*(zcount+2)));
                        
                        temp->setRepeating(true);
                        temp->setLocalPos(repPos);
                        
                        if(zcount > 0){
                            temp->setAnchor(true);
                        }
                        
                        repeats.at(j).push_back(temp);
                        xcount++;
                        if(xcount > numLayers){
                            xcount = -numLayers;
                            ycount++;
                        }
                        
                        if(ycount > numLayers){
                            ycount = -numLayers;
                            xcount = -numLayers;
                            zcount++;
                        }
                        
                    }
                    repCount++;
                }
                
            }
            
            if(repCount == 0){
                repeats.resize(0);
            }
        }
    } else if (a_key == GLFW_KEY_B){
        bcc = true;
        
        //!!TO BE MOVED!!
        if(changeBox){
            changeBox = false;
            box->setTransparencyLevel(0);
            freezeAtoms = !freezeAtoms;
            
            cTexture2dPtr texture = cTexture2d::create();
            
            // load texture file
            bool fileload = texture->loadFromFile(
                                                  RESOURCE_PATH("../resources/images/spheremap-3.jpg"));
            if (!fileload) {
#if defined(_MSVC)
                fileload = texture->loadFromFile("../resources/images/spheremap-3.jpg");
#endif
            }
            if (!fileload) {
                cout << "Error - Texture image failed to load correctly." << endl;
                close();
            }
            
            //sets up lattice constants
            double horzlc = .02;
            double vertlc = horzlc;
            
            double maxLen = BOUNDARY_LIMIT;
            
            if(horzlc < BOUNDARY_LIMIT/5 || horzlc < BOUNDARY_LIMIT/5 || vertlc < BOUNDARY_LIMIT/5){
                maxLen = BOUNDARY_LIMIT/10;
            }
            
            int numLayers = maxLen/horzlc - 1;
            int numRepeatAtoms = pow(((2*numLayers)+1),2);
            //cout << "(" << numLayers << ", " << numRepeatAtoms << ")";
            int numZLayers = 3;
            numRepeatAtoms *= numZLayers;
            if (numLayers == 0){
                numRepeatAtoms = 0;
            }
            //bcc
            repeats.resize(numRepeatAtoms);
            int xlay = -numLayers;
            int ylay = -numLayers;
            int zlay = 0;
            bool offset = false;
            for(int i = 0; i < numRepeatAtoms; i++){
                //cout << "(" << xlay << ", " << ylay << ", " << zlay << ")\n";
                Atom *temp = new Atom(SPHERE_RADIUS, SPHERE_MASS);
                world->addChild(temp);
                world->addChild(temp->getVelVector());
                temp->setTexture(texture);
                temp->m_texture->setSphericalMappingEnabled(true);
                temp->setUseTexture(true);
                
                //offsets every other layer
                cVector3d repPos = cVector3d(xlay*horzlc, ylay*horzlc, (zlay+2)*vertlc);
                if(offset){
                    repPos.sub(horzlc/2, horzlc/2, 0);
                }
                
                temp->setRepeating(true);
                temp->setLocalPos(repPos);
                
                //   if(zlay > 0){     -- this is too slow to work properly
                temp->setAnchor(true);
                //  }
                
                repeats.at(i).push_back(temp);
                
                xlay++;
                
                if(xlay > numLayers){
                    xlay = -numLayers;
                    ylay++;
                }
                
                if(ylay > numLayers){
                    xlay = -numLayers;
                    ylay = -numLayers;
                    zlay++;
                    offset = !offset;
                }
            }
        }
        
    } else if (a_key == GLFW_KEY_F){
        fcc = true;
        
        //!!TO BE MOVED!!
        if(changeBox){
            changeBox = false;
            box->setTransparencyLevel(0);
            freezeAtoms = !freezeAtoms;
            
            cTexture2dPtr texture = cTexture2d::create();
            
            // load texture file
            bool fileload = texture->loadFromFile(
                                                  RESOURCE_PATH("../resources/images/spheremap-3.jpg"));
            if (!fileload) {
#if defined(_MSVC)
                fileload = texture->loadFromFile("../resources/images/spheremap-3.jpg");
#endif
            }
            if (!fileload) {
                cout << "Error - Texture image failed to load correctly." << endl;
                close();
            }
            
            //sets up lattice constants
            double horzlc = .02;
            double vertlc = horzlc;
            
            double maxLen = BOUNDARY_LIMIT;
            
            if(horzlc < BOUNDARY_LIMIT/5 || horzlc < BOUNDARY_LIMIT/5 || vertlc < BOUNDARY_LIMIT/5){
                maxLen = BOUNDARY_LIMIT/10;
            }
            
            int numLayers = maxLen/horzlc - 1;
            int numRepeatAtoms = pow(((2*numLayers)+1),2);
            //cout << "(" << numLayers << ", " << numRepeatAtoms << ")";
            int numZLayers = 3;
            numRepeatAtoms *= numZLayers;
            if (numLayers == 0){
                numRepeatAtoms = 0;
            }
            //fcc
            repeats.resize(numRepeatAtoms);
            int xlay = -numLayers;
            int ylay = -numLayers;
            int zlay = 0;
            bool xoffset = true;
            bool offsetLayer = false;
            
            for(int i = 0; i < numRepeatAtoms; i++){
                cout << "(" << xlay << ", " << ylay << ", " << zlay << ")\n";
                Atom *temp = new Atom(SPHERE_RADIUS, SPHERE_MASS);
                world->addChild(temp);
                world->addChild(temp->getVelVector());
                temp->setTexture(texture);
                temp->m_texture->setSphericalMappingEnabled(true);
                temp->setUseTexture(true);
                
                //offsets every other layer
                cVector3d repPos = cVector3d(xlay*horzlc, ylay*horzlc, (zlay+2)*vertlc);
                
                if(offsetLayer){
                    if(xoffset){
                        repPos.sub(horzlc/2, 0, 0);
                    }
                }
                
                temp->setRepeating(true);
                temp->setLocalPos(repPos);
                
                //   if(zlay > 0){     -- this is too slow to work properly
                temp->setAnchor(true);
                //  }
                
                repeats.at(i).push_back(temp);
                
                
                xlay++;
                
                if(xlay > numLayers){
                    xlay = -numLayers;
                    ylay++;
                    if(offsetLayer){
                        xoffset = !xoffset;
                    }
                }
                
                if(ylay > numLayers){
                    xlay = -numLayers;
                    ylay = -numLayers;
                    zlay++;
                    offsetLayer = !offsetLayer;
                }
                
            }
        }
        
    } else if (a_key == GLFW_KEY_0){
        if(checkIndices){
            if(millIndCount < 3){
                millerIndices[millIndCount] = 0;
                millIndCount++;
            } else {
                //bcc or fcc
            }
        }
        
    } else if (a_key == GLFW_KEY_1){
        if(checkIndices){
            if(millIndCount < 3){
                millerIndices[millIndCount] = 1;
                millIndCount++;
            } else {
                //bcc or fcc
            }
        }
    }
    
}

void mouseButtonCallback(GLFWwindow *a_window, int a_button, int a_action,
                         int a_mods) {
    // store mouse position
    double x, y;
    
    // detect for any collision between mouse and scene
    cCollisionRecorder recorder;
    cCollisionSettings settings;
    
    if (a_button == GLFW_MOUSE_BUTTON_LEFT && a_action == GLFW_PRESS) {
        glfwGetCursorPos(window, &x, &y);
        bool hit =
        camera->selectWorld(x, (height - y), width, height, recorder, settings);
        if (hit) {
            cGenericObject *selected = recorder.m_nearestCollision.m_object;
            selectedAtom = (Atom *)selected;
            selectedPoint = recorder.m_nearestCollision.m_globalPos;
            selectedAtomOffset =
            recorder.m_nearestCollision.m_globalPos - selectedAtom->getLocalPos();
            mouseState = MOUSE_SELECTION;
        }
    } else if (a_button == GLFW_MOUSE_BUTTON_RIGHT && a_action == GLFW_PRESS) {
        glfwGetCursorPos(window, &x, &y);
        bool hit =
        camera->selectWorld(x, (height - y), width, height, recorder, settings);
        if (hit) {
            // retrieve Atom selected by mouse
            cGenericObject *selected = recorder.m_nearestCollision.m_object;
            selectedAtom = (Atom *)selected;
            // Toggle anchor status and color
            if (selectedAtom->isAnchor()) {
                selectedAtom->setAnchor(false);
            } else if (!selectedAtom->isCurrent() && !(selectedAtom->isRepeating())) {  // cannot set current to anchor
                selectedAtom->setAnchor(true);
            }
            mouseState = MOUSE_SELECTION;
        }
    } else {
        mouseState = MOUSE_IDLE;
    }
}
//------------------------------------------------------------------------------

void close(void) {
    // stop the simulation
    simulationRunning = false;
    // wait for graphics and haptics loops to terminate
    while (!simulationFinished) {
        cSleepMs(100);
    }
    // delete resources
    delete hapticsThread;
    delete world;
    delete handler;
}

//------------------------------------------------------------------------------

void updateGraphics(void) {
    /////////////////////////////////////////////////////////////////////
    // UPDATE WIDGETS
    /////////////////////////////////////////////////////////////////////
    
    // update haptic and graphic rate data
    labelRates->setText(cStr(freqCounterGraphics.getFrequency(), 0) + " Hz / " +
                        cStr(freqCounterHaptics.getFrequency(), 0) + " Hz");
    
    // update position of label
    labelRates->setLocalPos((int)(0.5 * (width - labelRates->getWidth())), 15);
    
    /////////////////////////////////////////////////////////////////////
    // RENDER SCENE
    /////////////////////////////////////////////////////////////////////
    
    // update shadow maps (if any)
    world->updateShadowMaps(false, false);
    
    // render world
    camera->renderView(width, height);
    // wait until all GL commands are completed
    glFinish();
    // check for any OpenGL errors
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) cout << "Error: " << gluErrorString(err) << endl;
}


//------------------------------------------------------------------------------

void updateHaptics(void) {
    Atom *current;
    Atom *previous;
    
    // simulation in now running
    simulationRunning = true;
    simulationFinished = false;
    
    // a flag to indicate if haptic forces are active
    bool flagHapticsEnabled = false;
    
    // reset clock
    cPrecisionClock clock;
    clock.reset();
    
    // open a connection to haptic device
    hapticDevice->open();
    
    // calibrate device (if necessary)
    hapticDevice->calibrate();
    
    // Track which atom is currently being moved
    int curr_atom = 0;
    int anchor_atom = 1;
    int anchor_atom_hold = 1;
    
    // main haptic simulation loop
    
    bool button1_changed = false;
    bool button2_changed = false;
    bool button3_changed = false;
    bool is_anchor = true;
    while (simulationRunning) {
        /////////////////////////////////////////////////////////////////////
        // SIMULATION TIME
        /////////////////////////////////////////////////////////////////////
        
        // stop the simulation clock
        clock.stop();
        
        // read the time increment in seconds
        double timeInterval = cMin(0.001, clock.getCurrentTimeSeconds());
        
        // restart the simulation clock
        // clock.reset();
        clock.start();
        
        // signal frequency counter
        freqCounterHaptics.signal(1);
        
        /////////////////////////////////////////////////////////////////////////
        // READ HAPTIC DEVICE
        /////////////////////////////////////////////////////////////////////////
        
        // read position
        cVector3d position;
        hapticDevice->getPosition(position);
        // Scale position to use more of the screen
        // increase to use more of the screen
        position *= 2.0;
        // read user-switch status (button 0)
        
        /////////////////////////////////////////////////////////////////////////
        // UPDATE SIMULATION
        /////////////////////////////////////////////////////////////////////////
        
        // Update current atom based on if the user pressed the far left button
        // The point of button2_changed is to make it so that it only switches one
        // atom if the button is touched Otherwise it flips out
        
        bool button0;
        hapticDevice->getUserSwitch(0, button0);
        bool button1;
        hapticDevice->getUserSwitch(1, button1);
        bool button2;
        hapticDevice->getUserSwitch(2, button2);
        bool button3;
        hapticDevice->getUserSwitch(3, button3);
        // Changes the camera when button2 is pressed
        if (button2) {
            if (!button2_changed) {
                // rotates camera around in square
                switch (curr_camera) {
                    case 1:
                        camera->setSphericalPolarRad(0);
                        camera->setSphericalAzimuthRad(0);
                        updateCameraLabel(camera_pos, camera);
                        break;
                    case 2:
                        camera->setSphericalPolarRad(0);
                        camera->setSphericalAzimuthRad(M_PI);
                        updateCameraLabel(camera_pos, camera);
                        ;
                        break;
                    case 3:
                        camera->setSphericalPolarRad(M_PI);
                        camera->setSphericalAzimuthRad(M_PI);
                        updateCameraLabel(camera_pos, camera);
                        break;
                    case 4:
                        curr_camera = 0;
                        camera->setSphericalPolarRad(M_PI);
                        camera->setSphericalAzimuthRad(0);
                        updateCameraLabel(camera_pos, camera);
                        break;
                }
                curr_camera++;
                button2_changed = true;
            }
        } else
            button2_changed = false;
        // Changes the current atom being controlled when button 1 is pressed
        // JD: edit so that we use remainder function in C++ and remove previous if
        // else statement
        if (button1) {
            if (!button1_changed) {
                // computes current atom by taking the remainder of the curr_atom +1 and
                // number of spheres
                int previous_curr_atom = curr_atom;
                curr_atom = remainder(curr_atom + 1, spheres.size());
                if (curr_atom < 0) {
                    curr_atom = spheres.size() + curr_atom;
                }
                // Skip anchored atoms; will eventually terminate at previous_curr_atom
                while (spheres[curr_atom]->isAnchor()) {
                    curr_atom = remainder(curr_atom + 1, spheres.size());
                    if (curr_atom < 0) {
                        curr_atom = spheres.size() + curr_atom;
                    }
                }
                
                current = spheres[curr_atom];
                previous = spheres[previous_curr_atom];
                
                cVector3d A = current->getLocalPos();
                
                // Change attributes of previous current and new current
                previous->setCurrent(false);
                previous->setLocalPos(A);
                current->setCurrent(true);
                current->setLocalPos(position);
                cVector3d translate =
                (previous->getLocalPos()) - (current->getLocalPos());
                Atom *traverser;
                for (int i = 0; i < spheres.size(); i++) {
                    traverser = spheres[i];
                    if (i != curr_atom) {
                        if (i == (previous_curr_atom)) {
                            traverser->setLocalPos(previous->getLocalPos() -
                                                   (2.0 * translate));
                            cVector3d positions = traverser->getLocalPos();
                        } else {
                            traverser->setLocalPos(traverser->getLocalPos() - (translate));
                            cVector3d positions = traverser->getLocalPos();
                        }
                    }
                }
                button1_changed = true;
            }
        } else {
            button1_changed = false;
        }
        
        // update frozen state label
        string trueFalse = freezeAtoms ? "true" : "false";
        isFrozen->setText("Freeze simulation: " + trueFalse);
        auto isFrozenWidth = (width - isFrozen->getWidth()) - 5;
        isFrozen->setLocalPos(isFrozenWidth, 15);
        
        if (!freezeAtoms) {
            // compute forces for all spheres
            double potentialEnergy = 0;
            
            // JD: edited this so that many operations are removed out of the inner
            // loop This loop is for computing the force on atom i
            for (int i = 0; i < spheres.size() + repeats.size(); i++) {
                // compute force on atom
                cVector3d force;
                int count = 0;
                if(i < spheres.size()){
                    current = spheres[i];
                } else {
                    //if there are repeating atoms
                    //int temp = i-spheres.size();
                    //cout << "(" + cStr(temp) + ", " + cStr(count) + ")";
                    
                    //if statement seems redundant, but an error will be thrown occasionally if not checking every time
                    if(repeats.at(i-spheres.size()).size() > 0 && count < repeats.at(i-spheres.size()).size()){
                        current = repeats.at(i-spheres.size()).at(count);
                        count++;
                        if(count >= repeats.at(i-spheres.size()).size()){
                            count = 0;
                        } else {
                            i--;
                        }
                    }
                }
                cVector3d pos0 = current->getLocalPos();
                // check forces with all other spheres
                force.zero();
                
                // this loop is for finding all of atom i's neighbors
                for (int j = 0; j < spheres.size() + repeats.size(); j++) {
                    // Don't compute forces between an atom and itself
                    if (i != j) {
                        // get position of sphere
                        cVector3d pos1;
                        int innerCount = 0;
                        if(j < spheres.size()){
                            pos1 = spheres[j]->getLocalPos();
                        } else {
                            //if there are repeating atoms
                            //int temp = i-spheres.size();
                            //cout << "(" + cStr(temp) + ", " + cStr(count) + ")";
                            
                            //if statement seems redundant, but an error will be thrown occasionally if not checking every time
                            if(repeats.at(j-spheres.size()).size() > 0 && innerCount < repeats.at(j-spheres.size()).size()){
                                pos1 = repeats.at(j-spheres.size()).at(innerCount)->getLocalPos();
                                innerCount++;
                                if(innerCount >= repeats.at(j-spheres.size()).size()){
                                    innerCount = 0;
                                } else {
                                    j--;
                                }
                            }
                        }
                        // compute direction vector from sphere 0 to 1
                        
                        cVector3d dir01 = cNormalize(pos0 - pos1);
                        
                        // compute distance between both spheres
                        double distance = cDistance(pos0, pos1) / DIST_SCALE;
                        if (energySurface == LENNARD_JONES) {
                            potentialEnergy += getLennardJonesEnergy(distance);
                        } else if (energySurface == MORSE) {
                            potentialEnergy += getMorseEnergy(distance);
                        }
                        if (!button0) {
                            double appliedForce;
                            if (energySurface == LENNARD_JONES) {
                                appliedForce = getLennardJonesForce(distance);
                            } else if (energySurface == MORSE) {
                                appliedForce = getMorseForce(distance);
                            }
                            force.add(appliedForce * dir01);
                        }
                    }
                }
                
                if(force.length() > 10000){
                    force.normalize();
                    force.mul(10000);
                }
                
                current->setForce(force);
                cVector3d sphereAcc = (force / current->getMass());
                current->setVelocity(
                                     V_DAMPING * (current->getVelocity() + timeInterval * sphereAcc));
                
                if(current->getVelocity().length() > 100){
                    current->getVelocity().normalize();
                    current->getVelocity().mul(100);
                }
                
                // compute position
                cVector3d spherePos_change = timeInterval * current->getVelocity() +
                cSqr(timeInterval) * sphereAcc;
                
                cVector3d spherePos = current->getLocalPos() + spherePos_change;
                
                
                double magnitude = force.length();
                
                if (magnitude > 5) {
                    cout << i << " velocity " << current->getVelocity().length() << endl;
                    cout << i << " force " << force.length() << endl;
                    cout << i << " acceleration " << sphereAcc.length() << endl;
                    cout << i << " time " << timeInterval << endl;
                    cout << i << " position of  " << timeInterval << endl;
                }
                
                if(!current->isRepeating()){
                    //A is the current position, B is the position to move to
                    cVector3d A = current->getLocalPos();
                    cVector3d B = spherePos;
                    
                    //holds intersect point/norm if an intersect is made
                    cVector3d intersectPoint;
                    cVector3d intersectNorm;
                    
                    cVector3d tempPos;
                    cVector3d tempA;
                    tempA.copyfrom(A);
                    
                    tempPos.copyfrom(spherePos);
                    
                    
                    //north plane
                    if(cIntersectionSegmentPlane(A, B, northPlanePos, northPlaneNorm, intersectPoint, intersectNorm) == 1){
                        spherePos.zero();
                        spherePos.copyfrom(intersectPoint);
                        spherePos.y(spherePos.y() - (BOUNDARY_LIMIT*2-.01));
                        if(!checkBounds(spherePos, cVector3d(0,0,0), BOUNDARY_LIMIT,BOUNDARY_LIMIT,BOUNDARY_LIMIT)){
                            spherePos.copyfrom(tempPos);
                        }
                    }
                    
                    //south plane
                    if(cIntersectionSegmentPlane(A, B, southPlanePos, southPlaneNorm, intersectPoint, intersectNorm) == 1){
                        spherePos.zero();
                        spherePos.copyfrom(intersectPoint);
                        spherePos.y(spherePos.y() + (BOUNDARY_LIMIT*2-.01));
                        if(!checkBounds(spherePos, cVector3d(0,0,0), BOUNDARY_LIMIT,BOUNDARY_LIMIT,BOUNDARY_LIMIT)){
                            spherePos.copyfrom(tempPos);
                        }
                    }
                    
                    //east plane
                    if(cIntersectionSegmentPlane(A, B, eastPlanePos, eastPlaneNorm, intersectPoint, intersectNorm) == 1){
                        spherePos.zero();
                        spherePos.copyfrom(intersectPoint);
                        spherePos.x(spherePos.x() - (BOUNDARY_LIMIT*2-.01));
                        if(!checkBounds(spherePos, cVector3d(0,0,0), BOUNDARY_LIMIT,BOUNDARY_LIMIT,BOUNDARY_LIMIT)){
                            spherePos.copyfrom(tempPos);
                        }
                    }
                    
                    //west plane
                    if(cIntersectionSegmentPlane(A, B, westPlanePos, westPlaneNorm, intersectPoint, intersectNorm) == 1){
                        spherePos.zero();
                        spherePos.copyfrom(intersectPoint);
                        spherePos.x(spherePos.x() + (BOUNDARY_LIMIT*2-.01));
                        if(!checkBounds(spherePos, cVector3d(0,0,0), BOUNDARY_LIMIT,BOUNDARY_LIMIT,BOUNDARY_LIMIT)){
                            spherePos.copyfrom(tempPos);
                        }
                    }
                    
                    //forward plane
                    if(cIntersectionSegmentPlane(A, B, forwardPlanePos, forwardPlaneNorm, intersectPoint, intersectNorm) == 1){
                        spherePos.zero();
                        spherePos.copyfrom(intersectPoint);
                        spherePos.z(spherePos.z() - (BOUNDARY_LIMIT*2-.01));
                        if(!checkBounds(spherePos, cVector3d(0,0,0), BOUNDARY_LIMIT,BOUNDARY_LIMIT,BOUNDARY_LIMIT)){
                            spherePos.copyfrom(tempPos);
                        }
                    }
                    
                    //back plane
                    if(cIntersectionSegmentPlane(A, B, backPlanePos, backPlaneNorm, intersectPoint, intersectNorm) == 1){
                        spherePos.zero();
                        spherePos.copyfrom(intersectPoint);
                        spherePos.z(spherePos.z() + (BOUNDARY_LIMIT*2-.01));
                        if(!checkBounds(spherePos, cVector3d(0,0,0), BOUNDARY_LIMIT,BOUNDARY_LIMIT,BOUNDARY_LIMIT)){
                            spherePos = tempPos;
                        }
                    }
                }
                
                if (!current->isCurrent()) {
                    if (!current->isAnchor()) {
                        current->setLocalPos(spherePos);
                    }
                }
                
                if(!checkBounds(current->getLocalPos(), cVector3d(0,0,0), BOUNDARY_LIMIT,BOUNDARY_LIMIT,BOUNDARY_LIMIT) && !current->isRepeating()){
                    cout << "ATOM OUT OF BOUNDS";
                    
                }
            }
            
            
            
            current = spheres[curr_atom];
            current->setLocalPos(position);
            
            // JD: moved this out of nested for loop so that test is set only when
            // fully calculated update haptic and graphic rate data
            LJ_num->setText("Potential Energy: " + cStr((potentialEnergy / 2), 5));
            
            // update position of label
            LJ_num->setLocalPos(0, 15, 0);
            
            // count the number of anchored atoms
            auto anchored{0};
            for (auto i{0}; i < spheres.size(); i++) {
                if (spheres[i]->isAnchor()) {
                    anchored++;
                }
            }
            num_anchored->setText(to_string(anchored) + " anchored / " +
                                  to_string(spheres.size()) + " total");
            auto num_anchored_width = (width - num_anchored->getWidth()) - 5;
            num_anchored->setLocalPos(num_anchored_width, 0);
            
            // Update scope
            double currentTime = clock.getCurrentTimeSeconds();
            // rounds current time to the nearest tenth
            double currentTimeRounded = double(int(currentTime * 10 + .5)) / 10;
            // The number fmod() is compared to is the threshold, this adjusts the
            // timescale
            if (fmod(currentTime, currentTimeRounded) <= .01) {
                scope->setSignalValues(potentialEnergy / 2, global_minimum);
            }
            
            // scale the graph if the minimum isn't known
            if (!global_min_known) {
                if ((potentialEnergy / 2) < global_minimum) {
                    global_minimum = (potentialEnergy / 2);
                }
                
                if (global_minimum < scope->getRangeMin()) {
                    auto new_lower = scope->getRangeMin() - 25;
                    auto new_upper = scope->getRangeMax() - 25;
                    scope->setRange(new_lower, new_upper);
                    scope_upper->setText(cStr(scope->getRangeMax()));
                    scope_lower->setText(cStr(scope->getRangeMin()));
                }
            }
        }
        cVector3d force = current->getForce();
        /////////////////////////////////////////////////////////////////////////
        // FORCE VECTOR
        /////////////////////////////////////////////////////////////////////////
        for (int i = 0; i < spheres.size(); i++) {
            spheres[i]->updateVelVector();
        }
        /////////////////////////////////////////////////////////////////////////
        // APPLY FORCES
        /////////////////////////////////////////////////////////////////////////
        
        // scale the force according to the max stiffness the device can render
        double stiffnessRatio = 0.5;
        if (hapticDeviceMaxStiffness < HAPTIC_STIFFNESS)
            stiffnessRatio = hapticDeviceMaxStiffness / HAPTIC_STIFFNESS;
        if (force.length() > 10) force = 10. * cNormalize(force);
        // send computed force to haptic device
        hapticDevice->setForce(stiffnessRatio * force);
    }
    // close  connection to haptic device
    hapticDevice->close();
    
    // exit haptics thread
    simulationFinished = true;
}

void mouseMotionCallback(GLFWwindow *a_window, double a_posX, double a_posY) {
    if ((selectedAtom != NULL) && (mouseState == MOUSE_SELECTION) &&
        (selectedAtom->isAnchor())) {
        // get the vector that goes from the camera to the selected point (mouse
        // click)
        cVector3d vCameraObject = selectedPoint - camera->getLocalPos();
        
        // get the vector that point in the direction of the camera. ("where the
        // camera is looking at")
        cVector3d vCameraLookAt = camera->getLookVector();
        
        // compute the angle between both vectors
        double angle = cAngle(vCameraObject, vCameraLookAt);
        
        // compute the distance between the camera and the plane that intersects the
        // object and which is parallel to the camera plane
        double distanceToObjectPlane = vCameraObject.length() * cos(angle);
        
        // convert the pixel in mouse space into a relative position in the world
        double factor =
        (distanceToObjectPlane * tan(0.5 * camera->getFieldViewAngleRad())) /
        (0.5 * height);
        double posRelX = factor * (a_posX - (0.5 * width));
        double posRelY = factor * ((height - a_posY) - (0.5 * height));
        // compute the new position in world coordinates
        cVector3d pos = camera->getLocalPos() +
        distanceToObjectPlane * camera->getLookVector() +
        posRelX * camera->getRightVector() +
        posRelY * camera->getUpVector();
        
        // compute position of object by taking in account offset
        cVector3d posObject = pos - selectedAtomOffset;
        
        // apply new position to object
        selectedAtom->setLocalPos(posObject);
    }
}

void writeToCon(string fileName) {
    ofstream writeFile;
    writeFile.open(fileName);
    writeFile << "Generated by haptic device" << endl << endl;
    writeFile << "100.000000   100.000000   100.000000" << endl
    << "90.000000    90.000000    90.000000" << endl
    << endl
    << endl;
    writeFile << "1" << endl
    << spheres.size() << endl
    << "1.007940" << endl
    << "H" << endl
    << "Coordinates of Component 1" << endl;
    writeFile.precision(7);
    writeFile << centerCoords[0] << " " << centerCoords[1] << " "
    << centerCoords[2] << " 0 0" << endl;
    for (int i = 1; i < spheres.size(); i++) {
        cVector3d pos = spheres[i]->getLocalPos();
        writeFile << (pos.x() / 0.02) + centerCoords[0] << " "
        << (pos.y() / 0.02) + centerCoords[1] << " "
        << (pos.z() / 0.02) + centerCoords[2] << " 0 " << i << endl;
    }
    writeFile.close();
}

void addLabel(cLabel *&label) {
    label = new cLabel(font);
    label->m_fontColor.setBlack();
    camera->m_frontLayer->addChild(label);
}

void updateCameraLabel(cLabel *&camera_pos, cCamera *&camera) {
    camera_pos->setText("Camera located at: (" +
                        cStr(rho * sin(camera->getSphericalPolarRad()) *
                             cos(camera->getSphericalAzimuthRad())) +
                        ", " +
                        cStr(rho * sin(camera->getSphericalPolarRad()) *
                             sin(camera->getSphericalAzimuthRad())) +
                        ", " + cStr(rho * cos(camera->getSphericalPolarRad())) +
                        ")");
}
