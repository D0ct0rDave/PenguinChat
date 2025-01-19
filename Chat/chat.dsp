# Microsoft Developer Studio Project File - Name="chat" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=chat - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "chat.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "chat.mak" CFG="chat - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "chat - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "chat - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "chat - Win32 Release"

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
# ADD BASE RSC /l 0xc0a /d "NDEBUG"
# ADD RSC /l 0xc0a /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "chat - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "../vc/TerrainTexturizer/palett~1" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /YX /FD /c
# ADD BASE RSC /l 0xc0a /d "_DEBUG"
# ADD RSC /l 0xc0a /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "chat - Win32 Release"
# Name "chat - Win32 Debug"
# Begin Group "Texture"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\Vc\TerrainTexturizer\Palette Textures MipMaps\Palette.cpp"
# End Source File
# Begin Source File

SOURCE="..\Vc\TerrainTexturizer\Palette Textures MipMaps\Palette.h"
# End Source File
# Begin Source File

SOURCE="..\Vc\TerrainTexturizer\Palette Textures MipMaps\TexMem.cpp"
# End Source File
# Begin Source File

SOURCE="..\Vc\TerrainTexturizer\Palette Textures MipMaps\TexMem.h"
# End Source File
# Begin Source File

SOURCE="..\Vc\TerrainTexturizer\Palette Textures MipMaps\texture.cpp"
# End Source File
# Begin Source File

SOURCE="..\Vc\TerrainTexturizer\Palette Textures MipMaps\texture.h"
# End Source File
# Begin Source File

SOURCE="..\Vc\TerrainTexturizer\Palette Textures MipMaps\TGA.cpp"
# End Source File
# Begin Source File

SOURCE="..\Vc\TerrainTexturizer\Palette Textures MipMaps\TGA.h"
# End Source File
# Begin Source File

SOURCE="..\Vc\TerrainTexturizer\Palette Textures MipMaps\Tgawr.cpp"
# End Source File
# End Group
# Begin Source File

SOURCE=.\chat.cpp
# End Source File
# Begin Source File

SOURCE=.\chat.h
# End Source File
# Begin Source File

SOURCE=.\comm.cpp
# End Source File
# Begin Source File

SOURCE=.\comm.h
# End Source File
# Begin Source File

SOURCE=.\cript.cpp
# End Source File
# Begin Source File

SOURCE=.\cript.h
# End Source File
# Begin Source File

SOURCE=.\files.cpp
# End Source File
# Begin Source File

SOURCE=.\files.h
# End Source File
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# Begin Source File

SOURCE=.\misc.cpp
# End Source File
# Begin Source File

SOURCE=.\misc.h
# End Source File
# Begin Source File

SOURCE=.\user.cpp
# End Source File
# Begin Source File

SOURCE=.\user.h
# End Source File
# End Target
# End Project
