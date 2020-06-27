# ==============================
# Fichier:			Makefile
# Projet:			Einstein
# Ecrit par:		Paul Guyot (pguyot@kallisys.net)
# 
# Créé le:			1/8/2004
# Tabulation:		4 espaces
# 
# Copyright:		© 2004-2005, 2020 by Paul Guyot.
# 					Tous droits réservés pour tous pays.
# ===========

NCT_PROJECTS    ?= ../../../Projets/NewtonDev/NCT_Projects/

CC              := arm-none-eabi-gcc
CXX             := arm-none-eabi-g++
AS              := arm-none-eabi-as
LINKER          := arm-none-eabi-ld
OBJCOPY         := arm-none-eabi-objcopy

REX             := Rex

MACHINEFLAGS    := -march=armv3 -mbig-endian
ASFLAGS         := ${MACHINEFLAGS}
CODEGENFLAGS    := -nostdinc -fPIC -fdata-sections -ffunction-sections -O3 -Os
DEFINES         := -D__arm -DhasPCMCIA -DforARM -DNO_ERRNO_H -DSTDC -DTARGET_OS_NEWTON
CFLAGS          := ${MACHINEFLAGS} ${CODEGENFLAGS} ${DEFINES}
CXXFLAGS        := ${MACHINEFLAGS} ${CODEGENFLAGS} ${DEFINES} -fno-exceptions -fno-rtti
OBJDIR          := obj
LINKFLAGS       := -EB -shared --gc-sections --no-allow-shlib-undefined --no-export-dynamic -x --no-undefined

NEWTONINCLUDES  := -I$(NCT_PROJECTS)/DDKIncludes -I$(NCT_PROJECTS)/DDKIncludes/Bootstrap -I$(NCT_PROJECTS)/DDKIncludes/CLibrary -I$(NCT_PROJECTS)/DDKIncludes/CommAPI -I$(NCT_PROJECTS)/DDKIncludes/Communications -I$(NCT_PROJECTS)/DDKIncludes/Frames -I$(NCT_PROJECTS)/DDKIncludes/HAL -I$(NCT_PROJECTS)/DDKIncludes/Lantern -I$(NCT_PROJECTS)/DDKIncludes/OS600 -I$(NCT_PROJECTS)/DDKIncludes/Packages -I$(NCT_PROJECTS)/DDKIncludes/Power -I$(NCT_PROJECTS)/DDKIncludes/QD -I$(NCT_PROJECTS)/DDKIncludes/Toolbox -I$(NCT_PROJECTS)/DDKIncludes/UtilityClasses -I$(NCT_PROJECTS)/DDKIncludes/PCMCIA -I$(NCT_PROJECTS)/Includes -I$(NCT_PROJECTS)/Includes/CLibrary -I$(NCT_PROJECTS)/Includes/Frames -I$(NCT_PROJECTS)/Includes/Utilities -I$(NCT_PROJECTS)/LanternDriver_DDK/Includes -I$(NCT_PROJECTS)/NSandDDK

RUNTIMESRCDIR   := NewtonRuntime/
RUNTIMECXXSRCS  := $(shell find $(RUNTIMESRCDIR) -type f -name "*.cpp")
RUNTIMEASMSRCS  := $(shell find $(RUNTIMESRCDIR) -type f -name "*.s")
RUNTIMECXXOBJS  := $(patsubst $(RUNTIMESRCDIR)/%,$(OBJDIR)/%,$(RUNTIMECXXSRCS:.cpp=.cpp.o))
RUNTIMEASMOBJS  := $(patsubst $(RUNTIMESRCDIR)/%,$(OBJDIR)/%,$(RUNTIMEASMSRCS:.s=.s.o))
RUNTIMEOBJS     := $(RUNTIMECXXOBJS) $(RUNTIMEASMOBJS)

VPATH           := $(RUNTIMESRCDIR):../K/Misc/

# --- specific ---	
all : \
	$(OBJDIR) Einstein.rex

install : all
	cp Einstein.rex ../_Data_/

$(OBJDIR) :
	mkdir $(OBJDIR)

Einstein.rex : \
	Einstein.rex.src \
	$(OBJDIR)/EinsteinFlashDriver.bin \
	$(OBJDIR)/EinsteinPlatformDriver.pkg \
	$(OBJDIR)/EinsteinSoundDriver.pkg \
	$(OBJDIR)/EinsteinBatteryDriver.pkg \
	$(OBJDIR)/EinsteinScreenDriver.pkg \
	$(OBJDIR)/EinsteinTabletDriver.pkg \
	$(OBJDIR)/EinsteinVersionString.pkg \
	$(OBJDIR)/EinsteinStartupDriver.pkg \
	$(OBJDIR)/EinsteinSerialDriver.pkg \
	$(OBJDIR)/EinsteinInTranslator.pkg \
	$(OBJDIR)/EinsteinOutTranslator.pkg \
	$(OBJDIR)/EinsteinNativeCalls.pkg \
	NSRuntime/Einstein\ NS\ Runtime.pkg \
	../Packages/Keyboards/Keyboards.pkg
	rm -f Einstein.rex
	${REX} -o Einstein.rex Einstein.rex.src

$(OBJDIR)/%.pkg : $(OBJDIR)/%.elf
	rm -f $@
	ELFtoPKG -o $@ $(patsubst $(OBJDIR)/%,%,$(@:.pkg=:Einstein)) -protocol $< -autoLoad -autoRemove
	SetFile -t 'pkg ' -c 'pkgX' $@

$(OBJDIR)/%.cpp.o : %.cpp
	$(CXX) $(CXXFLAGS) $(NEWTONINCLUDES) -I.. -c $< -o $@

$(OBJDIR)/%.s.o : %.s
	$(AS) $(ASFLAGS) -c $< -o $@

GLUEOBJS := $(OBJDIR)/Glue.s.o $(OBJDIR)/UGlue.cpp.o
EINSTEIN_FLASH_DRIVER_OBJS := $(OBJDIR)/TEinsteinFlashDriver.impl.s.o $(OBJDIR)/TEinsteinFlashDriver.cpp.o $(OBJDIR)/TEinsteinFlashDriverPrims.s.o
EINSTEIN_PLATFORM_DRIVER_OBJS := $(OBJDIR)/TMainPlatformDriver.impl.s.o $(OBJDIR)/TMainPlatformDriver.cpp.o $(OBJDIR)/TMainPlatformDriverPrims.s.o $(GLUEOBJS) $(RUNTIMEOBJS)
EINSTEIN_SOUND_DRIVER_OBJS := $(OBJDIR)/PMainSoundDriver.impl.s.o $(OBJDIR)/PMainSoundDriver.cpp.o $(OBJDIR)/PMainSoundDriverPrims.s.o $(GLUEOBJS) $(RUNTIMEOBJS)
EINSTEIN_BATTERY_DRIVER_OBJS := $(OBJDIR)/PMainBatteryDriver.impl.s.o $(OBJDIR)/PMainBatteryDriver.cpp.o $(OBJDIR)/PMainBatteryDriverPrims.s.o
EINSTEIN_SCREEN_DRIVER_OBJS := $(OBJDIR)/TMainDisplayDriver.impl.s.o $(OBJDIR)/TMainDisplayDriver.cpp.o $(OBJDIR)/TMainDisplayDriverPrims.s.o $(RUNTIMEOBJS)
EINSTEIN_TABLET_DRIVER_OBJS := $(OBJDIR)/TMainTabletDriver.impl.s.o $(OBJDIR)/TMainTabletDriver.cpp.o $(OBJDIR)/TMainTabletDriverPrims.s.o $(RUNTIMEOBJS)
EINSTEIN_VERSION_STRING_OBJS := $(OBJDIR)/TMainVersionString.impl.s.o $(OBJDIR)/TMainVersionString.cpp.o $(RUNTIMEOBJS)
EINSTEIN_STARTUP_DRIVER_OBJS := $(OBJDIR)/TMainStartupDriver.impl.s.o $(OBJDIR)/TMainStartupDriver.cpp.o
EINSTEIN_SERIAL_DRIVER_OBJS := $(OBJDIR)/TSerialChipVoyager.impl.s.o $(OBJDIR)/TSerialChipVoyager.cpp.o $(OBJDIR)/TSerialChipVoyagerPrims.s.o
EINSTEIN_IN_TRANSLATOR_OBJS := $(OBJDIR)/PEinsteinInTranslator.impl.s.o $(OBJDIR)/PEinsteinInTranslator.cpp.o  $(OBJDIR)/PEinsteinInTranslatorPrims.s.o
EINSTEIN_OUT_TRANSLATOR_OBJS := $(OBJDIR)/PEinsteinOutTranslator.impl.s.o $(OBJDIR)/PEinsteinOutTranslator.cpp.o  $(OBJDIR)/PEinsteinOutTranslatorPrims.s.o $(RUNTIMEOBJS)
EINSTEIN_NATIVE_CALLS_OBJS := $(OBJDIR)/TEinsteinNativeCalls.impl.s.o $(OBJDIR)/TEinsteinNativeCalls.cpp.o  $(OBJDIR)/TEinsteinNativeCallsPrims.s.o

$(OBJDIR)/EinsteinFlashDriver.elf : $(EINSTEIN_FLASH_DRIVER_OBJS)
	$(LINKER) $(LINKFLAGS) -Tprotocol.ld --version-script=protocol.map $^ -o $@

$(OBJDIR)/EinsteinFlashDriver.bin : $(OBJDIR)/EinsteinFlashDriver.elf
	$(OBJCOPY) -O binary --only-section=.interp --only-section=.text $^ $@

$(OBJDIR)/EinsteinPlatformDriver.elf : $(EINSTEIN_PLATFORM_DRIVER_OBJS)
	$(LINKER) $(LINKFLAGS) -Tprotocol.ld --version-script=protocol.map $^ -o $@

$(OBJDIR)/EinsteinSoundDriver.elf : $(EINSTEIN_SOUND_DRIVER_OBJS)
	$(LINKER) $(LINKFLAGS) -Tprotocol.ld --version-script=protocol.map $^ -o $@

$(OBJDIR)/EinsteinBatteryDriver.elf : $(EINSTEIN_BATTERY_DRIVER_OBJS)
	$(LINKER) $(LINKFLAGS) -Tprotocol.ld --version-script=protocol.map $^ -o $@

$(OBJDIR)/EinsteinScreenDriver.elf : $(EINSTEIN_SCREEN_DRIVER_OBJS)
	$(LINKER) $(LINKFLAGS) -Tprotocol.ld --version-script=protocol.map $^ -o $@

$(OBJDIR)/EinsteinTabletDriver.elf : $(EINSTEIN_TABLET_DRIVER_OBJS)
	$(LINKER) $(LINKFLAGS) -Tprotocol.ld --version-script=protocol.map $^ -o $@

$(OBJDIR)/EinsteinVersionString.elf : $(EINSTEIN_VERSION_STRING_OBJS)
	$(LINKER) $(LINKFLAGS) -Tprotocol.ld --version-script=protocol.map $^ -o $@

$(OBJDIR)/EinsteinStartupDriver.elf : $(EINSTEIN_STARTUP_DRIVER_OBJS)
	$(LINKER) $(LINKFLAGS) -Tprotocol.ld --version-script=protocol.map $^ -o $@

$(OBJDIR)/EinsteinSerialDriver.elf : $(EINSTEIN_SERIAL_DRIVER_OBJS)
	$(LINKER) $(LINKFLAGS) -Tprotocol.ld --version-script=protocol.map $^ -o $@

$(OBJDIR)/EinsteinInTranslator.elf : $(EINSTEIN_IN_TRANSLATOR_OBJS)
	$(LINKER) $(LINKFLAGS) -Tprotocol.ld --version-script=protocol.map $^ -o $@

$(OBJDIR)/EinsteinOutTranslator.elf : $(EINSTEIN_OUT_TRANSLATOR_OBJS)
	$(LINKER) $(LINKFLAGS) -Tprotocol.ld --version-script=protocol.map $^ -o $@

$(OBJDIR)/EinsteinNativeCalls.elf : $(EINSTEIN_NATIVE_CALLS_OBJS)
	$(LINKER) $(LINKFLAGS) -Tprotocol.ld --version-script=protocol.map $^ -o $@

# --- clean ---
clean :
	rm -rf obj Einstein.rex

## =================================================== ##
## Can't open /usr/fortunes.  Lid stuck on cookie jar. ##
## =================================================== ##

