#include <GLFW/glfw3.h>

// callback when a key is pressed
void keyCallback(GLFWwindow *a_window, int a_key, int a_scancode, int a_action,
                 int a_mods);
                 
// callback to handle mouse click
void mouseButtonCallback(GLFWwindow *a_window, int a_button, int a_action,
                         int a_mods);

// callback to handle mouse motion
void mouseMotionCallback(GLFWwindow *a_window, double a_posX, double a_posY);
