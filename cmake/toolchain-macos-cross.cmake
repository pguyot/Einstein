# Toolchain file for cross-compiling from Linux to macOS using Clang + LLD

set(CMAKE_SYSTEM_NAME Darwin)

if(NOT DEFINED CMAKE_OSX_ARCHITECTURES OR CMAKE_OSX_ARCHITECTURES STREQUAL "")
    if(DEFINED ENV{MACOSX_ARCH} AND NOT "$ENV{MACOSX_ARCH}" STREQUAL "")
        set(CMAKE_OSX_ARCHITECTURES "$ENV{MACOSX_ARCH}" CACHE STRING "macOS target architecture")
    else()
        set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "macOS target architecture")
    endif()
endif()

set(CMAKE_SYSTEM_PROCESSOR "${CMAKE_OSX_ARCHITECTURES}")

if(NOT DEFINED CMAKE_OSX_DEPLOYMENT_TARGET OR CMAKE_OSX_DEPLOYMENT_TARGET STREQUAL "")
    if(DEFINED ENV{MACOSX_DEPLOYMENT_TARGET} AND NOT "$ENV{MACOSX_DEPLOYMENT_TARGET}" STREQUAL "")
        set(CMAKE_OSX_DEPLOYMENT_TARGET "$ENV{MACOSX_DEPLOYMENT_TARGET}" CACHE STRING "macOS deployment target")
    elseif(CMAKE_OSX_ARCHITECTURES STREQUAL "arm64")
        set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "macOS deployment target")
    else()
        set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13" CACHE STRING "macOS deployment target")
    endif()
endif()

if(NOT DEFINED CMAKE_OSX_SYSROOT OR CMAKE_OSX_SYSROOT STREQUAL "")
    if(DEFINED ENV{MACOSX_SDK} AND NOT "$ENV{MACOSX_SDK}" STREQUAL "")
        set(CMAKE_OSX_SYSROOT "$ENV{MACOSX_SDK}" CACHE PATH "macOS SDK path")
    endif()
endif()

if(NOT CMAKE_OSX_SYSROOT OR NOT EXISTS "${CMAKE_OSX_SYSROOT}")
    message(FATAL_ERROR "macOS SDK not found. Set CMAKE_OSX_SYSROOT or MACOSX_SDK to a valid MacOSX*.sdk directory.")
endif()

set(CMAKE_FIND_ROOT_PATH "${CMAKE_OSX_SYSROOT}")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

find_program(CMAKE_C_COMPILER NAMES clang REQUIRED)
find_program(CMAKE_CXX_COMPILER NAMES clang++ REQUIRED)
find_program(CMAKE_OBJC_COMPILER NAMES clang REQUIRED)
find_program(CMAKE_OBJCXX_COMPILER NAMES clang++ REQUIRED)
find_program(CMAKE_AR NAMES llvm-ar REQUIRED)
find_program(CMAKE_RANLIB NAMES llvm-ranlib REQUIRED)
find_program(CMAKE_STRIP NAMES llvm-strip)
find_program(CMAKE_INSTALL_NAME_TOOL NAMES llvm-install-name-tool install_name_tool)

set(_darwin_target "${CMAKE_OSX_ARCHITECTURES}-apple-darwin")
set(CMAKE_C_COMPILER_TARGET "${_darwin_target}")
set(CMAKE_CXX_COMPILER_TARGET "${_darwin_target}")
set(CMAKE_OBJC_COMPILER_TARGET "${_darwin_target}")
set(CMAKE_OBJCXX_COMPILER_TARGET "${_darwin_target}")

set(_darwin_linker_flags "-fuse-ld=lld")

set(_darwin_rt_src "${CMAKE_CURRENT_LIST_DIR}/darwin-os-version-check.c")
if(EXISTS "${_darwin_rt_src}" AND CMAKE_BINARY_DIR AND NOT CMAKE_BINARY_DIR MATCHES "CMakeScratch")
    set(_darwin_rt_obj "${CMAKE_BINARY_DIR}/CMakeFiles/darwin_os_version_check.o")
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/CMakeFiles")
    execute_process(
        COMMAND "${CMAKE_C_COMPILER}"
            --target=${_darwin_target}
            -arch "${CMAKE_OSX_ARCHITECTURES}"
            -isysroot "${CMAKE_OSX_SYSROOT}"
            -mmacosx-version-min=${CMAKE_OSX_DEPLOYMENT_TARGET}
            -O2 -c "${_darwin_rt_src}" -o "${_darwin_rt_obj}"
        RESULT_VARIABLE _darwin_rt_res
        ERROR_VARIABLE _darwin_rt_err
    )
    if(NOT _darwin_rt_res EQUAL 0)
        message(FATAL_ERROR "Failed to compile Darwin runtime helper: ${_darwin_rt_err}")
    endif()
    set(_darwin_linker_flags "${_darwin_linker_flags} \"${_darwin_rt_obj}\"")
endif()

set(CMAKE_EXE_LINKER_FLAGS_INIT "${_darwin_linker_flags}")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "${_darwin_linker_flags}")
set(CMAKE_MODULE_LINKER_FLAGS_INIT "${_darwin_linker_flags}")
