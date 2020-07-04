# Microsoft Developer Studio Generated NMAKE File, Format Version 4.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=SuiteP - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to SuiteP - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "SuiteP - Win32 Release" && "$(CFG)" != "SuiteP - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "SuiteP.mak" CFG="SuiteP - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SuiteP - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "SuiteP - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "SuiteP - Win32 Debug"
RSC=rc.exe
CPP=cl.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "SuiteP - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\SuiteP.exe"

CLEAN : 
	-@erase ".\Release\SuiteP.exe"
	-@erase ".\Release\SuiteP.obj"
	-@erase ".\Release\SuiteP.pch"
	-@erase ".\Release\StdAfx.obj"
	-@erase ".\Release\SuitePDlg.obj"
	-@erase ".\Release\SuitePX.obj"
	-@erase ".\Release\SuiteP.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "DIL_ForDebug" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_MBCS" /D "DIL_ForDebug" /Fp"$(INTDIR)/SuiteP.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/SuiteP.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/SuiteP.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/SuiteP.pdb" /machine:I386 /out:"$(OUTDIR)/SuiteP.exe" 
LINK32_OBJS= \
	"$(INTDIR)/SuiteP.obj" \
	"$(INTDIR)/StdAfx.obj" \
	"$(INTDIR)/SuitePDlg.obj" \
	"$(INTDIR)/SuitePX.obj" \
	"$(INTDIR)/SuiteP.res" \
	".\DIL2D.lib"

"$(OUTDIR)\SuiteP.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "SuiteP - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\SuiteP.exe" "$(OUTDIR)\SuiteP.pch"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\vc40.idb"
	-@erase ".\Debug\SuiteP.pch"
	-@erase ".\Debug\SuiteP.exe"
	-@erase ".\Debug\StdAfx.obj"
	-@erase ".\Debug\SuitePX.obj"
	-@erase ".\Debug\SuitePDlg.obj"
	-@erase ".\Debug\SuiteP.obj"
	-@erase ".\Debug\SuiteP.res"
	-@erase ".\Debug\SuiteP.ilk"
	-@erase ".\Debug\SuiteP.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "DIL_ForDebug" /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_MBCS" /D "DIL_ForDebug" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/SuiteP.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/SuiteP.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/SuiteP.pdb" /debug /machine:I386 /out:"$(OUTDIR)/SuiteP.exe" 
LINK32_OBJS= \
	"$(INTDIR)/StdAfx.obj" \
	"$(INTDIR)/SuitePX.obj" \
	"$(INTDIR)/SuitePDlg.obj" \
	"$(INTDIR)/SuiteP.obj" \
	"$(INTDIR)/SuiteP.res" \
	".\DIL2D.lib"

"$(OUTDIR)\SuiteP.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "SuiteP - Win32 Release"
# Name "SuiteP - Win32 Debug"

!IF  "$(CFG)" == "SuiteP - Win32 Release"

!ELSEIF  "$(CFG)" == "SuiteP - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\SuiteP.cpp
DEP_CPP_SUITE=\
	".\StdAfx.h"\
	".\SuiteP.h"\
	".\SuitePDlg.h"\
	".\SuitePX.h"\
	".\CDIL.h"\
	".\PDIL.h"\
	".\DIL.h"\
	".\FDIL.h"\
	

"$(INTDIR)\SuiteP.obj" : $(SOURCE) $(DEP_CPP_SUITE) "$(INTDIR)"\
 "$(INTDIR)\SuiteP.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\SuitePDlg.cpp
DEP_CPP_SUITEP=\
	".\StdAfx.h"\
	".\SuiteP.h"\
	".\SuitePDlg.h"\
	".\SuitePX.h"\
	".\CDIL.h"\
	".\PDIL.h"\
	".\DIL.h"\
	".\FDIL.h"\
	

"$(INTDIR)\SuitePDlg.obj" : $(SOURCE) $(DEP_CPP_SUITEP) "$(INTDIR)"\
 "$(INTDIR)\SuiteP.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\StdAfx.cpp
DEP_CPP_STDAF=\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "SuiteP - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS"\
 /D "DIL_ForDebug" /Fp"$(INTDIR)/SuiteP.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\SuiteP.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "SuiteP - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_MBCS" /D "DIL_ForDebug" /Fp"$(INTDIR)/SuiteP.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\SuiteP.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SuiteP.rc
DEP_RSC_SUITEP_=\
	".\res\SuiteP.ico"\
	".\res\SuiteP.rc2"\
	

"$(INTDIR)\SuiteP.res" : $(SOURCE) $(DEP_RSC_SUITEP_) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\SuitePX.cpp
DEP_CPP_SUITEPX=\
	".\StdAfx.h"\
	".\SuiteP.h"\
	".\SuitePX.h"\
	".\CDIL.h"\
	".\PDIL.h"\
	".\DIL.h"\
	".\FDIL.h"\
	

"$(INTDIR)\SuitePX.obj" : $(SOURCE) $(DEP_CPP_SUITEPX) "$(INTDIR)"\
 "$(INTDIR)\SuiteP.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\DIL2D.lib

!IF  "$(CFG)" == "SuiteP - Win32 Release"

!ELSEIF  "$(CFG)" == "SuiteP - Win32 Debug"

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
