#ifndef OPENGL_H
#define OPENGL_H

#include <GLFW/glfw3.h>
using namespace std;

int  openGL(GLFWerrorfun errorCallback, cStereoMode stereoMode, GLFWkeyfun keyCallback, GLFWcursorposfun mouseMotionCallback, GLFWmousebuttonfun mouseButtonCallback, GLFWwindowsizefun windowSizeCallback);
#endif