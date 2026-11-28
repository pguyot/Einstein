cmake_minimum_required(VERSION 3.21)
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

# 1. Build cDCL Host Tools (Rex, ELFtoPKG, ELFtoNTK)
if(DEFINED cdcl_SOURCE_DIR)
    set(_dcl_source_args SOURCE_DIR ${cdcl_SOURCE_DIR})
else()
    set(_dcl_source_args
        GIT_REPOSITORY https://github.com/ekoeppen/cDCL.git
        GIT_TAG        3.2.4
    )
endif()

ExternalProject_Add(dcl_host
    ${_dcl_source_args}
    CMAKE_ARGS     -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
                   -DCMAKE_C_COMPILER=${HOST_C_COMPILER}
                   -DCMAKE_CXX_COMPILER=${HOST_CXX_COMPILER}
                   -DCDCL_BUILD_TOOLS=ON
                   -DCDCL_BUILD_TESTS=OFF
                   -DCDCL_INSTALL=ON
                   -DBUILD_SHARED_LIBS=OFF
    INSTALL_DIR    ${CMAKE_CURRENT_BINARY_DIR}/dcl-install
)

# 2. Build tntk (Host Tools, without DCL dependency)
ExternalProject_Add(tntk_host
    GIT_REPOSITORY https://github.com/ekoeppen/tntk.git
    GIT_TAG        master
    CMAKE_ARGS     -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
                   -DCMAKE_C_COMPILER=${HOST_C_COMPILER}
                   -DCMAKE_CXX_COMPILER=${HOST_CXX_COMPILER}
                   -DUSE_DCL=OFF
                   -DDCL_LIBRARY:FILEPATH=
    INSTALL_DIR    ${CMAKE_CURRENT_BINARY_DIR}/tntk-install
)

# Export variables to parent scope
set(DCL_BIN_DIR ${CMAKE_CURRENT_BINARY_DIR}/dcl-install/bin PARENT_SCOPE)
set(TNTK_BIN_DIR ${CMAKE_CURRENT_BINARY_DIR}/tntk-install/bin PARENT_SCOPE)
