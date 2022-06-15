
#include "globals.h"
#include <GLFW/glfw3.h>
using namespace std;

int  openGL(GLFWerrorfun errorCallback, cStereoMode stereoMode, GLFWkeyfun keyCallback, GLFWcursorposfun mouseMotionCallback, GLFWmousebuttonfun mouseButtonCallback, GLFWwindowsizefun windowSizeCallback){
  if (!glfwInit()) {
    cout << "failed initialization" << endl;
    cSleepMs(1000);
    return 1;
  }
  // set error callback
  glfwSetErrorCallback(errorCallback);

  // set OpenGL version
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

  // set active stereo mode
  if (stereoMode == C_STEREO_ACTIVE) {
    glfwWindowHint(GLFW_STEREO, GL_TRUE);
  } else {
    glfwWindowHint(GLFW_STEREO, GL_FALSE);
  }

  // compute desired size of window
  const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

  //    const w *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
  int windowWidth = 0.8 * mode->height;
  int windowHeight = 0.5 * mode->height;
  int windowX = 0.5 * (mode->width - windowWidth);
  int windowY = 0.5 * (mode->height - windowHeight);

  // create display context
  window = glfwCreateWindow(windowWidth, windowHeight, "CHAI3D", NULL, NULL);
  if (!window) {
    cout << "failed to create window" << endl;
    cSleepMs(1000);
    glfwTerminate();
    return 1;
  }

  // get width and height of window
  glfwGetWindowSize(window, &width, &height);

  // set position of window
  glfwSetWindowPos(window, windowX, windowY);

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

  return 0;

}