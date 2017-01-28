# ==============================
# Fichier:			Makefile
# Projet:			ROM Dumper
# Ecrit par:		Paul Guyot (pguyot@kallisys.net)
# 
# CrŽŽ le:			13/1/2005
# Tabulation:		4 espaces
# 
# Copyright (c) 2005, Paul Guyot.
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 
# * Redistributions of source code must retain the above copyright
#   notice and the following disclaimer. 
# * Redistributions in binary form must reproduce the above
#   copyright notice and the following disclaimer in the
#   documentation and/or other materials provided with the
#   distribution.
# * Neither the name of Kallisys nor the names of its contributors
#   may be used to endorse or promote products derived from this
#   software without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT
# HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# ===========
# $Id$
# ===========

# Makefile for GNU Make using toolserver(1) v1.0 or higher.
# You need to install the NCT examples provided with toolserver(1) to get
# this working.

# Once all this is installed, type make to build ROMDumper native code.
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
		-i "{NCT_Includes}:NSandDDK:"

# Libraries I'm linking with:

LibrariesCommon		=	'"{NCT_Libraries}Newton Public APIs.o"'
LibrariesDebug		=	
LibrariesNTKC		=	
LibrariesPart		=	

# Binaries:

ObjectsNTKC	=	\
		ROMDumper.o \
		ROMDumper.exp.o

# -------- Rules --------

# --- Common rules ---

%.exp.a : %.exp
	NCTBuildMain $< $@
%.impl.a	: %.impl.h
	ProtocolGen -ImplementationGlue $< -package ${CPlusOptionsDefines} ${CPlusIncludes} -stdout > $@
%.a : %.h
	ProtocolGen -InterfaceGlue $< ${CPlusOptionsDefines} ${CPlusIncludes} -stdout > $@

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

all : ROMDumper.ntkc

# --- Specific rules ---
# I need specific rules for the NTKC entry points. They should be compiled with -gf to make AIFtoNTK happy.

ROMDumper.o	:	ROMDumper.cp
	${CPlus} $(^F) ${CPlusOptionsCommon} ${CPlusOptionsNoDebug} ${CPlusOptionsNTKC} \
		${CPlusIncludes} -o $(@F)

# ------ Package rules ------

# --- NoDebug rules ---
# --- Debug rules ---

# ------ NTKC modules ------

# --- NoDebug rules ---

%.ntkc	:	%.sym %.exp
	${AIFtoNTK} ${LocalAIFtoNTKOptions} -via $(*F).exp $(^F) -o $(@F)
	Rez ${NCTTools}NCTIcons.r -i ${RIncludes} -append -o $(@F)
	SetFile -a C $(@F)

ROMDumper.sym	:	${ObjectsNTKC}
	${Link} ${LinkOptionsCommon} ${LinkOptionsNTKC} \
		${LibrariesCommon} ${LibrariesNTKC} $(^F) -o $(@F)

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
