# ==============================
# Fichier:			Makefile
# Projet:			Einstein NS Runtime
# Ecrit par:		Paul Guyot (pguyot@kallisys.net)
# 
# CrŽŽ le:			18/1/2005
# Tabulation:		4 espaces
# 
# Copyright (c) 2005, Paul Guyot.
# All rights reserved.
# ===========
# $Id: Makefile 147 2005-09-29 20:17:58Z paul $
# ===========

# Makefile for GNU Make using toolserver(1) v1.0 or higher.
# You need to install the NCT examples provided with toolserver(1) to get
# this working.

# Once all this is installed, type make to build NSRuntime native code.
# (remark: this Makefile uses GNU Make-isms. On MacOS X, make is GNU Make).

# -------- Variables --------

# Commands & options:
Link				=	ARMLink
LinkOptionsCommon	=	-dupok -remove
LinkOptionsNoDebug	=	-bin
LinkOptionsDebug	=	-aif -bin -dsuppress -rel
LinkOptionsNTKC		=	-debug -aif -bin -dsuppress -rel
LinkOptionsPart		=	
Asm					=	ARM6Asm
AsmOptionsCommon	=
AsmOptionsNoDebug	=
AsmOptionsDebug		=	-g
CPlus				=	ARMCpp
CPlusOptionsDefines	=	-d hasPCMCIA -d forARM -d TARGET_OS_NEWTON -d NO_ERRNO_H ${MoreCDefines}
CPlusOptionsCommon	=	-facqy -zo -W -cfront ${CPlusOptionsDefines}
CPlusOptionsNoDebug	=	
CPlusOptionsDebug	=	-d forDebug
CPlusOptionsNTKC	=	-gf
AIFtoNTK			=	AIFtoNTK
NCTTools			=	/Users/paul/Applications/Programmation/MPW/MPW/NCT_Folder/Tools/
RIncludes			=	/Developer/Headers/FlatCarbon/

KPath_Classic		=	'"Marvin:Users:paul:Bibliotheques:K"'
KPath_POSIX			=	/Users/paul/Bibliotheques/K

# Include paths:
AsmIncludes			=	-i "{DDK_Libraries-dir}"
CPlusIncludes		=	\
		-i "{DDK_Includes-dir}" \
		-i "{DDK_Includes-dir}Bootstrap:" \
		-i "{DDK_Includes-dir}CLibrary:" \
		-i "{DDK_Includes-dir}CommAPI:" \
		-i "{DDK_Includes-dir}Communications:" \
		-i "{DDK_Includes-dir}Frames:" \
		-i "{DDK_Includes-dir}HAL:" \
		-i "{DDK_Includes-dir}OS600:" \
		-i "{DDK_Includes-dir}Packages:" \
		-i "{DDK_Includes-dir}Power:" \
		-i "{DDK_Includes-dir}Printing:" \
		-i "{DDK_Includes-dir}QD:" \
		-i "{DDK_Includes-dir}Toolbox:" \
		-i "{DDK_Includes-dir}UtilityClasses:" \
		-i "{DDK_Includes-dir}PCMCIA:" \
		-i "{NCT_Includes}" \
		-i "{NCT_Includes}Frames:" \
		-i "{NCT_Includes}Utilities:" \
		-i "{NCT_Includes}:NSandDDK:" \
		-i ${KPath_Classic} \
		-i ":::"

# Libraries I'm linking with:

LibrariesCommon		=	'"{NCT_Libraries}Newton Public APIs.o"'
LibrariesDebug		=	
LibrariesNTKC		=	
LibrariesPart		=	

# Binaries:

ObjectsNTKC	=	\
		NSRuntime.o \
		TNSRuntimeEventHandler.o \
		TNSRuntimeEventHandlerGlue.o \
		NSRuntime.exp.o \
		RelocHack.o \
		TMainPlatformDriverPrims.o \
		TMainPlatformDriver.o \
		TNativeCalls.o

# -------- Rules --------

# --- Common rules ---

%.exp.a : %.exp
	NCTBuildMain $< $@
%.impl.a	: %.impl.h
	ProtocolGen -ImplementationGlue $< -package ${CPlusOptionsDefines} ${CPlusIncludes} -stdout > $@

# --- No Debug rules ---

%.o : %.c
	${CPlus} $< ${CPlusOptionsCommon} ${CPlusOptionsNoDebug} ${CPlusIncludes} -o $(@F)
%.o : %.cp
	${CPlus} $< ${CPlusOptionsCommon} ${CPlusOptionsNoDebug} ${CPlusIncludes} -o $(@F)
%.o : %.a
	${Asm} $< ${AsmOptionsCommon} ${AsmOptionsNoDebug} ${AsmIncludes} -o $(@F)

# --- Debug rules ---

%.d.o : %.c
	${CPlus} $< ${CPlusOptionsCommon} ${CPlusOptionsNoDebug} ${CPlusIncludes} -o $(@F)
%.d.o : %.cp
	${CPlus} $< ${CPlusOptionsCommon} ${CPlusOptionsDebug} ${CPlusIncludes} -o $(@F)
%.d.o : %.a
	${Asm} $< ${AsmOptionsCommon} ${AsmOptionsDebug} ${AsmIncludes} -o $(@F)

# --- all ---

all : NSRuntime.ntkc

# --- Specific rules ---
# I need specific rules for the NTKC entry points. They should be compiled with -gf to make AIFtoNTK happy.

NSRuntime.o	:	NSRuntime.cp
	${CPlus} $(^F) ${CPlusOptionsCommon} ${CPlusOptionsNoDebug} ${CPlusOptionsNTKC} \
		${CPlusIncludes} -o $(@F)

# ------ Package rules ------

# --- NoDebug rules ---
# --- Debug rules ---

# ------ NTKC modules ------

# --- NoDebug rules ---

%.ntkc	:	%.sym %.exp.o
	${AIFtoNTK} ${LocalAIFtoNTKOptions} -via $(*F).exp $(^F) -o $(@F)
	Rez ${NCTTools}NCTIcons.r -i ${RIncludes} -append -o $(@F)
	SetFile -a C $(@F)

NSRuntime.sym	:	${ObjectsNTKC}
	${Link} ${LinkOptionsCommon} ${LinkOptionsNTKC} \
		${LibrariesCommon} ${LibrariesNTKC} $(^F) -o $(@F)

# --- copy ---

TMainPlatformDriverPrims.o : ../../TMainPlatformDriverPrims.o
	cp ../../TMainPlatformDriverPrims.o .

TMainPlatformDriver.o : ../../TMainPlatformDriver.o
	cp ../../TMainPlatformDriver.o .

TNativeCalls.o : ../../TNativeCalls.o
	cp ../../TNativeCalls.o .

# --- clean ---
clean :
	rm -f Objects/*/*

## ======================================================================= ##
## 4.2 BSD UNIX #57: Sun Jun 1 23:02:07 EDT 1986                           ##
##                                                                         ##
## You swing at the Sun.  You miss.  The Sun swings.  He hits you with a   ##
## 575MB disk!  You read the 575MB disk.  It is written in an alien        ##
## tongue and cannot be read by your tired Sun-2 eyes.  You throw the      ##
## 575MB disk at the Sun.  You hit!  The Sun must repair your eyes.  The   ##
## Sun reads a scroll.  He hits your 130MB disk!  He has defeated the      ##
## 130MB disk!  The Sun reads a scroll.  He hits your Ethernet board!  He  ##
## has defeated your Ethernet board!  You read a scroll of "postpone until ##
## Monday at 9 AM".  Everything goes dark...                               ##
##                 -- /etc/motd, cbosgd                                    ##
## ======================================================================= ##
