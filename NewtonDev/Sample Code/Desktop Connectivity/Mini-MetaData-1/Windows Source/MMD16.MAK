# Microsoft Visual C++ generated build script - Do not modify

PROJ = MMD16
DEBUG = 1
PROGTYPE = 0
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = -d_DEBUG
R_RCDEFINES = -dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = C:\MMD\
USEMFC = 0
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC = INTERFAC.C  
FIRSTCPP =             
RC = rc
CFLAGS_D_WEXE = /nologo /G2 /W3 /Zi /AL /Od /D "_DEBUG" /D "forWindows" /FR /GA /Fd"MMD16.PDB"
CFLAGS_R_WEXE = /nologo /W3 /AM /O1 /D "NDEBUG" /FR /GA 
LFLAGS_D_WEXE = /NOLOGO /ONERROR:NOEXE /NOD /PACKC:61440 /CO /ALIGN:16 /STACK:10240
LFLAGS_R_WEXE = /NOLOGO /ONERROR:NOEXE /NOD /PACKC:61440 /ALIGN:16 /STACK:10240
LIBS_D_WEXE = oldnames libw commdlg shell olecli olesvr llibcew
LIBS_R_WEXE = oldnames libw commdlg shell olecli olesvr mlibcew
RCFLAGS = /nologo
RESFLAGS = /nologo
RUNFLAGS = 
DEFFILE = MMD16.DEF
OBJS_EXT = 
LIBS_EXT = CDIL_DLL.LIB 
!if "$(DEBUG)" == "1"
CFLAGS = $(CFLAGS_D_WEXE)
LFLAGS = $(LFLAGS_D_WEXE)
LIBS = $(LIBS_D_WEXE)
MAPFILE = nul
RCDEFINES = $(D_RCDEFINES)
!else
CFLAGS = $(CFLAGS_R_WEXE)
LFLAGS = $(LFLAGS_R_WEXE)
LIBS = $(LIBS_R_WEXE)
MAPFILE = nul
RCDEFINES = $(R_RCDEFINES)
!endif
!if [if exist MSVC.BND del MSVC.BND]
!endif
SBRS = INTERFAC.SBR \
		PROTOCOL.SBR


CDIL_DLL_DEP = 

INTERFAC_DEP = c:\mmd\interfac.h \
	c:\mmd\dilcpipe.h \
	c:\mmd\protocol.h


PROTOCOL_DEP = c:\mmd\protocol.h \
	c:\mmd\dilcpipe.h \
	c:\mmd\interfac.h


MMD_RCDEP = c:\mmd\mmd.ico


all:	$(PROJ).EXE $(PROJ).BSC

INTERFAC.OBJ:	INTERFAC.C $(INTERFAC_DEP)
	$(CC) $(CFLAGS) $(CCREATEPCHFLAG) /c INTERFAC.C

PROTOCOL.OBJ:	PROTOCOL.C $(PROTOCOL_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c PROTOCOL.C

MMD.RES:	MMD.RC $(MMD_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -r MMD.RC


$(PROJ).EXE::	MMD.RES

$(PROJ).EXE::	INTERFAC.OBJ PROTOCOL.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
INTERFAC.OBJ +
PROTOCOL.OBJ +
$(OBJS_EXT)
$(PROJ).EXE
$(MAPFILE)
c:\msvc15\lib\+
c:\msvc15\mfc\lib\+
CDIL_DLL.LIB+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF
	$(RC) $(RESFLAGS) MMD.RES $@
	@copy $(PROJ).CRF MSVC.BND

$(PROJ).EXE::	MMD.RES
	if not exist MSVC.BND 	$(RC) $(RESFLAGS) MMD.RES $@

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
