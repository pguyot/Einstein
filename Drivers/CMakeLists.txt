cmake_minimum_required(VERSION 3.10)
enable_language(ASM)

set(NCT_PROJECTS $ENV{NCT_PROJECTS})

set(LINK_OPTIONS
    -L ${CMAKE_SOURCE_DIR}
    -Tprotocol.ld --version-script=protocol.map)

set(INCLUDE_DIRECTORIES
    .
    ${CMAKE_SOURCE_DIR}/..
    ${NCT_PROJECTS}/DDKIncludes
    ${NCT_PROJECTS}/DDKIncludes/Bootstrap
    ${NCT_PROJECTS}/DDKIncludes/CLibrary
    ${NCT_PROJECTS}/DDKIncludes/CommAPI
    ${NCT_PROJECTS}/DDKIncludes/Communications
    ${NCT_PROJECTS}/DDKIncludes/Frames
    ${NCT_PROJECTS}/DDKIncludes/HAL
    ${NCT_PROJECTS}/DDKIncludes/Lantern
    ${NCT_PROJECTS}/DDKIncludes/OS600
    ${NCT_PROJECTS}/DDKIncludes/Packages
    ${NCT_PROJECTS}/DDKIncludes/Power
    ${NCT_PROJECTS}/DDKIncludes/QD
    ${NCT_PROJECTS}/DDKIncludes/Toolbox
    ${NCT_PROJECTS}/DDKIncludes/UtilityClasses
    ${NCT_PROJECTS}/DDKIncludes/PCMCIA
    ${NCT_PROJECTS}/Includes
    ${NCT_PROJECTS}/Includes/CLibrary
    ${NCT_PROJECTS}/Includes/Frames
    ${NCT_PROJECTS}/Includes/Utilities
    ${NCT_PROJECTS}/LanternDriver_DDK/Includes
    ${NCT_PROJECTS}/NSandDDK
    ${NCT_PROJECTS})

file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/obj)

# -- Shared runtime, containing the NewtonOS APIs and additional glue code

project(Runtime)
add_library(Runtime
    Glue.s UGlue.cpp NewtonRuntime/NewtonRuntime.cpp NewtonRuntime/NewtonPublicAPIs.s NativePrimitives.s)
target_include_directories(Runtime PRIVATE ${INCLUDE_DIRECTORIES})
target_link_options(Runtime PRIVATE ${LINK_OPTIONS})

# -- Drivers to interface with the Einstein emulator and host system

macro(define_driver DRIVER)
    project(${DRIVER})
    add_executable(${DRIVER} ${ARGN})
    add_dependencies(${DRIVER} Runtime)
    target_link_libraries(${DRIVER} Runtime)
    target_include_directories(${DRIVER} PRIVATE ${INCLUDE_DIRECTORIES})
    target_link_options(${DRIVER} PRIVATE ${LINK_OPTIONS})
    add_custom_command(TARGET ${DRIVER} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E rm -f ${CMAKE_SOURCE_DIR}/obj/$<TARGET_FILE_NAME:${DRIVER}>.pkg
        COMMAND ELFtoPKG -o ${CMAKE_SOURCE_DIR}/obj/$<TARGET_FILE_NAME:${DRIVER}>.pkg $<TARGET_FILE_NAME:${DRIVER}>:Einstein -protocol
        $<TARGET_FILE:${DRIVER}> -autoLoad -autoRemove)
endmacro()

define_driver(EinsteinBatteryDriver PMainBatteryDriver.cpp PMainBatteryDriver.impl.s PMainBatteryDriverPrims.s)
define_driver(EinsteinStartupDriver TMainStartupDriver.cpp TMainStartupDriver.impl.s)
define_driver(EinsteinPlatformDriver TMainPlatformDriver.cpp TMainPlatformDriver.impl.s TMainPlatformDriverPrims.s)
define_driver(EinsteinSoundDriver PMainSoundDriver.cpp PMainSoundDriver.impl.s PMainSoundDriverPrims.s)
define_driver(EinsteinScreenDriver TMainDisplayDriver.cpp TMainDisplayDriver.impl.s TMainDisplayDriverPrims.s)
define_driver(EinsteinTabletDriver TMainTabletDriver.cpp TMainTabletDriver.impl.s TMainTabletDriverPrims.s)
define_driver(EinsteinVersionString TMainVersionString.cpp TMainVersionString.impl.s)
define_driver(EinsteinSerialVoyager TSerialChipVoyager.cpp TSerialChipVoyager.impl.s TSerialChipVoyagerPrims.s)
define_driver(EinsteinInTranslator PEinsteinInTranslator.cpp PEinsteinInTranslator.impl.s PEinsteinInTranslatorPrims.s)
define_driver(EinsteinOutTranslator PEinsteinOutTranslator.cpp PEinsteinOutTranslator.impl.s PEinsteinOutTranslatorPrims.s)
define_driver(EinsteinNativeCalls TEinsteinNativeCalls.cpp TEinsteinNativeCalls.impl.s TEinsteinNativeCallsPrims.s)
define_driver(EinsteinSerialDriver  TSerialChipEinstein.cpp TSerialChipEinstein.impl.s TSerialChipEinsteinPrims.s)
define_driver(EinsteinFlashDriver TEinsteinFlashDriver.cpp TEinsteinFlashDriver.impl.s TEinsteinFlashDriverPrims.s)
add_custom_command(TARGET EinsteinFlashDriver POST_BUILD
    COMMAND ${OBJCOPY} -O binary $<TARGET_FILE:EinsteinFlashDriver> ${CMAKE_SOURCE_DIR}/obj/$<TARGET_FILE_NAME:EinsteinFlashDriver>.bin)

# -- Additional packages

add_subdirectory(EinsteinPortEnabler)

# -- Custom target to create the REx, will be always built and installed to the _Data_ directory

project(Einstein)
add_custom_target(Einstein ALL)
add_dependencies(Einstein
    EinsteinBatteryDriver
    EinsteinStartupDriver
    EinsteinFlashDriver
    EinsteinPlatformDriver
    EinsteinSoundDriver
    EinsteinScreenDriver
    EinsteinTabletDriver
    EinsteinVersionString
    EinsteinSerialDriver
    EinsteinSerialVoyager
    EinsteinInTranslator
    EinsteinOutTranslator
    EinsteinNativeCalls
    portenabler)
add_custom_command(TARGET Einstein
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMAND ${CMAKE_COMMAND} -E rm -f Einstein.rex
    COMMAND Rex -o Einstein.rex Einstein.rex.src)
install(FILES ${CMAKE_SOURCE_DIR}/Einstein.rex DESTINATION ${CMAKE_SOURCE_DIR}/../_Data_/)
