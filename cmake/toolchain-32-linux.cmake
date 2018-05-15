SET(CMAKE_SYSTEM_PROCESSOR "i686")

SET(CMAKE_C_FLAGS             "-m32" CACHE STRING "C compiler flags"   FORCE)
SET(CMAKE_CXX_FLAGS           "-m32" CACHE STRING "C++ compiler flags" FORCE)

SET(LIB32 /usr/lib) # Fedora

IF(EXISTS /usr/lib32)
    SET(LIB32 /usr/lib32) # Arch, Solus
ENDIF()

SET(CMAKE_SYSTEM_LIBRARY_PATH ${LIB32} CACHE STRING "system library search path" FORCE)
SET(CMAKE_LIBRARY_PATH        ${LIB32} CACHE STRING "library search path"        FORCE)

# this is probably unlikely to be needed, but just in case
SET(CMAKE_EXE_LINKER_FLAGS    "-m32 -L${LIB32}" CACHE STRING "executable linker flags"     FORCE)
SET(CMAKE_SHARED_LINKER_FLAGS "-m32 -L${LIB32}" CACHE STRING "shared library linker flags" FORCE)
SET(CMAKE_MODULE_LINKER_FLAGS "-m32 -L${LIB32}" CACHE STRING "module linker flags"         FORCE)

# on Fedora and Arch and similar, point pkgconfig at 32 bit .pc files. We have
# to include the regular system .pc files as well (at the end), because some
# are not always present in the 32 bit directory
IF(EXISTS ${LIB32}/pkgconfig)
    SET(ENV{PKG_CONFIG_LIBDIR} ${LIB32}/pkgconfig:/usr/share/pkgconfig:/usr/lib/pkgconfig:/usr/lib64/pkgconfig)
ENDIF()
