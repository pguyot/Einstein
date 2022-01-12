#
# Add source files required to build Einstein.
#

list ( APPEND cmake_sources
  K/Unicode/CMakeLists.txt
)

list ( APPEND common_sources
	K/Unicode/UUTF16CStr.cpp
	K/Unicode/UUTF16CStr.h
	K/Unicode/UUTF16Conv.cpp
	K/Unicode/UUTF16Conv.h
	K/Unicode/UnicodeDefinitions.h
)

