# haptic-device
Code for the FRI haptic device project

## Build Instructions

### Windows

### Linux
1. Download the multiplatform release from [chai3d](http://www.chai3d.org/download/releases)
2. Install the prereqs
    * libusb-1.0-0-dev
    * libasound2-dev
    * freeglut3-dev
    * xorg-dev
3. Unzip the sdk and build using  `cd chai3d-3.2.0 && make`
4. Clone this repo into the chai3d-3.2.0 directory
   * `git clone https://github.com/styu99/haptic-device.git`
5. Build LJ-test with `cd haptic-device && make`
6. Navigate to `chai3d-3.2.0/bin/lin-x86_64`
7. Run as root: `sudo ./haptic device`
8. To get the haptic device working you may need to adding rules to udev
    * `sudo cp chai3d-3.2.0/external/DHD/doc/linux/51-forcedimension.rules /etc/udev/rules.d`
 


## Reference
The textbook is too big to upload so here's the link: http://www.charleshouserjr.com/Cplus2.pdf


## Notes

* The buttons are labeled 0-3, starting at the center and going clockwise for user switches
* Button naming convention in LJ-test.cpp (example = name in LJ-test.cp
* p)
    * button 0 = button
        * does nothing
    * button 1 = button2
        * this button changes the current atom being used
    * button3 = freebutton
        * also does nothin
    * button2  = button3
        * this changes the camera position
* e