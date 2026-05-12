set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# MinGW target triple (defaults to 64-bit Windows)
if(NOT DEFINED MINGW_TRIPLE)
  set(MINGW_TRIPLE "x86_64-w64-mingw32")
endif()

# Do not search target environment for host build tools
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Compilers and binutils
if(NOT CMAKE_C_COMPILER)
  find_program(CMAKE_C_COMPILER NAMES ${MINGW_TRIPLE}-gcc gcc REQUIRED)
endif()

if(NOT CMAKE_CXX_COMPILER)
  find_program(CMAKE_CXX_COMPILER NAMES ${MINGW_TRIPLE}-g++ g++ REQUIRED)
endif()

if(NOT CMAKE_RC_COMPILER)
  find_program(CMAKE_RC_COMPILER NAMES ${MINGW_TRIPLE}-windres windres)
endif()

# Discover sysroot if available
if(NOT DEFINED CMAKE_FIND_ROOT_PATH AND CMAKE_C_COMPILER)
  execute_process(
    COMMAND ${CMAKE_C_COMPILER} -print-sysroot
    OUTPUT_VARIABLE _sysroot
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
  )
  if(_sysroot AND EXISTS "${_sysroot}")
    set(CMAKE_FIND_ROOT_PATH "${_sysroot}")
  endif()
endif()

# Prevent host pkg-config from leaking native host libraries during cross-compilation
if(CMAKE_CROSSCOMPILING)
  set(ENV{PKG_CONFIG_LIBDIR} "")
  set(ENV{PKG_CONFIG_PATH} "")
  if(DEFINED CMAKE_FIND_ROOT_PATH)
    set(ENV{PKG_CONFIG_SYSROOT_DIR} "${CMAKE_FIND_ROOT_PATH}")
  endif()
endif()
