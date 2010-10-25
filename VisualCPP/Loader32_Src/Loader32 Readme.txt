This is the Readme.txt file for the Loader32 program.

This program is written to work with the AppSecure card applet included with the submitted source code. It should be loaded on the card with an applet AID of 010203040506 and a size of 650.

This project was written with Visual C++ version 6.0.

The Project->Settings->C/C++->Preprocessor->Additional Include Directories
points to the include files for slbIop4 and slbScu.

The Project->Settings->Link->Object/Library modules references the location of slbiop4.lib, slbscu.lib, and winscard.lib

Instructions:-
The Loader32.exe file is used to load the protected executable. Just run Loader32.exe, insert the egate card which is loaded with the card applet and click on connect. The application is automatically run.
