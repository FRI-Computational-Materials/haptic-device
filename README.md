# haptic-device
Code for the FRI haptic device project

## Build Instructions

### Windows
FOLLOW THESE STEPS TO INSTALL HAPTIC DEVICE SOFTWARE ON WINDOWS

1. Download Force Dimension drivers for the Novint Falcon from [force dimension](http://www.forcedimension.com/download/sdk). Most recent as of 10/3/18 is SDK 3.7.
2. Run the .exe file to install drivers.
3. Download the CHAI3D libraries from [CHAI3D](http://chai3d.org/download/releases). I think both the multi-platform and Windows versions work, but I got mine to work based off the multi-platform one.
4. If you don’t have it, install the Community version of Visual Studio (for free!) from [visual studio](https://visualstudio.microsoft.com/downloads/?utm_medium=microsoft&utm_source=docs.microsoft.com&utm_campaign=button+cta&utm_content=download+vs2017).
5. SUPER IMPORTANT: Follow all of the instructions in the CHAI3D documentation for setting things up at [force dimension tutorial]( http://chai3d.org/download/doc/html/wrapper-overview.html). Go to the section titled ‘Creating an Application.’
6. NOTE: For Visual Studio 2017, there is an issue with making an ‘Empty Project’ as they removed the option in this version. Instead, select the Wizard and check the Empty option when the prompt appears.



### Linux
1. Download the multiplatform release from [chai3d](http://www.chai3d.org/download/releases)
2. Install the prereqs
    * libusb-1.0-0-dev
    * libasound2-dev
    * freeglut3-dev
    * xorg-dev
3. **TODO** - setup directory structure 


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