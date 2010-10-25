This is the Readme.txt file for the AppSecure program.

This C++ GUI is written to work with the AppSecure card applet included in the source code submitted for egate. It should be loaded on the card with an applet AID of 010203040506 and a size of 650.

This project was written with Visual C++ version 6.0.

The Project->Settings->C/C++->Preprocessor->Additional Include Directories points to the include files for slbIop4 and slbScu.

The Project->Settings->Link->Object/Library modules references the location of slbiop4.lib, slbscu.lib, and winscard.lib

Instructions:-

Run AppSecure.exe, connect to the egate card with the AppSecure card applet loaded onto it. Also instantiate the applet with AID 010203040506.

Choose the reader and click on "Connect" button.

If everything was fine then a new button "AppSecure..." appears in the middle of the screen. Click on it and select an .EXE file to protect. Now a file called "encryptkey.dat" and the secure file "SecureFile.EXE" is created in the same directory. The original EXE is left alone and not meddled with.

Now try running SecureFile.EXE, an illegal exception occure because there is no code section in that file. 

Now the application is secure. Copy Loader32.EXE to the same directory as SecureFile.EXE and encryptkey. Insert the card which was used to store the code section and run loader32.exe and choose the file "SecureFile.EXE". Now as though by magic the application runs:-)

Regards.
Deepak Prabhakara.
