#include "globals.h"
#include "utility.h"
#include <GLFW/glfw3.h>
#include <sys/stat.h>
#include <fstream>
#include "atom.h"
#include "chai3d.h"

Atom *current;
Atom *previous;
int just_unanchored = 0;

void keyCallback(GLFWwindow *a_window, int a_key, int a_scancode, int a_action,
                 int a_mods) {
  // filter calls that only include a key press
  if ((a_action != GLFW_PRESS) && (a_action != GLFW_REPEAT)) {
    return;
  } else if ((a_key == GLFW_KEY_ESCAPE) || (a_key == GLFW_KEY_Q)) {
    // option - exit
    glfwSetWindowShouldClose(a_window, GLFW_TRUE);
  } else if (a_key == GLFW_KEY_F) {  // option - toggle fullscreen
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
    assert(just_unanchored = 5);
    just_unanchored = 0;
  } else if (a_key == GLFW_KEY_T) {
    // option - save screenshot to file
    cImagePtr image = cImage::create();
    camera->m_frontLayer->removeChild(scope);
    camera->renderView(width, height);
    camera->copyImageBuffer(image);
    camera->m_frontLayer->addChild(scope);
    int index = 0;
    string filename_stem = "screenshot" + to_string(spheres.size()) + "_";
    while (fileExists(filename_stem + to_string(index) + ".png")) {
      index++;
    }
    image->saveToFile(filename_stem + to_string(index) + ".png");
    screenshotCounter = 5000;
  } else if (a_key == GLFW_KEY_SPACE) {  // freeze simulation
    freezeAtoms = !freezeAtoms;
  } else if (a_key == GLFW_KEY_C) {  // save atoms to con file
    ofstream writeFile;
    string dir1 = "./log/";
    struct stat buffer;
    if (stat(dir1.c_str(), &buffer) != 0) { // Check if log directory exists
      char cstr[dir1.size() + 1];
      strcpy(cstr, dir1.c_str());
      mkdir(cstr, 0777);
    }

    // Find local date
    time_t now = time(0);
    tm *ltm = localtime(&now);
    int year = 1900 + ltm -> tm_year;
    int month = 1 + ltm -> tm_mon;
    int day = ltm -> tm_mday;
    string date = to_string(month) + "-" + to_string(day) + "-" + to_string(year);
    string dir2 = dir1 + date + "/";
    if (stat(dir2.c_str(), &buffer) != 0) { // Check if date directory exists
      char cstr[dir2.size() + 1];
      strcpy(cstr, dir2.c_str());
      mkdir(cstr, 0777);
    }
    // Prevent overwriting .con files
    int index = 0;
    while (fileExists(dir2 + "atoms" + to_string(index) + ".con")) {
      index++;
    }
    writeConCounter = 5000;
    writeToCon(dir2 + "atoms" + to_string(index) + ".con");
    cout << "LOGGED AT " + date + " atoms" + to_string(index) + ".con" << endl;
  } else if (a_key == GLFW_KEY_Y) {
    // anchor all atoms while maintaining control
    for (auto i{0}; i < spheres.size(); i++) {
      if (!spheres[i]->isAnchor() && !(spheres[i]->isCurrent())) {
        spheres[i]->setAnchor(true);
      }
    }
  } else if (a_key == GLFW_KEY_UP || a_key == GLFW_KEY_DOWN) {
        int direction = (a_key == GLFW_KEY_UP) ? 1 : -1;
        camera->setSphericalPolarRad(camera->getSphericalPolarRad() +
                                     (M_PI / 50) * direction);

	// normalize between 0 and 2pi
        if (camera->getSphericalPolarRad() < 0) {
            camera->setSphericalPolarRad(fmod(camera->getSphericalPolarRad(), (2 * M_PI)) + 2 * M_PI);
	} else if (camera->getSphericalPolarRad() > 2 * M_PI) {
	  camera->setSphericalPolarRad(fmod(camera->getSphericalPolarRad(), (2 * M_PI)));
	  }

        updateCameraLabel(camera_pos, camera);
    } else if (a_key == GLFW_KEY_RIGHT || a_key == GLFW_KEY_LEFT) {
        int direction = (a_key == GLFW_KEY_RIGHT) ? 1 : -1;
        camera->setSphericalAzimuthRad(camera->getSphericalAzimuthRad() +
                                       (M_PI / 50) * direction);

	// normalize between 0 and 2pi
        if (camera->getSphericalAzimuthRad() < 0) {
            camera->setSphericalAzimuthRad(fmod(camera->getSphericalAzimuthRad(), (2 * M_PI)) + 2 * M_PI);
	} else if (camera->getSphericalAzimuthRad() > 2 * M_PI) {
	  camera->setSphericalAzimuthRad(fmod(camera->getSphericalAzimuthRad(), (2 * M_PI)));
	  }

        updateCameraLabel(camera_pos, camera);

  } else if (a_key == GLFW_KEY_LEFT_BRACKET ||
             a_key == GLFW_KEY_RIGHT_BRACKET) {
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
  } else if ((a_key == GLFW_KEY_W) || (a_key == GLFW_KEY_S) || (a_key == GLFW_KEY_A) 
      || (a_key == GLFW_KEY_D) || (a_key == GLFW_KEY_Z) || (a_key == GLFW_KEY_X)) {
      // Move current atom
      // Identify current atom
      int curr_atom = 0;
      for(int i = 0; i < spheres.size(); i++){
        if (spheres[i] -> isCurrent()){
	  curr_atom = i;
	  }
	}
      current = spheres[curr_atom];

      // Get current position, direction, and vectors
      cVector3d A = current->getLocalPos();
      double x_vect, y_vect, z_vect, direction;
      direction = ((a_key == GLFW_KEY_Z) || (a_key == GLFW_KEY_W) || (a_key == GLFW_KEY_A)) ? -0.005 : 0.005;
      // Define vectors by direction choice
      // Z/X is in/out, W/S is up/down, and A/D is left/right
      if ((a_key == GLFW_KEY_Z) || (a_key == GLFW_KEY_X)){
	x_vect = rho * sin(camera->getSphericalPolarRad()) * cos(camera->getSphericalAzimuthRad()) ;
	y_vect = rho * sin(camera->getSphericalPolarRad()) * sin(camera->getSphericalAzimuthRad());
	z_vect = rho * cos(camera->getSphericalPolarRad());
      } else if ((a_key == GLFW_KEY_W) || (a_key == GLFW_KEY_S)){
	x_vect = rho * sin(camera->getSphericalPolarRad() + M_PI / 2) * cos(camera->getSphericalAzimuthRad()) ;
	y_vect = rho * sin(camera->getSphericalPolarRad() + M_PI / 2) * sin(camera->getSphericalAzimuthRad());
	z_vect = rho * cos(camera->getSphericalPolarRad() + M_PI / 2);
      } else if ((a_key == GLFW_KEY_A) || (a_key == GLFW_KEY_D)){
	x_vect = rho * sin(camera->getSphericalPolarRad()) * cos(camera->getSphericalAzimuthRad() + 3 * M_PI / 2) ;
	y_vect = rho * sin(camera->getSphericalPolarRad()) * sin(camera->getSphericalAzimuthRad() + 3 * M_PI / 2);
	z_vect = 0;
	if (camera->getSphericalPolarRad() == 0){
	  y_vect = camera->getSphericalRadius();
	} else if (camera->getSphericalPolarRad() < M_PI){
	  direction *= -1;
	  }
	}

      // Move atom accordingly
      current->setLocalPos(direction * x_vect + A.x(), direction * y_vect + A.y(), direction * z_vect + A.z());
  }else if(a_key == GLFW_KEY_LEFT_CONTROL || a_key == GLFW_KEY_RIGHT_CONTROL){
      helpPanel->setShowPanel(!helpPanel->getShowPanel());
      if(helpPanel->getShowPanel()){
        camera->m_frontLayer->addChild(helpHeader);
        for(int i = 0; i < hotkeyKeys.size(); i++){
          camera->m_frontLayer->addChild(hotkeyKeys[i]);
          camera->m_frontLayer->addChild(hotkeyFunctions[i]);
        }
      }else{
        camera->m_frontLayer->removeChild(helpHeader);
        for(int i = 0; i < hotkeyKeys.size(); i++){
          camera->m_frontLayer->removeChild(hotkeyKeys[i]);
          camera->m_frontLayer->removeChild(hotkeyFunctions[i]);
        }
      }
  }
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
        double factor = (distanceToObjectPlane * tan(0.5 *
                        camera->getFieldViewAngleRad())) / (0.5 * height);
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

void mouseButtonCallback(GLFWwindow *a_window, int a_button, int a_action,
                         int a_mods) {
    // store mouse position and current atom position
    double x, y;
    current = spheres[0];
    cVector3d A = current->getLocalPos();

    // detect for any collision between mouse and scene
    cCollisionRecorder recorder;
    cCollisionSettings settings;
    if (a_button == GLFW_MOUSE_BUTTON_LEFT && a_action == GLFW_PRESS) {
	int curr_atom = 0;
        int previous_curr_atom;
        glfwGetCursorPos(window, &x, &y);
        bool hit =
        camera->selectWorld(x, (height - y), width, height, recorder, settings);
        if (hit) {
            cGenericObject *selected = recorder.m_nearestCollision.m_object;
            selectedAtom = (Atom *)selected;

	    // Change selected atom to current atom if not anchor
            if (!selectedAtom->isAnchor() && !selectedAtom->isCurrent()) {
		for(int i = 0; i < spheres.size(); i++){
                    if (spheres[i] -> isCurrent()){
			previous_curr_atom = i;
		    } else if (spheres[i] == selectedAtom){
			curr_atom = i;
			}
		}
	        current = spheres[curr_atom];
            	previous = spheres[previous_curr_atom];
		cVector3d A = previous->getLocalPos();

		previous->setCurrent(false);
		previous->setLocalPos(A); 
		current->setCurrent(true);

		selectedAtom->setCurrent(true);

	    // If selected atom is an anchor allow for it to be moved
            } else if (selectedAtom->isAnchor()){
	      selectedPoint = recorder.m_nearestCollision.m_globalPos;
              selectedAtomOffset =
              recorder.m_nearestCollision.m_globalPos - selectedAtom->getLocalPos();
	      }
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
            } else if (!selectedAtom->isCurrent()) {  // cannot set current to anchor
                selectedAtom->setAnchor(true);
            }
            mouseState = MOUSE_SELECTION;
        }
    } else {
        mouseState = MOUSE_IDLE;
    }
}
