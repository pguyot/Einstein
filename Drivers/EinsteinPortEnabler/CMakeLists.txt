cmake_minimum_required(VERSION 3.10)
project(portenabler)
set(CMAKE_EXECUTABLE_SUFFIX ".elf")
enable_language(ASM)

find_program(TNTK tntk)
if(${TNTK} STREQUAL "TNTK-NOTFOUND")
    message("tntk not found, using precompiled package")
    add_custom_target(portenabler ALL)
    return()
endif()

set(NCT_PROJECTS $ENV{NCT_PROJECTS})
add_executable(portenabler main.cpp ../NewtonRuntime/NewtonPublicAPIs.s ../NewtonRuntime/NewtonPrivateAPIs.s)
target_include_directories(portenabler PRIVATE
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
target_compile_options(portenabler PRIVATE
    -march=armv4 -mbig-endian
    $<$<COMPILE_LANGUAGE:C,CXX>:
        -O2
        -D__arm -DhasPCMCIA -DforARM -DNO_ERRNO_H -DSTDC
        -fPIC -fdata-sections -ffunction-sections
        -Wno-multichar -Wno-endif-labels -Wno-builtin-declaration-mismatch -Wno-write-strings>
    $<$<COMPILE_LANGUAGE:C>: -Werror=incompatible-pointer-types>
    $<$<COMPILE_LANGUAGE:CXX>: -fno-exceptions -fno-rtti>)
target_link_options(portenabler PRIVATE
    -L ${PROJECT_SOURCE_DIR}
    -EB -shared --gc-sections
    --no-allow-shlib-undefined --no-export-dynamic --no-undefined -x
    -Tportenabler.ld --version-script=portenabler.ver)
add_custom_command(TARGET portenabler POST_BUILD
    COMMAND rm -f ${PROJECT_SOURCE_DIR}/portenabler.ntkc
    COMMAND ELFtoNTK -via ${PROJECT_SOURCE_DIR}/portenabler.exp -o ${PROJECT_SOURCE_DIR}/portenabler.ntkc $<TARGET_FILE:portenabler>)
add_custom_command(TARGET portenabler POST_BUILD
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMAND ${TNTK} -c -P ${NCT_PROJECTS}/Platforms portenabler.nprj)
set_directory_properties(PROPERTIES ADDITIONAL_CLEAN_FILES "portenabler.ntkc portenabler.pkg")
