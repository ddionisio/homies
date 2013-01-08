# Microsoft Developer Studio Project File - Name="homies" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=homies - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "homies.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "homies.mak" CFG="homies - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "homies - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "homies - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "homies - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "homies - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib dxguid.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "homies - Win32 Release"
# Name "homies - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Main"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\homies.cpp
# End Source File
# Begin Source File

SOURCE=.\homies_cfg.cpp
# End Source File
# Begin Source File

SOURCE=.\homies_init.cpp
# End Source File
# Begin Source File

SOURCE=.\homies_kill.cpp
# End Source File
# Begin Source File

SOURCE=.\homies_loop.cpp
# End Source File
# Begin Source File

SOURCE=.\homies_util.cpp
# End Source File
# Begin Source File

SOURCE=.\mywin.cpp
# End Source File
# End Group
# Begin Group "Procs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\homiesProc_edit.cpp
# End Source File
# Begin Source File

SOURCE=.\homiesProc_editMap.cpp
# End Source File
# Begin Source File

SOURCE=.\homiesProc_editNew.cpp
# End Source File
# Begin Source File

SOURCE=.\homiesProc_game.cpp
# End Source File
# Begin Source File

SOURCE=.\homiesProc_gameover.cpp
# End Source File
# Begin Source File

SOURCE=.\homiesProc_gothiscore.cpp
# End Source File
# Begin Source File

SOURCE=.\homiesProc_lose.cpp
# End Source File
# Begin Source File

SOURCE=.\homiesProc_mainmenu.cpp
# End Source File
# Begin Source File

SOURCE=.\homiesProc_topten.cpp
# End Source File
# Begin Source File

SOURCE=.\homiesProc_win.cpp
# End Source File
# Begin Source File

SOURCE=.\homiesProc_winall.cpp
# End Source File
# End Group
# Begin Group "Interfaces"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\homies_cursor.cpp
# End Source File
# Begin Source File

SOURCE=.\homies_homie.cpp
# End Source File
# Begin Source File

SOURCE=.\homies_map.cpp
# End Source File
# End Group
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "Types"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\homies_type_cursor.h
# End Source File
# Begin Source File

SOURCE=.\homies_type_homie.h
# End Source File
# Begin Source File

SOURCE=.\homies_type_map.h
# End Source File
# Begin Source File

SOURCE=.\homies_types.h
# End Source File
# End Group
# Begin Group "General"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\BASS.H
# End Source File
# Begin Source File

SOURCE=.\common.h
# End Source File
# Begin Source File

SOURCE=.\homies.h
# End Source File
# Begin Source File

SOURCE=.\homies_i.h
# End Source File
# Begin Source File

SOURCE=.\homies_menu_id.h
# End Source File
# Begin Source File

SOURCE=.\mywin.h
# End Source File
# End Group
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\BASS.LIB
# End Source File
# End Group
# End Target
# End Project
