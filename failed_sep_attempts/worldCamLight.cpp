#include "globals.h"
using namespace std;
using namespace chai3d;

#include "atom.h"
#include "chai3d.h"
//#include "globals.h"
#include "inputHandling.h"
#include "potentials.h"
#include "utility.h"
//#include "PyAMFF/PyAMFF.h"
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

void  worldCamLight(cWorld *world, cStereoMode stereoMode, cSpotLight *light){
  // create a new world.
  cout << "Test!" << endl;
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
}
