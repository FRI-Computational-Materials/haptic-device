Installing all the Drivers/SDKs/Software:


1. Download Force Dimension drivers for the Novint Falcon from [force dimension](http://www.forcedimension.com/download/sdk). Most recent as of 10/3/18 is SDK 3.7.
2. Run the .exe file to install drivers.
3. Download the CHAI3D libraries from [CHAI3D](http://chai3d.org/download/releases). I think both the multi-platform and Windows versions work, but I got mine to work based off the multi-platform one.
4. If you don’t have it, install the Community version of Visual Studio (for free!) from [visual studio](https://visualstudio.microsoft.com/downloads/?utm_medium=microsoft&utm_source=docs.microsoft.com&utm_campaign=button+cta&utm_content=download+vs2017).


Creating Projects:

1. Follow the whole "Getting Started" part of the "Creating an Application" section found [here](http://chai3d.org/download/doc/html/wrapper-overview.html).

If you plan on creating your own project:

1. Launch Visual Studio and Select File/New/Project.
2. Select the "Wizard" option, type in a name for your project, and make sure to put your project folder in the application/external/CHAI3D/examples/GLFW folder. Make sure to check the "Empty Project" option when prompted.
3. Click on Build/Configuration Manager, and wait for the window to pop up.
4. Click on the drop-down box that says "Active Solution Platform," then <New>, then x64, and then finalize by hitting OK.
5. Click on File/Add/Existing Project, and navigate to application/external/CHAI3D to add the CHAI3D-VS2015.vcxproj to our project.
6. Repeat Step 5 but with the application/external/CHAI3D/extras/freeglut folder, and the freeglut-VS2015.vcxproj instead.
7. Right click on your project name in the Solution Explorer, and click on "Set as Startup Project."
8. Now we set the dependencies - click on Project/Project Dependencies, and select both CHAI3D and freeglut.
9. Now, right click on your project name and hit Add/New Item and select a .cpp file. Name it what you want ~ this is where your source code goes.
10. Time to set Project Properties! Easiest way to do this is to open up the .vcxproj file of any of the examples found in application/external/CHAI3D/examples/GLFW. Open the project in VS, right click on it, and open its properties. You need to copy the following properties from the example project into yours:
    a. Start by setting your configuration and platform to All Configurations and All Platforms at the top of the window.
    b. Copy Configuration Properties/General/Output Directory Intermediate Directory
    c. Copy Configuration Properties/C/C++/General/Additional Include Directories and C/C++/Language/Enable Run-Time Type Information
    d. Copy Configuration Properties/Linker/General/Additional Library Directories, Linker/Input/Additional Dependencies, and Linker/System/Console (Choose the /SUBSYSTEM:CONSOLE option).
11. Preprocessor Directives!
    a. Set your platform to Win32.
    b. In C/C++/Language, edit Processor Definitions, and assign the directive "WIN32."
    c. Set the x64 platform.
    d. in C/C++/Language, edit Processor Definitions and include WIN64.

If you plan on using one of the CHAI3D examples:
1. Follow Steps 1-6 from the individual project instructions.
2. In addition to adding CHAI3D and FreeGLUT, also add the .vcxproj of whatever example project you want to use from application/external/CHAI3D/examples/GLFW.
3. Set the example's project as the Startup Project.
4. You're all set! The Project Properties and Preprocessor Directives should all have been preset to the correct values.
