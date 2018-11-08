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
#include "chai3d.h"
//------------------------------------------------------------------------------
#include <GLFW/glfw3.h>
#include <math.h>
#include <ctime>
#include <chrono>
//------------------------------------------------------------------------------
using namespace chai3d;
using namespace std;
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// GENERAL SETTINGS
//------------------------------------------------------------------------------

// stereo Mode
/*
    C_STEREO_DISABLED:            Stereo is disabled 
    C_STEREO_ACTIVE:              Active stereo for OpenGL NVDIA QUADRO cards
    C_STEREO_PASSIVE_LEFT_RIGHT:  Passive stereo where L/R images are rendered next to each other
    C_STEREO_PASSIVE_TOP_BOTTOM:  Passive stereo where L/R images are rendered above each other
*/
cStereoMode stereoMode = C_STEREO_DISABLED;

// fullscreen mode
bool fullscreen = false;


// mirrored display
bool mirroredDisplay = false;


//------------------------------------------------------------------------------
// DECLARED CONSTANTS
//------------------------------------------------------------------------------

// number of spheres in the scene
const int NUM_SPHERES = 6;

// radius of each sphere
const double SPHERE_RADIUS = 0.008;

//number of cameras
const int NUM_CAM = 4;




//------------------------------------------------------------------------------
// DECLARED VARIABLES
//------------------------------------------------------------------------------

// a world that contains all objects of the virtual environment
cWorld *world;

// a camera to render the world in the window display
cCamera *camera;

// a light source to illuminate the objects in the world
cSpotLight *light;

//cToolCursor* tool;

// a haptic device handler
cHapticDeviceHandler *handler;

// a pointer to the current haptic device
cGenericHapticDevicePtr hapticDevice;

// highest stiffness the current haptic device can render
double hapticDeviceMaxStiffness;

// sphere objects
cShapeSphere *spheres[NUM_SPHERES];

// linear velocity of each sphere
cVector3d sphereVel[NUM_SPHERES];


// a colored background
cBackground *background;

// a font for rendering text
cFontPtr font;

// a label to display the rate [Hz] at which the simulation is running
cLabel *labelRates;

// a label to explain what is happening
cLabel *labelMessage;
//a label to show the potential energy
cLabel *LJ_num;

//a label to display the total energy of the system
cLabel *total_energy;

// a flag that indicates if the haptic simulation is currently running
bool simulationRunning = false;

// a flag that indicates if the haptic simulation has terminated
bool simulationFinished = true;

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

// a line representing the velocity vector of the haptic device
//cShapeLine* velocity;

// An array of velocity vectors
cShapeLine *velVectors[NUM_SPHERES];


//------------------------------------------------------------------------------
// DECLARED MACROS
//------------------------------------------------------------------------------
// convert to resource path
#define RESOURCE_PATH(p)    (char*)((resourceRoot+string(p)).c_str())


//------------------------------------------------------------------------------
// DECLARED FUNCTIONS
//------------------------------------------------------------------------------

// callback when the window display is resized
void windowSizeCallback(GLFWwindow *a_window, int a_width, int a_height);

// callback when an error GLFW occurs
void errorCallback(int error, const char *a_description);

// callback when a key is pressed
void keyCallback(GLFWwindow *a_window, int a_key, int a_scancode, int a_action, int a_mods);

// this function renders the scene
void updateGraphics(void);

// this function contains the main haptics simulation loop
void updateHaptics(void);

// this function closes the application
void close(void);



//------------------------------------------------------------------------------
// DECLARED MACROS
//------------------------------------------------------------------------------
// convert to resource path
#define RESOURCE_PATH(p)    (char*)((resourceRoot+string(p)).c_str())


//==============================================================================
/*
    DEMO:   09-magnets.cpp

    This example illustrates how to create a simple dynamic simulation using
    small sphere shape primitives. All dynamics and collisions are computed
    in the haptics thread.
*/
//==============================================================================
//bool button3_changed = false;
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
    cout << "Demo: LJ-TEST" << endl;
    cout << "Force-Vector Branch" << endl;
    cout << "-----------------------------------" << endl << endl << endl;
    cout << endl << endl;


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

    // set resize callback
    glfwSetWindowSizeCallback(window, windowSizeCallback);

    // set current display context
    glfwMakeContextCurrent(window);

    // sets the swap interval for the current display context
    glfwSwapInterval(swapInterval);

#ifdef GLEW_VERSION
    // initialize GLEW library
    if (glewInit() != GLEW_OK)
    {
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

    // position and orient the camera



    cVector3d camerapos(0.2, 0.0, 0.2);
    cVector3d lookat(0.00, 0.0, 0.00);
    cVector3d up(0.0, 0.0, 1.0);
    camera->set(camerapos,    // camera position (eye)
                lookat,    // lookat position (target)
                up);   // direction of the (up) vector 

    // set the near and far clipping planes of the camera
    // anything in front or behind these clipping planes will not be rendered
    camera->setClippingPlanes(0.01, 10.0);

    // set stereo mode
    camera->setStereoMode(stereoMode);

    // set stereo eye separation and focal length (applies only if stereo is enabled)
    camera->setStereoEyeSeparation(0.03);
    camera->setStereoFocalLength(1.8);

    // set vertical mirrored display mode
    camera->setMirrorVertical(mirroredDisplay);

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
    //light->m_shadowMap->setQualityLow();
    light->m_shadowMap->setQualityHigh();

    // set shadow factor
    world->setShadowIntensity(0.3);

    // set light cone half angle
    light->setCutOffAngleDeg(30);

    // create a small line to illustrate velocity of the haptic device
    //velocity = new cShapeLine(cVector3d(0,0,0),
    //                         cVector3d(0,0,0));
    
    // insert line inside world
    //world->addChild(velocity);

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

    // if the haptic devices carries a gripper, enable it to behave like a user switch
    hapticDevice->setEnableGripperUserSwitch(true);
    //bool button3;
    //hapticDevice-> getUserSwitch(2, button3);
    //tool->start();


    //--------------------------------------------------------------------------
    // CREATE PLANE
    //--------------------------------------------------------------------------
/*
    // create mesh
    cMesh* plane = new cMesh();

    // add mesh to world
    world->addChild(plane);

    // create plane primitive
    cCreateMap(plane, 0.2, 1.0, 50, 50);

    // compile object
    plane->setUseDisplayList(true);

    // set color properties
    plane->m_material->setWhite();
*/

    //--------------------------------------------------------------------------
    // CREATE SPHERES
    //--------------------------------------------------------------------------

    // create texture
    cTexture2dPtr texture = cTexture2d::create();

    // load texture file
    bool fileload = texture->loadFromFile(RESOURCE_PATH("../resources/images/spheremap-3.jpg"));
    if (!fileload) {
#if defined(_MSVC)
        fileload = texture->loadFromFile("../../../bin/resources/images/spheremap-3.jpg");
#endif
    }
    if (!fileload) {
        cout << "Error - Texture image failed to load correctly." << endl;
        close();
        return (-1);
    }

    // create spheres
    for (int i = 0; i < NUM_SPHERES; i++) {
        // create a sphere and define its radius
        cShapeSphere *sphere = new cShapeSphere(SPHERE_RADIUS);

        // create a small line to illustrate velocity
        cShapeLine *velocity = new cShapeLine(cVector3d(0,0,0),
                                              cVector3d(0,0,0));
            

        // store pointer to sphere primitive
        spheres[i] = sphere;

        //store pointer to line
        velVectors[i] = velocity;

        // add sphere primitive to world
        world->addChild(sphere);

        world->addChild(velocity);

        // set the position of the object at the center of the world

        if (i != 0) {
            /*if(i == 1)
            {
                sphere->setLocalPos(0.0172897, 0.01, 0.03925);
            }
            else
            { */
            sphere->setLocalPos(0.8 * SPHERE_RADIUS * (double) (i + 4) * cos(1.0 * (double) (i)),
                                0.8 * SPHERE_RADIUS * (double) (i + 4) * sin(1.0 * (double) (i)),
                                .030 + SPHERE_RADIUS + ((double) (i + 4)) / 4000.);
            //}

        }


        double initialposx = 0.8 * SPHERE_RADIUS * (double) (i + 4) * cos(1.0 * (double) (i));
        double initialposy = 0.8 * SPHERE_RADIUS * (double) (i + 4) * sin(1.0 * (double) (i));
        double initialposz = .030 + SPHERE_RADIUS + ((double) (i + 4)) / 4000.;
        //cout << i << " " << initialposx << ", " << initialposy << ", " << initialposz << ", " << endl;


        // set graphic properties of sphere
        sphere->setTexture(texture);
        sphere->m_texture->setSphericalMappingEnabled(true);
        sphere->setUseTexture(true);

        // Set the first sphere (the one being controlled to red initially)
        if (i == 0) {
            sphere->m_material->setRed();
        } else {
            sphere->m_material->setWhite();
        }

    }
    for (int i = 0; i < NUM_SPHERES; i++) {
        cVector3d posA = spheres[i]->getLocalPos();
        for (int j = 0; j < NUM_SPHERES; j++) {
            if (i != j) {
                cVector3d posB = spheres[j]->getLocalPos();
                double distancex = (cDistance(posA, posB)) / .02;
                //cout << i << " and " << j << " distance " << distancex << endl;
            }
        }
    }
    //--------------------------------------------------------------------------
    // WIDGETS
    //--------------------------------------------------------------------------

    // create a font
    font = NEW_CFONTCALIBRI20();

    // create a label to display the haptic and graphic rate of the simulation
    labelRates = new cLabel(font);
    labelRates->m_fontColor.setBlack();
    camera->m_frontLayer->addChild(labelRates);

    // create a label with a small message
    labelMessage = new cLabel(font);
    camera->m_frontLayer->addChild(labelMessage);

    // set font color
    labelMessage->m_fontColor.setBlack();

    // set text message
    labelMessage->setText("interact with magnetic spheres - press user switch to disable magnetic effect");
    //potential energy label
    LJ_num = new cLabel(font);
    camera->m_frontLayer->addChild(LJ_num);
    LJ_num->m_fontColor.setBlack();

    //total energy label
    total_energy = new cLabel(font);
    total_energy->m_fontColor.setBlack();
    camera->m_frontLayer->addChild(total_energy);


    // create a background
    background = new cBackground();
    camera->m_backLayer->addChild(background);

    // set aspect ration of background image a constant
    background->setFixedAspectRatio(true);

    // load background image
    fileload = background->loadFromFile(RESOURCE_PATH("../resources/images/background.png"));
    if (!fileload) {
#if defined(_MSVC)
        fileload = background->loadFromFile("../../../bin/resources/images/background.png");
#endif
    }
    if (!fileload) {
        cout << "Error - Image failed to load correctly." << endl;
        close();
        return (-1);
    }


    //--------------------------------------------------------------------------
    // START SIMULATION
    //--------------------------------------------------------------------------

    // create a thread which starts the main haptics rendering loop
    hapticsThread = new cThread();
    hapticsThread->start(updateHaptics, CTHREAD_PRIORITY_HAPTICS);

    // setup callback when application exits
    atexit(close);


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

    // update position of message label
    labelMessage->setLocalPos((int) (0.5 * (width - labelMessage->getWidth())), 50);
}

//------------------------------------------------------------------------------

void errorCallback(int a_error, const char *a_description) {
    cout << "Error: " << a_description << endl;
}

//------------------------------------------------------------------------------

void keyCallback(GLFWwindow *a_window, int a_key, int a_scancode, int a_action, int a_mods) {
    // filter calls that only include a key press
    if ((a_action != GLFW_PRESS) && (a_action != GLFW_REPEAT)) {
        return;
    }

        // option - exit
    else if ((a_key == GLFW_KEY_ESCAPE) || (a_key == GLFW_KEY_Q)) {
        glfwSetWindowShouldClose(a_window, GLFW_TRUE);
    }

        // option - toggle fullscreen
    else if (a_key == GLFW_KEY_F) {
        // toggle state variable
        fullscreen = !fullscreen;

        // get handle to monitor
        GLFWmonitor *monitor = glfwGetPrimaryMonitor();

        // get information about monitor
        const GLFWvidmode *mode = glfwGetVideoMode(monitor);

        // set fullscreen or window mode
        if (fullscreen) {
            glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
            glfwSwapInterval(swapInterval);
        } else {
            int w = 2. * mode->height;
            int h = 1.5 * mode->height;
            int x = 1.5 * (mode->width - w);
            int y = 1.5 * (mode->height - h);
            glfwSetWindowMonitor(window, NULL, x, y, w, h, mode->refreshRate);
            glfwSwapInterval(swapInterval);
        }
    }

        // option - toggle vertical mirroring
    else if (a_key == GLFW_KEY_M) {
        mirroredDisplay = !mirroredDisplay;
        camera->setMirrorVertical(mirroredDisplay);
    }

}

//------------------------------------------------------------------------------

void close(void) {
    // stop the simulation
    simulationRunning = false;

    // wait for graphics and haptics loops to terminate
    while (!simulationFinished) { cSleepMs(100); }

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
    labelRates->setLocalPos((int) (0.5 * (width - labelRates->getWidth())), 15);


    /////////////////////////////////////////////////////////////////////
    // RENDER SCENE
    /////////////////////////////////////////////////////////////////////

    // update shadow maps (if any)
    world->updateShadowMaps(false, mirroredDisplay);

    // render world
    camera->renderView(width, height);
    /*bool button3;
    hapticDevice-> getUserSwitch(2, button3);
    if(button3)
	{
		if(!button3_changed)
		{
			double rotate= 0.5;
			camera->set(cVector3d(.1 +.5, 0.0, .1 - .5),    // camera position (eye)
                cVector3d(0.0, 0.0, 0.0),    // lookat position (target)
                cVector3d(0.0, 0.0, 0.1));   // direction of the (up) vector
			button3_changed = true;
		}
		
	}
	else
		button3_changed = false; */
    // wait until all GL commands are completed
    glFinish();

    // check for any OpenGL errors
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) cout << "Error: " << gluErrorString(err) << endl;
}

//------------------------------------------------------------------------------

void updateHaptics(void) {
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

    //Array for all velocities
    //cVector3d sphereVel[NUM_SPHERES];
    //cout << sphereVel[NUM_SPHERES] << endl;
    // main haptic simulation loop

    bool button2_changed = false;
    bool button3_changed = false;
    while (simulationRunning) {
        /////////////////////////////////////////////////////////////////////
        // SIMULATION TIME
        /////////////////////////////////////////////////////////////////////

        // stop the simulation clock
        clock.stop();

        // read the time increment in seconds
        double timeInterval = cMin(0.001, clock.getCurrentTimeSeconds());

        // restart the simulation clock
        //clock.reset();
        clock.start();

        // signal frequency counter
        freqCounterHaptics.signal(1);


        /////////////////////////////////////////////////////////////////////////
        // READ HAPTIC DEVICE
        /////////////////////////////////////////////////////////////////////////

        // read position 
        cVector3d position;
        hapticDevice->getPosition(position);
        //Scale position to use more of the screen
        //increase to use more of the screen
        position *= 2.0;
        // read user-switch status (button 0)

        // read linear and angular velocity
        cVector3d linearVelocity;
        hapticDevice->getLinearVelocity(linearVelocity);

        cVector3d angularVelocity;
        hapticDevice->getAngularVelocity(angularVelocity);


        /////////////////////////////////////////////////////////////////////////
        // UPDATE SIMULATION
        /////////////////////////////////////////////////////////////////////////


        // position of walls and ground
        const double WALL_GROUND = 0.0 + SPHERE_RADIUS;
        const double WALL_LEFT = -0.1;
        const double WALL_RIGHT = 0.2;
        const double WALL_FRONT = 0.08;
        const double WALL_BACK = -0.08;
        const double SPHERE_STIFFNESS = 500.0;
        const double SPHERE_MASS = 0.02;
        const double K_DAMPING = 0.001;      //0.996;
        const double K_MAGNET = 500.0;
        const double HAPTIC_STIFFNESS = 1000.0;
        const double SIGMA = 1.0;
        const double EPSILON = 1.0;
        //Scales the distance betweens atoms
        const double DIST_SCALE = .02;
        // clear forces for all spheres
        cVector3d sphereFce[NUM_SPHERES];

        //Update current atom based on if the user pressed the far left button
        //The point of button2_changed is to make it so that it only switches one atom if the button is touched Otherwise it flips out

        bool button;
        hapticDevice->getUserSwitch(0, button);
        bool button2;
        hapticDevice->getUserSwitch(1, button2);
        //bool button3;
        //hapticDevice->getUserSwitch(2, button3);
        bool freebutton;
        hapticDevice->getUserSwitch(3, freebutton);

        bool trackfreeze[NUM_SPHERES];
        //cout << button2_changed << " first" << endl;

        bool button3;
        hapticDevice->getUserSwitch(2, button3);
        
//////////////////////////////////////////////////////////////
        if (button3) {
            if (!button3_changed) {
                //I was trying to make the camera rotate around the cluster of atoms in a cirle but there is still some problems with it
                /*if(theta > 6.283185)
                {
                    theta = 0;
                }
                theta +=  .4;


                //x = cos(theta) * (x - position.get(0)) - sin(theta) * (y - position.get(2)) + position.get(2);
                //y = sin(theta)  * (x - position.get(0)) + cos(theta) * (y  - position.get(2)) + position.get(2);

                x = (cos(theta) * (x)) - (sin(theta) * (y));
                y = (sin(theta)  * (x)) + (cos(theta) * (y));
                cVector3d radius = cVector3d(x, 0.0, y) - cVector3d(0.0,0.0,0.0);
                cout << radius.length() << endl;
                camera->set(cVector3d(x, 0.0 , y), cVector3d(0.0, 0.0, 0.0),cVector3d(0.0, 0.0, 1.0));
                //cout << theta << " " << x << " " << y << endl;*/
                //rotates camera around in square
                switch (curr_camera) {
                    case 1:
                        camera->set(cVector3d(.2, 0.0, -.2), cVector3d(0.0, 0.0, 0.0), cVector3d(0.0, 0.0, 0.1));
                        break;
                    case 2:
                        camera->set(cVector3d(-.2, 0.0, .2), cVector3d(0.0, 0.0, 0.0), cVector3d(0.0, 0.0, 0.1));
                        break;
                    case 3:
                        camera->set(cVector3d(-.2, 0.0, -.2), cVector3d(0.0, 0.0, 0.0), cVector3d(0.0, 0.0, 0.1));
                        break;
                    case 4:
                        camera->set(cVector3d(.2, 0.0, .2), cVector3d(0.0, 0.0, 0.0), cVector3d(0.0, 0.0, 0.1));
                        curr_camera = 0;
                        break;
                }
                curr_camera++;
                button3_changed = true;
            }

        } else
            button3_changed = false;

        // Changes the current atom being controlled when button 1 is pressed 
        if (button2) {

            if (!button2_changed) {
                if (curr_atom == NUM_SPHERES - 1) {
                    cVector3d B = spheres[0]->getLocalPos();
                    curr_atom = 0;
                    // TODO - make sure this works, changes color of the current sphere in this special case
                    // TODO - note: this will only change colors after toggle
                    spheres[curr_atom]->setLocalPos(position);
                    spheres[curr_atom]->m_material->setRed();
                    spheres[NUM_SPHERES - 1]->setLocalPos(B);
                    spheres[NUM_SPHERES - 1]->m_material->setWhite();
                    //curr_atom = curr_atom - NUM_SPHERES + 1;
                    cVector3d translate_last =
                            (spheres[NUM_SPHERES - 1]->getLocalPos()) - (spheres[curr_atom]->getLocalPos());
                    for (int i = 0; i < NUM_SPHERES; i++) {
                        if (i != curr_atom) {

                            if (i == (NUM_SPHERES - 1)) {
                                spheres[i]->setLocalPos(
                                        spheres[NUM_SPHERES - 1]->getLocalPos() - (2.0 * translate_last));
                                cVector3d positions = spheres[i]->getLocalPos();
                                //cout << i << " " << positions  << endl;
                            } else {
                                spheres[i]->setLocalPos(spheres[i]->getLocalPos() - (translate_last));
                                cVector3d positions = spheres[i]->getLocalPos();
                                //cout << i << " " << positions  << endl;
                            }
                        }
                    }
                } else {
                    /*for(int i =0; i < NUM_SPHERES; i++)
                    {
                        cVector3d positions = spheres[i]-> getLocalPos();
                        cout << i << " " << positions  << endl;
                    } */
                    cVector3d A = spheres[curr_atom + 1]->getLocalPos();
                    curr_atom++;
                    // Change the current atom & its color, set the previous one back
                    spheres[curr_atom]->setLocalPos(position);
                    spheres[curr_atom]->m_material->setRed();
                    spheres[curr_atom - 1]->setLocalPos(A);
                    spheres[curr_atom - 1]->m_material->setWhite();
                    cVector3d translate = (spheres[curr_atom - 1]->getLocalPos()) - (spheres[curr_atom]->getLocalPos());
                    for (int i = 0; i < NUM_SPHERES; i++) {
                        if (i != curr_atom) {
                            if (i == (curr_atom - 1)) {
                                spheres[i]->setLocalPos(spheres[curr_atom - 1]->getLocalPos() - (2.0 * translate));
                                cVector3d positions = spheres[i]->getLocalPos();
                                //cout << i << " " << positions  << endl;
                            } else {
                                spheres[i]->setLocalPos(spheres[i]->getLocalPos() - (translate));
                                cVector3d positions = spheres[i]->getLocalPos();
                                //cout << i << " " << positions  << endl;
                            }
                        }
                    }
                }
                button2_changed = true;

            }
        } else
            button2_changed = false;


        for (int i = 0; i < NUM_SPHERES; i++) {
            sphereFce[i].zero();
        }
        // compute forces for all spheres
        double lj_PE = 0;
        for (int i = 0; i < NUM_SPHERES; i++) {

            // compute force on atom
            cVector3d force;
            cVector3d pos0 = spheres[i]->getLocalPos();
            // check forces with all other spheres
            force.zero();

            for (int j = 0; j < NUM_SPHERES; j++) {
                //Don't compute forces between an atom and itself
                if (i != j) {
                    // get position of sphere
                    cVector3d pos1 = spheres[j]->getLocalPos();

                    // compute direction vector from sphere 0 to 1

                    cVector3d dir01 = cNormalize(pos0 - pos1);

                    // compute distance between both spheres
                    double distance = cDistance(pos0, pos1) / DIST_SCALE;


                    //cout << "array " << (sizeof(lj_potential)/sizeof(*lj_potential)) << endl;
                    double lj_potential[NUM_SPHERES];
                    lj_potential[i] = {4 * EPSILON * (pow(SIGMA / distance, 12) - pow(SIGMA / distance, 6))};

                    //for(int i = 0; i < NUM_SPHERES; i++)
                    //{


                    lj_PE = lj_PE + lj_potential[i];

                    //}
                    // update haptic and graphic rate data
                    LJ_num->setText("Potential Energy: " + cStr((lj_PE / 2), 5));

                    // update position of label
                    LJ_num->setLocalPos(650, 500);
                    // Compute LJ forces
                    if (!button) {

                        double lj =
                                -4 * EPSILON * ((-12 * pow(SIGMA / distance, 13)) - (-6 * pow(SIGMA / distance, 7)));
                        force.add(lj * dir01);

                        //cout << distance << " " << i << endl;
                        //cout << i << " " << force << endl;
                    }

                    //if(clock.getCurrentTimeSeconds() > 2)
                    //{

                    sphereFce[i] = force;
                    // update velocity and position of all spheres
                    // compute acceleration
                    cVector3d sphereAcc = (force / SPHERE_MASS);
                    sphereVel[i] = K_DAMPING * (sphereVel[i] + timeInterval * sphereAcc);
        
                    // compute /position
                    //cout << i << " " << sphereVel << endl;
                    cVector3d spherePos_change = timeInterval * sphereVel[i] + cSqr(timeInterval) * sphereAcc;
                    double magnitude = spherePos_change.length();
                    cVector3d spherePos =
                            spheres[i]->getLocalPos() + timeInterval * sphereVel[i] + cSqr(timeInterval) * sphereAcc;
                    if (magnitude > 5) {
                        cout << i << " velocity " << sphereVel[i].length() << endl;
                        cout << i << " force " << force.length() << endl;
                        cout << i << " acceleration " << sphereAcc.length() << endl;
                        cout << i << " time " << timeInterval << endl;
                    }

                    // update value to sphere object
                    double kinetic_energy = .5 * SPHERE_MASS * pow(sphereVel[i].length(), 2);
                    //cout << kinetic_energy << endl;

                    //total_energy->setText("Total Energy: " + cStr(lj_potential + (.5 * SPHERE_MASS * pow(sphereVel[i].length(), 2)), 5));

                    // update position of label
                    total_energy->setLocalPos(650, 480);


                    if (i != curr_atom) {
                        //hapticDevice->getPosition(spherePos);
                        spheres[i]->setLocalPos(spherePos);
                        //cout << "atom " << i << " " << spherePos << endl;
                    }


                    //}
                }


            }

        }
// check forces with ground and walls
/*            
  */
        //spheres[i]->setLocalPos(spherePos);

        //Update info of curr_atom
        //if(clock.getCurrentTimeSeconds() > 5)
        //{
        spheres[curr_atom]->setLocalPos(position);
        //}
        //cout << "current " << position << endl;

        cVector3d force = sphereFce[curr_atom];

        // update force vector arrow
        //velocity->m_pointA = position;
        //velocity->m_pointB = cAdd(position, linearVelocity);
        for (int i = 0; i < NUM_SPHERES; i++){
            velVectors[i]->m_pointA = spheres[i]->getLocalPos();
            //velVectors[i]->m_pointB = cAdd(spheres[i]->getLocalPos(),
            //                               sphereVel[i]);
            
            velVectors[i]->m_pointB = cAdd(spheres[i]->getLocalPos(),
                                           sphereVel[i] * 500);
            
        }
        cout <<  endl;

        //cout << force << endl;
        /////////////////////////////////////////////////////////////////////////
        // APPLY FORCES
        /////////////////////////////////////////////////////////////////////////

        // scale the force according to the max stiffness the device can render

        double stiffnessRatio = 1.0;
        if (hapticDeviceMaxStiffness < HAPTIC_STIFFNESS)
            stiffnessRatio = hapticDeviceMaxStiffness / HAPTIC_STIFFNESS;
        if (force.length() > 10)
            force = 10. * cNormalize(force);
        // send computed force to haptic device


        hapticDevice->setForce(stiffnessRatio * force);

    }

    // close  connection to haptic device
    hapticDevice->close();


    // exit haptics thread
    simulationFinished = true;

}

//------------------------------------------------------------------------------
