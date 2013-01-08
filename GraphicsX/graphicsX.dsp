# Microsoft Developer Studio Project File - Name="graphicsX" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=graphicsX - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "graphicsX.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "graphicsX.mak" CFG="graphicsX - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "graphicsX - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "graphicsX - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Rotten/graphicsX", VAAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "graphicsX - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "graphicsX - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "graphicsX - Win32 Release"
# Name "graphicsX - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "ParticleFX"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GFX_ParticleFX.cpp
# End Source File
# Begin Source File

SOURCE=.\PARFX_gas.cpp
# End Source File
# Begin Source File

SOURCE=.\PARFX_gas2.cpp
# End Source File
# End Group
# Begin Group "Model"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GFX_Model.cpp
# End Source File
# Begin Source File

SOURCE=.\GFX_ModelGenMap.cpp
# End Source File
# Begin Source File

SOURCE=.\GFX_ModelLoad.cpp
# End Source File
# End Group
# Begin Group "Object"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GFX_Object.cpp
# End Source File
# Begin Source File

SOURCE=.\GFX_ObjectLoad.cpp
# End Source File
# End Group
# Begin Group "Map"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GFX_Map.cpp
# End Source File
# Begin Source File

SOURCE=.\GFX_MapGenMidptFrac.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\d3dapp.cpp
# End Source File
# Begin Source File

SOURCE=.\d3dfont.cpp
# End Source File
# Begin Source File

SOURCE=.\ddutil.cpp
# End Source File
# Begin Source File

SOURCE=.\dxutil.cpp
# End Source File
# Begin Source File

SOURCE=.\GFX_BkgrndFX.cpp
# End Source File
# Begin Source File

SOURCE=.\GFX_Camera.cpp
# End Source File
# Begin Source File

SOURCE=.\GFX_ErrorCheck.cpp
# End Source File
# Begin Source File

SOURCE=.\GFX_Font.cpp
# End Source File
# Begin Source File

SOURCE=.\GFX_Light.cpp
# End Source File
# Begin Source File

SOURCE=.\GFX_Main.cpp
# End Source File
# Begin Source File

SOURCE=.\GFX_Math.cpp
# End Source File
# Begin Source File

SOURCE=.\GFX_Misc.cpp
# End Source File
# Begin Source File

SOURCE=.\GFX_Pager.cpp
# End Source File
# Begin Source File

SOURCE=.\GFX_Sprite.cpp
# End Source File
# Begin Source File

SOURCE=.\GFX_Texture.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\d3d8.h
# End Source File
# Begin Source File

SOURCE=.\d3d8types.h
# End Source File
# Begin Source File

SOURCE=.\d3dfont.h
# End Source File
# Begin Source File

SOURCE=.\d3dx8core.h
# End Source File
# Begin Source File

SOURCE=.\d3dx8math.h
# End Source File
# Begin Source File

SOURCE=.\d3dx8mesh.h
# End Source File
# Begin Source File

SOURCE=.\d3dx8tex.h
# End Source File
# Begin Source File

SOURCE=.\dxutil.h
# End Source File
# Begin Source File

SOURCE=.\gdiX.h
# End Source File
# Begin Source File

SOURCE=.\gdiX_bkgrndFX.h
# End Source File
# Begin Source File

SOURCE=.\gdix_i.h
# End Source File
# Begin Source File

SOURCE=.\gdiX_particleFX.h
# End Source File
# Begin Source File

SOURCE=.\gdiX_particleFX_i.h
# End Source File
# End Group
# End Target
# End Project
