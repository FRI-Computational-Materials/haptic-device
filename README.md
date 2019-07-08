# haptic-device
Code for the FRI haptic device project

**NOTE:** When launching the program, you may need to calibrate the device by pushing it in and out until it is recognized.

## OPTIONS
Specify the # of atoms at launch like so:
```
./haptic-device 38
```
If you don't pass in anything, the default is five.

You can also read in an existing configuration:
```
./haptic-device example.con
```
Make sure the .con file is in ../resources/data.

Choose the potential energy surface by adding a second argument:
```
./haptic-device 25 morse
```
The default is Lennard-Jones.

## Build Instructions

### Windows

Windows development temporarily halted (unable to compile on Visual Studio).
Check WINDOWS.md for details on installation.


### Linux
1. Download the multiplatform release from [chai3d](http://www.chai3d.org/download/releases)
2. Run the following commands
   ```
   sudo add-apt-repository universe
   sudo apt update
   ```
3. Install the following packages using the command
    ```
    sudo apt-get install <package_name>
    ```
    * libusb-1.0-0-dev
    * libasound2-dev
    * freeglut3-dev
    * xorg-dev
4. Run "make" in the chai3d-3.2.0 directory
5. Clone this repo into the chai3d-3.2.0 directory
6. Create the directory `data` in `bin/resources` and move the file `global_minima.txt` to here.
7. Run `make` in the `haptic-device` folder - be sure to do this after any changes.
8. Your directory structure should look like so:
<pre>
chai3d-3.2.0/
├── bin
│   ├── lin-x86_64
│   ├── mac-i386
│   ├── mac-x86_64
│   ├── resources
│       └── <b>data</b>
│           └── <b>global_minima.txt</b>
│
│   ├── win-Win32
│   └── win-x64
└── <b>haptic-device</b>
    ├── <b>LJ.cpp</b>
    ├── <b>Makefile</b>
    ├── <b>obj</b>
    └── <b>README.md</b>
</pre>

9. Note that you may to change lines involving the relative file path
```c++
bool fileload = texture->loadFromFile(RESOURCE_PATH("../resources/images/spheremap-3.jpg"));
```
to the absolute file path.

10. Run the following commands while in chai3d-3.20/:
  * `sudo cp ./external/DHD/doc/linux/51-forcedimension.rules /etc/udev/rules.d`
  * `sudo udevadm control --reload-rules && udevadm trigger`

10. Navigate to `lin-x86_64` and run `./haptic-device` to launch the program.



### MacOS
1. Download the latest release of CHAI3D for Mac OS X from [chai3d](http://www.chai3d.org/download/releases)
2. Make sure you have XCode downloaded, and follow instructions from the file entitled "getting-started.html" located in the doc folder of chai3d
3. Copy over all of the files from haptic-device into one of the CHAI3D examples, and rename LJ.cpp to to the same name of the .cpp file already in the CHAI3D example, such as "01-mydevice.cpp". If you're getting an error, make sure that you don't have a dupicate LJ.cpp file.
4. Run the example.


## Reference
The textbook is too big to upload so here's the link: http://www.charleshouserjr.com/Cplus2.pdf


## Notes

* The buttons are labeled 0-3, starting at the center and going clockwise for user switches
* Button naming convention in LJ-test.cpp (example = name in LJ-test.cpp)
    * button 0 = button
        * No function.
    * button 1 = button2
        * Changes the current atom being controlled by the haptic device.
    * button 2  = button3
        * Changes the camera position.
    * button 3 = freebutton
        * No function.
* Keyboard hotkeys:
    * `q` or `ESC`
        * Quit program
    * `f`
        * Toggle full screen
    * `u`
        * Un-anchor all atoms
    * `s`
        * Screenshot atomic configuration without graph
    * `SPACE`
        * Freeze atom movement
    * `c`
        * Save configuration to .con file
    * `a`
        * Anchor all atoms     
    * `ARROW KEYS`
        * Move camera
    * `[` and `]`
        * Zoom in/out
    * `r`
        * Reset camera
