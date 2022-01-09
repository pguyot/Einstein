SET(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
include(CMakeForceCompiler)

find_program(CMAKE_C_COMPILER NAMES arm-none-eabi-gcc arm-eabi-gcc)
find_program(CMAKE_CXX_COMPILER NAMES arm-none-eabi-g++ arm-eabi-g++)
find_program(CMAKE_ASM_COMPILER NAMES arm-none-eabi-as arm-eabi-as)
find_program(OBJCOPY NAMES arm-none-eabi-objcopy arm-eabi-objcopy)
find_program(CMAKE_LINKER NAMES arm-none-eabi-ld arm-eabi-ld)
set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_LINKER} <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")

get_filename_component(TOOLCHAIN_BIN_DIR ${CMAKE_C_COMPILER} DIRECTORY)
get_filename_component(TOOLCHAIN_PREFIX ${TOOLCHAIN_BIN_DIR} DIRECTORY)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

unset (CMAKE_LINKER CACHE)

set(CMAKE_FIND_ROOT_PATH ${TOOLCHAIN_PREFIX} ${CMAKE_PREFIX_PATH})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# -- Set default definitions for C, GCC and ASM code generation, warnings and definitions

set(MACHINE_FLAGS "-march=armv4 -mbig-endian")
set(DEFINES "-D__arm -DhasPCMCIA -DforARM -DNO_ERRNO_H -DSTDC -DTARGET_OS_NEWTON")
set(WARNINGS "-Wno-multichar -Wno-endif-labels -Wno-builtin-declaration-mismatch -Wno-write-strings")
set(CODE_GENERATION "-O3 -Os -fPIC -fdata-sections -ffunction-sections")

set(CMAKE_ASM_FLAGS "${MACHINE_FLAGS}" CACHE STRING "" FORCE)
set(CMAKE_C_FLAGS "${MACHINE_FLAGS} ${DEFINES} ${WARNINGS} ${CODE_GENERATION} -Werror=incompatible-pointer-types" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS "${MACHINE_FLAGS} ${DEFINES} ${WARNINGS} ${CODE_GENERATION} -fno-exceptions -fno-rtti" CACHE STRING "" FORCE)
set(CMAKE_CXX_LINK_FLAGS "-EB -shared --gc-sections --no-allow-shlib-undefined --no-export-dynamic --no-undefined -x" CACHE STRING "" FORCE)
