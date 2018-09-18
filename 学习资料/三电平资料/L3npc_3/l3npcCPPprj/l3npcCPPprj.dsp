# Microsoft Developer Studio Project File - Name="l3npcCPPprj" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=l3npcCPPprj - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "l3npcCPPprj.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "l3npcCPPprj.mak" CFG="l3npcCPPprj - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "l3npcCPPprj - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "l3npcCPPprj - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "l3npcCPPprj - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "l3npcCPPprj - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ  /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ  /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "l3npcCPPprj - Win32 Release"
# Name "l3npcCPPprj - Win32 Debug"
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\Include\clarke.h
# End Source File
# Begin Source File

SOURCE=..\Include\dmctype.h
# End Source File
# Begin Source File

SOURCE=..\..\DSP281x_headers\include\DSP281x_Adc.h
# End Source File
# Begin Source File

SOURCE=..\..\DSP281x_headers\include\DSP281x_DefaultIsr.h
# End Source File
# Begin Source File

SOURCE=..\..\DSP281x_headers\include\DSP281x_Device.h
# End Source File
# Begin Source File

SOURCE=..\Include\DSP281x_Device.h
# End Source File
# Begin Source File

SOURCE=..\..\DSP281x_headers\include\DSP281x_Ev.h
# End Source File
# Begin Source File

SOURCE=..\..\DSP281x_common\include\DSP281x_GlobalPrototypes.h
# End Source File
# Begin Source File

SOURCE=..\..\DSP281x_headers\include\DSP281x_Gpio.h
# End Source File
# Begin Source File

SOURCE=..\..\DSP281x_headers\include\DSP281x_PieCtrl.h
# End Source File
# Begin Source File

SOURCE=..\..\DSP281x_headers\include\DSP281x_PieVect.h
# End Source File
# Begin Source File

SOURCE=..\..\DSP281x_common\include\DSP281x_SWPrioritizedIsrLevels.h
# End Source File
# Begin Source File

SOURCE=..\..\DSP281x_headers\include\DSP281x_SysCtrl.h
# End Source File
# Begin Source File

SOURCE=..\Include\f281xpwm.h
# End Source File
# Begin Source File

SOURCE=..\Include\f281xpwmdac.h
# End Source File
# Begin Source File

SOURCE=..\Include\ipark.h
# End Source File
# Begin Source File

SOURCE=..\Include\IQmathLib.h
# End Source File
# Begin Source File

SOURCE=..\Include\l3_adc.h
# End Source File
# Begin Source File

SOURCE=..\Include\l3_calc.h
# End Source File
# Begin Source File

SOURCE=..\Include\l3_global.h
# End Source File
# Begin Source File

SOURCE=..\Include\l3_ints.h
# End Source File
# Begin Source File

SOURCE=..\Include\l3_main.h
# End Source File
# Begin Source File

SOURCE=..\Include\park.h
# End Source File
# Begin Source File

SOURCE=..\Include\pid_reg3.h
# End Source File
# Begin Source File

SOURCE=..\Include\svgen_dq.h
# End Source File
# End Group
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\Src\dmc_clarke.c
# End Source File
# Begin Source File

SOURCE=..\Src\dmc_ipark.c
# End Source File
# Begin Source File

SOURCE=..\Src\dmc_park.c
# End Source File
# Begin Source File

SOURCE=..\Src\dmc_pid_reg3.c
# End Source File
# Begin Source File

SOURCE=..\Src\dmc_svgen_dq.c
# End Source File
# Begin Source File

SOURCE=..\Src\DSP281x_DefaultIsr.c
# End Source File
# Begin Source File

SOURCE=..\Src\DSP281x_SysCtrl.c
# End Source File
# Begin Source File

SOURCE=..\Src\l3_adc.c
# End Source File
# Begin Source File

SOURCE=..\Src\l3_calc.c
# End Source File
# Begin Source File

SOURCE=..\Src\l3_ints.c
# End Source File
# Begin Source File

SOURCE=..\Src\l3_main.c
# End Source File
# End Group
# End Target
# End Project
