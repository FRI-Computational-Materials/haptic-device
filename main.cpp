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
#include "globals.h"
#include "declared_vars.h"
#include "inputHandling.h"
#include "potentials.h"
#include "utility.h"
#include "PyAMFF/PyAMFF.h"
#include "boundaryConditions.h"
#include "optimizer.h"
#include "defaultArg.h"
#include "openGL.h"

//#include "button.h"
//------------------------------------------------------------------------------
#include <GLFW/glfw3.h>
#include <python3.8/Python.h>
#include <math.h>
#include <unistd.h>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
//------------------------------------------------------------------------------
using namespace chai3d;
using namespace std;

int main(int argc, char *argv[]) {
  //--------------------------------------------------------------------------
  // INITIALIZATION
  //--------------------------------------------------------------------------
  cout << endl;
  cout << "-----------------------------------" << endl;
  cout << "CHAI3D" << endl;
  cout << "Press CTRL for help" << endl;
  cout << "-----------------------------------" << endl << endl << endl;
  cout << endl << endl;
  // seed random
  srand(time(NULL));
  //--------------------------------------------------------------------------
  // OPEN GL - WINDOW DISPLAY
  //--------------------------------------------------------------------------
  // initialize GLFW library
  openGL(errorCallback, stereoMode, keyCallback, mouseMotionCallback, mouseButtonCallback, windowSizeCallback);
  
#ifdef GLEW_VERSION // I feel like these next few lines can be placed in openGL.h but I'm not confident in that and don't have a way to see if moving would break it
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
  //worldCamLight(world, stereoMode, light);

  
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

  // Declare variables needed for calculator constructor (cell, pbc), atoms object (mass, atomic number), and placing of initial atoms (positions)
  // either no arguments were given or argument was an integer
  if (argc == 1 || isNumber(argv[1])) {
    defaultArg(argc, argv, world, texture);
  } else {  // read in specified file
    //pepperoni(argv, world, texture);

    // Begin python instance and set path
    Py_Initialize();
    PyRun_SimpleString("import sys\nsys.path.append('../../haptic-device/')\n");

    // Create variables for the function and module name, return tuple object, and
    PyObject *pName, *pModule, *pFunc, *pFileName, *pCallTuple;
    PyObject *pResult;
    std::vector<std::vector<float>> positions;
    std::vector<int> startingAtomicNrs;
    int nAtoms;

    // Import python module
    pName = PyUnicode_FromString("ase_file_io");
    pModule = PyImport_Import(pName);

    // return an error if PyImport_Import can't find the pModule
    if (pModule != NULL) {
      // get function
      pFunc = PyObject_GetAttrString(pModule, "get_state_information");

      if (pFunc && PyCallable_Check(pFunc)) {
        // Send filename to function. pResult is a python tuple that will be unpacked to get out values that we want
        pFileName = PyUnicode_FromString(argv[1]);
        pCallTuple = PyTuple_New(1); // this is because CallObject requires that you pass in a tuple containing the data you want
        PyTuple_SetItem(pCallTuple, 0, pFileName);
        pResult = PyObject_CallObject(pFunc, pCallTuple);

        // get number of atoms
        nAtoms = (int)PyLong_AsLong(PyList_GetItem(pResult, 0));

        // Unpack Positions and extract values
        for (int i = 0; i < nAtoms; i++) {
          // unpack positions
          positions.push_back(std::vector<float>({
            PyFloat_AsDouble(PyList_GetItem(pResult, 3 * i + 1)),
            PyFloat_AsDouble(PyList_GetItem(pResult, 3 * i + 2)),
            PyFloat_AsDouble(PyList_GetItem(pResult, 3 * i + 3))
          }));
          // unpack atomic numbers
          startingAtomicNrs.push_back((int)PyLong_AsLong(PyList_GetItem(pResult, 3*nAtoms + 1 + i)));
        }

        // decref everything (this breaks our python instance. Will try to implement correctly later)
        //Py_DECREF(pName);
        //Py_DECREF(pModule);
        //Py_DECREF(pResult);
        //Py_DECREF(pFunc);
        //Py_DECREF(pFileName);
        //Py_DECREF(pCallTuple);

        // end python instance (doing this caused a segfault)
        //Py_FinalizeEx(); //As of 05/26/2022, the decrefs and ending python instance are not causing any obervable issues - Sydney :)
      }
    } else {
      std::cout << "Error: module not found" << std::endl;
    }

    // create atoms objects, put them in spheres, world
    for (int i = 0; i < nAtoms; i++) {
      // Create atom pointer
      Atom* newAtom = new Atom(SPHERE_RADIUS, startingAtomicNrs[i]);

      // store pointer to sphere primitive
      spheres.push_back(newAtom);

      // add sphere primitive to world
      world->addChild(newAtom);

      // add line to world
      world->addChild(newAtom->getVelVector());

      // set graphic properties of sphere
      newAtom->setTexture(texture);
      newAtom->m_texture->setSphericalMappingEnabled(true);
      newAtom->setUseTexture(true);

      // Set the positions of all atoms
      if (i == 0) {
        // make very first atom the current atom
        newAtom->setCurrent(true);
        // get coordinates from PositionTriplet
        for (int j = 0; j < 3; j++) {
          centerCoords[j] = positions[0][j];
        }
        // set first atom at center of view
        newAtom->setLocalPos(0.0, 0.0, 0.0);
      } else {
        // Anchor by default
        newAtom->setAnchor(true);
        // scale coordinates and insert
        newAtom->setLocalPos(
          0.02 * (positions[i][0] - centerCoords[0]), // position offset -- should probably disappear once we get boxes working
          0.02 * (positions[i][1] - centerCoords[1]),
          0.02 * (positions[i][2] - centerCoords[2])
        );
      }    
    }
  }
  // Done reading any sort of info. Now setting up calculator selection
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
    if (arg == "morse" || arg == "m") {
      energySurface = MORSE;
      calculatorPtr = new morseCalculator();
    }else if (arg == "pyamff" || arg == "p"){
      energySurface = PYAMFF;
      //calculatorPtr = new pyamffCalculator(spheres.size());
      calculatorPtr = new pyamffCalculator(spheres);
    }else if (arg == "ase" || arg == "a"){

      // These are placeholders for the moment
      energySurface = ASE;
      int atomicNums[1] = {1};
      const double box[1] = {1.0};
      std::string stry = "";
      calculatorPtr = new aseCalculator(stry, atomicNums, box);
    }
    else if (arg == "lennard-jones" || arg == "lj") {
      calculatorPtr = new ljCalculator();
    }
  }
  else {
    calculatorPtr = new ljCalculator();
  }
  //--------------------------------------------------------------------------
  // WIDGETS
  //--------------------------------------------------------------------------
  // create a label to display the haptic and graphic rate of the simulation
  addLabel(labelRates);

  // potential energy label
  addLabel(potE_label);

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

  addLabel(writeConLabel);
  addLabel(screenshotLabel);

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
  global_minimum = getGlobalMinima(spheres.size()); // If all hydrogens, use get global minima, else unknown
  for (int i = 0; i < spheres.size(); i++) {
    if (spheres[i]->getAtomicNumber() != 1) {
      global_minimum = 0;
      break;
      }
    }
  double lower_bound, upper_bound;
  if (global_minimum != 0){// && energySurface == LENNARD_JONES){
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
  //scope_upper->m_fontColor.setBlack();
  //scope_lower->m_fontColor.setBlack();
  cColorf panelColor = cColorf();
  // BlueCadet
  panelColor.setBlueCadet();

  helpPanel = new cPanel();
  helpPanel->setColor(panelColor);
  helpPanel->setSize(520, 500);
  camera->m_frontLayer->addChild(helpPanel);
  helpPanel->setShowPanel(false);

  cFontPtr headerFont = NEW_CFONTCALIBRI40();
  helpHeader = new cLabel(headerFont);
  helpHeader->m_fontColor.setBlack();
  helpHeader->setText("HOTKEYS AND INSTRUCTIONS");
  helpHeader->setShowPanel(false);

  // create hotkey labels
  addHotkeyLabel("f", "toggle fullscreen");
  addHotkeyLabel("q, ESC", "quit program");
  addHotkeyLabel("y", "anchor all atoms");
  addHotkeyLabel("u", "unanchor all atoms");
  addHotkeyLabel("w/a", "move current atom up/down");
  addHotkeyLabel("s/d", "move current atom left/right");
  addHotkeyLabel("z/x", "move current atom in/out");
  addHotkeyLabel("ARROW KEYS", "rotate camera");
  addHotkeyLabel("RIGHT CLICK", "anchor/unanchor atom");
  addHotkeyLabel("LEFT CLICK", "change selected atom");
  addHotkeyLabel("[", "zoom in");
  addHotkeyLabel("]", "zoom out");
  addHotkeyLabel("r", "reset camera");
  addHotkeyLabel("t", "screenshot atoms");
  addHotkeyLabel("c", "save configuration to .con");
  addHotkeyLabel("SPACE", "freeze atoms");
  addHotkeyLabel("CTRL", "toggle help panel");

  //
  screenshotLabel->setText("Screenshot taken");
  writeConLabel->setText("Con file written");

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
  } else if (energySurface == ASE) {
    potentialName = "ASE";
  } else if (energySurface == PYAMFF) {
    potentialName = "PyAMFF";
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

void windowSizeCallback(GLFWwindow *a_window, int a_width, int a_height) {
  // update window size
  width = a_width;
  height = a_height;
}

void errorCallback(int a_error, const char *a_description) {
  cout << "Error: " << a_description << endl;
}

void close(void) {  // stop the simulation
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
//-----------------------------------------------------------------------------
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
  //char conn;
  //cout << "Haptic device connected? (y/n)" << endl;
  //cin >> conn;
  //bool connection = ((conn == 'y') || (conn == 'Y')) ? true : false;

  while (simulationRunning) {
    /////////////////////////////////////////////////////////////////////
    // SIMULATION TIME
    /////////////////////////////////////////////////////////////////////
    // stop the simulation clock
    clock.stop();

    // read the haptic refresh rate and scale it to milliseconds
    double time = freqCounterHaptics.getFrequency() / 1000000;
    // cout << time << endl;
    // set the time interval for the simulation based on refresh time
    double timeInterval = cMin(time, clock.getCurrentTimeSeconds());

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
    } else {
      button2_changed = false;
      }

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

    screenshotLabel->setLocalPos(5, height - 20);
    if (screenshotCounter == 5000) {
      camera->m_frontLayer->addChild(screenshotLabel);
      screenshotCounter--;
    } else if (screenshotCounter > 0) {
      screenshotCounter--;
    } else if (screenshotCounter == 0) {
      camera->m_frontLayer->removeChild(screenshotLabel);
      screenshotCounter--;
    } else if (screenshotCounter == -2) {
      camera->m_frontLayer->removeChild(screenshotLabel);
    }

    writeConLabel->setLocalPos(5, height - 40);
    if(writeConCounter == 5000){
        camera->m_frontLayer->addChild(writeConLabel);
        writeConCounter--;
    }else if(writeConCounter > 0){
        writeConCounter--;
    }else if(writeConCounter == 0){
        camera->m_frontLayer->removeChild(writeConLabel);
        writeConCounter--;
    }else if(writeConCounter == -2){
        camera->m_frontLayer->removeChild(writeConLabel);
    }


    if (!freezeAtoms) {
      optimize(current, timeInterval, curr_atom, position, potE_label, num_anchored,
               flagHapticsEnabled, global_minimum, global_min_known, scope_upper, scope_lower, clock);
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
    if (hapticDeviceMaxStiffness < HAPTIC_STIFFNESS) {
      stiffnessRatio = hapticDeviceMaxStiffness / HAPTIC_STIFFNESS;
    }
    if (force.length() > 10) {
      force = 10. * cNormalize(force);
    }
    // send computed force to haptic device
    hapticDevice->setForce(stiffnessRatio * force);
  }
  // close  connection to haptic device
  hapticDevice->close();

  // exit haptics thread
  simulationFinished = true;

  // Close the calculator
  delete calculatorPtr;
}
