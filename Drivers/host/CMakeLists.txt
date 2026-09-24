cmake_minimum_required(VERSION 3.14)
project(EinsteinHostTools)

include(ExternalProject)

# 1. Build DCL (Host Tools)
ExternalProject_Add(dcl_host
    GIT_REPOSITORY https://github.com/pguyot/DCL.git
    GIT_TAG        master
    CMAKE_ARGS     -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
                   -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                   -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    INSTALL_DIR    ${CMAKE_CURRENT_BINARY_DIR}/dcl-install
)

# 2. Build tntk (Host Tools)
ExternalProject_Add(tntk_host
    GIT_REPOSITORY https://github.com/ekoeppen/tntk.git
    GIT_TAG        master
    CMAKE_ARGS     -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
                   -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                   -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                   -DCMAKE_INSTALL_RPATH=${CMAKE_CURRENT_BINARY_DIR}/dcl-install/lib
    INSTALL_DIR    ${CMAKE_CURRENT_BINARY_DIR}/tntk-install
    DEPENDS        dcl_host
)

# Export variables to parent scope
set(DCL_BIN_DIR ${CMAKE_CURRENT_BINARY_DIR}/dcl-install/bin PARENT_SCOPE)
set(TNTK_BIN_DIR ${CMAKE_CURRENT_BINARY_DIR}/tntk-install/bin PARENT_SCOPE)
