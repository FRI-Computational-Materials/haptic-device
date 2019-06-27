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

Windows release has been halted, development all on Linux now ~

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
        * also does nothing
    * button2  = button3
        * this changes the camera position
* Keyboard hotkeys:
    * `q` or `ESC`
        * quit program
    * `f`
        * toggle fullscreen
    * `u`
        * unanchor all atoms
    * `s`
        * screenshot atomic configuration without graph
    * `SPACE`
        * freeze atom movement
    * `c`
        * save configuration to .con file
    * `a`
        * anchor all atoms     
    * `ARROW KEYS`
        * move camera
    * `[` and `]`
        * zoom in/out
    * `r`
        * reset camera
