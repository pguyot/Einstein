# ==============================
# Fichier:			Makefile
# Projet:			Einstein
# Ecrit par:		Paul Guyot (pguyot@kallisys.net)
# 
# Créé le:			1/8/2004
# Tabulation:		4 espaces
# 
# Copyright:		© 2004-2005 by Paul Guyot.
# 					Tous droits réservés pour tous pays.
# ===========
# $Id: Makefile 150 2006-01-10 05:55:43Z paul $
# ===========

# --- variables ---
CPlusOptionsDefines	=	-d hasPCMCIA -d forARM -d TARGET_OS_NEWTON -d NO_ERRNO_H
CPlusOptions			=	-facqy -zo -W -cfront ${CPlusOptionsDefines}
LinkOptions			=	-dupok -remove -bin

# Package options:
PackerCopyright		=	'"Copyright (c) 2004-2007 Paul Guyot."'

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
		-i ${KPath_Classic}

Libraries			=	'"{NCT_Libraries}Newton Public APIs.o"'

headers = \
	Headers/NativeCallsDefines.h \
	Headers/PlatformGestalt.h \
	Headers/PlatformEvents.h \
	Headers/UserInfoDefinitions.h \
	Headers/Version.h

# --- rules ---

%.impl.a	: %.impl.h ${headers}
	ProtocolGen -ImplementationGlue $< -package ${CPlusOptionsDefines} ${CPlusIncludes} -stdout > $@
%.o : %.c ${headers}
	ARMCpp $< ${CPlusOptions} ${CPlusIncludes} -o $(@F)
%.o : %.cp ${headers}
	ARMCpp $< ${CPlusOptions} ${CPlusIncludes} -o $(@F)
%.o : %.a
	ARM6Asm $< ${AsmIncludes} -o $(@F)

# --- specific ---	
all : \
	TNativeCalls.o \
	Einstein.rex

TNativeCalls.a : TNativeCalls.h ${headers}
	ProtocolGen -InterfaceGlue $< ${CPlusOptionsDefines} ${CPlusIncludes} -stdout > $@

# --- cf headers ---	

Headers/%.h	:	../Emulator/Platform/%.h
	mkdir -p Headers/
	tr '\n' '\r' < $< > $@

Headers/%.h	:	../Emulator/Host/%.h
	mkdir -p Headers/
	tr '\n' '\r' < $< > $@

Headers/%.h	:	../Emulator/NativeCalls/%.h
	mkdir -p Headers/
	tr '\n' '\r' < $< > $@

Headers/%.h	:	../app/%.h
	mkdir -p Headers/
	tr '\n' '\r' < $< > $@

install : all
	cp Einstein.rex ../_Data_/

Einstein.rex : \
	Einstein.rex.src \
	EinsteinFlashDriver.bin \
	EinsteinPlatformDriver.pkg \
	EinsteinSoundDriver.pkg \
	EinsteinBatteryDriver.pkg \
	EinsteinScreenDriver.pkg \
	EinsteinTabletDriver.pkg \
	EinsteinVersionString.pkg \
	EinsteinStartupDriver.pkg \
	EinsteinSerialDriver.pkg \
	EinsteinInTranslator.pkg \
	EinsteinOutTranslator.pkg \
	EinsteinNativeCalls.pkg \
	NSRuntime/Einstein\ NS\ Runtime.pkg \
	../Packages/Keyboards/Keyboards.pkg
	Rex -o Einstein.rex Einstein.rex.src

RelocHack.o : ${KPath_POSIX}/K/Misc/RelocHack.a
	ARM6Asm "${KPath_Classic}:K:Misc:RelocHack.a" ${AsmIncludes} -o $@
	
EinsteinFlashDriver.bin :	TEinsteinFlashDriver.impl.o \
							TEinsteinFlashDriver.o \
							TEinsteinFlashDriverPrims.o
	ARMLink ${LinkOptions} ${Libraries} \
		$(^F) -o $(@F)

EinsteinPlatformDriver.bin :	TMainPlatformDriver.impl.o \
								TMainPlatformDriver.o \
								TMainPlatformDriverPrims.o \
								RelocHack.o \
								Glue.o \
								UGlue.o
	ARMLink ${LinkOptions} ${Libraries} \
		$(^F) -o $(@F)

EinsteinPlatformDriver.pkg	: EinsteinPlatformDriver.bin
	Packer -o $(@F) \
		"EinsteinPlatformDriver:Einstein" -packageid '" no "' -copyright ${PackerCopyright} \
		-version 01 -invisible \
		-protocol $(<F) -autoload
	SetFile -t "pkg " -c "pkgX" $@

EinsteinSoundDriver.bin :		PMainSoundDriver.impl.o \
								PMainSoundDriver.o \
								PMainSoundDriverPrims.o \
								RelocHack.o \
								Glue.o \
								UGlue.o
	ARMLink ${LinkOptions} ${Libraries} \
		$(^F) -o $(@F)

EinsteinSoundDriver.pkg	: EinsteinSoundDriver.bin
	Packer -o $(@F) \
		"EinsteinSoundDriver:Einstein" -packageid '" no "' -copyright ${PackerCopyright} \
		-version 01 -invisible \
		-protocol $(<F) -autoload
	SetFile -t "pkg " -c "pkgX" $@

EinsteinBatteryDriver.bin :		PMainBatteryDriver.impl.o \
								PMainBatteryDriver.o \
								PMainBatteryDriverPrims.o
	ARMLink ${LinkOptions} ${Libraries} \
		$(^F) -o $(@F)

EinsteinBatteryDriver.pkg	: EinsteinBatteryDriver.bin
	Packer -o $(@F) \
		"EinsteinBatteryDriver:Einstein" -packageid '" no "' -copyright ${PackerCopyright} \
		-version 01 -invisible \
		-protocol $(<F) -autoload
	SetFile -t "pkg " -c "pkgX" $@

EinsteinScreenDriver.bin :		TMainDisplayDriver.impl.o \
								TMainDisplayDriver.o \
								TMainDisplayDriverPrims.o
	ARMLink ${LinkOptions} ${Libraries} \
		$(^F) -o $(@F)

EinsteinScreenDriver.pkg	: EinsteinScreenDriver.bin
	Packer -o $(@F) \
		"EinsteinScreenDriver:Einstein" -packageid '" no "' -copyright ${PackerCopyright} \
		-version 01 -invisible \
		-protocol $(<F) -autoload
	SetFile -t "pkg " -c "pkgX" $@

EinsteinTabletDriver.bin :		TMainTabletDriver.impl.o \
								TMainTabletDriver.o \
								TMainTabletDriverPrims.o \
								RelocHack.o
	ARMLink ${LinkOptions} ${Libraries} \
		$(^F) -o $(@F)

EinsteinTabletDriver.pkg	: EinsteinTabletDriver.bin
	Packer -o $(@F) \
		"EinsteinTabletDriver:Einstein" -packageid '" no "' -copyright ${PackerCopyright} \
		-version 01 -invisible \
		-protocol $(<F) -autoload
	SetFile -t "pkg " -c "pkgX" $@

EinsteinVersionString.bin :		TMainVersionString.impl.o \
								TMainVersionString.o
	ARMLink ${LinkOptions} ${Libraries} \
		$(^F) -o $(@F)

EinsteinVersionString.pkg	: EinsteinVersionString.bin
	Packer -o $(@F) \
		"EinsteinVersionString:Einstein" -packageid '" no "' -copyright ${PackerCopyright} \
		-version 01 -invisible \
		-protocol $(<F) -autoload
	SetFile -t "pkg " -c "pkgX" $@

EinsteinStartupDriver.bin :		TMainStartupDriver.impl.o \
								TMainStartupDriver.o
	ARMLink ${LinkOptions} ${Libraries} \
		$(^F) -o $(@F)

EinsteinStartupDriver.pkg	: EinsteinStartupDriver.bin
	Packer -o $(@F) \
		"EinsteinStartupDriver:Einstein" -packageid '" no "' -copyright ${PackerCopyright} \
		-version 01 -invisible \
		-protocol $(<F) -autoload
	SetFile -t "pkg " -c "pkgX" $@

EinsteinSerialDriver.bin :		TSerialChipVoyager.impl.o \
								TSerialChipVoyager.o \
								TSerialChipVoyagerPrims.o
	ARMLink ${LinkOptions} ${Libraries} \
		$(^F) -o $(@F)

EinsteinSerialDriver.pkg	: EinsteinSerialDriver.bin
	Packer -o $(@F) \
		"EinsteinSerialDriver:Einstein" -packageid '" no "' -copyright ${PackerCopyright} \
		-version 01 -invisible \
		-protocol $(<F) -autoload
	SetFile -t "pkg " -c "pkgX" $@

EinsteinInTranslator.bin :		PEinsteinInTranslator.impl.o \
								PEinsteinInTranslator.o \
								PEinsteinInTranslatorPrims.o
	ARMLink ${LinkOptions} ${Libraries} \
		$(^F) -o $(@F)

EinsteinInTranslator.pkg	: EinsteinInTranslator.bin
	Packer -o $(@F) \
		"EinsteinInTranslator:Einstein" -packageid '" no "' -copyright ${PackerCopyright} \
		-version 01 -invisible \
		-protocol $(<F) -autoload
	SetFile -t "pkg " -c "pkgX" $@

EinsteinOutTranslator.bin :		PEinsteinOutTranslator.impl.o \
								PEinsteinOutTranslator.o \
								PEinsteinOutTranslatorPrims.o
	ARMLink ${LinkOptions} ${Libraries} \
		$(^F) -o $(@F)

EinsteinOutTranslator.pkg	: EinsteinOutTranslator.bin
	Packer -o $(@F) \
		"EinsteinOutTranslator:Einstein" -packageid '" no "' -copyright ${PackerCopyright} \
		-version 01 -invisible \
		-protocol $(<F) -autoload
	SetFile -t "pkg " -c "pkgX" $@

EinsteinNativeCalls.bin :		TEinsteinNativeCalls.impl.o \
								TEinsteinNativeCalls.o \
								TEinsteinNativeCallsPrims.o
	ARMLink ${LinkOptions} ${Libraries} \
		$(^F) -o $(@F)

EinsteinNativeCalls.pkg	: EinsteinNativeCalls.bin
	Packer -o $(@F) \
		"EinsteinNativeCalls:Einstein" -packageid '" no "' -copyright ${PackerCopyright} \
		-version 01 -invisible \
		-protocol $(<F) -autoload
	SetFile -t "pkg " -c "pkgX" $@

# --- clean ---
clean :
	rm -f *.o *.impl.a *.bin *.pkg

## =================================================== ##
## Can't open /usr/fortunes.  Lid stuck on cookie jar. ##
## =================================================== ##
