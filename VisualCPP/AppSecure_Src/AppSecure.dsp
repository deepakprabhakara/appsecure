# Microsoft Developer Studio Project File - Name="AppSecure" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=AppSecure - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AppSecure.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AppSecure.mak" CFG="AppSecure - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AppSecure - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "AppSecure - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AppSecure - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_DEBUG" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\Middleware Interfaces\slbiop" /I "..\..\..\Middleware Interfaces\slbscu" /I "C:\Program Files\Schlumberger\Smart Cards and Terminals\Cyberflex Access Kits\v4\Middleware Interfaces\slbiop" /I "C:\Program Files\Schlumberger\Smart Cards and Terminals\Cyberflex Access Kits\v4\Middleware Interfaces\slbscu" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_DEBUG" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "NDEBUG" /D "_DEBUG" /mktyplib203 /win32 /win32
# ADD MTL /nologo /D "NDEBUG" /D "_DEBUG" /mktyplib203 /win32 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL" /d "_DEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 slbiop4.lib slbscu.lib winscard.lib slbiop4.lib slbscu.lib winscard.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"..\..\..\Middleware Interfaces\slbiop\WIN2K_XP" /libpath:"..\..\..\Middleware Interfaces\slbscu" /libpath:"C:\Program Files\Schlumberger\Smart Cards and Terminals\Cyberflex Access Kits\v4\Middleware Interfaces\slbiop\WIN2K_XP" /libpath:"C:\Program Files\Schlumberger\Smart Cards and Terminals\Cyberflex Access Kits\v4\Middleware Interfaces\slbscu"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "AppSecure - Win32 Debug"

# PROP Use_MFC 2
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "C:\Program Files\Schlumberger\Smart Cards and Terminals\Cyberflex Access Kits\v4\Middleware Interfaces\slbiop" /I "C:\Program Files\Schlumberger\Smart Cards and Terminals\Cyberflex Access Kits\v4\Middleware Interfaces\slbscu" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /Fp"Debug/SimpleString.pch" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /d "_AFXDLL"
BSC32=bscmake.exe
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 slbiop4.lib slbscu.lib winscard.lib /nologo /subsystem:windows /incremental:yes /debug /machine:I386 /pdbtype:sept /libpath:"C:\Program Files\Schlumberger\Smart Cards and Terminals\Cyberflex Access Kits\v4\Middleware Interfaces\slbiop\WIN2K_XP" /libpath:"C:\Program Files\Schlumberger\Smart Cards and Terminals\Cyberflex Access Kits\v4\Middleware Interfaces\slbscu"
# SUBTRACT LINK32 /pdb:none /nodefaultlib

!ENDIF 

# Begin Target

# Name "AppSecure - Win32 Release"
# Name "AppSecure - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AppProtector.cpp
# End Source File
# Begin Source File

SOURCE=.\AppSecure.cpp
# End Source File
# Begin Source File

SOURCE=.\AppSecure.rc
# End Source File
# Begin Source File

SOURCE=.\AppSecureDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AppProtector.h
# End Source File
# Begin Source File

SOURCE=.\AppSecure.h
# End Source File
# Begin Source File

SOURCE=.\AppSecureDlg.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\butterfly.ico
# End Source File
# Begin Source File

SOURCE=.\res\SimpleString.ico
# End Source File
# Begin Source File

SOURCE=.\res\SimpleString.rc2
# End Source File
# End Group
# End Target
# End Project
