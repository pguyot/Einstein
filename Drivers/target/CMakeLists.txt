cmake_minimum_required(VERSION 3.14)
project(EinsteinDriversTarget)
enable_language(ASM)

# These variables should be passed from the parent Drivers project
# REX_TOOL, ELFTOPKG_TOOL, ELFTONTK_TOOL, TNTK_TOOL, NCT_PROJECTS

set(LINK_OPTIONS
    -L ${CMAKE_CURRENT_SOURCE_DIR}/..
    -T ${CMAKE_CURRENT_SOURCE_DIR}/../protocol.ld --version-script=${CMAKE_CURRENT_SOURCE_DIR}/../protocol.map)

set(INCLUDE_DIRECTORIES
    .
    ${EINSTEIN_ROOT}
    ${EINSTEIN_ROOT}/Drivers
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

file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../obj)

# -- Shared runtime
add_library(Runtime STATIC
    ../Glue.s ../UGlue.cpp ../NewtonRuntime/NewtonRuntime.cpp ../NewtonRuntime/NewtonPublicAPIs.s ../NativePrimitives.s)
target_include_directories(Runtime PRIVATE ${INCLUDE_DIRECTORIES})
target_link_options(Runtime PRIVATE ${LINK_OPTIONS})

macro(define_driver DRIVER)
    add_executable(${DRIVER} ${ARGN})
    target_link_libraries(${DRIVER} Runtime)
    target_include_directories(${DRIVER} PRIVATE ${INCLUDE_DIRECTORIES})
    target_link_options(${DRIVER} PRIVATE ${LINK_OPTIONS})
    add_custom_command(TARGET ${DRIVER} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E rm -f ${CMAKE_CURRENT_SOURCE_DIR}/../obj/$<TARGET_FILE_NAME:${DRIVER}>.pkg
        COMMAND ${ELFTOPKG_TOOL} -o ${CMAKE_CURRENT_SOURCE_DIR}/../obj/$<TARGET_FILE_NAME:${DRIVER}>.pkg $<TARGET_FILE_NAME:${DRIVER}>:Einstein -protocol
        $<TARGET_FILE:${DRIVER}> -autoLoad -autoRemove)
endmacro()

define_driver(EinsteinBatteryDriver ../PMainBatteryDriver.cpp ../PMainBatteryDriver.impl.s ../PMainBatteryDriverPrims.s)
define_driver(EinsteinStartupDriver ../TMainStartupDriver.cpp ../TMainStartupDriver.impl.s)
define_driver(EinsteinPlatformDriver ../TMainPlatformDriver.cpp ../TMainPlatformDriver.impl.s ../TMainPlatformDriverPrims.s)
define_driver(EinsteinSoundDriver ../PMainSoundDriver.cpp ../PMainSoundDriver.impl.s ../PMainSoundDriverPrims.s)
define_driver(EinsteinScreenDriver ../TMainDisplayDriver.cpp ../TMainDisplayDriver.impl.s ../TMainDisplayDriverPrims.s)
define_driver(EinsteinTabletDriver ../TMainTabletDriver.cpp ../TMainTabletDriver.impl.s ../TMainTabletDriverPrims.s)
define_driver(EinsteinVersionString ../TMainVersionString.cpp ../TMainVersionString.impl.s)
define_driver(EinsteinSerialVoyager ../TSerialChipVoyager.cpp ../TSerialChipVoyager.impl.s ../TSerialChipVoyagerPrims.s)
define_driver(EinsteinInTranslator ../PEinsteinInTranslator.cpp ../PEinsteinInTranslator.impl.s ../PEinsteinInTranslatorPrims.s)
define_driver(EinsteinOutTranslator ../PEinsteinOutTranslator.cpp ../PEinsteinOutTranslator.impl.s ../PEinsteinOutTranslatorPrims.s)
define_driver(EinsteinNativeCalls ../TEinsteinNativeCalls.cpp ../TEinsteinNativeCalls.impl.s ../TEinsteinNativeCallsPrims.s)
define_driver(EinsteinSerialDriver  ../TSerialChipEinstein.cpp ../TSerialChipEinstein.impl.s ../TSerialChipEinsteinPrims.s)
define_driver(EinsteinFlashDriver ../TEinsteinFlashDriver.cpp ../TEinsteinFlashDriver.impl.s ../TEinsteinFlashDriverPrims.s)
add_custom_command(TARGET EinsteinFlashDriver POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:EinsteinFlashDriver> ${CMAKE_CURRENT_SOURCE_DIR}/../obj/$<TARGET_FILE_NAME:EinsteinFlashDriver>.bin)

# -- PortEnabler (Uses TNTK)
# Note: EinsteinPortEnabler CMakeLists would need to be updated to use variables passed from here.
# For now, we call the custom target.
add_subdirectory(../EinsteinPortEnabler ${CMAKE_CURRENT_BINARY_DIR}/portenabler)

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
    portenabler_package)

add_custom_command(TARGET Einstein POST_BUILD
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/..
    COMMAND ${CMAKE_COMMAND} -E rm -f Einstein.rex
    COMMAND ${REX_TOOL} -o Einstein.rex Einstein.rex.src)


# -- The NE2000 network card driver
find_package(Python3 COMPONENTS Interpreter REQUIRED)
add_executable(NE2K
    ../NE2000Driver/NE2000.cpp ../NE2000Driver/LanternDriverAPI.cpp
    ../NE2000Driver/NE2000.impl.s ../NE2000Driver/NE2000Prims.s)
add_dependencies(NE2K Runtime)
target_link_libraries(NE2K Runtime)
target_include_directories(NE2K PRIVATE ${INCLUDE_DIRECTORIES})
target_compile_options(NE2K PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-Wno-pmf-conversions>)
target_link_options(NE2K PRIVATE ${LINK_OPTIONS})
add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/../obj/NE2K.pkg
    COMMAND ${CMAKE_COMMAND} -E rm -f ${CMAKE_CURRENT_SOURCE_DIR}/../obj/NE2K.protocol.pkg ${CMAKE_CURRENT_SOURCE_DIR}/../obj/NE2K.pkg
    COMMAND ${ELFTOPKG_TOOL} -o ${CMAKE_CURRENT_SOURCE_DIR}/../obj/NE2K.protocol.pkg NE2K -protocol $<TARGET_FILE:NE2K> -autoLoad -autoRemove
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../NE2000Driver/AddNSPart.py
        ${CMAKE_CURRENT_SOURCE_DIR}/../obj/NE2K.protocol.pkg ${CMAKE_CURRENT_SOURCE_DIR}/../NE2000Driver/NE2K.auto 408 auto 0x81
        "NE2K" "Einstein network card driver, based on Apple sample code"
        ${CMAKE_CURRENT_SOURCE_DIR}/../obj/NE2K.pkg
    DEPENDS NE2K ${CMAKE_CURRENT_SOURCE_DIR}/../NE2000Driver/AddNSPart.py ${CMAKE_CURRENT_SOURCE_DIR}/../NE2000Driver/NE2K.auto)
add_custom_target(NE2K.pkg ALL DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../obj/NE2K.pkg)
add_custom_command(TARGET NE2K.pkg POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/../obj/NE2K.pkg ${CMAKE_CURRENT_SOURCE_DIR}/../../_Data_/NE2K.pkg)
