# Microsoft Visual C++ generated build script - Do not modify

PROJ = SOUP16
DEBUG = 1
PROGTYPE = 0
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = -d_DEBUG
R_RCDEFINES = -dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = C:\SOUPDRNK\
USEMFC = 0
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC = SOUPDRNK.C  
FIRSTCPP =             
RC = rc
CFLAGS_D_WEXE = /nologo /G2 /W3 /Zi /AL /Gx- /Od /D "_DEBUG" /D "_windows" /D "forWin16" /D "_windows" /FR /GA /Fd"SOUP16.PDB"
CFLAGS_R_WEXE = /nologo /W3 /AM /O1 /D "NDEBUG" /D "forWin16" /D "_windows" /FR /GA 
LFLAGS_D_WEXE = /NOLOGO /ONERROR:NOEXE /NOD /PACKC:61440 /CO /ALIGN:16 /STACK:10240
LFLAGS_R_WEXE = /NOLOGO /ONERROR:NOEXE /NOD /PACKC:61440 /ALIGN:16 /STACK:10240
LIBS_D_WEXE = oldnames libw commdlg shell olecli olesvr llibcew
LIBS_R_WEXE = oldnames libw commdlg shell olecli olesvr mlibcew
RCFLAGS = /nologo
RESFLAGS = /nologo
RUNFLAGS = 
DEFFILE = SOUP16.DEF
OBJS_EXT = 
LIBS_EXT = FDIL_DLL.LIB 
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
SBRS = SOUPDRNK.SBR \
		ENGINE.SBR


FDIL_DLL_DEP = 

SOUPDRNK_DEP = c:\soupdrnk\dilcpipe.h \
	c:\soupdrnk\hlfdil.h \
	c:\soupdrnk\soupdrnk.h \
	c:\soupdrnk\engine.h


ENGINE_DEP = c:\soupdrnk\dilcpipe.h \
	c:\soupdrnk\hlfdil.h \
	c:\soupdrnk\soupdrnk.h \
	c:\soupdrnk\engine.h


SOUPDRNK_RCDEP = c:\soupdrnk\soupdrnk.h \
	c:\soupdrnk\soupdrnk.ico


all:	$(PROJ).EXE $(PROJ).BSC

SOUPDRNK.OBJ:	SOUPDRNK.C $(SOUPDRNK_DEP)
	$(CC) $(CFLAGS) $(CCREATEPCHFLAG) /c SOUPDRNK.C

ENGINE.OBJ:	ENGINE.C $(ENGINE_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c ENGINE.C

SOUPDRNK.RES:	SOUPDRNK.RC $(SOUPDRNK_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -r SOUPDRNK.RC


$(PROJ).EXE::	SOUPDRNK.RES

$(PROJ).EXE::	SOUPDRNK.OBJ ENGINE.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
SOUPDRNK.OBJ +
ENGINE.OBJ +
$(OBJS_EXT)
$(PROJ).EXE
$(MAPFILE)
c:\msvc15\lib\+
c:\msvc15\mfc\lib\+
FDIL_DLL.LIB+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF
	$(RC) $(RESFLAGS) SOUPDRNK.RES $@
	@copy $(PROJ).CRF MSVC.BND

$(PROJ).EXE::	SOUPDRNK.RES
	if not exist MSVC.BND 	$(RC) $(RESFLAGS) SOUPDRNK.RES $@

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
