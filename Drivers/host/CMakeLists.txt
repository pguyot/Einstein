cmake_minimum_required(VERSION 3.14)
project(EinsteinHostTools)

include(ExternalProject)

if(CMAKE_CROSSCOMPILING)
    # When cross-compiling, host tools must be built with native host compilers
    find_program(HOST_C_COMPILER NAMES gcc clang cc REQUIRED)
    find_program(HOST_CXX_COMPILER NAMES g++ clang++ c++ REQUIRED)
else()
    set(HOST_C_COMPILER "${CMAKE_C_COMPILER}")
    set(HOST_CXX_COMPILER "${CMAKE_CXX_COMPILER}")
endif()

# 1. Build DCL (Host Tools)
ExternalProject_Add(dcl_host
    GIT_REPOSITORY https://github.com/pguyot/DCL.git
    GIT_TAG        master
    CMAKE_ARGS     -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
                   -DCMAKE_C_COMPILER=${HOST_C_COMPILER}
                   -DCMAKE_CXX_COMPILER=${HOST_CXX_COMPILER}
    INSTALL_DIR    ${CMAKE_CURRENT_BINARY_DIR}/dcl-install
)

# 2. Build tntk (Host Tools)
ExternalProject_Add(tntk_host
    GIT_REPOSITORY https://github.com/ekoeppen/tntk.git
    GIT_TAG        master
    CMAKE_ARGS     -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
                   -DCMAKE_C_COMPILER=${HOST_C_COMPILER}
                   -DCMAKE_CXX_COMPILER=${HOST_CXX_COMPILER}
                   -DCMAKE_PREFIX_PATH=${CMAKE_CURRENT_BINARY_DIR}/dcl-install
                   -DCMAKE_CXX_FLAGS=-I${CMAKE_CURRENT_BINARY_DIR}/dcl-install/include\ -DTARGET_RT_BIG_ENDIAN=0\ -DTARGET_RT_LITTLE_ENDIAN=1\ -include\ string.h
                   -DCMAKE_INSTALL_RPATH=${CMAKE_CURRENT_BINARY_DIR}/dcl-install/lib
    INSTALL_DIR    ${CMAKE_CURRENT_BINARY_DIR}/tntk-install
    DEPENDS        dcl_host
)

# Export variables to parent scope
set(DCL_BIN_DIR ${CMAKE_CURRENT_BINARY_DIR}/dcl-install/bin PARENT_SCOPE)
set(TNTK_BIN_DIR ${CMAKE_CURRENT_BINARY_DIR}/tntk-install/bin PARENT_SCOPE)
